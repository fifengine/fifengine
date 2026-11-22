// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "eventchannel/eventmanager.h"
#include "eventchannel/mouse/imouselistener.h"
#include "eventchannel/mouse/mouseevent.h"
#include "eventchannel/sdl/isdleventlistener.h"
#include "eventchannel/touch/itouchlistener.h"
#include "eventchannel/touch/touchevent.h"
#include "eventchannel/touch/touchtracker.h"
#include "eventchannel/touch/touchtranslation.h"
#include "util/time/timemanager.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/window/window.h"
#include "video/window/windowsettings.h"

using Catch::Approx;
using FIFE::MouseEvent;
using FIFE::PinchTracker;
using FIFE::TouchEvent;
using FIFE::TouchTracker;
namespace TT = FIFE::TouchTranslation;

namespace
{
    /** Event objects timestamp themselves through TimeManager.
     *
     * Keeps a TimeManager alive for tests that construct MouseEvent outside
     * the full input environment. Reuses an existing one if a previous test
     * still owns it.
     */
    struct EventObjectFixture
    {
            std::shared_ptr<FIFE::TimeManager> timeManager;

            EventObjectFixture()
            {
                if (!FIFE::TimeManager::exists()) {
                    timeManager = std::make_shared<FIFE::TimeManager>();
                }
            }
    };
} // namespace

namespace
{
    class MinimalMouseListener : public FIFE::IMouseListener
    {
        public:
            void mousePressed(MouseEvent& /*evt*/) override
            {
            }
            void mouseMoved(MouseEvent& /*evt*/) override
            {
            }
            void mouseDragged(MouseEvent& /*evt*/) override
            {
            }
    };

    class MinimalTouchListener : public FIFE::ITouchListener
    {
        public:
            void touchDown(TouchEvent& /*evt*/) override
            {
            }
    };
} // namespace

// ---------------------------------------------------------------------------
// TouchTracker: identity, lifetime, midpoint, device type
// ---------------------------------------------------------------------------

TEST_CASE("TouchTracker keys fingers by touch and finger id", "[event][touch]")
{
    TouchTracker tracker;

    CHECK(tracker.begin(1, 10, 0.1F, 0.2F, 0.5F));
    // Same SDL_FingerID on a different SDL_TouchID must be a distinct finger.
    CHECK(tracker.begin(2, 10, 0.3F, 0.4F, 0.5F));

    CHECK(tracker.fingerCount() == 2U);
    CHECK(tracker.fingerCount(1) == 1U);
    CHECK(tracker.fingerCount(2) == 1U);
    CHECK(tracker.contains(1, 10));
    CHECK(tracker.contains(2, 10));
    CHECK_FALSE(tracker.contains(3, 10));
}

TEST_CASE("TouchTracker begin on an existing finger updates instead of duplicating", "[event][touch]")
{
    TouchTracker tracker;
    REQUIRE(tracker.begin(1, 10, 0.1F, 0.2F, 0.5F));
    CHECK_FALSE(tracker.begin(1, 10, 0.7F, 0.8F, 0.9F));

    CHECK(tracker.fingerCount() == 1U);

    float mx = 0.0F;
    float my = 0.0F;
    REQUIRE(tracker.midpoint(mx, my));
    CHECK(mx == Approx(0.7F));
    CHECK(my == Approx(0.8F));
}

TEST_CASE("TouchTracker update and end only affect the matching finger", "[event][touch]")
{
    TouchTracker tracker;
    REQUIRE(tracker.begin(1, 10, 0.1F, 0.1F, 0.0F));
    REQUIRE(tracker.begin(2, 10, 0.2F, 0.2F, 0.0F));

    CHECK_FALSE(tracker.update(3, 10, 0.9F, 0.9F, 0.0F));
    CHECK(tracker.fingerCount() == 2U);

    CHECK(tracker.update(1, 10, 0.4F, 0.6F, 0.5F));
    CHECK_FALSE(tracker.end(1, 99));
    CHECK(tracker.fingerCount() == 2U);

    CHECK(tracker.end(1, 10));
    CHECK_FALSE(tracker.end(1, 10));
    CHECK(tracker.fingerCount() == 1U);
    CHECK(tracker.contains(2, 10));
    CHECK_FALSE(tracker.contains(1, 10));
}

