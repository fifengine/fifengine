// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "fifechanmanager.h"

// Standard C++ library includes
#include <cassert>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// OpenGL - include GLEW first before any other OpenGL headers
#ifdef HAVE_OPENGL
    #include <GL/glew.h>
#endif

// 3rd party library includes
#include <fifechan/backends/sdl3/sdl.hpp>
#include <fifechan/focushandler.hpp>
#include <fifechan/key.hpp>

#include <fifechan.hpp>

// FIFE includes
#ifdef HAVE_OPENGL
    #include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#endif
#include "eventchannel/key/key.h"
#include "eventchannel/key/keyevent.h"
#include "eventchannel/mouse/mouseevent.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "gui/fifechan/console/console.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/filesystem.h"
#include "video/fonts/fontinstanceifontadapter.h"
#include "video/fonts/fontmanager.h"
#include "video/renderbackend.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_GUI);
            return log;
        }
    } // namespace

    FifechanManager::FifechanManager() :
        m_fcn_gui(std::make_unique<fcn::Gui>()),
        m_gui_graphics(nullptr),
        m_focushandler(nullptr),
        m_fcn_topcontainer(std::make_unique<fcn::Container>()),
        m_imgloader(std::make_unique<GuiImageLoader>()),
        m_input(std::make_unique<fcn::sdl3::Input>()),
        m_console(nullptr),
        m_cursor(nullptr),
        m_defaultfont(nullptr),
        m_had_mouse(false),
        m_had_widget(false),
        m_lastMotionX(0),
        m_lastMotionY(0),
        m_fontsize(0),
        m_logic_executed(false),
        m_enabled_console(true)
    {

        m_fcn_gui->setInput(m_input.get());
        fcn::Image::setImageLoader(m_imgloader.get());

        m_fcn_gui->setTop(m_fcn_topcontainer.get());
        m_focushandler = m_fcn_topcontainer->_getFocusHandler();

        m_fcn_topcontainer->setOpaque(false);
        m_fcn_topcontainer->setFocusable(false);
    }

    FifechanManager::~FifechanManager() = default;

    bool FifechanManager::onSdlEvent(SDL_Event& evt)
    {
        if (m_input == nullptr) {
            FL_WARN(_log(), "FifechanManager, FifechanGUI->getInput == 0 ... discarding events!");
            return false;
        }

        bool const overWidget = m_fcn_topcontainer->getWidgetAt(m_lastMotionX, m_lastMotionY) !=
                                nullptr; // NOLINT(cppcoreguidelines-init-variables)

        switch (evt.type) {
        case SDL_EVENT_MOUSE_WHEEL:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            m_had_widget = overWidget;
            [[fallthrough]];
        case SDL_EVENT_MOUSE_BUTTON_UP:
            // Always send the button up/down events to fifechan
            m_input->pushInput(evt);

            // Button was pressed over a widget and still is over a widget
            // so we mark the event as processed.
            if (m_had_widget && overWidget) {
                return true;
            }

            // Button wasn't pressed over a widget so we want to release focus
            // no matter what.
            if (!m_had_widget) {
                m_focushandler->focusNone();
            }

            // Button up was processed by fifechan but there was no widget under
            // the mouse at the time.  Don't mark it as processed here so the
            // other listeners have a chance to process the event.
            return false;

        case SDL_EVENT_MOUSE_MOTION:
            m_lastMotionX = static_cast<int32_t>(evt.motion.x);
            m_lastMotionY = static_cast<int32_t>(evt.motion.y);
            if (m_fcn_topcontainer->getWidgetAt(static_cast<int>(evt.motion.x), static_cast<int>(evt.motion.y)) !=
                nullptr) {
                m_had_mouse = true;
                m_input->pushInput(evt);
                return true;
            }
            if (m_had_mouse) {
                // We only keep the mouse if a widget/window has requested
                // dragging.
                m_had_mouse = m_focushandler->getDraggedWidget() != nullptr;
                m_input->pushInput(evt);
                return true;
            }
            return false;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            if (m_focushandler->getFocused() != nullptr) {
                m_input->pushInput(evt);
                return true;
            }
            return false;

        case SDL_EVENT_TEXT_INPUT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            // don't consume TEXTINPUT / WINDOWEVENTS
            m_input->pushInput(evt);
            return false;

        default:
            return false;
        }
    }

    void FifechanManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        if (m_backend == "SDL") { // NOLINT(bugprone-branch-clone)
            dynamic_cast<SdlGuiGraphics*>(m_gui_graphics.get())->updateTarget();
        } else {
            dynamic_cast<OpenGLGuiGraphics*>(m_gui_graphics.get())->updateTarget();
        }
        m_fcn_topcontainer->setDimension(
            fcn::Rectangle(
                static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height)));
        invalidateFonts();
        if (m_console != nullptr) {
            m_console->reLayout();
        }
    }

    fcn::Gui* FifechanManager::getFifechanGUI() const
    {
        return m_fcn_gui.get();
    }

    void FifechanManager::add(fcn::Widget* widget)
    {
        if (!m_widgets.contains(widget)) {
            m_fcn_topcontainer->add(widget);
            m_widgets.insert(widget);
        }
    }

    void FifechanManager::remove(fcn::Widget* widget)
    {
        if (m_widgets.contains(widget)) {
            m_widgets.erase(widget);
            m_fcn_topcontainer->remove(widget);
        }
    }

    void FifechanManager::setConsoleEnabled(bool console)
    {
        m_enabled_console = console;
    }

    bool FifechanManager::isConsoleEnabled() const
    {
        return m_enabled_console;
    }

    void FifechanManager::init(std::string const & backend, int32_t screenWidth, int32_t screenHeight)
    {
#ifdef HAVE_OPENGL
        if (backend == "SDL") {
            m_gui_graphics = std::make_unique<SdlGuiGraphics>();
        } else if (backend == "OpenGL") {
            m_gui_graphics = std::make_unique<OpenGLGuiGraphics>();
        } else {
            // should never get here
            assert(0);
        }
#else
        if (backend == "SDL") {
            m_gui_graphics = std::make_unique<SdlGuiGraphics>();
        } else {
            // should never get here
            assert(0);
        }
#endif
        m_backend = backend;

        m_fcn_gui->setGraphics(m_gui_graphics.get());
        if (m_enabled_console) {
            m_console = std::make_unique<Console>();
        }

        resizeTopContainer(0, 0, static_cast<uint32_t>(screenWidth), static_cast<uint32_t>(screenHeight));
    }

    fcn::Font* FifechanManager::createFont(std::string const & path, uint32_t size)
    {
        std::string fontpath   = path;
        int32_t const fontsize = static_cast<int32_t>(size);

        if (fontpath.empty()) {
            fontpath = m_fontpath;
        }

        FL_WARN(
            _log(),
            std::format(
                "createFont: path='{}' fontpath='{}' size={} fontsize={} m_fontManager={}",
                path,
                fontpath,
                size,
                fontsize,
                static_cast<void*>(m_fontManager)));

        FontHandle const handle = m_fontManager->getFontHandle(fontpath, fontsize);
        auto instance           = m_fontManager->getFontInstance(handle);
        auto adapter            = std::make_unique<FontInstanceIFontAdapter>(std::move(instance));
        if (auto definition = m_fontManager->getFontDefinition(handle)) {
            adapter->setAntiAlias(definition->antialias);
            adapter->setGlyphSpacing(definition->glyphSpacing);
            adapter->setRowSpacing(definition->rowSpacing);
            adapter->setDynamicColoring(definition->recoloring);
            adapter->setColor(definition->color.r, definition->color.g, definition->color.b, definition->color.a);
        }
        auto* ptr = adapter.get();
        m_fonts.push_back(std::move(adapter));
        return ptr;
    }

    void FifechanManager::releaseFont(fcn::Font const * font)
    {
        auto i = m_fonts.begin();
        while (i != m_fonts.end()) {
            if (i->get() == font) {
                m_fonts.erase(i);
                return;
            }
            ++i;
        }
    }

    void FifechanManager::invalidateFonts()
    {
        auto it = m_fonts.begin();
        while (it != m_fonts.end()) {
            auto* ifont = dynamic_cast<IFont*>(it->get());
            if (ifont != nullptr) {
                ifont->invalidate();
            }
            ++it;
        }
    }

    fcn::Font* FifechanManager::setDefaultFont(std::string const & path, uint32_t size)
    {
        m_fontpath = path;
        m_fontsize = static_cast<int32_t>(size);

        m_defaultfont = createFont();
        fcn::Widget::setGlobalFont(m_defaultfont);
        if (m_console != nullptr) {
            m_console->setIOFont(m_defaultfont);
            m_console->reLayout();
        }

        return m_defaultfont;
    }

    void FifechanManager::turn()
    {
        if (!m_logic_executed) {
            m_fcn_gui->logic();
        }
        m_logic_executed = false;
        m_fcn_gui->draw();
    }

    KeyEvent FifechanManager::translateKeyEvent(fcn::KeyEvent const & fcnevt)
    {
        KeyEvent keyevt;
        if (fcnevt.getType() == fcn::KeyEvent::Type::Pressed) {
            keyevt.setType(KeyEvent::PRESSED);
        } else if (fcnevt.getType() == fcn::KeyEvent::Type::Released) {
            keyevt.setType(KeyEvent::RELEASED);
        } else {
            FL_WARN(
                _log(),
                std::format(
                    "FifechanManager::translateKeyEvent() - Unknown event type: {}",
                    static_cast<int>(fcnevt.getType())));
            keyevt.setType(KeyEvent::UNKNOWN);
        }
        keyevt.setShiftPressed(fcnevt.isShiftPressed());
        keyevt.setControlPressed(fcnevt.isControlPressed());
        keyevt.setAltPressed(fcnevt.isAltPressed());
        keyevt.setMetaPressed(fcnevt.isMetaPressed());
        keyevt.setNumericPad(fcnevt.isNumericPad());

        // Convert from fifechan keyval to FIFE keyval
        int32_t keyval = fcnevt.getKey().getValue();
        keyval         = convertFifechanKeyToFifeKey(keyval);

        keyevt.setKey(Key(static_cast<Key::KeyType>(keyval)));

        return keyevt;
    }

    MouseEvent FifechanManager::translateMouseEvent(fcn::MouseEvent const & fcnevt)
    {
        MouseEvent mouseevt;
        mouseevt.setShiftPressed(fcnevt.isShiftPressed());
        mouseevt.setControlPressed(fcnevt.isControlPressed());
        mouseevt.setAltPressed(fcnevt.isAltPressed());
        mouseevt.setMetaPressed(fcnevt.isMetaPressed());
        mouseevt.setX(fcnevt.getX());
        mouseevt.setY(fcnevt.getY());

        switch (fcnevt.getType()) {
        case fcn::MouseEvent::Type::Pressed:
            mouseevt.setType(MouseEvent::PRESSED);
            break;
        case fcn::MouseEvent::Type::Released:
            mouseevt.setType(MouseEvent::RELEASED);
            break;
        case fcn::MouseEvent::Type::Moved:
            mouseevt.setType(MouseEvent::MOVED);
            break;
        case fcn::MouseEvent::Type::Clicked:
            mouseevt.setType(MouseEvent::CLICKED);
            break;
        case fcn::MouseEvent::Type::Entered:
            mouseevt.setType(MouseEvent::ENTERED);
            break;
        case fcn::MouseEvent::Type::Exited:
            mouseevt.setType(MouseEvent::EXITED);
            break;
        case fcn::MouseEvent::Type::Dragged:
            mouseevt.setType(MouseEvent::DRAGGED);
            break;
        case fcn::MouseEvent::Type::WheelMovedDown:
            mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
            break;
        case fcn::MouseEvent::Type::WheelMovedUp:
            mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
            break;
        case fcn::MouseEvent::Type::WheelMovedRight:
            mouseevt.setType(MouseEvent::WHEEL_MOVED_RIGHT);
            break;
        case fcn::MouseEvent::Type::WheelMovedLeft:
            mouseevt.setType(MouseEvent::WHEEL_MOVED_LEFT);
            break;
        default:
            mouseevt.setType(MouseEvent::UNKNOWN_EVENT);
        }

        switch (fcnevt.getButton()) {
        case fcn::MouseEvent::Button::Left:
            mouseevt.setButton(MouseEvent::LEFT);
            break;
        case fcn::MouseEvent::Button::Right:
            mouseevt.setButton(MouseEvent::RIGHT);
            break;
        case fcn::MouseEvent::Button::Middle:
            mouseevt.setButton(MouseEvent::MIDDLE);
            break;
        case fcn::MouseEvent::Button::X1:
            mouseevt.setButton(MouseEvent::X1);
            break;
        case fcn::MouseEvent::Button::X2:
            mouseevt.setButton(MouseEvent::X2);
            break;
        case fcn::MouseEvent::Button::Empty:
            mouseevt.setButton(MouseEvent::EMPTY);
            break;
        default:
            mouseevt.setButton(MouseEvent::UNKNOWN_BUTTON);
            break;
        }
        return mouseevt;
    }

    void FifechanManager::setTabbingEnabled(bool tabbing)
    {
        m_fcn_gui->setTabbingEnabled(tabbing);
    }

    bool FifechanManager::isTabbingEnabled() const
    {
        return m_fcn_gui->isTabbingEnabled();
    }

    int32_t FifechanManager::convertFifechanKeyToFifeKey(int32_t value)
    {
        // Both fifechan and fifengine now use the same SDL3 keycodes 1:1.
        return value;
    }
} // namespace FIFE
