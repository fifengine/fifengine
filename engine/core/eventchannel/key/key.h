/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_EVENTCHANNEL_KEY_H
#define FIFE_EVENTCHANNEL_KEY_H

// Standard C++ library includes
//

// 3rd party library includes
//
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
namespace FIFE {

	/** Represents a key.
	 */
	class Key {
	public:
		/** Codes for different keys
		 */
		enum KeyType {
			INVALID_KEY			= SDLK_UNKNOWN,
			ENTER				= SDLK_RETURN,
			ESCAPE				= SDLK_ESCAPE,
			BACKSPACE			= SDLK_BACKSPACE,
			TAB					= SDLK_TAB,
			SPACE				= SDLK_SPACE,
			EXCLAIM				= SDLK_EXCLAIM,
			QUOTEDBL			= SDLK_QUOTEDBL,
			HASH				= SDLK_HASH,
			PERCENT				= SDLK_PERCENT,
			DOLLAR				= SDLK_DOLLAR,
			AMPERSAND			= SDLK_AMPERSAND,
			QUOTE				= SDLK_QUOTE,
			LEFTPAREN			= SDLK_LEFTPAREN,
			RIGHTPAREN			= SDLK_RIGHTPAREN,
			ASTERISK			= SDLK_ASTERISK,
			PLUS				= SDLK_PLUS,
			COMMA				= SDLK_COMMA,
			MINUS				= SDLK_MINUS,
			PERIOD				= SDLK_PERIOD,
			SLASH				= SDLK_SLASH,
			NUM_0				= SDLK_0,
			NUM_1				= SDLK_1,
			NUM_2				= SDLK_2,
			NUM_3				= SDLK_3,
			NUM_4				= SDLK_4,
			NUM_5				= SDLK_5,
			NUM_6				= SDLK_6,
			NUM_7				= SDLK_7,
			NUM_8				= SDLK_8,
			NUM_9				= SDLK_9,
			COLON				= SDLK_COLON,
			SEMICOLON			= SDLK_SEMICOLON,
			LESS				= SDLK_LESS,
			EQUALS				= SDLK_EQUALS,
			GREATER				= SDLK_GREATER,
			QUESTION			= SDLK_QUESTION,
			AT					= SDLK_AT,
			LEFTBRACKET			= SDLK_LEFTBRACKET,
			BACKSLASH			= SDLK_BACKSLASH,
			RIGHTBRACKET		= SDLK_RIGHTBRACKET,
			CARET				= SDLK_CARET,
			UNDERSCORE			= SDLK_UNDERSCORE,
			BACKQUOTE			= SDLK_BACKQUOTE,
			A					= SDLK_a,
			B					= SDLK_b,
			C					= SDLK_c,
			D					= SDLK_d,
			E					= SDLK_e,
			F					= SDLK_f,
			G					= SDLK_g,
			H					= SDLK_h,
			I					= SDLK_i,
			J					= SDLK_j,
			K					= SDLK_k,
			L					= SDLK_l,
			M					= SDLK_m,
			N					= SDLK_n,
			O					= SDLK_o,
			P					= SDLK_p,
			Q					= SDLK_q,
			R					= SDLK_r,
			S					= SDLK_s,
			T					= SDLK_t,
			U					= SDLK_u,
			V					= SDLK_v,
			W					= SDLK_w,
			X					= SDLK_x,
			Y					= SDLK_y,
			Z					= SDLK_z,

			CAPS_LOCK			= SDLK_CAPSLOCK,
		
			F1					= SDLK_F1,
			F2					= SDLK_F2,
			F3					= SDLK_F3,
			F4					= SDLK_F4,
			F5					= SDLK_F5,
			F6					= SDLK_F6,
			F7					= SDLK_F7,
			F8					= SDLK_F8,
			F9					= SDLK_F9,
			F10					= SDLK_F10,
			F11					= SDLK_F11,
			F12					= SDLK_F12,

			PRINT_SCREEN		= SDLK_PRINTSCREEN,
			SCROLL_LOCK			= SDLK_SCROLLLOCK,
			PAUSE				= SDLK_PAUSE,
			INSERT				= SDLK_INSERT,
			HOME				= SDLK_HOME,
			PAGE_UP				= SDLK_PAGEUP,
			DELETE				= SDLK_DELETE,
			END					= SDLK_END,
			PAGE_DOWN			= SDLK_PAGEDOWN,
			RIGHT				= SDLK_RIGHT,
			LEFT				= SDLK_LEFT,
			DOWN				= SDLK_DOWN,
			UP					= SDLK_UP,

