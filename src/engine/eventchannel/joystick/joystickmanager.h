// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_JOYSTICKMANAGER_H
#define FIFE_EVENTCHANNEL_JOYSTICKMANAGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <deque>
#include <list>
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "ijoystickcontroller.h"
#include "ijoysticklistener.h"
#include "joystick.h"
#include "joystickevent.h"
#include "loaders/native/input/controllermappingloader.h"
#include "savers/native/input/controllermappingsaver.h"
#include "util/base/fife_stdint.h"

namespace FIFE
{

    /**  Joystick Manager manages all events related to Joysticks and Gamecontrollers
     */
    class /*FIFE_API*/ JoystickManager : public IJoystickController, public IEventSource
    {
        public:
            /** Constructor.
             */
            JoystickManager();

            /** Destructor
             */
            ~JoystickManager() override;

            /** Adds a joystick with the given device index.
             */
            Joystick* addJoystick(int32_t deviceIndex);

            /** Return the joystick with the given instance id.
             */
            Joystick* getJoystick(int32_t instanceId);

            /** Removes the given joystick, can be reused.
             */
            void removeJoystick(Joystick* joystick);

            /** Return the number of joysticks / gamecontrollers.
             */
            uint8_t getJoystickCount() const;

            /** Loads controller mappings from given file and if possible, it opens the related controllers.
             */
            void loadMapping(std::string const & file);

            /** Saves controller mapping for given GUID in the specified file.
             */
            void saveMapping(std::string const & guid, std::string const & file);

            /** Saves all controller mappings that were used during the season.
             */
            void saveMappings(std::string const & file);

            /** Return the controller mapping for given GUID as string.
             */
            std::string getStringMapping(std::string const & guid);

            /** Sets controller mapping from string and adds or updates the related controllers.
             */
            void setStringMapping(std::string const & mapping);

            // Implementation from IJoystickController.
            void addJoystickListener(IJoystickListener* listener) override;
            void addJoystickListenerFront(IJoystickListener* listener) override;
            void removeJoystickListener(IJoystickListener* listener) override;

            /** Creates and process joystick events.
             */
            void processJoystickEvent(SDL_Event event);

            /** Creates and process gamecontroller events.
             */
            void processControllerEvent(SDL_Event event);

            /** Dispatches joystick / controller events.
             */
            void dispatchJoystickEvent(JoystickEvent& evt);

            // Implementation from IEventSource.
            EventSourceType getEventSourceType() override;

        private:
            /** Return GUID for given device index as string.
             */
            std::string getGuidString(int32_t deviceIndex);

            /** Converts the int16 in -1.0 to 1.0 range.
             */
            float convertRange(int16_t value);

            /** Adds GUID from controller. Needed for saving.
             */
            void addControllerGuid(Joystick* joystick);

            /** Removes / decal controller GUID.
             */
            void removeControllerGuid(Joystick* joystick);

            //! Loader for gamecontroller mapping.
            ControllerMappingLoader m_mappingLoader;

            //! Saver for gamecontroller mapping.
            ControllerMappingSaver m_mappingSaver;

            //! All active / connected Joysticks.
            std::vector<Joystick*> m_activeJoysticks;

            //! All "known" Joysticks. Useful if a user reconnect a Joystick.
            std::vector<Joystick*> m_joysticks;

            //! Map to hold the relation between Joystick InstanceId and JoystickId
            std::map<int32_t, uint32_t> m_joystickIndices;

            //! Each sort of gamepad have a GUID from SDL. Indicates the number of gamepads with given GUID are
            //! connected.
            std::map<std::string, uint8_t> m_gamepadGuids;

            //! The Joystick listeners.
            std::deque<IJoystickListener*> m_joystickListeners;
    };
} // namespace FIFE

#endif
