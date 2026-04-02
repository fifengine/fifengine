// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ILISTENER_H
#define FIFE_EVENTCHANNEL_ILISTENER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE
{
    /**  Base Listener
     */
    class IListener
    {
    public:
        /** Indicates if the listener is active.
         */
        virtual bool isActive()
        {
            return m_active;
        }

        /** Changes the listener status.
         * @param active The value to change the listener to active / inactive.
         */
        virtual void setActive(bool active)
        {
            m_active = active;
        }

        virtual ~IListener() = default;

    protected:
        IListener()
        {
            m_active = false;
        }

    private:
        bool m_active;
    };

} // namespace FIFE

#endif