TEST_CASE("TouchTracker midpoint averages active fingers", "[event][touch]")
{
    TouchTracker tracker;

    float mx = 1.0F;
    float my = 1.0F;
    CHECK_FALSE(tracker.midpoint(mx, my));

    REQUIRE(tracker.begin(1, 10, 0.2F, 0.4F, 0.0F));
    REQUIRE(tracker.begin(1, 11, 0.6F, 0.8F, 0.0F));

    REQUIRE(tracker.midpoint(mx, my));
    CHECK(mx == Approx(0.4F));
    CHECK(my == Approx(0.6F));
}

TEST_CASE("TouchTracker distinguishes direct touch from trackpads", "[event][touch]")
{
    TouchTracker tracker;

    // Unknown devices are treated as direct touch screens.
    CHECK_FALSE(tracker.isTrackpad(99));

    tracker.setDeviceType(1, SDL_TOUCH_DEVICE_DIRECT);
    tracker.setDeviceType(2, SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE);
    tracker.setDeviceType(3, SDL_TOUCH_DEVICE_INDIRECT_RELATIVE);

    CHECK_FALSE(tracker.isTrackpad(1));
    CHECK(tracker.isTrackpad(2));
    CHECK(tracker.isTrackpad(3));
    CHECK_FALSE(tracker.isTrackpad(4));

    tracker.clear();
    CHECK(tracker.fingerCount() == 0U);
    // clear() also drops device types.
    CHECK_FALSE(tracker.isTrackpad(2));
}

// ---------------------------------------------------------------------------
// Coordinate conversion
// ---------------------------------------------------------------------------

TEST_CASE("Normalized touch coordinates map to window points", "[event][touch][translation]")
{
    TT::PointI const center = TT::normalizedToWindowPoints(0.5F, 0.5F, 800, 600);
    CHECK(center.x == 400);
    CHECK(center.y == 300);

    TT::PointI const origin = TT::normalizedToWindowPoints(0.0F, 0.0F, 800, 600);
    CHECK(origin.x == 0);
    CHECK(origin.y == 0);

    TT::PointI const bottomRight = TT::normalizedToWindowPoints(1.0F, 1.0F, 800, 600);
    CHECK(bottomRight.x == 800);
    CHECK(bottomRight.y == 600);
}

TEST_CASE("Normalized touch coordinates are clamped to the window", "[event][touch][translation]")
{
    // SDL may report values outside 0..1 for touches that leave the window.
    TT::PointI const topLeft = TT::normalizedToWindowPoints(-0.25F, -0.25F, 800, 600);
    CHECK(topLeft.x == 0);
    CHECK(topLeft.y == 0);

    TT::PointI const bottomRight = TT::normalizedToWindowPoints(1.25F, 1.25F, 800, 600);
    CHECK(bottomRight.x == 800);
    CHECK(bottomRight.y == 600);
}

TEST_CASE("Normalized touch coordinates handle degenerate window sizes", "[event][touch][translation]")
{
    TT::PointI const noWidth = TT::normalizedToWindowPoints(0.5F, 0.5F, 0, 600);
    CHECK(noWidth.x == 0);
    CHECK(noWidth.y == 0);

    TT::PointI const negativeSize = TT::normalizedToWindowPoints(0.5F, 0.5F, -10, -20);
    CHECK(negativeSize.x == 0);
    CHECK(negativeSize.y == 0);

    // Values are rounded, not truncated.
    TT::PointI const rounded = TT::normalizedToWindowPoints(0.001F, 0.999F, 800, 600);
    CHECK(rounded.x == 1);
    CHECK(rounded.y == 599);
}

// ---------------------------------------------------------------------------
// Wheel arbitration
// ---------------------------------------------------------------------------

TEST_CASE("Scroll arbitration picks the dominant axis", "[event][touch][translation]")
{
    using TT::ScrollAxis;

    CHECK(TT::classifyScroll(0.2F, 0.01F).axis == ScrollAxis::HORIZONTAL);
    CHECK(TT::classifyScroll(0.01F, 0.2F).axis == ScrollAxis::VERTICAL);

    // A tiny horizontal jitter must never override an intentional vertical pan.
    CHECK(TT::classifyScroll(0.009F, 0.2F).axis == ScrollAxis::VERTICAL);

    // Exact tie favours vertical so diagonal noise cannot trigger horizontal.
    CHECK(TT::classifyScroll(0.2F, 0.2F).axis == ScrollAxis::VERTICAL);
}

