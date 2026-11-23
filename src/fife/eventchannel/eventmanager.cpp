// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "eventmanager.h"

// Standard C++ library includes
#include <algorithm>
#include <cmath>
#include <deque>
#include <format>
#include <iostream>
#include <memory>
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/command/command.h"
#include "eventchannel/joystick/joystickmanager.h"
#include "eventchannel/key/ikeyfilter.h"
#include "eventchannel/key/key.h"
#include "eventchannel/key/keyevent.h"
#include "eventchannel/mouse/mouseevent.h"
#include "eventchannel/touch/touchtranslation.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "video/renderbackend.h"
#include "video/window/window.h"

namespace FIFE
{
    namespace
    {
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_EVTCHANNEL);
            return log;
        }

        Window* touchWindow()
        {
            if (!RenderBackend::exists()) {
                return nullptr;
            }
            return RenderBackend::instance()->getWindowObject();
        }

        uint32_t touchWindowId()
        {
            Window* const window = touchWindow();
            if ((window == nullptr) || (window->getSDLWindow() == nullptr)) {
                return 0;
            }
            return SDL_GetWindowID(window->getSDLWindow());
        }
    } // namespace

    EventManager::EventManager() :

        m_keyfilter(nullptr),
        m_mousestate(0),
        m_mostrecentbtn(MouseEvent::EMPTY),
        m_mouseSensitivity(0.0),
        m_acceleration(false),
        m_warp(false),
        m_enter(false),
        m_oldX(0),
        m_oldY(0),
        m_lastTicks(0),
        m_oldVelocity(0.0),
        m_touchPointerDown(false),
        m_touchLastX(0),
        m_touchLastY(0)
    {
    }

    EventManager::~EventManager() = default;

    void EventManager::applyInputHints()
    {
        // Policy B: FIFE owns touch translation. SDL's synthetic mouse events
        // would double-fire and, on diagonal touchpad pans, could turn a
        // vertical gesture into a horizontal wheel callback.
        SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

        // Never let a real mouse generate synthetic touch feedback either.
        SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

        // Desktop trackpads must keep emitting normal pointer/wheel events;
        // only their FINGER_* stream is translated when the OS provides one.
        SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "0");
    }

    namespace
    {
        template <typename T>
        void addListener(std::deque<T>& vec, T& listener)
        {
            if (!listener->isActive()) {
                listener->setActive(true);
                vec.push_back(listener);
            }
        }

        template <typename T>
        void addListenerFront(std::deque<T>& vec, T& listener)
        {
            if (!listener->isActive()) {
                listener->setActive(true);
                vec.push_front(listener);
            }
        }

        template <typename T>
        void removeListener(std::deque<T>& vec, T& listener)
        {
            if (listener->isActive()) {
                listener->setActive(false);

                auto it = std::find_if(vec.begin(), vec.end(), [&](T const & item) {
                    return item == listener;
                });

                if (it != vec.end()) {
                    vec.erase(it);
                }
            }
        }
    } // namespace

    void EventManager::addCommandListener(ICommandListener* listener)
    {
        addListener<ICommandListener*>(m_commandListeners, listener);
    }

    void EventManager::addCommandListenerFront(ICommandListener* listener)
    {
        addListenerFront<ICommandListener*>(m_commandListeners, listener);
    }

    void EventManager::removeCommandListener(ICommandListener* listener)
    {
        removeListener<ICommandListener*>(m_commandListeners, listener);
    }

    void EventManager::addKeyListener(IKeyListener* listener)
    {
        addListener<IKeyListener*>(m_keyListeners, listener);
    }

    void EventManager::addKeyListenerFront(IKeyListener* listener)
    {
        addListenerFront<IKeyListener*>(m_keyListeners, listener);
    }

    void EventManager::removeKeyListener(IKeyListener* listener)
    {
        removeListener<IKeyListener*>(m_keyListeners, listener);
    }

    void EventManager::addTextListener(ITextListener* listener)
    {
        addListener<ITextListener*>(m_textListeners, listener);
    }

    void EventManager::addTextListenerFront(ITextListener* listener)
    {
        addListenerFront<ITextListener*>(m_textListeners, listener);
    }

    void EventManager::removeTextListener(ITextListener* listener)
    {
        removeListener<ITextListener*>(m_textListeners, listener);
    }

    void EventManager::addMouseListener(IMouseListener* listener)
    {
        addListener<IMouseListener*>(m_mouseListeners, listener);
    }

    void EventManager::addMouseListenerFront(IMouseListener* listener)
    {
        addListenerFront<IMouseListener*>(m_mouseListeners, listener);
    }

    void EventManager::removeMouseListener(IMouseListener* listener)
    {
        removeListener<IMouseListener*>(m_mouseListeners, listener);
    }

    void EventManager::addTouchListener(ITouchListener* listener)
    {
        addListener<ITouchListener*>(m_touchListeners, listener);
    }

    void EventManager::addTouchListenerFront(ITouchListener* listener)
    {
        addListenerFront<ITouchListener*>(m_touchListeners, listener);
    }

    void EventManager::removeTouchListener(ITouchListener* listener)
    {
        removeListener<ITouchListener*>(m_touchListeners, listener);
    }

    void EventManager::addSdlEventListener(ISdlEventListener* listener)
    {
        addListener<ISdlEventListener*>(m_sdleventListeners, listener);
    }

    void EventManager::addSdlEventListenerFront(ISdlEventListener* listener)
    {
        addListenerFront<ISdlEventListener*>(m_sdleventListeners, listener);
    }

    void EventManager::removeSdlEventListener(ISdlEventListener* listener)
    {
        removeListener<ISdlEventListener*>(m_sdleventListeners, listener);
    }

    void EventManager::addDropListener(IDropListener* listener)
    {
        addListener<IDropListener*>(m_dropListeners, listener);
    }

    void EventManager::addDropListenerFront(IDropListener* listener)
    {
        addListenerFront<IDropListener*>(m_dropListeners, listener);
    }

    void EventManager::removeDropListener(IDropListener* listener)
    {
        removeListener<IDropListener*>(m_dropListeners, listener);
    }

    void EventManager::addJoystickListener(IJoystickListener* listener)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->addJoystickListener(listener);
        }
    }

    void EventManager::addJoystickListenerFront(IJoystickListener* listener)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->addJoystickListenerFront(listener);
        }
    }

    void EventManager::removeJoystickListener(IJoystickListener* listener)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->removeJoystickListener(listener);
        }
    }

    void EventManager::dispatchCommand(Command& command)
    {
        std::deque<ICommandListener*> listeners = m_commandListeners;
        auto i                                  = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive()) {
                continue;
            }
            (*i)->onCommand(command);
            if (command.isConsumed()) {
                break;
            }
        }
    }

    void EventManager::dispatchKeyEvent(KeyEvent& evt)
    {
        std::deque<IKeyListener*> listeners = m_keyListeners;
        auto i                              = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive() || (evt.isConsumedByWidgets() && !(*i)->isGlobalListener())) {
                continue;
            }
            switch (evt.getType()) {
            case KeyEvent::UNKNOWN:
                break;
            case KeyEvent::PRESSED:
                (*i)->keyPressed(evt);
                break;
            case KeyEvent::RELEASED:
                (*i)->keyReleased(evt);
                break;
            default:
                break;
            }
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    void EventManager::dispatchTextEvent(TextEvent& evt)
    {
        std::deque<ITextListener*> listeners = m_textListeners;
        auto i                               = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive()) {
                continue;
            }
            switch (evt.getType()) {
            case TextEvent::UNKNOWN:
                break;
            case TextEvent::INPUT:
                (*i)->textInput(evt);
                break;
            case TextEvent::EDIT:
                (*i)->textEdit(evt);
                break;
            default:
                break;
            }
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    void EventManager::dispatchMouseEvent(MouseEvent& evt)
    {
        std::deque<IMouseListener*> listeners = m_mouseListeners;
        auto i                                = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive() || (evt.isConsumedByWidgets() && !(*i)->isGlobalListener())) {
                continue;
            }
            switch (evt.getType()) {
            case MouseEvent::UNKNOWN_EVENT:
                break;
            case MouseEvent::MOVED:
                (*i)->mouseMoved(evt);
                break;
            case MouseEvent::PRESSED:
                (*i)->mousePressed(evt);
                break;
            case MouseEvent::RELEASED:
                (*i)->mouseReleased(evt);
                break;
            case MouseEvent::WHEEL_MOVED_DOWN:
                (*i)->mouseWheelMovedDown(evt);
                break;
            case MouseEvent::WHEEL_MOVED_UP:
                (*i)->mouseWheelMovedUp(evt);
                break;
            case MouseEvent::WHEEL_MOVED_RIGHT:
                (*i)->mouseWheelMovedRight(evt);
                break;
            case MouseEvent::WHEEL_MOVED_LEFT:
                (*i)->mouseWheelMovedLeft(evt);
                break;
            case MouseEvent::CLICKED:
                (*i)->mouseClicked(evt);
                break;
            case MouseEvent::ENTERED:
                (*i)->mouseEntered(evt);
                break;
            case MouseEvent::EXITED:
                (*i)->mouseExited(evt);
                break;
            case MouseEvent::DRAGGED:
                (*i)->mouseDragged(evt);
                break;
            default:
                break;
            }
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    void EventManager::dispatchTouchEvent(TouchEvent& evt)
    {
        std::deque<ITouchListener*> listeners = m_touchListeners;
        auto i                                = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive() || (evt.isConsumedByWidgets() && !(*i)->isGlobalListener())) {
                continue;
            }
            switch (evt.getType()) {
            case TouchEvent::UNKNOWN:
                break;
            case TouchEvent::DOWN:
                (*i)->touchDown(evt);
                break;
            case TouchEvent::UP:
                (*i)->touchUp(evt);
                break;
            case TouchEvent::MOTION:
                (*i)->touchMotion(evt);
                break;
            case TouchEvent::CANCELED:
                (*i)->touchCanceled(evt);
                break;
            case TouchEvent::PINCH_BEGIN:
                (*i)->pinchBegin(evt);
                break;
            case TouchEvent::PINCH_UPDATE:
                (*i)->pinchUpdate(evt);
                break;
            case TouchEvent::PINCH_END:
                (*i)->pinchEnd(evt);
                break;
            default:
                break;
            }
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    bool EventManager::dispatchSdlEvent(SDL_Event& evt)
    {
        bool ret                                 = false;
        std::deque<ISdlEventListener*> listeners = m_sdleventListeners;
        auto i                                   = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive()) {
                continue;
            }
            ret = ret || (*i)->onSdlEvent(evt);
        }
        return ret;
    }

    void EventManager::dispatchDropEvent(DropEvent& evt)
    {
        std::deque<IDropListener*> listeners = m_dropListeners;
        auto i                               = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive()) {
                continue;
            }
            (*i)->fileDropped(evt);
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    bool EventManager::combineEvents(SDL_Event& event1, SDL_Event const & event2)
    {
        if (event1.type == event2.type) {
            switch (event1.type) {
            case SDL_EVENT_MOUSE_MOTION:
                if (event1.motion.state == event2.motion.state) {
                    event1.motion.x = event2.motion.x;
                    event1.motion.y = event2.motion.y;
                    event1.motion.xrel += event2.motion.xrel;
                    event1.motion.yrel += event2.motion.yrel;
                    return true;
                }
                return false;
            default:
                break;
            }
        }
        return false;
    }

    void EventManager::processEvents()
    {
        // The double SDL_PollEvent calls don't throw away events,
        // but try to combine (mouse motion) events.
        SDL_Event event;
        SDL_Event next_event;
        bool has_next_event = SDL_PollEvent(&event);
        while (has_next_event) {
            has_next_event = SDL_PollEvent(&next_event);
            if (has_next_event && combineEvents(event, next_event)) {
                continue;
            }
            switch (event.type) {
            case SDL_EVENT_QUIT: {
                Command cmd;
                cmd.setSource(this);
                cmd.setCommandType(CMD_QUIT_GAME);
                dispatchCommand(cmd);
            } break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
            case SDL_EVENT_WINDOW_MOVED:
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            case SDL_EVENT_WINDOW_MINIMIZED:
            case SDL_EVENT_WINDOW_RESTORED:
            case SDL_EVENT_WINDOW_MAXIMIZED:
            case SDL_EVENT_WINDOW_SHOWN:
            case SDL_EVENT_WINDOW_HIDDEN:
            case SDL_EVENT_WINDOW_EXPOSED:
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                processWindowEvent(event);
                break;

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                processKeyEvent(event);
                break;

            // case SDL_EVENT_TEXT_EDITING: // is buggy with SDL 2.0.1
            case SDL_EVENT_TEXT_INPUT:
                processTextEvent(event);
                break;

            case SDL_EVENT_MOUSE_WHEEL:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                processMouseEvent(event);
                break;

            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
            case SDL_EVENT_FINGER_CANCELED:
            case SDL_EVENT_PINCH_BEGIN:
            case SDL_EVENT_PINCH_UPDATE:
            case SDL_EVENT_PINCH_END:
                processTouchEvent(event);
                break;

            case SDL_EVENT_DROP_FILE:
                processDropEvent(event);
                break;

            case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
            case SDL_EVENT_JOYSTICK_BUTTON_UP:
            case SDL_EVENT_JOYSTICK_AXIS_MOTION:
            case SDL_EVENT_JOYSTICK_HAT_MOTION:
            case SDL_EVENT_JOYSTICK_ADDED:
            case SDL_EVENT_JOYSTICK_REMOVED: {
                if (m_joystickManager != nullptr) {
                    m_joystickManager->processJoystickEvent(event);
                }
                break;
            }
            case SDL_EVENT_DISPLAY_ADDED:
            case SDL_EVENT_DISPLAY_REMOVED:
            case SDL_EVENT_DISPLAY_ORIENTATION:
            case SDL_EVENT_DISPLAY_MOVED:
            case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED:
            case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
                processWindowEvent(event);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
            case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                if (m_joystickManager != nullptr) {
                    m_joystickManager->processControllerEvent(event);
                }
                break;
            }
            default:
                // Surface dropped/unhandled events so future input regressions
                // (for example a new PINCH_* event) are visible.
                FL_DBG(_log(), std::format("Unhandled SDL event type: {}", static_cast<int32_t>(event.type)));
                break;
            }
            if (has_next_event) {
                event = next_event;
            }
        }
    }

    void EventManager::processWindowEvent(SDL_Event event)
    {
        // A lost/hidden window can swallow FINGER_UP/CANCELED; release any
        // synthetic pointer instead of leaving a button stuck down.
        if ((event.type == SDL_EVENT_WINDOW_FOCUS_LOST) || (event.type == SDL_EVENT_WINDOW_HIDDEN) ||
            (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)) {
            cancelTouchGesture();
        }

        if (dispatchSdlEvent(event)) {
            return;
        }

        CommandType ct = CMD_UNKNOWN;
        switch (event.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            ct = CMD_QUIT_GAME;
            break;

        case SDL_EVENT_WINDOW_MOUSE_ENTER:
            ct = CMD_MOUSE_FOCUS_GAINED;
            break;

        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            ct = CMD_MOUSE_FOCUS_LOST;
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
            ct = CMD_INPUT_FOCUS_GAINED;
            break;

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            ct = CMD_INPUT_FOCUS_LOST;
            break;

        case SDL_EVENT_WINDOW_SHOWN:
            ct = CMD_APP_RESTORED;
            break;

        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_HIDDEN:
            ct = CMD_APP_ICONIFIED;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            ct = CMD_WINDOW_RESIZED;
            break;

        case SDL_EVENT_WINDOW_MOVED:
            ct = CMD_WINDOW_MOVED;
            break;

        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            ct = CMD_WINDOW_DPI_CHANGED;
            break;

        default:
            ct = CMD_UNKNOWN;
        }
        if (ct != CMD_UNKNOWN) {
            Command cmd;
            cmd.setCommandType(ct);
            dispatchCommand(cmd);
        }
    }

    void EventManager::processKeyEvent(SDL_Event event)
    {
        KeyEvent keyevt;
        keyevt.setSource(this);
        fillKeyEvent(event, keyevt);
        m_keystatemap[static_cast<int32_t>(keyevt.getKey().getValue())] = (keyevt.getType() == KeyEvent::PRESSED);
        // if event is not filtered it gets dispatched, even it is a function key
        if ((m_keyfilter == nullptr) || !m_keyfilter->isFiltered(keyevt)) {
            if (dispatchSdlEvent(event)) {
                keyevt.consumedByWidgets();
            }
        }

        dispatchKeyEvent(keyevt);
    }

    void EventManager::processTextEvent(SDL_Event event)
    {
        if (dispatchSdlEvent(event)) {
            return;
        }

        TextEvent txtevt;
        txtevt.setSource(this);
        fillTextEvent(event, txtevt);
        dispatchTextEvent(txtevt);
    }

    void EventManager::processMouseEvent(SDL_Event event)
    {
        // Defense in depth: if another library re-enables SDL's touch→mouse
        // synthesis, drop the synthetic duplicate instead of processing it
        // twice (FIFE already translated the finger event).
        if (TouchTranslation::isTouchSynthesizedMouse(event)) {
            FL_DBG(_log(), "Ignoring SDL mouse event synthesized from touch input");
            return;
        }

        if (!TouchTranslation::isMouseEventType(event.type)) {
            FL_DBG(
                _log(),
                std::format("processMouseEvent got non-mouse event type: {}", static_cast<int32_t>(event.type)));
            return;
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION && (!Mathf::Equal(m_mouseSensitivity, 0.0) || m_acceleration)) {
            int32_t tmp_x = static_cast<int32_t>(event.motion.x);
            int32_t tmp_y = static_cast<int32_t>(event.motion.y);
            if (m_enter) {
                m_oldX        = tmp_x;
                m_oldY        = tmp_y;
                m_oldVelocity = 0.0;
                m_enter       = false;
            }

            float modifier = 0.0F;
            if (m_acceleration) {
                uint64_t const ticks = TimeManager::instance()->getTicks64();
                auto difference      = static_cast<float>((ticks - m_lastTicks) + 1);
                m_lastTicks          = ticks;
                auto dx              = static_cast<float>(tmp_x - m_oldX);
                auto dy              = static_cast<float>(tmp_y - m_oldY);
                float const distance = Mathf::Sqrt((dx * dx) + (dy * dy));
                auto acceleration    = (distance / difference) / difference;
                float velocity       = (m_oldVelocity + (acceleration * difference)) / 2;
                velocity             = std::min(velocity, m_mouseSensitivity + 1);
                m_oldVelocity        = velocity;
                modifier             = velocity;
            } else {
                modifier = m_mouseSensitivity;
            }

            int32_t const tmp_xrel = tmp_x - m_oldX;
            int32_t const tmp_yrel = tmp_y - m_oldY;
            if ((tmp_xrel != 0) || (tmp_yrel != 0)) {
                Rect const screen    = RenderBackend::instance()->getArea();
                int32_t const x_fact = static_cast<int32_t>(roundf(static_cast<float>(tmp_xrel) * modifier));
                int32_t const y_fact = static_cast<int32_t>(roundf(static_cast<float>(tmp_yrel) * modifier));
                if ((tmp_x + x_fact) > screen.w) {
                    tmp_x = screen.w;
                } else if ((tmp_x + x_fact) < screen.x) {
                    tmp_x = screen.x;
                } else {
                    tmp_x += x_fact;
                }

                if (tmp_y + y_fact > screen.h) {
                    tmp_y = screen.h;
                } else if ((tmp_y + y_fact) < screen.y) {
                    tmp_y = screen.y;
                } else {
                    tmp_y += y_fact;
                }
                m_oldX         = tmp_x;
                m_oldY         = tmp_y;
                event.motion.x = static_cast<float>(tmp_x);
                event.motion.y = static_cast<float>(tmp_y);

                // don't trigger an event handler when warping
                m_warp                   = true;
                SDL_Window* const window = RenderBackend::exists() ? RenderBackend::instance()->getWindow() : nullptr;
                if (window != nullptr) {
                    SDL_WarpMouseInWindow(window, static_cast<float>(tmp_x), static_cast<float>(tmp_y));
                }
                m_warp = false;
            }
        }

        MouseEvent mouseevt;
        mouseevt.setSource(this);
        fillMouseEvent(event, mouseevt);
        fillModifiers(mouseevt);
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            m_mousestate |= static_cast<int32_t>(mouseevt.getButton());
            m_mostrecentbtn = mouseevt.getButton();
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            m_mousestate &= ~static_cast<int32_t>(mouseevt.getButton());
        }

        if (dispatchSdlEvent(event)) {
            mouseevt.consumedByWidgets();
        }

        dispatchMouseEvent(mouseevt);
    }

    void EventManager::processTouchEvent(SDL_Event const & event)
    {
        // Pinch events carry their own union member and are not finger events.
        switch (event.type) {
        case SDL_EVENT_PINCH_BEGIN:
            m_pinchTracker.begin();
            emitPinchTouchEvent(TouchEvent::PINCH_BEGIN, m_pinchTracker.scale());
            return;
        case SDL_EVENT_PINCH_UPDATE:
            if (m_pinchTracker.update(event.pinch.scale)) {
                emitPinchTouchEvent(TouchEvent::PINCH_UPDATE, m_pinchTracker.scale());
            }
            return;
        case SDL_EVENT_PINCH_END:
            emitPinchTouchEvent(TouchEvent::PINCH_END, m_pinchTracker.scale());
            m_pinchTracker.end();
            return;
        default:
            break;
        }

        SDL_TouchFingerEvent const & tf = event.tfinger;

        // SDL can synthesize touches from the mouse when
        // SDL_HINT_MOUSE_TOUCH_EVENTS=1. That is a feedback path, not new input.
        if (tf.touchID == SDL_MOUSE_TOUCHID) {
            return;
        }

        Window* const window = touchWindow();
        if (window == nullptr) {
            return;
        }

        TouchTranslation::PointI const point = TouchTranslation::normalizedToWindowPoints(
            tf.x, tf.y, window->getWidthInPoints(), window->getHeightInPoints());

        switch (event.type) {
        case SDL_EVENT_FINGER_DOWN: {
            m_touchTracker.setDeviceType(tf.touchID, SDL_GetTouchDeviceType(tf.touchID));
            m_touchTracker.begin(tf.touchID, tf.fingerID, tf.x, tf.y, tf.pressure);
            emitFingerTouchEvent(TouchEvent::DOWN, tf, point);

            // Trackpads already deliver pointer motion/wheel through SDL.
            if (m_touchTracker.isTrackpad(tf.touchID)) {
                break;
            }

            std::size_t const count = m_touchTracker.fingerCount();
            if (count == 1) {
                m_touchPointerDown = true;
                m_touchLastX       = point.x;
                m_touchLastY       = point.y;
                dispatchTouchMouseButton(true, point.x, point.y);
            } else if (m_touchPointerDown) {
                // A second finger turns a one-finger drag into a pan. Finish
                // the synthetic drag at its last position so no button sticks.
                m_touchPointerDown = false;
                dispatchTouchMouseButton(false, m_touchLastX, m_touchLastY);
            }
            break;
        }

        case SDL_EVENT_FINGER_MOTION: {
            m_touchTracker.setDeviceType(tf.touchID, SDL_GetTouchDeviceType(tf.touchID));

            // A missed DOWN must not make the finger invisible forever.
            if (!m_touchTracker.update(tf.touchID, tf.fingerID, tf.x, tf.y, tf.pressure)) {
                m_touchTracker.begin(tf.touchID, tf.fingerID, tf.x, tf.y, tf.pressure);
            }

            std::size_t const count = m_touchTracker.fingerCount(tf.touchID);
            m_touchLastX            = point.x;
            m_touchLastY            = point.y;
            emitFingerTouchEvent(TouchEvent::MOTION, tf, point);

            if (m_touchTracker.isTrackpad(tf.touchID) || (count >= 2)) {
                // Some Linux touchpads deliver scrolling as finger motion
                // instead of SDL_EVENT_MOUSE_WHEEL. Multi-finger motion is
                // also always interpreted as scrolling.
                dispatchTouchScroll(tf.dx, tf.dy, point.x, point.y);
            } else if (count == 1) {
                dispatchTouchMouseMotion(
                    point.x,
                    point.y,
                    tf.dx * static_cast<float>(window->getWidthInPoints()),
                    tf.dy * static_cast<float>(window->getHeightInPoints()),
                    m_touchPointerDown);
            }
            break;
        }

        case SDL_EVENT_FINGER_UP: {
            bool const trackpad = m_touchTracker.isTrackpad(tf.touchID);
            m_touchTracker.end(tf.touchID, tf.fingerID);
            emitFingerTouchEvent(TouchEvent::UP, tf, point);

            if (!trackpad && (m_touchTracker.fingerCount() == 0)) {
                m_touchLastX = point.x;
                m_touchLastY = point.y;
                if (m_touchPointerDown) {
                    m_touchPointerDown = false;
                    dispatchTouchMouseButton(false, point.x, point.y);
                }
            }
            break;
        }

        case SDL_EVENT_FINGER_CANCELED: {
            m_touchTracker.end(tf.touchID, tf.fingerID);
            emitFingerTouchEvent(TouchEvent::CANCELED, tf, point);
            // CANCELED releases the synthetic pointer but must not emit a click.
            cancelTouchGesture();
            break;
        }

        default:
            break;
        }
    }

    void EventManager::emitFingerTouchEvent(
        TouchEvent::TouchEventType type, SDL_TouchFingerEvent const & finger, TouchTranslation::PointI const & point)
    {
        TouchEvent evt;
        evt.setSource(this);
        evt.setType(type);
        evt.setTouchId(static_cast<uint64_t>(finger.touchID));
        evt.setFingerId(static_cast<uint64_t>(finger.fingerID));
        evt.setX(point.x);
        evt.setY(point.y);
        evt.setNormalizedX(finger.x);
        evt.setNormalizedY(finger.y);
        evt.setDeltaX(finger.dx);
        evt.setDeltaY(finger.dy);
        evt.setPressure(finger.pressure);
        evt.setFingerCount(static_cast<int32_t>(m_touchTracker.fingerCount(finger.touchID)));
        evt.setTrackpad(m_touchTracker.isTrackpad(finger.touchID));
        fillModifiers(evt);
        dispatchTouchEvent(evt);
    }

    void EventManager::emitPinchTouchEvent(TouchEvent::TouchEventType type, float scale)
    {
        TouchEvent evt;
        evt.setSource(this);
        evt.setType(type);
        evt.setScale(scale);
        evt.setFingerCount(static_cast<int32_t>(m_touchTracker.fingerCount()));

        float normalizedX = 0.0F;
        float normalizedY = 0.0F;
        if (m_touchTracker.midpoint(normalizedX, normalizedY)) {
            evt.setNormalizedX(normalizedX);
            evt.setNormalizedY(normalizedY);

            Window* const window = touchWindow();
            if (window != nullptr) {
                TouchTranslation::PointI const point = TouchTranslation::normalizedToWindowPoints(
                    normalizedX, normalizedY, window->getWidthInPoints(), window->getHeightInPoints());
                evt.setX(point.x);
                evt.setY(point.y);
            }
        }

        fillModifiers(evt);
        dispatchTouchEvent(evt);
    }

    void EventManager::dispatchTouchMouseButton(bool down, int32_t x, int32_t y)
    {
        MouseEvent mouseevt;
        mouseevt.setSource(this);
        mouseevt.setType(down ? MouseEvent::PRESSED : MouseEvent::RELEASED);
        mouseevt.setButton(MouseEvent::LEFT);
        mouseevt.setX(x);
        mouseevt.setY(y);
        fillModifiers(mouseevt);

        SDL_Event synthetic{};
        synthetic.type            = down ? SDL_EVENT_MOUSE_BUTTON_DOWN : SDL_EVENT_MOUSE_BUTTON_UP;
        synthetic.button.windowID = touchWindowId();
        synthetic.button.which    = SDL_TOUCH_MOUSEID;
        synthetic.button.button   = SDL_BUTTON_LEFT;
        synthetic.button.down     = down;
        synthetic.button.clicks   = 1;
        synthetic.button.x        = static_cast<float>(x);
        synthetic.button.y        = static_cast<float>(y);

        if (dispatchSdlEvent(synthetic)) {
            mouseevt.consumedByWidgets();
        }
        dispatchMouseEvent(mouseevt);
    }

    void EventManager::dispatchTouchMouseMotion(int32_t x, int32_t y, float relX, float relY, bool dragging)
    {
        MouseEvent mouseevt;
        mouseevt.setSource(this);
        mouseevt.setType(dragging ? MouseEvent::DRAGGED : MouseEvent::MOVED);
        mouseevt.setButton(dragging ? MouseEvent::LEFT : MouseEvent::EMPTY);
        mouseevt.setX(x);
        mouseevt.setY(y);
        fillModifiers(mouseevt);

        SDL_Event synthetic{};
        synthetic.type            = SDL_EVENT_MOUSE_MOTION;
        synthetic.motion.windowID = touchWindowId();
        synthetic.motion.which    = SDL_TOUCH_MOUSEID;
        synthetic.motion.state    = dragging ? SDL_BUTTON_LMASK : 0;
        synthetic.motion.x        = static_cast<float>(x);
        synthetic.motion.y        = static_cast<float>(y);
        synthetic.motion.xrel     = relX;
        synthetic.motion.yrel     = relY;

        if (dispatchSdlEvent(synthetic)) {
            mouseevt.consumedByWidgets();
        }
        dispatchMouseEvent(mouseevt);
    }

    void EventManager::dispatchTouchScroll(float deltaX, float deltaY, int32_t x, int32_t y)
    {
        TouchTranslation::ScrollStep const step = TouchTranslation::classifyScroll(deltaX, deltaY);
        MouseEvent::MouseEventType const type   = TouchTranslation::wheelTypeFor(step);
        if (type == MouseEvent::UNKNOWN_EVENT) {
            return;
        }

        MouseEvent mouseevt;
        mouseevt.setSource(this);
        mouseevt.setType(type);
        mouseevt.setButton(MouseEvent::EMPTY);
        mouseevt.setX(x);
        mouseevt.setY(y);
        // Expose SDL's wheel convention: positive X is right, positive Y is up.
        mouseevt.setWheelDeltaX(step.deltaX);
        mouseevt.setWheelDeltaY(-step.deltaY);
        fillModifiers(mouseevt);

        SDL_Event synthetic{};
        synthetic.type            = SDL_EVENT_MOUSE_WHEEL;
        synthetic.wheel.windowID  = touchWindowId();
        synthetic.wheel.which     = SDL_TOUCH_MOUSEID;
        synthetic.wheel.x         = (step.axis == TouchTranslation::ScrollAxis::HORIZONTAL) ? step.deltaX : 0.0F;
        synthetic.wheel.y         = (step.axis == TouchTranslation::ScrollAxis::VERTICAL) ? -step.deltaY : 0.0F;
        synthetic.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
        synthetic.wheel.mouse_x   = static_cast<float>(x);
        synthetic.wheel.mouse_y   = static_cast<float>(y);

        if (dispatchSdlEvent(synthetic)) {
            mouseevt.consumedByWidgets();
        }
        dispatchMouseEvent(mouseevt);
    }

    void EventManager::cancelTouchGesture()
    {
        if (m_touchPointerDown) {
            m_touchPointerDown = false;
            dispatchTouchMouseButton(false, m_touchLastX, m_touchLastY);
        }
        resetTouchState();
    }

    void EventManager::resetTouchState()
    {
        if (m_pinchTracker.isActive()) {
            emitPinchTouchEvent(TouchEvent::PINCH_END, m_pinchTracker.scale());
        }
        m_pinchTracker.end();
        m_touchTracker.clear();
        m_touchPointerDown = false;
        m_touchLastX       = 0;
        m_touchLastY       = 0;
    }

    void EventManager::processDropEvent(SDL_Event event)
    {
        // only dispatched as DropEvent
        // if (dispatchSdlEvent(event)) {
        //	return;
        //}

        std::string const path(event.drop.data);

        DropEvent drop;
        drop.setPath(path);
        drop.setSource(this);
        dispatchDropEvent(drop);
    }

    void EventManager::fillMouseEvent(SDL_Event const & sdlevt, MouseEvent& mouseevt)
    {
        if (m_warp) {
            return;
        }

        // Never read a union member that does not belong to the event type.
        // This is the guard that keeps a stray finger event out of the mouse
        // path (and out of sdlevt.button.*).
        if (!TouchTranslation::isMouseEventType(sdlevt.type)) {
            FL_DBG(
                _log(),
                std::format("fillMouseEvent ignored non-mouse event type: {}", static_cast<int32_t>(sdlevt.type)));
            return;
        }

        float pointerX = 0.0F;
        float pointerY = 0.0F;
        switch (sdlevt.type) {
        case SDL_EVENT_MOUSE_MOTION:
            pointerX = sdlevt.motion.x;
            pointerY = sdlevt.motion.y;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            pointerX = sdlevt.button.x;
            pointerY = sdlevt.button.y;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            pointerX = sdlevt.wheel.mouse_x;
            pointerY = sdlevt.wheel.mouse_y;
            break;
        default:
            return;
        }

        // Any backend with no FIFE Window attached (for example the headless
        // dummy backend) has no window to derive the virtual screen size from.
        // Fall back to the backend's configured dimensions so synthetic events
        // are normalized to the same coordinate space the widgets live in.
        // The windowed path is deliberately left untouched.
        if (RenderBackend::exists()) {
            RenderBackend* const backend = RenderBackend::instance();
            if (backend->getWindowObject() == nullptr) {
                uint32_t const width  = backend->getWidth();
                uint32_t const height = backend->getHeight();
                if ((width > 0U) && (height > 0U)) {
                    pointerX = std::clamp(pointerX, 0.0F, static_cast<float>(width - 1U));
                    pointerY = std::clamp(pointerY, 0.0F, static_cast<float>(height - 1U));
                }
            }
        }

        mouseevt.setX(static_cast<int32_t>(pointerX));
        mouseevt.setY(static_cast<int32_t>(pointerY));

        mouseevt.setButton(MouseEvent::EMPTY);
        mouseevt.setType(MouseEvent::MOVED);
        if ((sdlevt.type == SDL_EVENT_MOUSE_BUTTON_UP) || (sdlevt.type == SDL_EVENT_MOUSE_BUTTON_DOWN)) {
            switch (sdlevt.button.button) {
            case SDL_BUTTON_LEFT:
                mouseevt.setButton(MouseEvent::LEFT);
                break;
            case SDL_BUTTON_RIGHT:
                mouseevt.setButton(MouseEvent::RIGHT);
                break;
            case SDL_BUTTON_MIDDLE:
                mouseevt.setButton(MouseEvent::MIDDLE);
                break;
            case SDL_BUTTON_X1:
                mouseevt.setButton(MouseEvent::X1);
                break;
            case SDL_BUTTON_X2:
                mouseevt.setButton(MouseEvent::X2);
                break;
            default:
                mouseevt.setButton(MouseEvent::UNKNOWN_BUTTON);
                break;
            }

            if (sdlevt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouseevt.setType(MouseEvent::RELEASED);
            } else {
                mouseevt.setType(MouseEvent::PRESSED);
            }
        }

        if (sdlevt.type == SDL_EVENT_MOUSE_WHEEL) {
            float wheelX = sdlevt.wheel.x;
            float wheelY = sdlevt.wheel.y;
            if (sdlevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                wheelX = -wheelX;
                wheelY = -wheelY;
            }

            mouseevt.setWheelDeltaX(wheelX);
            mouseevt.setWheelDeltaY(wheelY);

            // Dominant-axis arbitration: horizontal must not overwrite a
            // vertical wheel event. Ties stay vertical.
            float const absX = std::abs(wheelX);
            float const absY = std::abs(wheelY);
            if ((absY != 0.0F) && (absY >= absX)) {
                mouseevt.setType((wheelY > 0.0F) ? MouseEvent::WHEEL_MOVED_UP : MouseEvent::WHEEL_MOVED_DOWN);
            } else if (absX != 0.0F) {
                mouseevt.setType((wheelX > 0.0F) ? MouseEvent::WHEEL_MOVED_RIGHT : MouseEvent::WHEEL_MOVED_LEFT);
            }
        }

        if ((mouseevt.getType() == MouseEvent::MOVED) && ((m_mousestate & m_mostrecentbtn) != 0)) {
            mouseevt.setType(MouseEvent::DRAGGED);
            mouseevt.setButton(m_mostrecentbtn);
        }
    }

    void EventManager::fillKeyEvent(SDL_Event const & sdlevt, KeyEvent& keyevt)
    {
        if (sdlevt.type == SDL_EVENT_KEY_DOWN) {
            keyevt.setType(KeyEvent::PRESSED);
        } else if (sdlevt.type == SDL_EVENT_KEY_UP) {
            keyevt.setType(KeyEvent::RELEASED);
        } else {
            // FL_WARN(
            // _log(),
            // std::format(
            // "fillKeyEvent() Invalid key event type of {}.  Ignoring event.", static_cast<int>(sdlevt.type)));
            return;
        }

        keyevt.setShiftPressed((sdlevt.key.mod & SDL_KMOD_SHIFT) != 0);
        keyevt.setControlPressed((sdlevt.key.mod & SDL_KMOD_CTRL) != 0);
        keyevt.setAltPressed((sdlevt.key.mod & SDL_KMOD_ALT) != 0);
        keyevt.setMetaPressed((sdlevt.key.mod & SDL_KMOD_GUI) != 0); // currently gui/super keys
        keyevt.setNumericPad((sdlevt.key.mod & SDL_KMOD_NUM) != 0);
        keyevt.setKey(Key(static_cast<Key::KeyType>(sdlevt.key.key)));
    }

    void EventManager::fillTextEvent(SDL_Event const & sdlevt, TextEvent& txtevt)
    {
        if (sdlevt.type == SDL_EVENT_TEXT_INPUT) {
            txtevt.setType(TextEvent::INPUT);
            Text const t(sdlevt.text.text);
            txtevt.setText(t);
        } else if (sdlevt.type == SDL_EVENT_TEXT_EDITING) {
            txtevt.setType(TextEvent::EDIT);
            Text const t(sdlevt.edit.text, sdlevt.edit.start, sdlevt.edit.length);
            txtevt.setText(t);
        } else {
            // FL_WARN(
            // _log(),
            // std::format(
            // "fillTextEvent() Invalid text event type of {}.  Ignoring event.", static_cast<int>(sdlevt.type)));
        }
    }

    void EventManager::fillModifiers(InputEvent& evt)
    {
        evt.setAltPressed(
            (static_cast<int>(m_keystatemap[Keys::MODE]) | static_cast<int>(m_keystatemap[Keys::LALT]) |
             static_cast<int>(m_keystatemap[Keys::RALT])) != 0);
        evt.setControlPressed(
            (static_cast<int>(m_keystatemap[Keys::LCTRL]) | static_cast<int>(m_keystatemap[Keys::RCTRL])) != 0);
        evt.setShiftPressed(
            (static_cast<int>(m_keystatemap[Keys::LSHIFT]) | static_cast<int>(m_keystatemap[Keys::RSHIFT])) != 0);
    }

    EventSourceType EventManager::getEventSourceType()
    {
        return ES_ENGINE;
    }

    void EventManager::setKeyFilter(IKeyFilter* keyFilter)
    {
        m_keyfilter = keyFilter;
    }

    void EventManager::setMouseSensitivity(float sensitivity)
    {
        if (sensitivity < -0.99F) {
            sensitivity = -0.99F;
        } else if (sensitivity > 10.0F) {
            sensitivity = 10.0F;
        }
        m_mouseSensitivity = sensitivity;
    }

    float EventManager::getMouseSensitivity() const
    {
        return m_mouseSensitivity;
    }

    void EventManager::setMouseAccelerationEnabled(bool acceleration)
    {
        m_acceleration = acceleration;
    }

    bool EventManager::isMouseAccelerationEnabled() const
    {
        return m_acceleration;
    }

    bool EventManager::isClipboardText() const
    {
        return SDL_HasClipboardText();
    }

    std::string EventManager::getClipboardText() const
    {
        std::string text;
        if (SDL_HasClipboardText()) {
            auto clipboard = std::unique_ptr<char, decltype(&SDL_free)>(SDL_GetClipboardText(), &SDL_free);
            if (clipboard) {
                text = std::string(clipboard.get());
            }
        }
        return text;
    }

    void EventManager::setClipboardText(std::string const & text)
    {
        SDL_SetClipboardText(text.c_str());
    }

    void EventManager::setJoystickSupport(bool support)
    {
        if (support && (m_joystickManager == nullptr)) {
            m_joystickManager = std::make_unique<JoystickManager>();
        } else if (!support && (m_joystickManager != nullptr)) {
            m_joystickManager = nullptr;
        }
    }

    bool EventManager::isKeyPressed(Key::KeyType key) const
    {
        auto it = m_keystatemap.find(static_cast<int32_t>(key));
        return it != m_keystatemap.end() && it->second;
    }

    Joystick* EventManager::getJoystick(int32_t instanceId)
    {
        if (m_joystickManager != nullptr) {
            return m_joystickManager->getJoystick(instanceId);
        }
        return nullptr;
    }

    uint8_t EventManager::getJoystickCount() const
    {
        if (m_joystickManager != nullptr) {
            return m_joystickManager->getJoystickCount();
        }
        return 0;
    }

    void EventManager::loadGamepadMapping(std::string const & file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->loadMapping(file);
        }
    }

    void EventManager::saveGamepadMapping(std::string const & guid, std::string const & file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->saveMapping(guid, file);
        }
    }

    void EventManager::saveGamepadMappings(std::string const & file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->saveMappings(file);
        }
    }

    std::string EventManager::getGamepadStringMapping(std::string const & guid)
    {
        std::string mapping;
        if (m_joystickManager != nullptr) {
            mapping = m_joystickManager->getStringMapping(guid);
        }
        return mapping;
    }

    void EventManager::setGamepadStringMapping(std::string const & mapping)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->setStringMapping(mapping);
        }
    }
} // namespace FIFE
