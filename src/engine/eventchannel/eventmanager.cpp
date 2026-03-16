// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <deque>
#include <iostream>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "eventchannel/command/command.h"
#include "eventchannel/joystick/joystickmanager.h"
#include "eventchannel/key/ikeyfilter.h"
#include "eventchannel/key/key.h"
#include "eventchannel/key/keyevent.h"
#include "eventchannel/mouse/mouseevent.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/renderbackend.h"

#include "eventmanager.h"

namespace FIFE
{
    static Logger _log(LM_EVTCHANNEL);

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
        m_joystickManager(nullptr)
    {
    }

    EventManager::~EventManager()
    {
        delete m_joystickManager;
    }

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

            auto it = std::find_if(vec.begin(), vec.end(), [&](const T& item) {
                return item == listener;
            });

            if (it != vec.end()) {
                vec.erase(it);
            }
        }
    }

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

    bool EventManager::combineEvents(SDL_Event& event1, const SDL_Event& event2)
    {
        if (event1.type == event2.type) {
            switch (event1.type) {
            case SDL_MOUSEMOTION:
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
        bool has_next_event = (SDL_PollEvent(&event) != 0);
        while (has_next_event) {
            has_next_event = (SDL_PollEvent(&next_event) != 0);
            if (has_next_event && combineEvents(event, next_event)) {
                continue;
            }
            switch (event.type) {
            case SDL_QUIT: {
                Command cmd;
                cmd.setSource(this);
                cmd.setCommandType(CMD_QUIT_GAME);
                dispatchCommand(cmd);
            } break;

            case SDL_WINDOWEVENT:
                processWindowEvent(event);
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                processKeyEvent(event);
                break;

            // case SDL_TEXTEDITING: // is buggy with SDL 2.0.1
            case SDL_TEXTINPUT:
                processTextEvent(event);
                break;

            case SDL_MOUSEWHEEL:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
                processMouseEvent(event);
                break;

            case SDL_DROPFILE:
                processDropEvent(event);
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            case SDL_JOYAXISMOTION:
            case SDL_JOYHATMOTION:
            case SDL_JOYDEVICEADDED:
            case SDL_JOYDEVICEREMOVED: {
                if (m_joystickManager != nullptr) {
                    m_joystickManager->processJoystickEvent(event);
                }
                break;
            }
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERAXISMOTION: {
                if (m_joystickManager != nullptr) {
                    m_joystickManager->processControllerEvent(event);
                }
                break;
            }
            default:
                break;
            }
            if (has_next_event) {
                event = next_event;
            }
        }
    }

    void EventManager::processWindowEvent(SDL_Event event)
    {
        if (dispatchSdlEvent(event)) {
            return;
        }

        CommandType ct = CMD_UNKNOWN;
        switch (event.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            ct = CMD_QUIT_GAME;
            break;

        case SDL_WINDOWEVENT_ENTER:
            ct = CMD_MOUSE_FOCUS_GAINED;
            break;

        case SDL_WINDOWEVENT_LEAVE:
            ct = CMD_MOUSE_FOCUS_LOST;
            break;

        case SDL_WINDOWEVENT_FOCUS_GAINED:
            ct = CMD_INPUT_FOCUS_GAINED;
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            ct = CMD_INPUT_FOCUS_LOST;
            break;

        case SDL_WINDOWEVENT_SHOWN:
            ct = CMD_APP_RESTORED;
            break;

        case SDL_WINDOWEVENT_MINIMIZED:
        case SDL_WINDOWEVENT_HIDDEN:
            ct = CMD_APP_ICONIFIED;
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
        m_keystatemap[keyevt.getKey().getValue()] = (keyevt.getType() == KeyEvent::PRESSED);
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
        if (event.type == SDL_MOUSEMOTION && (!Mathf::Equal(m_mouseSensitivity, 0.0) || m_acceleration)) {
            uint16_t tmp_x = event.motion.x;
            uint16_t tmp_y = event.motion.y;
            if (m_enter) {
                m_oldX        = tmp_x;
                m_oldY        = tmp_y;
                m_oldVelocity = 0.0;
                m_enter       = false;
            }

            float modifier = 0.0f;
            if (m_acceleration) {
                uint32_t ticks    = SDL_GetTicks();
                auto difference   = static_cast<float>((ticks - m_lastTicks) + 1);
                m_lastTicks       = ticks;
                auto dx           = static_cast<float>(tmp_x - m_oldX);
                auto dy           = static_cast<float>(tmp_y - m_oldY);
                float distance    = Mathf::Sqrt((dx * dx) + (dy * dy));
                auto acceleration = (distance / difference) / difference;
                float velocity    = (m_oldVelocity + acceleration * difference) / 2;
                if (velocity > m_mouseSensitivity + 1) {
                    velocity = m_mouseSensitivity + 1;
                }
                m_oldVelocity = velocity;
                modifier      = velocity;
            } else {
                modifier = m_mouseSensitivity;
            }

            auto tmp_xrel = static_cast<int16_t>(tmp_x - m_oldX);
            auto tmp_yrel = static_cast<int16_t>(tmp_y - m_oldY);
            if ((tmp_xrel != 0) || (tmp_yrel != 0)) {
                Rect screen = RenderBackend::instance()->getArea();
                auto x_fact = static_cast<int16_t>(round(static_cast<float>(tmp_xrel * modifier)));
                auto y_fact = static_cast<int16_t>(round(static_cast<float>(tmp_yrel * modifier)));
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
                event.motion.x = tmp_x;
                event.motion.y = tmp_y;
                m_warp         = true; // don't trigger an event handler when warping
                SDL_WarpMouseInWindow(RenderBackend::instance()->getWindow(), tmp_x, tmp_y);
                m_warp = false;
            }
        }

        MouseEvent mouseevt;
        mouseevt.setSource(this);
        fillMouseEvent(event, mouseevt);
        fillModifiers(mouseevt);
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            m_mousestate |= static_cast<int32_t>(mouseevt.getButton());
            m_mostrecentbtn = mouseevt.getButton();
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            m_mousestate &= ~static_cast<int32_t>(mouseevt.getButton());
        }

        if (dispatchSdlEvent(event)) {
            mouseevt.consumedByWidgets();
        }

        dispatchMouseEvent(mouseevt);
    }

    void EventManager::processDropEvent(SDL_Event event)
    {
        // only dispatched as DropEvent
        // if (dispatchSdlEvent(event)) {
        //	return;
        //}

        char* tmp = event.drop.file;
        std::string path(tmp);
        SDL_free(tmp);

        DropEvent drop;
        drop.setPath(path);
        drop.setSource(this);
        dispatchDropEvent(drop);
    }

    void EventManager::fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt)
    {
        if (m_warp) {
            return;
        }

        mouseevt.setX(sdlevt.button.x);
        mouseevt.setY(sdlevt.button.y);

        mouseevt.setButton(MouseEvent::EMPTY);
        mouseevt.setType(MouseEvent::MOVED);
        if ((sdlevt.type == SDL_MOUSEBUTTONUP) || (sdlevt.type == SDL_MOUSEBUTTONDOWN)) {
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

            if (sdlevt.button.state == SDL_RELEASED) {
                mouseevt.setType(MouseEvent::RELEASED);
            } else {
                mouseevt.setType(MouseEvent::PRESSED);
            }
        }
        if (sdlevt.type == SDL_MOUSEWHEEL) {
#if SDL_VERSION_ATLEAST(2, 0, 4)
            if (sdlevt.wheel.y > 0 || (sdlevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && sdlevt.wheel.y < 0)) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
            } else if (sdlevt.wheel.y < 0 || (sdlevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && sdlevt.wheel.y > 0)) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
            }
            if (sdlevt.wheel.x > 0 || (sdlevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && sdlevt.wheel.x < 0)) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_RIGHT);
            } else if (sdlevt.wheel.x < 0 || (sdlevt.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && sdlevt.wheel.x > 0)) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_LEFT);
            }