TEST_CASE("Scroll arbitration applies a horizontal dead zone", "[event][touch][translation]")
{
    using TT::ScrollAxis;

    CHECK(TT::classifyScroll(0.0F, 0.0F).axis == ScrollAxis::NONE);
    CHECK(TT::classifyScroll(0.004F, 0.003F).axis == ScrollAxis::NONE);
    CHECK(TT::classifyScroll(0.008F, 0.007F).axis == ScrollAxis::NONE);

    // Below the default dead zone, even if it is the larger axis.
    CHECK(TT::classifyScroll(0.005F, 0.001F).axis == ScrollAxis::NONE);

    // Slow vertical pans must still scroll; only horizontal jitter is gated.
    CHECK(TT::classifyScroll(0.0F, 0.004F).axis == ScrollAxis::VERTICAL);

    // At/above the dead zone and clearly dominant -> horizontal.
    CHECK(TT::classifyScroll(0.02F, 0.019F).axis == ScrollAxis::HORIZONTAL);
}

TEST_CASE("Scroll arbitration honours a custom dead zone", "[event][touch][translation]")
{
    using TT::ScrollAxis;

    CHECK(TT::classifyScroll(0.05F, 0.0F, 0.1F).axis == ScrollAxis::NONE);
    CHECK(TT::classifyScroll(0.1F, 0.0F, 0.1F).axis == ScrollAxis::HORIZONTAL);
    CHECK(TT::classifyScroll(0.2F, 0.0F, 0.1F).axis == ScrollAxis::HORIZONTAL);

    // A negative dead zone behaves like zero.
    CHECK(TT::classifyScroll(0.001F, 0.0F, -1.0F).axis == ScrollAxis::HORIZONTAL);
}

TEST_CASE("Scroll direction matches SDL/FIFE wheel conventions", "[event][touch][translation]")
{
    // Finger moving right/down must scroll right/down.
    CHECK(TT::classifyScroll(0.2F, 0.0F).direction == 1);
    CHECK(TT::classifyScroll(-0.2F, 0.0F).direction == -1);
    CHECK(TT::classifyScroll(0.0F, 0.2F).direction == 1);
    CHECK(TT::classifyScroll(0.0F, -0.2F).direction == -1);
    CHECK(TT::classifyScroll(0.0F, 0.0F).direction == 0);
}

TEST_CASE("Scroll steps map to mouse wheel event types", "[event][touch][translation]")
{
    CHECK(TT::wheelTypeFor(TT::classifyScroll(0.2F, 0.0F)) == MouseEvent::WHEEL_MOVED_RIGHT);
    CHECK(TT::wheelTypeFor(TT::classifyScroll(-0.2F, 0.0F)) == MouseEvent::WHEEL_MOVED_LEFT);
    CHECK(TT::wheelTypeFor(TT::classifyScroll(0.0F, 0.2F)) == MouseEvent::WHEEL_MOVED_DOWN);
    CHECK(TT::wheelTypeFor(TT::classifyScroll(0.0F, -0.2F)) == MouseEvent::WHEEL_MOVED_UP);
    CHECK(TT::wheelTypeFor(TT::classifyScroll(0.0F, 0.0F)) == MouseEvent::UNKNOWN_EVENT);
}

// ---------------------------------------------------------------------------
// Pinch accumulation
// ---------------------------------------------------------------------------

TEST_CASE("PinchTracker accumulates scale and resets on end", "[event][touch][translation]")
{
    PinchTracker pinch;
    CHECK_FALSE(pinch.isActive());
    CHECK(pinch.scale() == Approx(1.0F));

    pinch.begin();
    CHECK(pinch.isActive());
    CHECK(pinch.scale() == Approx(1.0F));

    CHECK(pinch.update(2.0F));
    CHECK(pinch.scale() == Approx(2.0F));
    CHECK(pinch.update(0.5F));
    CHECK(pinch.scale() == Approx(1.0F));

    pinch.end();
    CHECK_FALSE(pinch.isActive());
    CHECK(pinch.scale() == Approx(1.0F));

    // Updates outside a gesture are ignored.
    CHECK_FALSE(pinch.update(4.0F));
    CHECK(pinch.scale() == Approx(1.0F));
}

// ---------------------------------------------------------------------------
// MouseEvent wheel deltas
// ---------------------------------------------------------------------------

TEST_CASE_METHOD(EventObjectFixture, "MouseEvent exposes signed wheel deltas", "[event][mouse]")
{
    MouseEvent evt;
    CHECK(evt.getWheelDeltaX() == Approx(0.0F));
    CHECK(evt.getWheelDeltaY() == Approx(0.0F));

    evt.setWheelDeltaX(1.5F);
    evt.setWheelDeltaY(-2.25F);
    CHECK(evt.getWheelDeltaX() == Approx(1.5F));
    CHECK(evt.getWheelDeltaY() == Approx(-2.25F));
}

