// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_JOYSTICK_H
#define FIFE_EVENTCHANNEL_JOYSTICK_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes

namespace FIFE
{

    /** Represents a Joystick and if available the Gamecontroller.
     */
    class /*FIFE_API*/ Joystick
    {
        public:
            // Hat positions.
            enum Hat : int8_t
            {
                HAT_INVALID   = -1,
                HAT_CENTERED  = 0,
                HAT_UP        = 1,
                HAT_RIGHT     = 2,
                HAT_DOWN      = 4,
                HAT_LEFT      = 8,
                HAT_RIGHTUP   = 3,
                HAT_RIGHTDOWN = 6,
                HAT_LEFTUP    = 9,
                HAT_LEFTDOWN  = 12
            };

            // The list of axes available from a controller.
            enum ContollerAxis : int8_t
            {
                CONTOLLER_AXIS_INVALID = -1,
                CONTOLLER_AXIS_LEFTX,
                CONTOLLER_AXIS_LEFTY,
                CONTOLLER_AXIS_RIGHTX,
                CONTOLLER_AXIS_RIGHTY,
                CONTOLLER_AXIS_TRIGGERLEFT,
                CONTOLLER_AXIS_TRIGGERRIGHT,
                CONTOLLER_AXIS_MAX
            };

            // The list of buttons available from a controller.
            enum ControllerButton : int8_t
            {
                CONTOLLER_BUTTON_INVALID = -1,
                CONTOLLER_BUTTON_A,
                CONTOLLER_BUTTON_B,
                CONTOLLER_BUTTON_X,
                CONTOLLER_BUTTON_Y,
                CONTOLLER_BUTTON_BACK,
                CONTOLLER_BUTTON_GUIDE,
                CONTOLLER_BUTTON_START,
                CONTOLLER_BUTTON_LEFTSTICK,
                CONTOLLER_BUTTON_RIGHTSTICK,
                CONTOLLER_BUTTON_LEFTSHOULDER,
                CONTOLLER_BUTTON_RIGHTSHOULDER,
                CONTOLLER_BUTTON_DPAD_UP,
                CONTOLLER_BUTTON_DPAD_DOWN,
                CONTOLLER_BUTTON_DPAD_LEFT,
                CONTOLLER_BUTTON_DPAD_RIGHT,
                CONTOLLER_BUTTON_MAX
            };

            /** Constructor
             * @param joystickId The identifier for the Joystick.
             * @param deviceIndex The index number from SDL2.
             */
            Joystick(int32_t joystickId, int32_t deviceIndex);

            /** Destructor
             */
            ~Joystick();

            /** Return the instance id of the joystick.
             */
            int32_t getInstanceId() const;

            /** Sets the instance id of the joystick.
             */
            int32_t getJoystickId() const;

            /** Sets the device index of the joystick.
             */
            void setDeviceIndex(int32_t deviceIndex);

            /** Return the device index of the joystick.
             */
            int32_t getDeviceIndex() const;

            /** Return the GUID of the joystick / gamecontroller class as string.
             */
            std::string const & getGuid();

            /** Return the name of the joystick.
             */
            std::string const & getName();

            /** Opens / activates the joystick and sets values.
             */
            void open();

            /** Closes / deactivates the joystick.
             */
            void close();

            /** Indicates if the joystick / gamecontroller is connected.
             */
            bool isConnected() const;

            /** Indicates if this a controller.
             */
            bool isController() const;

            /** Opens / activates the gamecontroller, only possible with valid GUID mapping.
             */
            void openController();

            /** Closes / deactivates the gamecontroller.
             */
            void closeController();

            /** Return the number of axes.
             */
            uint8_t getNumberOfAxes() const;

            /** Return the number of buttons.
             */
            uint8_t getNumberOfButtons() const;

            /** Return the number of hats.
             */
            uint8_t getNumberOfHats() const;

            /** Return the current value for given axis.
             * @param axis For joysticks the index number, for controller see ContollerAxis.
             */
            float getAxisValue(int8_t axis) const;

            /** Return the hat value (see Hat positions), for given hat index.
             */
            int8_t getHatValue(int8_t hat) const;

            /** Return the current value for given axis.
             * @param button For joysticks the index number, for controller see ControllerButton.
             */
            bool isButtonPressed(int8_t button) const;

        private:
            /** Converts the int16 in -1.0 to 1.0 range.
             */
            float convertRange(int16_t value) const;

            //! SDLs joystick handle
            SDL_Joystick* m_joystickHandle;
            //! SDLs controller handle
            SDL_Gamepad* m_controllerHandle;
            //! SDLs joystick id (different from device index)
            SDL_JoystickID m_instanceId;
            //! Our joystick id
            int32_t m_joystickId;
            //! SDLs device index
            int32_t m_deviceIndex;
            //! The GUID as string
            std::string m_guidStr;
            //! The name of the joystick / controller
            std::string m_name;
    };

} // namespace FIFE

#endif