			NUM_LOCK			= SDLK_NUMLOCKCLEAR,
			KP_DIVIDE			= SDLK_KP_DIVIDE,
			KP_MULTIPLY			= SDLK_KP_MULTIPLY,
			KP_MINUS			= SDLK_KP_MINUS,
			KP_PLUS				= SDLK_KP_PLUS,
			KP_ENTER			= SDLK_KP_ENTER,
			KP_0				= SDLK_KP_0,
			KP_1				= SDLK_KP_1,
			KP_2				= SDLK_KP_2,
			KP_3				= SDLK_KP_3,
			KP_4				= SDLK_KP_4,
			KP_5				= SDLK_KP_5,
			KP_6				= SDLK_KP_6,
			KP_7				= SDLK_KP_7,
			KP_8				= SDLK_KP_8,
			KP_9				= SDLK_KP_9,
			KP_PERIOD			= SDLK_KP_PERIOD,

			APPLICATION			= SDLK_APPLICATION,
			POWER				= SDLK_POWER,
			KP_EQUALS			= SDLK_KP_EQUALS,
			F13					= SDLK_F13,
			F14					= SDLK_F14,
			F15					= SDLK_F15,
			F16					= SDLK_F16,
			F17					= SDLK_F17,
			F18					= SDLK_F18,
			F19					= SDLK_F19,
			F20					= SDLK_F20,
			F21					= SDLK_F21,
			F22					= SDLK_F22,
			F23					= SDLK_F23,
			F24					= SDLK_F24,
			EXECUTE				= SDLK_EXECUTE,
			HELP				= SDLK_HELP,
			MENU				= SDLK_MENU,
			SELECT				= SDLK_SELECT,
			STOP				= SDLK_STOP,
			AGAIN				= SDLK_AGAIN,
			UNDO				= SDLK_UNDO,
			CUT					= SDLK_CUT,
			COPY				= SDLK_COPY,
			PASTE				= SDLK_PASTE,
			FIND				= SDLK_FIND,
			MUTE				= SDLK_MUTE,
			VOLUME_UP			= SDLK_VOLUMEUP,
			VOLUME_DOWN			= SDLK_VOLUMEDOWN,
			KP_COMMA			= SDLK_KP_COMMA,
			KP_EQUALSAS400		= SDLK_KP_EQUALSAS400,

			ERASE				= SDLK_ALTERASE,
			SYS_REQ				= SDLK_SYSREQ,
			CANCEL				= SDLK_CANCEL,
			CLEAR				= SDLK_CLEAR,
			PRIOR				= SDLK_PRIOR,
			RETURN				= SDLK_RETURN2,
			SEPARATOR			= SDLK_SEPARATOR,
			FORTH				= SDLK_OUT,
			OPER				= SDLK_OPER,
			CLEAR_AGAIN			= SDLK_CLEARAGAIN,
			CR_SEL				= SDLK_CRSEL,
			EX_SEL				= SDLK_EXSEL,

			KP_00				= SDLK_KP_00,
			KP_000				= SDLK_KP_000,
			THOUSANDS_SEPARATOR	= SDLK_THOUSANDSSEPARATOR,
			DECIMAL_SEPARATOR	= SDLK_DECIMALSEPARATOR,
			CURRENCY_UNIT		= SDLK_CURRENCYUNIT,
			CURRENCY_SUBUNIT	= SDLK_CURRENCYSUBUNIT,
			KP_LEFTPAREN		= SDLK_KP_LEFTPAREN,
			KP_RIGHTPAREN		= SDLK_KP_RIGHTPAREN,
			KP_LEFTBRACE		= SDLK_KP_LEFTBRACE,
			KP_RIGHTBRACE		= SDLK_KP_RIGHTBRACE,
			KP_TAB				= SDLK_KP_TAB,
			KP_BACKSPACE		= SDLK_KP_BACKSPACE,
			KP_A				= SDLK_KP_A,
			KP_B				= SDLK_KP_B,
			KP_C				= SDLK_KP_C,
			KP_D				= SDLK_KP_D,
			KP_E				= SDLK_KP_E,
			KP_F				= SDLK_KP_F,
			KP_XOR				= SDLK_KP_XOR,
			KP_POWER			= SDLK_KP_POWER,
			KP_PERCENT			= SDLK_KP_PERCENT,
			KP_LESS				= SDLK_KP_LESS,
			KP_GREATER			= SDLK_KP_GREATER,
			KP_AMPERSAND		= SDLK_KP_AMPERSAND,
			KP_DBLAMPERSAND		= SDLK_KP_DBLAMPERSAND,
			KP_VERTICALBAR		= SDLK_KP_VERTICALBAR,
			KP_DBLVERTICALBAR	= SDLK_KP_DBLVERTICALBAR,
			KP_COLON			= SDLK_KP_COLON,
			KP_HASH				= SDLK_KP_HASH,
			KP_SPACE			= SDLK_KP_SPACE,
			KP_AT				= SDLK_KP_AT,
			KP_EXCLAM			= SDLK_KP_EXCLAM,
			KP_MEMSTORE			= SDLK_KP_MEMSTORE,
			KP_MEMRECALL		= SDLK_KP_MEMRECALL,
			KP_MEMCLEAR			= SDLK_KP_MEMCLEAR,
			KP_MEMADD			= SDLK_KP_MEMADD,
			KP_MEMSUBSTRACT		= SDLK_KP_MEMSUBTRACT,
			KP_MEMMULTIPLY		= SDLK_KP_MEMMULTIPLY,
			KP_MEMDIVIDE		= SDLK_KP_MEMDIVIDE,
			KP_PLUSMINUS		= SDLK_KP_PLUSMINUS,
			KP_CLEAR			= SDLK_KP_CLEAR,
			KP_CLEARENTRY		= SDLK_KP_CLEARENTRY,
			KP_BINARY			= SDLK_KP_BINARY,
			KP_OCTAL			= SDLK_KP_OCTAL,
			KP_DECIMAL			= SDLK_KP_DECIMAL,
			KP_HEXADECIMAL		= SDLK_KP_HEXADECIMAL,

