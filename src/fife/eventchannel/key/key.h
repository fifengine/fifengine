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

            // ── Backward-compatibility aliases ────────────────────────────
            static constexpr KeyType INVALID_KEY         = Keys::KEY_UNKNOWN;
            static constexpr KeyType ENTER               = Keys::RETURN;
            static constexpr KeyType ESCAPE              = Keys::ESCAPE;
            static constexpr KeyType BACKSPACE           = Keys::BACKSPACE;
            static constexpr KeyType TAB                 = Keys::TAB;
            static constexpr KeyType SPACE               = Keys::SPACE;
            static constexpr KeyType DELETE              = Keys::DELETE;
            static constexpr KeyType INSERT              = Keys::INSERT;
            static constexpr KeyType HOME                = Keys::HOME;
            static constexpr KeyType END                 = Keys::END;
            static constexpr KeyType PAGE_UP             = Keys::PAGEUP;
            static constexpr KeyType PAGE_DOWN           = Keys::PAGEDOWN;
            static constexpr KeyType KEY_DELETE          = Keys::DELETE;
            static constexpr KeyType BACKQUOTE           = Keys::GRAVE;
            static constexpr KeyType QUOTE               = Keys::APOSTROPHE;
            static constexpr KeyType QUOTEDBL            = Keys::DBLAPOSTROPHE;
            static constexpr KeyType NUM_LOCK            = Keys::NUMLOCKCLEAR;
            static constexpr KeyType PRINT_SCREEN        = Keys::PRINTSCREEN;
            static constexpr KeyType SCROLL_LOCK         = Keys::SCROLLLOCK;
            static constexpr KeyType CAPS_LOCK           = Keys::CAPSLOCK;
            static constexpr KeyType NUM_0               = Keys::_0;
            static constexpr KeyType NUM_1               = Keys::_1;
            static constexpr KeyType NUM_2               = Keys::_2;
            static constexpr KeyType NUM_3               = Keys::_3;
            static constexpr KeyType NUM_4               = Keys::_4;
            static constexpr KeyType NUM_5               = Keys::_5;
            static constexpr KeyType NUM_6               = Keys::_6;
            static constexpr KeyType NUM_7               = Keys::_7;
            static constexpr KeyType NUM_8               = Keys::_8;
            static constexpr KeyType NUM_9               = Keys::_9;
            static constexpr KeyType LEFT_CONTROL        = Keys::LCTRL;
            static constexpr KeyType RIGHT_CONTROL       = Keys::RCTRL;
            static constexpr KeyType LEFT_SHIFT          = Keys::LSHIFT;
            static constexpr KeyType RIGHT_SHIFT         = Keys::RSHIFT;
            static constexpr KeyType LEFT_ALT            = Keys::LALT;
            static constexpr KeyType RIGHT_ALT           = Keys::RALT;
            static constexpr KeyType LEFT_SUPER          = Keys::LGUI;
            static constexpr KeyType RIGHT_SUPER         = Keys::RGUI;
            static constexpr KeyType ALT_GR              = Keys::MODE;
            static constexpr KeyType AUDIO_NEXT          = Keys::MEDIA_NEXT_TRACK;
            static constexpr KeyType AUDIO_PREV          = Keys::MEDIA_PREVIOUS_TRACK;
            static constexpr KeyType AUDIO_STOP          = Keys::MEDIA_STOP;
            static constexpr KeyType AUDIO_PLAY          = Keys::MEDIA_PLAY;
            static constexpr KeyType AUDIO_MUTE          = Keys::MUTE;
            static constexpr KeyType MEDIASELECT         = Keys::MEDIA_SELECT;
            static constexpr KeyType VOLUME_UP           = Keys::VOLUMEUP;
            static constexpr KeyType VOLUME_DOWN         = Keys::VOLUMEDOWN;
            static constexpr KeyType SYS_REQ             = Keys::SYSREQ;
            static constexpr KeyType ERASE               = Keys::ALTERASE;
            static constexpr KeyType CLEAR_AGAIN         = Keys::CLEARAGAIN;
            static constexpr KeyType CR_SEL              = Keys::CRSEL;
            static constexpr KeyType EX_SEL              = Keys::EXSEL;
            static constexpr KeyType FORTH               = Keys::OUT;
            static constexpr KeyType NUM_LOCK_CLEAR      = Keys::NUMLOCKCLEAR;
            static constexpr KeyType THOUSANDS_SEPARATOR = Keys::THOUSANDSSEPARATOR;
            static constexpr KeyType DECIMAL_SEPARATOR   = Keys::DECIMALSEPARATOR;
            static constexpr KeyType CURRENCY_UNIT       = Keys::CURRENCYUNIT;
            static constexpr KeyType CURRENCY_SUBUNIT    = Keys::CURRENCYSUBUNIT;
            static constexpr KeyType KP_MEMSUBSTRACT     = Keys::KP_MEMSUBTRACT;
            static constexpr KeyType LESS                = Keys::KEY_LESS;
            static constexpr KeyType GREATER             = Keys::KEY_GREATER;

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

        private:
            KeyType m_key;
    };

} // namespace FIFE

#endif
