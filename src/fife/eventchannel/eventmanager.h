// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_EVENTMANAGER_H
#define FIFE_EVENTCHANNEL_EVENTMANAGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/command/command.h"
#include "eventchannel/command/icommandcontroller.h"
#include "eventchannel/command/icommandlistener.h"
#include "eventchannel/drop/dropevent.h"
#include "eventchannel/drop/idropcontroller.h"
#include "eventchannel/drop/idroplistener.h"
#include "eventchannel/key/ikeycontroller.h"
#include "eventchannel/key/ikeylistener.h"
#include "eventchannel/key/key.h"
#include "eventchannel/key/keyevent.h"
#include "eventchannel/mouse/imousecontroller.h"
#include "eventchannel/mouse/imouselistener.h"
#include "eventchannel/mouse/mouseevent.h"
#include "eventchannel/sdl/isdleventcontroller.h"
#include "eventchannel/sdl/isdleventlistener.h"
#include "eventchannel/text/itextcontroller.h"
#include "eventchannel/text/itextlistener.h"
#include "eventchannel/text/text.h"
#include "eventchannel/text/textevent.h"
#include "util/base/fife_stdint.h"

namespace FIFE
{

    class ICommandListener;
    class InputEvent;
    class IJoystickListener;
    class Joystick;
    class JoystickManager;
    class MouseEvent;
    class KeyEvent;
    class IKeyFilter;
    class DropEvent;

    /**  Event Manager manages all events related to FIFE
     */
    class FIFE_API EventManager :
        public ICommandController,
        public IKeyController,
        public ITextController,
        public IMouseController,
        public ISdlEventController,
        public IDropController,
        public IEventSource
    {
        public:
            /** Constructor.
             */
            EventManager();

            /** Destructor
             */
            ~EventManager() override;

            void addCommandListener(ICommandListener* listener) override;
            void addCommandListenerFront(ICommandListener* listener) override;
            void removeCommandListener(ICommandListener* listener) override;

            void dispatchCommand(Command& command) override;

            void addKeyListener(IKeyListener* listener) override;
            void addKeyListenerFront(IKeyListener* listener) override;
            void removeKeyListener(IKeyListener* listener) override;

            void addTextListener(ITextListener* listener) override;
            void addTextListenerFront(ITextListener* listener) override;
            void removeTextListener(ITextListener* listener) override;

            void addMouseListener(IMouseListener* listener) override;
            void addMouseListenerFront(IMouseListener* listener) override;
            void removeMouseListener(IMouseListener* listener) override;

            void addSdlEventListener(ISdlEventListener* listener) override;
            void addSdlEventListenerFront(ISdlEventListener* listener) override;
            void removeSdlEventListener(ISdlEventListener* listener) override;

            void addDropListener(IDropListener* listener) override;
            void addDropListenerFront(IDropListener* listener) override;
            void removeDropListener(IDropListener* listener) override;

            void addJoystickListener(IJoystickListener* listener);
            void addJoystickListenerFront(IJoystickListener* listener);
            void removeJoystickListener(IJoystickListener* listener);

            EventSourceType getEventSourceType() override;

            /** Process the SDL event queue.
             * This is to be called only by the engine itself once per frame.
             * It passes appropriate events to their listeners
             */
            void processEvents();

            void setKeyFilter(IKeyFilter* keyFilter);

            /** Sets mouse sensitivity
             * The sensitivity is limited to the range -0.99 - 10.0.
             */
            void setMouseSensitivity(float sensitivity);

            /** Gets mouse sensitivity
             */
            float getMouseSensitivity() const;

            /** Sets mouse acceleration
             * if mouse acceleration is enabled,
             * then the mouse sensitivity is used as speed max.
             */
            void setMouseAccelerationEnabled(bool acceleration);

            /** Returns if mouse acceleration is enabled or not.
             *
             *  @return True if mouse acceleration is enabled, false if not.
             */
            bool isMouseAccelerationEnabled() const;

            /** Returns if clipboard have text or not.
             *
             *  @return True if the clipboard contains text, false if not.
             */
            bool isClipboardText() const;

            /** Returns the clipboard text as UTF-8 string.
             *
             *  @return UTF-8 string or an empty string if the cliboard have no text.
             */
            std::string getClipboardText() const;

            /** Sets clipboard text.
             *
             *  @param text The text in UTF-8 format.
             */
            void setClipboardText(std::string const & text);

            /** Sets the joystick support to enabled or disabled.
             */
            void setJoystickSupport(bool support);

            bool isKeyPressed(Key::KeyType key) const;

            /** Return the joystick with the given instance id.
             */
            Joystick* getJoystick(int32_t instanceId);

            /** Return the number of joysticks / gamecontrollers.
             */
            uint8_t getJoystickCount() const;

            /** Loads controller mappings from given file and if possible, it opens the related controllers.
             */
            void loadGamepadMapping(std::string const & file);

            /** Saves controller mapping for given GUID in the specified file.
             */
            void saveGamepadMapping(std::string const & guid, std::string const & file);

            /** Saves all controller mappings that were used during the season.
             */
            void saveGamepadMappings(std::string const & file);

            /** Return the controller mapping for given GUID as string.
             */
            std::string getGamepadStringMapping(std::string const & guid);

            /** Sets controller mapping from string and adds or updates the related controllers.
             */
            void setGamepadStringMapping(std::string const & mapping);

        private:
            // Helpers for processEvents
            void processWindowEvent(SDL_Event event);
            void processKeyEvent(SDL_Event event);
            void processTextEvent(SDL_Event event);
            void processMouseEvent(SDL_Event event);
            void processDropEvent(SDL_Event event);
            bool combineEvents(SDL_Event& event1, SDL_Event const & event2);

            // Events dispatchers - only dispatchSdlevent may reject the event.
            bool dispatchSdlEvent(SDL_Event& evt);
            void dispatchKeyEvent(KeyEvent& evt);
            void dispatchTextEvent(TextEvent& evt);
            void dispatchMouseEvent(MouseEvent& evt);
            void dispatchDropEvent(DropEvent& evt);

            // Translate events
            void fillModifiers(InputEvent& evt);
            void fillKeyEvent(SDL_Event const & sdlevt, KeyEvent& keyevt);
            void fillTextEvent(SDL_Event const & sdlevt, TextEvent& txtevt);
            void fillMouseEvent(SDL_Event const & sdlevt, MouseEvent& mouseevt);

            // Listeners
            std::deque<ICommandListener*> m_commandListeners;
            std::deque<IKeyListener*> m_keyListeners;
            std::deque<ITextListener*> m_textListeners;
            std::deque<IMouseListener*> m_mouseListeners;
            std::deque<ISdlEventListener*> m_sdleventListeners;
            std::deque<IDropListener*> m_dropListeners;

            std::map<int32_t, bool> m_keystatemap;
            IKeyFilter* m_keyfilter;
            int32_t m_mousestate;
            MouseEvent::MouseButtonType m_mostrecentbtn;

            // m_mouseSensitivity is the mouse speed factor - 1, so a value of 0 won't
            // influence mouse speed, a value of 1 would double the speed and
            // -.5 would make it half the speed
            float m_mouseSensitivity;
            bool m_acceleration;
            bool m_warp;
            bool m_enter;
            int32_t m_oldX;
            int32_t m_oldY;
            uint64_t m_lastTicks;
            float m_oldVelocity;

            std::unique_ptr<JoystickManager> m_joystickManager;
    };
} // namespace FIFE

#endif