			LEFT_CONTROL		= SDLK_LCTRL,
			LEFT_SHIFT			= SDLK_LSHIFT,
			LEFT_ALT			= SDLK_LALT,
			LEFT_SUPER			= SDLK_LGUI,
			RIGHT_CONTROL		= SDLK_RCTRL,
			RIGHT_SHIFT			= SDLK_RSHIFT,
			RIGHT_ALT			= SDLK_RALT,
			RIGHT_SUPER			= SDLK_RGUI,

			ALT_GR				= SDLK_MODE,

			AUDIO_NEXT			= SDLK_AUDIONEXT,
			AUDIO_PREV			= SDLK_AUDIOPREV,
			AUDIO_STOP			= SDLK_AUDIOSTOP,
			AUDIO_PLAY			= SDLK_AUDIOPLAY,
			AUDIO_MUTE			= SDLK_AUDIOMUTE,
			MEDIASELECT			= SDLK_MEDIASELECT,
			WWW					= SDLK_WWW,
			MAIL				= SDLK_MAIL,
			CALCULATOR			= SDLK_CALCULATOR,
			COMPUTER			= SDLK_COMPUTER,
			AC_SEARCH			= SDLK_AC_SEARCH,
			AC_HOME				= SDLK_AC_HOME,
			AC_BACK				= SDLK_AC_BACK,
			AC_FORWARD			= SDLK_AC_FORWARD,
			AC_STOP				= SDLK_AC_STOP,
			AC_REFRESH			= SDLK_AC_REFRESH,
			AC_BOOKMARKS		= SDLK_AC_BOOKMARKS,

			BRIGHTNESS_DOWN		= SDLK_BRIGHTNESSDOWN,
			BRIGHTNESS_UP		= SDLK_BRIGHTNESSUP,
			DISPLAY_SWITCH		= SDLK_DISPLAYSWITCH,
			ILLUM_TOGGLE		= SDLK_KBDILLUMTOGGLE,
			ILLUM_DOWN			= SDLK_KBDILLUMDOWN,
			ILLUM_UP			= SDLK_KBDILLUMUP,
			EJECT				= SDLK_EJECT,
			SLEEP				= SDLK_SLEEP
		};
		/** Constructor
		 * @param key value of the key
		*/
		Key(KeyType key = INVALID_KEY):
			m_key(key) {}

		/** Destructor.
		 */
		virtual ~Key() {}

		/** Tells whether the Key can be represented as a string.
		 */
		bool isPrintable() const {
			std::string name = getAsString();
			if (!name.empty()) {
				return true;
			}
			return false;
		}

		bool isFunctionKey() const {
			// ToDo
			return (m_key >= F1 && m_key <= F12) || (m_key >= F13 && m_key <= F24);
		}

		/** Gets the value of the key.
		 */
		KeyType getValue() const {
			return m_key;
		}

		/** Gets the string representation of the key, encoded as UTF-8.
		 */
		std::string getAsString() const {
			std::string name(SDL_GetKeyName(static_cast<SDL_Keycode>(m_key)));
			return name;
		}

	private:
		KeyType m_key;
	};

} //FIFE

#endif