TEST_CASE_METHOD(EventObjectFixture, "TouchEvent exposes finger and pinch data", "[event][touch]")
{
    TouchEvent evt;
    CHECK(evt.getType() == TouchEvent::UNKNOWN);
    CHECK(evt.getTouchId() == 0U);
    CHECK(evt.getFingerId() == 0U);
    CHECK(evt.getX() == -1);
    CHECK(evt.getY() == -1);
    CHECK(evt.getScale() == Approx(1.0F));
    CHECK(evt.getFingerCount() == 0);
    CHECK_FALSE(evt.isTrackpad());
    CHECK_FALSE(evt.isPinch());

    evt.setType(TouchEvent::PINCH_UPDATE);
    evt.setTouchId(7U);
    evt.setFingerId(11U);
    evt.setX(320);
    evt.setY(240);
    evt.setNormalizedX(0.4F);
    evt.setNormalizedY(0.4F);
    evt.setDeltaX(-0.1F);
    evt.setDeltaY(0.2F);
    evt.setPressure(0.75F);
    evt.setScale(1.5F);
    evt.setFingerCount(2);
    evt.setTrackpad(true);

    CHECK(evt.getType() == TouchEvent::PINCH_UPDATE);
    CHECK(evt.getTouchId() == 7U);
    CHECK(evt.getFingerId() == 11U);
    CHECK(evt.getX() == 320);
    CHECK(evt.getY() == 240);
    CHECK(evt.getNormalizedX() == Approx(0.4F));
    CHECK(evt.getNormalizedY() == Approx(0.4F));
    CHECK(evt.getDeltaX() == Approx(-0.1F));
    CHECK(evt.getDeltaY() == Approx(0.2F));
    CHECK(evt.getPressure() == Approx(0.75F));
    CHECK(evt.getScale() == Approx(1.5F));
    CHECK(evt.getFingerCount() == 2);
    CHECK(evt.isTrackpad());
    CHECK(evt.isPinch());
    CHECK(evt.getName() == "TouchEvent");
    CHECK(TouchEvent::touchEventType2str(TouchEvent::PINCH_UPDATE) == "pinch_update");
}

TEST_CASE_METHOD(
    EventObjectFixture, "ITouchListener only requires the callbacks a subclass uses", "[event][touch][swig]")
{
    MinimalTouchListener listener;
    TouchEvent evt;

    // These defaults must exist so a Python director that omits them cannot
    // trigger a pure-virtual call crash.
    listener.touchUp(evt);
    listener.touchMotion(evt);
    listener.touchCanceled(evt);
    listener.pinchBegin(evt);
    listener.pinchUpdate(evt);
    listener.pinchEnd(evt);
    CHECK_FALSE(listener.isGlobalListener());
    listener.setGlobalListener(true);
    CHECK(listener.isGlobalListener());
}

// ---------------------------------------------------------------------------
// SWIG director hardening
// ---------------------------------------------------------------------------

TEST_CASE_METHOD(
    EventObjectFixture, "IMouseListener only requires the callbacks a subclass uses", "[event][mouse][swig]")
{
    MinimalMouseListener listener;
    MouseEvent evt;

    // These defaults must exist so a Python director that omits them cannot
    // trigger a pure-virtual call crash.
    listener.mouseWheelMovedUp(evt);
    listener.mouseWheelMovedDown(evt);
    listener.mouseWheelMovedLeft(evt);
    listener.mouseWheelMovedRight(evt);
    listener.mouseEntered(evt);
    listener.mouseExited(evt);
    listener.mouseReleased(evt);
    listener.mouseClicked(evt);
}

// ---------------------------------------------------------------------------
// Event-type guards
// ---------------------------------------------------------------------------

TEST_CASE("Only real mouse events reach the mouse translation path", "[event][mouse][translation]")
{
    CHECK(TT::isMouseEventType(SDL_EVENT_MOUSE_MOTION));
    CHECK(TT::isMouseEventType(SDL_EVENT_MOUSE_BUTTON_DOWN));
    CHECK(TT::isMouseEventType(SDL_EVENT_MOUSE_BUTTON_UP));
    CHECK(TT::isMouseEventType(SDL_EVENT_MOUSE_WHEEL));

    CHECK_FALSE(TT::isMouseEventType(SDL_EVENT_FINGER_DOWN));
    CHECK_FALSE(TT::isMouseEventType(SDL_EVENT_FINGER_MOTION));
    CHECK_FALSE(TT::isMouseEventType(SDL_EVENT_FINGER_UP));
    CHECK_FALSE(TT::isMouseEventType(SDL_EVENT_PINCH_UPDATE));
    CHECK_FALSE(TT::isMouseEventType(SDL_EVENT_KEY_DOWN));
}

