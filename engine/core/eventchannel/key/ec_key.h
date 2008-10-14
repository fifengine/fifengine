/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

	/** Represents a key or a character.
	 */
	class Key {
	public:
		/** Codes for different keys
		 */
		enum KeyType {
			INVALID_KEY   = -1,
			BACKSPACE     = SDLK_BACKSPACE,
			TAB           = SDLK_TAB,
			CLEAR         = SDLK_CLEAR,
			ENTER         = SDLK_RETURN,
			PAUSE         = SDLK_PAUSE,
			ESCAPE        = SDLK_ESCAPE,
			SPACE         = SDLK_SPACE,
			EXCLAIM       = SDLK_EXCLAIM,
			QUOTEDBL      = SDLK_QUOTEDBL,
			HASH          = SDLK_HASH,
			DOLLAR        = SDLK_DOLLAR,
			AMPERSAND     = SDLK_AMPERSAND,
			QUOTE         = SDLK_QUOTE,
			LEFTPAREN     = SDLK_LEFTPAREN,
			RIGHTPAREN    = SDLK_RIGHTPAREN,
			ASTERISK      = SDLK_ASTERISK,
			PLUS          = SDLK_PLUS,
			COMMA         = SDLK_COMMA,
			MINUS         = SDLK_MINUS,
			PERIOD        = SDLK_PERIOD,
			SLASH         = SDLK_SLASH,
			NUM_0         = SDLK_0,
			NUM_1         = SDLK_1,
			NUM_2         = SDLK_2,
			NUM_3         = SDLK_3,
			NUM_4         = SDLK_4,
			NUM_5         = SDLK_5,
			NUM_6         = SDLK_6,
			NUM_7         = SDLK_7,
			NUM_8         = SDLK_8,
			NUM_9         = SDLK_9,
			COLON         = SDLK_COLON,
			SEMICOLON     = SDLK_SEMICOLON,
			LESS          = SDLK_LESS,
			EQUALS        = SDLK_EQUALS,
			GREATER       = SDLK_GREATER,
			QUESTION      = SDLK_QUESTION,
			AT            = SDLK_AT,
			LEFTBRACKET   = SDLK_LEFTBRACKET,
			BACKSLASH     = SDLK_BACKSLASH,
			RIGHTBRACKET  = SDLK_RIGHTBRACKET,
			CARET         = SDLK_CARET,
			UNDERSCORE    = SDLK_UNDERSCORE,
			BACKQUOTE     = SDLK_BACKQUOTE,
			A             = SDLK_a,
			B             = SDLK_b,
			C             = SDLK_c,
			D             = SDLK_d,
			E             = SDLK_e,
			F             = SDLK_f,
			G             = SDLK_g,
			H             = SDLK_h,
			I             = SDLK_i,
			J             = SDLK_j,
			K             = SDLK_k,
			L             = SDLK_l,
			M             = SDLK_m,
			N             = SDLK_n,
			O             = SDLK_o,
			P             = SDLK_p,
			Q             = SDLK_q,
			R             = SDLK_r,
			S             = SDLK_s,
			T             = SDLK_t,
			U             = SDLK_u,
			V             = SDLK_v,
			W             = SDLK_w,
			X             = SDLK_x,
			Y             = SDLK_y,
			Z             = SDLK_z,
			DELETE        = SDLK_DELETE,
			WORLD_0       = SDLK_WORLD_0,
			WORLD_1       = SDLK_WORLD_1,
			WORLD_2       = SDLK_WORLD_2,
			WORLD_3       = SDLK_WORLD_3,
			WORLD_4       = SDLK_WORLD_4,
			WORLD_5       = SDLK_WORLD_5,
			WORLD_6       = SDLK_WORLD_6,
			WORLD_7       = SDLK_WORLD_7,
			WORLD_8       = SDLK_WORLD_8,
			WORLD_9       = SDLK_WORLD_9,
			WORLD_10      = SDLK_WORLD_10,
			WORLD_11      = SDLK_WORLD_11,
			WORLD_12      = SDLK_WORLD_12,
			WORLD_13      = SDLK_WORLD_13,
			WORLD_14      = SDLK_WORLD_14,
			WORLD_15      = SDLK_WORLD_15,
			WORLD_16      = SDLK_WORLD_16,
			WORLD_17      = SDLK_WORLD_17,
			WORLD_18      = SDLK_WORLD_18,
			WORLD_19      = SDLK_WORLD_19,
			WORLD_20      = SDLK_WORLD_20,
			WORLD_21      = SDLK_WORLD_21,
			WORLD_22      = SDLK_WORLD_22,
			WORLD_23      = SDLK_WORLD_23,
			WORLD_24      = SDLK_WORLD_24,
			WORLD_25      = SDLK_WORLD_25,
			WORLD_26      = SDLK_WORLD_26,
			WORLD_27      = SDLK_WORLD_27,
			WORLD_28      = SDLK_WORLD_28,
			WORLD_29      = SDLK_WORLD_29,
			WORLD_30      = SDLK_WORLD_30,
			WORLD_31      = SDLK_WORLD_31,
			WORLD_32      = SDLK_WORLD_32,
			WORLD_33      = SDLK_WORLD_33,
			WORLD_34      = SDLK_WORLD_34,
			WORLD_35      = SDLK_WORLD_35,
			WORLD_36      = SDLK_WORLD_36,
			WORLD_37      = SDLK_WORLD_37,
			WORLD_38      = SDLK_WORLD_38,
			WORLD_39      = SDLK_WORLD_39,
			WORLD_40      = SDLK_WORLD_40,
			WORLD_41      = SDLK_WORLD_41,
			WORLD_42      = SDLK_WORLD_42,
			WORLD_43      = SDLK_WORLD_43,
			WORLD_44      = SDLK_WORLD_44,
			WORLD_45      = SDLK_WORLD_45,
			WORLD_46      = SDLK_WORLD_46,
			WORLD_47      = SDLK_WORLD_47,
			WORLD_48      = SDLK_WORLD_48,
			WORLD_49      = SDLK_WORLD_49,
			WORLD_50      = SDLK_WORLD_50,
			WORLD_51      = SDLK_WORLD_51,
			WORLD_52      = SDLK_WORLD_52,
			WORLD_53      = SDLK_WORLD_53,
			WORLD_54      = SDLK_WORLD_54,
			WORLD_55      = SDLK_WORLD_55,
			WORLD_56      = SDLK_WORLD_56,
			WORLD_57      = SDLK_WORLD_57,
			WORLD_58      = SDLK_WORLD_58,
			WORLD_59      = SDLK_WORLD_59,
			WORLD_60      = SDLK_WORLD_60,
			WORLD_61      = SDLK_WORLD_61,
			WORLD_62      = SDLK_WORLD_62,
			WORLD_63      = SDLK_WORLD_63,
			WORLD_64      = SDLK_WORLD_64,
			WORLD_65      = SDLK_WORLD_65,
			WORLD_66      = SDLK_WORLD_66,
			WORLD_67      = SDLK_WORLD_67,
			WORLD_68      = SDLK_WORLD_68,
			WORLD_69      = SDLK_WORLD_69,
			WORLD_70      = SDLK_WORLD_70,
			WORLD_71      = SDLK_WORLD_71,
			WORLD_72      = SDLK_WORLD_72,
			WORLD_73      = SDLK_WORLD_73,
			WORLD_74      = SDLK_WORLD_74,
			WORLD_75      = SDLK_WORLD_75,
			WORLD_76      = SDLK_WORLD_76,
			WORLD_77      = SDLK_WORLD_77,
			WORLD_78      = SDLK_WORLD_78,
			WORLD_79      = SDLK_WORLD_79,
			WORLD_80      = SDLK_WORLD_80,
			WORLD_81      = SDLK_WORLD_81,
			WORLD_82      = SDLK_WORLD_82,
			WORLD_83      = SDLK_WORLD_83,
			WORLD_84      = SDLK_WORLD_84,
			WORLD_85      = SDLK_WORLD_85,
			WORLD_86      = SDLK_WORLD_86,
			WORLD_87      = SDLK_WORLD_87,
			WORLD_88      = SDLK_WORLD_88,
			WORLD_89      = SDLK_WORLD_89,
			WORLD_90      = SDLK_WORLD_90,
			WORLD_91      = SDLK_WORLD_91,
			WORLD_92      = SDLK_WORLD_92,
			WORLD_93      = SDLK_WORLD_93,
			WORLD_94      = SDLK_WORLD_94,
			WORLD_95      = SDLK_WORLD_95,
			KP0           = SDLK_KP0,
			KP1           = SDLK_KP1,
			KP2           = SDLK_KP2,
			KP3           = SDLK_KP3,
			KP4           = SDLK_KP4,
			KP5           = SDLK_KP5,
			KP6           = SDLK_KP6,
			KP7           = SDLK_KP7,
			KP8           = SDLK_KP8,
			KP9           = SDLK_KP9,
			KP_PERIOD     = SDLK_KP_PERIOD,
			KP_DIVIDE     = SDLK_KP_DIVIDE,
			KP_MULTIPLY   = SDLK_KP_MULTIPLY,
			KP_MINUS      = SDLK_KP_MINUS,
			KP_PLUS       = SDLK_KP_PLUS,
			KP_ENTER      = SDLK_KP_ENTER,
			KP_EQUALS     = SDLK_KP_EQUALS,
			UP            = SDLK_UP,
			DOWN          = SDLK_DOWN,
			RIGHT         = SDLK_RIGHT,
			LEFT          = SDLK_LEFT,
			INSERT        = SDLK_INSERT,
			HOME          = SDLK_HOME,
			END           = SDLK_END,
			PAGE_UP       = SDLK_PAGEUP,
			PAGE_DOWN     = SDLK_PAGEDOWN,
			F1            = SDLK_F1,
			F2            = SDLK_F2,
			F3            = SDLK_F3,
			F4            = SDLK_F4,
			F5            = SDLK_F5,
			F6            = SDLK_F6,
			F7            = SDLK_F7,
			F8            = SDLK_F8,
			F9            = SDLK_F9,
			F10           = SDLK_F10,
			F11           = SDLK_F11,
			F12           = SDLK_F12,
			F13           = SDLK_F13,
			F14           = SDLK_F14,
			F15           = SDLK_F15,
			NUM_LOCK      = SDLK_NUMLOCK,
			CAPS_LOCK     = SDLK_CAPSLOCK,
			SCROLL_LOCK   = SDLK_SCROLLOCK,
			RIGHT_SHIFT   = SDLK_RSHIFT,
			LEFT_SHIFT    = SDLK_LSHIFT,
			RIGHT_CONTROL = SDLK_RCTRL,
			LEFT_CONTROL  = SDLK_LCTRL,
			RIGHT_ALT     = SDLK_RALT,
			LEFT_ALT      = SDLK_LALT,
			RIGHT_META    = SDLK_RMETA,
			LEFT_META     = SDLK_LMETA,
			RIGHT_SUPER   = SDLK_RSUPER,
			LEFT_SUPER    = SDLK_LSUPER,
			ALT_GR        = SDLK_MODE,
			COMPOSE       = SDLK_COMPOSE,
			HELP          = SDLK_HELP,
			PRINT_SCREEN  = SDLK_PRINT,
			SYSREQ        = SDLK_SYSREQ,
			BREAK         = SDLK_BREAK,
			MENU          = SDLK_MENU,
			POWER         = SDLK_POWER,
			EURO          = SDLK_EURO,
			UNDO          = SDLK_UNDO
		};

		/** Constructor
		 * @param value value of the key
		 * @param codepoint Unicode representation of the key
		*/
		Key(KeyType key = INVALID_KEY, uint32_t codepoint = 0):
			m_key(key),
			m_unicode(codepoint) {}
			
		/** Destructor.
		 */
		virtual ~Key() {}
		
		/** Tells whether the Key can be represented as a string.
		 */
		bool isPrintable() const {
			// Unicode does not have control characters, so this should be sufficient
			return m_unicode != 0;
		}
		
		bool isFunctionKey() const {
			return m_key >= F1 && m_key <= F15;
		}
		
		/** Gets the value of the key. 
		 */
		KeyType getValue() const {
			return m_key;
		}

		/** Gets the string representation of the key, encoded as UTF-8. 
		 */
		std::string getAsString() const {
			// older sdl versions have the 3rd argument non const, but it is in fact const...
			char* converted = SDL_iconv_string("UTF-8", "UTF-32",
				const_cast<char*>(reinterpret_cast<const char*>(&m_unicode)), 4);
			std::string retval(converted);
			SDL_free(converted);
			return retval;
		}

	private:
		KeyType m_key;
		uint32_t m_unicode;
	};

} //FIFE

#endif