#else
            if (sdlevt.wheel.y > 0) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
            } else if (sdlevt.wheel.y < 0) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
            }
            if (sdlevt.wheel.x > 0) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_RIGHT);
            } else if (sdlevt.wheel.x < 0) {
                mouseevt.setType(MouseEvent::WHEEL_MOVED_LEFT);
            }
#endif
        }

        if ((mouseevt.getType() == MouseEvent::MOVED) && ((m_mousestate & m_mostrecentbtn) != 0)) {
            mouseevt.setType(MouseEvent::DRAGGED);
            mouseevt.setButton(m_mostrecentbtn);
        }
    }

    void EventManager::fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt)
    {
        if (sdlevt.type == SDL_KEYDOWN) {
            keyevt.setType(KeyEvent::PRESSED);
        } else if (sdlevt.type == SDL_KEYUP) {
            keyevt.setType(KeyEvent::RELEASED);
        } else {
            FL_WARN(
                _log, LMsg("fillKeyEvent()") << " Invalid key event type of " << sdlevt.type << ".  Ignoring event.");
            return;
        }

        SDL_Keysym keysym = sdlevt.key.keysym;
        keyevt.setShiftPressed((keysym.mod & KMOD_SHIFT) != 0);
        keyevt.setControlPressed((keysym.mod & KMOD_CTRL) != 0);
        keyevt.setAltPressed((keysym.mod & KMOD_ALT) != 0);
        keyevt.setMetaPressed((keysym.mod & KMOD_GUI) != 0); // currently gui/super keys
        keyevt.setNumericPad((keysym.mod & KMOD_NUM) != 0);
        keyevt.setKey(Key(static_cast<Key::KeyType>(keysym.sym)));
    }

    void EventManager::fillTextEvent(const SDL_Event& sdlevt, TextEvent& txtevt)
    {
        if (sdlevt.type == SDL_TEXTINPUT) {
            txtevt.setType(TextEvent::INPUT);
            Text t(sdlevt.text.text);
            txtevt.setText(t);
        } else if (sdlevt.type == SDL_TEXTEDITING) {
            txtevt.setType(TextEvent::EDIT);
            Text t(sdlevt.edit.text, sdlevt.edit.start, sdlevt.edit.length);
            txtevt.setText(t);
        } else {
            FL_WARN(
                _log, LMsg("fillTextEvent()") << " Invalid text event type of " << sdlevt.type << ".  Ignoring event.");
        }
    }

    void EventManager::fillModifiers(InputEvent& evt)
    {
        evt.setAltPressed(
            (static_cast<int>(m_keystatemap[Key::ALT_GR]) | static_cast<int>(m_keystatemap[Key::LEFT_ALT]) |
             static_cast<int>(m_keystatemap[Key::RIGHT_ALT])) != 0);
        evt.setControlPressed(
            (static_cast<int>(m_keystatemap[Key::LEFT_CONTROL]) |
             static_cast<int>(m_keystatemap[Key::RIGHT_CONTROL])) != 0);
        evt.setShiftPressed(
            (static_cast<int>(m_keystatemap[Key::LEFT_SHIFT]) | static_cast<int>(m_keystatemap[Key::RIGHT_SHIFT])) !=
            0);
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
        if (sensitivity < -0.99) {
            sensitivity = -0.99;
        } else if (sensitivity > 10.0) {
            sensitivity = 10.0;
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
        return SDL_HasClipboardText() != 0U;
    }

    std::string EventManager::getClipboardText() const
    {
        std::string text;
        if (SDL_HasClipboardText() != 0U) {
            text = std::string(SDL_GetClipboardText());
        }
        return text;
    }

    void EventManager::setClipboardText(const std::string& text)
    {
        SDL_SetClipboardText(text.c_str());
    }

    void EventManager::setJoystickSupport(bool support)
    {
        if (support && (m_joystickManager == nullptr)) {
            m_joystickManager = new JoystickManager();
        } else if (!support && (m_joystickManager != nullptr)) {
            delete m_joystickManager;
            m_joystickManager = nullptr;
        }
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

    void EventManager::loadGamepadMapping(const std::string& file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->loadMapping(file);
        }
    }

    void EventManager::saveGamepadMapping(const std::string& guid, const std::string& file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->saveMapping(guid, file);
        }
    }

    void EventManager::saveGamepadMappings(const std::string& file)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->saveMappings(file);
        }
    }

    std::string EventManager::getGamepadStringMapping(const std::string& guid)
    {
        std::string mapping;
        if (m_joystickManager != nullptr) {
            mapping = m_joystickManager->getStringMapping(guid);
        }
        return mapping;
    }

    void EventManager::setGamepadStringMapping(const std::string& mapping)
    {
        if (m_joystickManager != nullptr) {
            m_joystickManager->setStringMapping(mapping);
        }
    }
} // namespace FIFE