TEST_CASE("Touch-synthesized mouse events are detected", "[event][touch][translation]")
{
    SDL_Event evt{};

    evt.type         = SDL_EVENT_MOUSE_MOTION;
    evt.motion.which = SDL_TOUCH_MOUSEID;
    CHECK(TT::isTouchSynthesizedMouse(evt));

    evt.motion.which = 1;
    CHECK_FALSE(TT::isTouchSynthesizedMouse(evt));

    evt.type         = SDL_EVENT_MOUSE_BUTTON_DOWN;
    evt.button.which = SDL_TOUCH_MOUSEID;
    CHECK(TT::isTouchSynthesizedMouse(evt));

    evt.type        = SDL_EVENT_MOUSE_WHEEL;
    evt.wheel.which = SDL_TOUCH_MOUSEID;
    CHECK(TT::isTouchSynthesizedMouse(evt));

    evt.type = SDL_EVENT_FINGER_MOTION;
    CHECK_FALSE(TT::isTouchSynthesizedMouse(evt));
}

// ---------------------------------------------------------------------------
// End-to-end EventManager tests: real SDL event queue, real Window and
// RenderBackend, real listener interfaces. No mocks.
// ---------------------------------------------------------------------------

namespace
{
    struct TouchInputEnvironment
    {
            std::shared_ptr<FIFE::TimeManager> timeManager;
            FIFE::Window window;
            std::unique_ptr<FIFE::RenderBackendSDL> backend;

            TouchInputEnvironment()
            {
                // Event base objects timestamp themselves through TimeManager.
                // Reuse an existing one if another test already created it.
                if (!FIFE::TimeManager::exists()) {
                    timeManager = std::make_shared<FIFE::TimeManager>();
                }

                if (SDL_GetCurrentVideoDriver() == nullptr) {
                    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
                }

                // Same hint path the engine uses before video init.
                FIFE::EventManager::applyInputHints();

                if (!SDL_Init(SDL_INIT_VIDEO)) {
                    throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
                }

                window.create(
                    FIFE::WindowSettings{
                        .width = 800, .height = 600, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});

                backend = std::make_unique<FIFE::RenderBackendSDL>(SDL_Color{0, 0, 0, 255});
                backend->init("");
                backend->setWindowObject(&window);
            }

            ~TouchInputEnvironment()
            {
                // The backend holds a Window*; tear it down first.
                backend.reset();
                window.destroy();
            }
    };

    struct RecordedMouseEvent
    {
            MouseEvent::MouseEventType type    = MouseEvent::UNKNOWN_EVENT;
            MouseEvent::MouseButtonType button = MouseEvent::EMPTY;
            int32_t x                          = 0;
            int32_t y                          = 0;
            float wheelDeltaX                  = 0.0F;
            float wheelDeltaY                  = 0.0F;
    };

    class RecordingMouseListener : public FIFE::IMouseListener
    {
        public:
            std::vector<RecordedMouseEvent> events;

            void clear()
            {
                events.clear();
            }

