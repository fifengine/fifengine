// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_KEY_H
#define FIFE_EVENTCHANNEL_KEY_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "key_gen.h"

namespace FIFE
{

    /** Represents a key.
     *
     * The KeyType enum is auto-generated from SDL3's SDL_keycode.h
     * (see key_gen.h).  All SDLK_* constants are mirrored 1:1.
     * Old spellings (ENTER, INVALID_KEY) are provided as aliases
     * for backward compatibility.
     */
    class FIFE_API Key
    {
        public:
            using KeyType = FIFE::Keys::KeyType;

            /** Constructor
             * @param key value of the key
             */
            explicit Key(KeyType key = KeyType(0)) : m_key(key)
            {
            }

            Key(Key const &)            = default;
            Key& operator=(Key const &) = default;

            /** Destructor.
             */
            virtual ~Key() = default;

            /** Tells whether the Key can be represented as a string.
             */
            bool isPrintable() const
            {
                std::string const name = getAsString();
                return !name.empty();
            }

            bool is_printable() const
            {
                return isPrintable();
            }

            bool isFunctionKey() const
            {
                return (m_key >= Keys::F1 && m_key <= Keys::F24);
            }

            /** Gets the value of the key.
             */
            KeyType getValue() const
            {
                return m_key;
            }

            /** Gets the string representation of the key, encoded as UTF-8.
             */
            std::string getAsString() const
            {
                std::string name(SDL_GetKeyName(static_cast<SDL_Keycode>(m_key)));
                return name;
            }

            std::string get_name() const
            {
                return getAsString();
            }

        private:
            KeyType m_key;
    };

} // namespace FIFE

#endif