            void mousePressed(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseReleased(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseMoved(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseDragged(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseWheelMovedUp(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseWheelMovedDown(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseWheelMovedLeft(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseWheelMovedRight(MouseEvent& evt) override
            {
                record(evt);
            }
            void mouseClicked(MouseEvent& evt) override
            {
                record(evt);
            }

        private:
            void record(MouseEvent& evt)
            {
                events.push_back(
                    RecordedMouseEvent{
                        evt.getType(),
                        evt.getButton(),
                        evt.getX(),
                        evt.getY(),
                        evt.getWheelDeltaX(),
                        evt.getWheelDeltaY()});
            }
    };

    struct RecordedTouchEvent
    {
            TouchEvent::TouchEventType type = TouchEvent::UNKNOWN;
            uint64_t touchId                = 0;
            uint64_t fingerId               = 0;
            int32_t x                       = 0;
            int32_t y                       = 0;
            float normalizedX               = 0.0F;
            float normalizedY               = 0.0F;
            float deltaX                    = 0.0F;
            float deltaY                    = 0.0F;
            float pressure                  = 0.0F;
            float scale                     = 0.0F;
            int32_t fingerCount             = 0;
            bool trackpad                   = false;
    };

    class RecordingTouchListener : public FIFE::ITouchListener
    {
        public:
            std::vector<RecordedTouchEvent> events;

            void clear()
            {
                events.clear();
            }

            void touchDown(TouchEvent& evt) override
            {
                record(evt);
            }
            void touchUp(TouchEvent& evt) override
            {
                record(evt);
            }
            void touchMotion(TouchEvent& evt) override
            {
                record(evt);
            }
            void touchCanceled(TouchEvent& evt) override
            {
                record(evt);
            }
            void pinchBegin(TouchEvent& evt) override
            {
                record(evt);
            }
            void pinchUpdate(TouchEvent& evt) override
            {
                record(evt);
            }
            void pinchEnd(TouchEvent& evt) override
            {
                record(evt);
            }

        private:
            void record(TouchEvent& evt)
            {
                events.push_back(
                    RecordedTouchEvent{
                        evt.getType(),
                        evt.getTouchId(),
                        evt.getFingerId(),
                        evt.getX(),
                        evt.getY(),
                        evt.getNormalizedX(),
                        evt.getNormalizedY(),
                        evt.getDeltaX(),
                        evt.getDeltaY(),
                        evt.getPressure(),
                        evt.getScale(),
                        evt.getFingerCount(),
                        evt.isTrackpad()});
            }
    };

    class RecordingSdlListener : public FIFE::ISdlEventListener
    {
        public:
            std::vector<SDL_Event> events;

            void clear()
            {
                events.clear();
            }

            bool onSdlEvent(SDL_Event& evt) override
            {
                events.push_back(evt);
                return false;
            }
    };

    void flushSdlEvents()
    {
        SDL_PumpEvents();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        }
    }

    void pushFinger(
        uint32_t type, SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float dx = 0.0F, float dy = 0.0F)
    {
        SDL_Event event{};
        event.type             = type;
        event.tfinger.touchID  = touchId;
        event.tfinger.fingerID = fingerId;
        event.tfinger.x        = x;
        event.tfinger.y        = y;
        event.tfinger.dx       = dx;
        event.tfinger.dy       = dy;
        event.tfinger.pressure = 0.5F;
        REQUIRE(SDL_PushEvent(&event));
    }

    void pushMouseWheel(float x, float y, SDL_MouseWheelDirection direction, float mouseX, float mouseY)
    {
        SDL_Event event{};
        event.type            = SDL_EVENT_MOUSE_WHEEL;
        event.wheel.windowID  = 1;
        event.wheel.which     = 1;
        event.wheel.x         = x;
        event.wheel.y         = y;
        event.wheel.direction = direction;
        event.wheel.mouse_x   = mouseX;
        event.wheel.mouse_y   = mouseY;
        REQUIRE(SDL_PushEvent(&event));
    }

    void pushPinch(uint32_t type, float scale)
    {
        SDL_Event event{};
        event.type        = type;
        event.pinch.scale = scale;
        REQUIRE(SDL_PushEvent(&event));
    }
} // namespace

TEST_CASE("EventManager synthesizes a real mouse press and release for touch", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.5F, 0.5F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::PRESSED);
    CHECK(listener.events.at(0).button == MouseEvent::LEFT);
    CHECK(listener.events.at(0).x == 400);
    CHECK(listener.events.at(0).y == 300);

    listener.clear();
    pushFinger(SDL_EVENT_FINGER_UP, 1, 10, 0.5F, 0.5F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::RELEASED);
    CHECK(listener.events.at(0).x == 400);
    CHECK(listener.events.at(0).y == 300);
}

TEST_CASE("EventManager emits point coordinates to SDL/GUI listeners", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingSdlListener listener;
    manager.addSdlEventListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.25F, 0.75F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    SDL_Event const & synthetic = listener.events.at(0);
    REQUIRE(synthetic.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
    CHECK(synthetic.button.x == Approx(200.0F));
    CHECK(synthetic.button.y == Approx(450.0F));
}

TEST_CASE("EventManager dispatches raw finger events to touch listeners", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingTouchListener listener;
    manager.addTouchListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 3, 21, 0.25F, 0.75F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == TouchEvent::DOWN);
    CHECK(listener.events.at(0).touchId == 3U);
    CHECK(listener.events.at(0).fingerId == 21U);
    CHECK(listener.events.at(0).x == 200);
    CHECK(listener.events.at(0).y == 450);
    CHECK(listener.events.at(0).normalizedX == Approx(0.25F));
    CHECK(listener.events.at(0).normalizedY == Approx(0.75F));
    CHECK(listener.events.at(0).pressure == Approx(0.5F));
    CHECK(listener.events.at(0).fingerCount == 1);
    CHECK_FALSE(listener.events.at(0).trackpad);

    listener.clear();
    pushFinger(SDL_EVENT_FINGER_MOTION, 3, 21, 0.5F, 0.5F, 0.1F, -0.2F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == TouchEvent::MOTION);
    CHECK(listener.events.at(0).x == 400);
    CHECK(listener.events.at(0).y == 300);
    CHECK(listener.events.at(0).deltaX == Approx(0.1F));
    CHECK(listener.events.at(0).deltaY == Approx(-0.2F));

    listener.clear();
    pushFinger(SDL_EVENT_FINGER_UP, 3, 21, 0.5F, 0.5F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == TouchEvent::UP);
    CHECK(listener.events.at(0).fingerCount == 0);

    listener.clear();
    pushFinger(SDL_EVENT_FINGER_CANCELED, 3, 21, 0.5F, 0.5F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == TouchEvent::CANCELED);
}

TEST_CASE("EventManager dispatches pinch events with accumulated scale", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingTouchListener listener;
    manager.addTouchListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 4, 31, 0.2F, 0.4F);
    pushFinger(SDL_EVENT_FINGER_DOWN, 4, 32, 0.6F, 0.8F);
    manager.processEvents();
    listener.clear();

    pushPinch(SDL_EVENT_PINCH_BEGIN, 1.0F);
    pushPinch(SDL_EVENT_PINCH_UPDATE, 2.0F);
    pushPinch(SDL_EVENT_PINCH_UPDATE, 0.5F);
    pushPinch(SDL_EVENT_PINCH_END, 1.0F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 4U);

    CHECK(listener.events.at(0).type == TouchEvent::PINCH_BEGIN);
    CHECK(listener.events.at(0).scale == Approx(1.0F));
    CHECK(listener.events.at(0).fingerCount == 2);
    CHECK(listener.events.at(0).normalizedX == Approx(0.4F));
    CHECK(listener.events.at(0).normalizedY == Approx(0.6F));
    CHECK(listener.events.at(0).x == 320);
    CHECK(listener.events.at(0).y == 360);

    CHECK(listener.events.at(1).type == TouchEvent::PINCH_UPDATE);
    CHECK(listener.events.at(1).scale == Approx(2.0F));

    CHECK(listener.events.at(2).type == TouchEvent::PINCH_UPDATE);
    CHECK(listener.events.at(2).scale == Approx(1.0F));

    CHECK(listener.events.at(3).type == TouchEvent::PINCH_END);
    CHECK(listener.events.at(3).scale == Approx(1.0F));
}

TEST_CASE("Vertical touch pan never produces a horizontal wheel callback", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.4F, 0.5F);
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 11, 0.6F, 0.5F);
    manager.processEvents();
    listener.clear();

    // Tiny horizontal jitter with a dominant vertical move: this must stay
    // vertical instead of being routed as a horizontal wheel event.
    pushFinger(SDL_EVENT_FINGER_MOTION, 1, 10, 0.409F, 0.7F, 0.009F, 0.2F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::WHEEL_MOVED_DOWN);
    CHECK(listener.events.at(0).wheelDeltaY < 0.0F);
}

TEST_CASE("Horizontal touch pan maps to the matching wheel direction", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.4F, 0.5F);
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 11, 0.6F, 0.5F);
    manager.processEvents();
    listener.clear();

    pushFinger(SDL_EVENT_FINGER_MOTION, 1, 10, 0.6F, 0.51F, 0.2F, 0.01F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::WHEEL_MOVED_RIGHT);
    CHECK(listener.events.at(0).wheelDeltaX > 0.0F);
}

TEST_CASE("FINGER_CANCELED releases the synthetic pointer without a click", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.5F, 0.5F);
    manager.processEvents();
    listener.clear();

    pushFinger(SDL_EVENT_FINGER_MOTION, 1, 10, 0.6F, 0.4F, 0.1F, -0.1F);
    manager.processEvents();
    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::DRAGGED);

    listener.clear();
    pushFinger(SDL_EVENT_FINGER_CANCELED, 1, 10, 0.6F, 0.4F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::RELEASED);
    CHECK(listener.events.at(0).x == 480);
    CHECK(listener.events.at(0).y == 240);
}

TEST_CASE("A second finger finishes an in-progress one-finger drag", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.5F, 0.5F);
    manager.processEvents();
    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::PRESSED);

    // The second finger turns the gesture into a pan, so the synthetic drag
    // must be released instead of leaving the left button held.
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 11, 0.7F, 0.5F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 2U);
    CHECK(listener.events.at(1).type == MouseEvent::RELEASED);
    CHECK(listener.events.at(1).x == 400);
    CHECK(listener.events.at(1).y == 300);

    // Ending either finger of the pan must not emit another release.
    pushFinger(SDL_EVENT_FINGER_UP, 1, 10, 0.5F, 0.5F);
    pushFinger(SDL_EVENT_FINGER_UP, 1, 11, 0.7F, 0.5F);
    manager.processEvents();
    CHECK(listener.events.size() == 2U);
}

TEST_CASE("Focus loss releases a held touch pointer and resets drag state", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    pushFinger(SDL_EVENT_FINGER_DOWN, 1, 10, 0.5F, 0.5F);
    manager.processEvents();
    listener.clear();

    SDL_Event focusLost{};
    focusLost.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    REQUIRE(SDL_PushEvent(&focusLost));
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::RELEASED);
    CHECK(listener.events.at(0).x == 400);
    CHECK(listener.events.at(0).y == 300);

    // After the reset, a missing DOWN is recovered as a fresh move rather than
    // continuing the old drag.
    listener.clear();
    pushFinger(SDL_EVENT_FINGER_MOTION, 1, 10, 0.6F, 0.4F, 0.1F, -0.1F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::MOVED);
}

TEST_CASE("Real mouse wheel uses dominant-axis arbitration and deltas", "[event][mouse][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    // Vertical wheel with a little horizontal noise -> only vertical.
    pushMouseWheel(0.1F, 1.0F, SDL_MOUSEWHEEL_NORMAL, 100.0F, 200.0F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::WHEEL_MOVED_UP);
    CHECK(listener.events.at(0).wheelDeltaY == Approx(1.0F));
    CHECK(listener.events.at(0).wheelDeltaX == Approx(0.1F));
    CHECK(listener.events.at(0).x == 100);
    CHECK(listener.events.at(0).y == 200);

    listener.clear();
    pushMouseWheel(1.0F, 0.01F, SDL_MOUSEWHEEL_NORMAL, 300.0F, 400.0F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::WHEEL_MOVED_RIGHT);
    CHECK(listener.events.at(0).wheelDeltaX == Approx(1.0F));

    listener.clear();
    pushMouseWheel(0.0F, -1.0F, SDL_MOUSEWHEEL_FLIPPED, 300.0F, 400.0F);
    manager.processEvents();

    REQUIRE(listener.events.size() == 1U);
    CHECK(listener.events.at(0).type == MouseEvent::WHEEL_MOVED_UP);
    CHECK(listener.events.at(0).wheelDeltaY == Approx(1.0F));
}

TEST_CASE("SDL touch-synthesized mouse events are ignored under policy B", "[event][touch][integration]")
{
    TouchInputEnvironment env;
    FIFE::EventManager manager;
    RecordingMouseListener listener;
    manager.addMouseListener(&listener);

    flushSdlEvents();
    SDL_Event synthesized{};
    synthesized.type          = SDL_EVENT_MOUSE_BUTTON_DOWN;
    synthesized.button.which  = SDL_TOUCH_MOUSEID;
    synthesized.button.button = SDL_BUTTON_LEFT;
    synthesized.button.x      = 10.0F;
    synthesized.button.y      = 20.0F;
    REQUIRE(SDL_PushEvent(&synthesized));

    manager.processEvents();
    CHECK(listener.events.empty());
}

TEST_CASE("Input hints disable SDL touch/mouse synthesis", "[event][touch][integration]")
{
    FIFE::EventManager::applyInputHints();

    char const * const touchMouse = SDL_GetHint(SDL_HINT_TOUCH_MOUSE_EVENTS);
    char const * const mouseTouch = SDL_GetHint(SDL_HINT_MOUSE_TOUCH_EVENTS);
    char const * const trackpad   = SDL_GetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY);

    REQUIRE(touchMouse != nullptr);
    REQUIRE(mouseTouch != nullptr);
    REQUIRE(trackpad != nullptr);
    CHECK(std::string(touchMouse) == "0");
    CHECK(std::string(mouseTouch) == "0");
    CHECK(std::string(trackpad) == "0");
}
