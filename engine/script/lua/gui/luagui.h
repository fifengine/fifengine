/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_SCRIPT_LUA_GUI_LUAGUI_H
#define FIFE_SCRIPT_LUA_GUI_LUAGUI_H

// Standard C++ library includes
#include <string>
#include <vector>
#include <map>

// 3rd party library includes
#include "script/lua/lua.hpp"
#include "script/lua/lunar.h"
#include <guichan.hpp>
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "guichan_addon/advimage.h"
#include "guichan_addon/twobutton.h"
#include "guichan_addon/icon2.hpp"
#include "guichan_addon/font.h"
#include "util/log.h"
#include "script/lua/lua_ref.h"

/** Exports partial guichan functionality to Lua.
 *
 * Basically each class is derived from the respective gcn::* ancestor; each uses "lunar.h"
 * to create userdata-based 'objects' in the Lua state.
 * There are however a few problems concerning those classes derived from gcn::Widget:
 *
 * - multiple inheritance is not possible this way; single inheritance (which is all we need
 *   for now) works.
 *
 *   Maybe 'swig', 'tolua++' or 'luabind' would make this easier.
 *
 * - I ran into the 'dreaded (inheritance) diamond' (e.g.: gcn::Label inherits from gcn::Widget;
 *   luaGui::Widget is derived from gcn::Widget, luaGui::Label from luaGui::Widget & gcn::Label...
 *   borked)
 *
 *   I found few solutions, the easiest seem to involve code-duplication; the current state
 *   tries to minimize that. If you have better ideas, I'd like to hear them.
 *
 * \note
 * This code is in a different namespace, as I might want to use it outside of FIFE;
 * it is coupled to fife-gui only through Guimanager_LuaScript (and some FIFE::Log calls).
 *
 * Well, that isn't exactly true any more ..:-|
 *
 * @see http://guichan.sourceforge.net/api/
 * @see http://www.lua.org/
 * @see http://lua-users.org/wiki/CppBindingWithLunar
 */
namespace luaGui {

	typedef boost::shared_ptr<FIFE::LuaRef> LuaRefPtr;
	typedef std::map<gcn::Widget*, LuaRefPtr> Widget2RefPtrMap;

#if 0
	// I don't think we'll need the next two ?

	class Rectangle : public gcn::Rectangle {
		public:
			Rectangle(lua_State* L);
			int setAll(lua_State* L);
			int getAll(lua_State* L);
			int intersect(lua_State* L);
			int isPointInRec(lua_State* L);
			// --
			static const char className[];
			static Lunar<Rectangle>::RegType methods[];
	};
	class ClipRectangle : public gcn::ClipRectangle {
		public:
			ClipRectangle(lua_State* L);
			// --
			static const char className[];
			static Lunar<ClipRectangle>::RegType methods[];
	};
#endif
	class Color : public gcn::Color {
		public:
			Color(lua_State* L);
			int l_getRed(lua_State* L);
			int l_getGreen(lua_State* L);
			int l_getBlue(lua_State* L);
			int l_getAlpha(lua_State* L);
			int l_setRed(lua_State* L);
			int l_setGreen(lua_State* L);
			int l_setBlue(lua_State* L);
			int l_setAlpha(lua_State* L);
			// --
			static const char className[];
			static Lunar<Color>::RegType methods[];
	};

	class ListModel : public gcn::ListModel {
		public:
			ListModel(lua_State *L);
			virtual ~ListModel();
			int l_getNumberOfElements(lua_State *L);
			int l_getElementAt(lua_State *L);
			int l_addElement(lua_State *L);
			int l_clear(lua_State *L);
			int getNumberOfElements();
			std::string getElementAt(int k);
			
			// --
			static const char className[];
			static Lunar<ListModel>::RegType methods[];

		private:
			std::vector<std::string> elements;
	};

	/** Image class wrapper.
	 *
	 * Nothing to see, move along ;-)
	 *
	 * @bug The constructor needs an image path as (the only) parameter; if the file doesn't
	 * exist, a Not-Found exception is thrown, which is usually not a good thing inside a
	 * constructor.
	 */
	class Image : public gcn::AdvImage {
		public:
			Image(lua_State* L);
			int l_getWidth(lua_State* L);
			int l_getHeight(lua_State* L);
			int l_setAnimActive(lua_State *L);
			int l_setAnimDirection(lua_State *L);
			int l_setAnimEndCallback(lua_State *L);
			int l_isAnimation(lua_State *L);

			void animEndCallback();

			// --
			static const char className[];
			static Lunar<Image>::RegType methods[];
		private:
			std::string m_callbackName;
	};
	
	class FIFE::FontBase;
	class TTFont  {
		public:
			TTFont(lua_State* L);
			~TTFont();

			FIFE::FontBase* getFont() { return m_font; };

			int l_setColor(lua_State* L);
#if 0

			int getWidth(lua_State* L);
			int getHeight(lua_State* L);
			int isAntiAlias(lua_State* L);
			int setAntiAlias(lua_State* L);
			int getGlyphSpacing(lua_State* L);
			int setGlyphSpacing(lua_State* L);
			int getRowSpacing(lua_State* L);
			int setRowSpacing(lua_State* L);
#endif

			// --
			static const char className[];
			static Lunar<TTFont>::RegType methods[];

		private:
			FIFE::FontBase *m_font;
	};

	class ImageFont  {
		public:
			ImageFont(lua_State* L);
			~ImageFont();

			FIFE::FontBase* getFont() { return m_font; };

			static const char className[];
			static Lunar<ImageFont>::RegType methods[];

		private:
			FIFE::FontBase *m_font;
	};

	/** The base of all widgets - renamed to A(bstract)Widget.
	 *
	 * This class capsules access to the common gcn::Widget; functions exported to Lua
	 * should be prefixed with "l_" (just a convention). Furthermore there are a number
	 * of pure virtual accessor/mutator functions, which have to be implemented in each
	 * luaGui::* derived widget (there is a simple macro to generate inline functions
	 * for exactly that purpose).
	 *
	 * These functions are used (in the l_ functions) in _this_ class to proxy to the
	 * respective gcn::Widget functions; they are prefixed with "f_" (fake/forward) to
	 * help differniate between AWidget and gcn::Widget functions.
	 */
	class AWidget {
		public:
			// needs virtual destructor
			virtual ~AWidget();

			// access to gcn::Widget
			virtual void f_setSize(int, int) = 0;
			virtual int  f_getWidth() = 0;
			virtual int  f_getHeight() = 0;
			virtual void f_setPosition(int, int) = 0;
			virtual int  f_getX() = 0;
			virtual int  f_getY() = 0;
			virtual unsigned int f_getBorderSize() = 0;
			virtual void f_setBorderSize(unsigned int) = 0;
			virtual void f_setFocusable(bool) = 0;
			virtual bool f_isFocusable() = 0;
			virtual bool f_isFocused() = 0;
			virtual void f_setEnabled(bool) = 0;
			virtual bool f_isEnabled() = 0;
			virtual void f_setVisible(bool) = 0;
			virtual bool f_isVisible() = 0;
			virtual const std::string & f_getEventId() = 0;
			virtual void f_setEventId(const std::string &) = 0;
			virtual void f_setBaseColor(const gcn::Color & color) = 0;
			virtual void f_setForegroundColor(const gcn::Color & color) = 0;
			virtual void f_setBackgroundColor(const gcn::Color & color) = 0;
			virtual void f_requestModalFocus() = 0;
			virtual void f_releaseModalFocus() = 0;
			/*
				 virtual const gcn::Color & f_getBaseColor() = 0;
				 virtual const gcn::Color & f_getForegroundColor() = 0;
				 virtual const gcn::Color & f_getBackgroundColor() = 0;
				 */

			// exported to lua
			int l_setSize(lua_State *L);
			int l_getSize(lua_State *L);
			int l_setPosition(lua_State *L);
			int l_getPosition(lua_State *L);
			int l_setBorderSize(lua_State *L);
			int l_getBorderSize(lua_State *L);
			int l_setFocusable(lua_State *L);
			int l_isFocusable(lua_State *L);
			int l_isFocused(lua_State *L);
			int l_setEnabled(lua_State *L);
			int l_isEnabled(lua_State *L);
			int l_setVisible(lua_State *L);
			int l_isVisible(lua_State *L);
			int l_setEventId(lua_State *L);
			int l_getEventId(lua_State *L);
			int l_setBaseColor(lua_State *L);
			int l_setForegroundColor(lua_State *L);
			int l_setBackgroundColor(lua_State *L);
			int l_requestModalFocus(lua_State *L);
			int l_releaseModalFocus(lua_State *L);
			/*
				 int l_getBaseColor(lua_State *L);
				 int l_getForegroundColor(lua_State *L);
				 int l_getBackgroundColor(lua_State *L);
				 */

			// --

			//static const char className[];
			//static Lunar<AWidget>::RegType methods[];
			static gcn::Font* lua2font_cast(lua_State *L);
			static gcn::Color* lua2gcn_color_cast(lua_State *L);

		protected:
			FIFE::LuaRef m_font_ref;
	};

#define WIDGET_FOCUSED_FUNC isFocused
#define GET_WIDGET_EVENT_ID getActionEventId
#define SET_WIDGET_EVENT_ID setActionEventId
// amazing, isn't it?

#define LUAGUI_WIDGET_IMPL(name) \
	inline void f_setSize(int w, int h) { \
		gcn::name::setSize(w, h); \
	} \
	inline int f_getWidth() { \
		return gcn::name::getWidth(); \
	} \
	inline int f_getHeight() { \
		return gcn::name::getHeight(); \
	} \
	inline int f_getX() { \
		return gcn::name::getX(); \
	} \
	inline int f_getY() { \
		return gcn::name::getY(); \
	} \
	inline void f_setPosition(int x, int y) { \
		gcn::name::setPosition(x, y); \
	} \
	inline void f_setBorderSize(unsigned int v) { \
		gcn::name::setBorderSize(v); \
	} \
	inline unsigned int f_getBorderSize() { \
		return gcn::name::getBorderSize(); \
	} \
	inline void f_setFocusable(bool v) { \
		gcn::name::setFocusable(v); \
	} \
	inline bool f_isFocusable() { \
		return gcn::name::isFocusable(); \
	} \
	inline bool f_isFocused() { \
		return gcn::name::WIDGET_FOCUSED_FUNC(); \
	} \
	inline void f_setEnabled(bool v) { \
		gcn::name::setEnabled(v); \
	} \
	inline bool f_isEnabled() { \
		return gcn::name::isEnabled(); \
	} \
	inline void f_setVisible(bool v) { \
		gcn::name::setVisible(v); \
	} \
	inline bool f_isVisible() { \
		return gcn::name::isEnabled(); \
	} \
	inline const std::string & f_getEventId() { \
		return gcn::name::GET_WIDGET_EVENT_ID(); \
	} \
	inline void f_setEventId(const std::string & s) { \
		gcn::name::SET_WIDGET_EVENT_ID(s); \
	} \
	inline void f_setBaseColor(const gcn::Color & color) { \
		gcn::name::setBaseColor(color); \
	} \
	/*			inline const gcn::Color & f_getBaseColor() { \
					return gcn::name::getBaseColor(); \
					} \
					*/ \
	inline void f_setForegroundColor(const gcn::Color & color) { \
		gcn::name::setForegroundColor(color); \
	} \
	/*		inline const gcn::Color & f_getForegroundColor() { \
				return gcn::name::getForegroundColor(); \
				} \
				*/	\
	inline void f_setBackgroundColor(const gcn::Color & color) { \
		gcn::name::setBackgroundColor(color); \
	} \
	/*		inline const gcn::Color & f_getBackgroundColor() { \
				return gcn::name::getBackgroundColor(); \
				} \
				*/ \
	inline void f_requestModalFocus() { \
		gcn::name::requestModalFocus(); \
	} \
	inline void f_releaseModalFocus() { \
		gcn::name::releaseModalFocus(); \
	}

#define LUAGUI_SETFONT_IMPL(name)	int l_setFont(lua_State *L) { \
		gcn::Font* fp = lua2font_cast(L); \
		if (fp != NULL) { \
			lua_pushvalue(L, 1); \
			m_font_ref.ref(L, 1); \
			f_setFont(fp); \
		} \
		else \
			FIFE::Log("AWidget", FIFE::Log::LEVEL_WARN) << "Cannot set font; invalid font object"; \
		return 0; \
	}\
\
	inline void f_setFont(gcn::Font *fp) { \
		gcn::name::setFont(fp); \
		gcn::name::adjustSize(); \
	}


#define LUAGUI_WIDGET_DEF(name) \
	method(name, setPosition), \
	method(name, getPosition), \
	method(name, setSize), \
	method(name, getSize), \
	method(name, getBorderSize), \
	method(name, setBorderSize), \
	method(name, setFocusable), \
	method(name, isFocusable), \
	method(name, isFocused), \
	method(name, setEnabled), \
	method(name, isEnabled), \
	method(name, setVisible), \
	method(name, isVisible), \
	method(name, setEventId), \
	method(name, getEventId), \
	method(name, setBaseColor), \
	method(name, setForegroundColor), \
	method(name, setBackgroundColor), \
	method(name, requestModalFocus), \
	method(name, releaseModalFocus) \

	class Label : public AWidget, public gcn::Label {
		public:
			Label(lua_State *L);
			virtual ~Label();
			int l_adjustSize(lua_State *L); // to be removed
			int l_getAlignment(lua_State *L); // FIXME: missing consts
			int l_setAlignment(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(Label);
			LUAGUI_WIDGET_IMPL(Label);
			// --
			static const char className[];
			static Lunar<Label>::RegType methods[];

	};

	class CheckBox : public AWidget, public gcn::CheckBox {
		public:
			CheckBox(lua_State *L);
			virtual ~CheckBox();
			int l_isMarked(lua_State *L);
			int l_setMarked(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(CheckBox)
			LUAGUI_WIDGET_IMPL(CheckBox);
			// --
			static const char className[];
			static Lunar<CheckBox>::RegType methods[];
	};

	class Icon : public AWidget, public gcn::Icon2 {
		private:
			typedef struct { gcn::Image *pT; } userdataType;
			FIFE::LuaRef m_image_ref;
		public:
			Icon(lua_State *L);
			virtual ~Icon();

			int l_setImage(lua_State *L);

			LUAGUI_WIDGET_IMPL(Icon2);

			// --
			static const char className[];
			static Lunar<Icon>::RegType methods[];
	};

	class TextField : public AWidget, public gcn::TextField {
		public:
			TextField(lua_State *L);
			virtual ~TextField();
			int l_setText(lua_State *L);
			int l_getText(lua_State *L);
			int l_adjustSize(lua_State *L); // to be removed
			int l_adjustHeight(lua_State *L); // as well
			int l_getCaretPosition(lua_State *L);
			int l_setCaretPosition(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(TextField);
			LUAGUI_WIDGET_IMPL(TextField);
			// --
			static const char className[];
			static Lunar<TextField>::RegType methods[];
	};

	class Button : public AWidget, public gcn::Button {
		public:
			Button(lua_State *L);
			virtual ~Button();
			int l_isPressed(lua_State *L);
			int l_adjustSize(lua_State *L); // to be removed
			int l_getAlignment(lua_State *L);
			int l_setAlignment(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);

			void mouseIn();

			// -
			LUAGUI_SETFONT_IMPL(Button);
			LUAGUI_WIDGET_IMPL(Button);
			// --
			static const char className[];
			static Lunar<Button>::RegType methods[];
	};

	class TwoButton : public AWidget, public gcn::TwoButton {
		private:
			typedef struct { gcn::Image *pT; } userdataType;
		public:

			TwoButton(lua_State *L);
			virtual ~TwoButton() {};

			int l_getAlignment(lua_State *L);
			int l_setAlignment(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);

			// --
			LUAGUI_SETFONT_IMPL(Button);
			LUAGUI_WIDGET_IMPL(TwoButton);
			// --
			static const char className[];
			static Lunar<TwoButton>::RegType methods[];
	};

	class Container : public AWidget, public gcn::Container {
		public:
			Container(lua_State *L);
			virtual ~Container();

			int l_moveToTop(lua_State *L);
			int l_moveToBottom(lua_State *L);
			int l_setOpaque(lua_State *L);
			int l_isOpaque(lua_State *L);
			int l_add(lua_State *L);
			int l_remove(lua_State *L);
			int l_clear(lua_State *L);

			// -
			LUAGUI_WIDGET_IMPL(Container);
			// --
			static const char className[];
			static Lunar<Container>::RegType methods[];
		private:
			Widget2RefPtrMap m_child_refs;
	};

	class ScrollArea : public AWidget, public gcn::ScrollArea {
		public:
			ScrollArea(lua_State *L);
			virtual ~ScrollArea();

			int l_setContent(lua_State *L);
			int l_setScrollPolicy(lua_State *L);
			int l_getScrollPolicy(lua_State *L);
			int l_setScrollbarWidth(lua_State *L);
			int l_setScrollAmount(lua_State *L);
			int l_getMaxScroll(lua_State *L);

			// -
			LUAGUI_WIDGET_IMPL(ScrollArea);
			// --
			static const char className[];
			static Lunar<ScrollArea>::RegType methods[];
			
	};

	class TextBox : public AWidget, public gcn::TextBox {
		public:
			TextBox(lua_State *L);
			virtual ~TextBox();
			int l_setText(lua_State *L);
			int l_getText(lua_State *L);
			int l_getNumberOfRows(lua_State *L);
			int l_getTextRow(lua_State *L);
			int l_setTextRow(lua_State *L);
			int l_getCaretPosition(lua_State *L);
			int l_setCaretPosition(lua_State *L);
			int l_isOpaque(lua_State *L);
			int l_setOpaque(lua_State *L);
			int l_isEditable(lua_State *L);
			int l_setEditable(lua_State *L);
			int l_addRow(lua_State *L);
			int l_clear(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(TextBox);
			LUAGUI_WIDGET_IMPL(TextBox);
			// --
			static const char className[];
			static Lunar<TextBox>::RegType methods[];
	};

	class ListBox : public AWidget, public gcn::ListBox {
		private:
			typedef struct { gcn::ListModel *pT; } userdataType;
			FIFE::LuaRef m_listmodel_ref;
		public:
			ListBox(lua_State *L);
			virtual ~ListBox();
			int l_getSelected(lua_State *L);
			int l_setSelected(lua_State *L);
			int l_setListModel(lua_State *L);

			LUAGUI_SETFONT_IMPL(ListBox);
			LUAGUI_WIDGET_IMPL(ListBox);
			// --
			static const char className[];
			static Lunar<ListBox>::RegType methods[];
	};

	class DropDown : public AWidget, public gcn::DropDown {
		private:
			typedef struct { gcn::ListModel *pT; } userdataType;
			FIFE::LuaRef m_listmodel_ref;
		public:
			DropDown(lua_State *L);
			virtual ~DropDown();
			int l_getSelected(lua_State *L);
			int l_setSelected(lua_State *L);
			int l_setListModel(lua_State *L);

			int l_setFont(lua_State *L) {
				gcn::Font* fp = lua2font_cast(L);
				if (fp != NULL) {
					f_setFont(fp);
				}
				else
					FIFE::Log("AWidget", FIFE::Log::LEVEL_WARN) << "Cannot set font; invalid font object";
				return 0;
			}
			inline void f_setFont(gcn::Font *fp) { 
				mListBox->setFont(fp);
				setFont(fp);
			}

			LUAGUI_WIDGET_IMPL(DropDown);
			// --
			static const char className[];
			static Lunar<DropDown>::RegType methods[];

	};

	class RadioButton : public AWidget, public gcn::RadioButton {
		public:
			RadioButton(lua_State *L);
			virtual ~RadioButton();
			
			int l_isMarked(lua_State *L);
			int l_setMarked(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);
			int l_getGroup(lua_State *L);
			int l_setGroup(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(RadioButton);
			LUAGUI_WIDGET_IMPL(RadioButton);

			// --
			static const char className[];
			static Lunar<RadioButton>::RegType methods[];

	};

	class Slider : public AWidget, public gcn::Slider {
		public:
			Slider(lua_State *L);
			virtual ~Slider();

			int l_setScale(lua_State *L);
			int l_getScale(lua_State *L);
			int l_setValue(lua_State *L);
			int l_getValue(lua_State *L);

			int l_setOrientation(lua_State *L);
			int l_getOrientation(lua_State *L);
			
			LUAGUI_WIDGET_IMPL(Slider);

			// --
			static const char className[];
			static Lunar<Slider>::RegType methods[];

	};

	class Window : public AWidget, public gcn::Window {
		public:
			Window(lua_State *L);
			virtual ~Window();

			int l_getAlignment(lua_State *L); // FIXME: missing consts
			int l_setAlignment(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);
			int l_isOpaque(lua_State *L);
			int l_setOpaque(lua_State *L);
			int l_isMovable(lua_State *L);
			int l_setMovable(lua_State *L);
			int l_getTitleBarHeight(lua_State *L);
			int l_setTitleBarHeight(lua_State *L);
			int l_getPadding(lua_State *L);
			int l_setPadding(lua_State *L);
			int l_setContent(lua_State *L);
			int l_moveToTop(lua_State *L);
			int l_moveToBottom(lua_State *L);
			int l_add(lua_State *L);
			int l_remove(lua_State *L);
			int l_clear(lua_State *L);

			int l_setFont(lua_State *L) {
				gcn::Font* fp = lua2font_cast(L);
				if (fp != NULL) {
					f_setFont(fp);
				} else {
					// FIXME: WTF? the cast causes a long jump
					// anyway. 
					FIFE::Log("AWidget", FIFE::Log::LEVEL_WARN) << "Cannot set font; invalid font object";
				}
				return 0;
			}
			inline void f_setFont(gcn::Font *fp) { 
				setFont(fp);
			}
			LUAGUI_WIDGET_IMPL(Window);

			// --
			static const char className[];
			static Lunar<Window>::RegType methods[];
		private:
			Widget2RefPtrMap m_child_refs;

	};

	class ClickLabel : public AWidget, public gcn::Button {
		public:
			ClickLabel(lua_State *L);
			virtual ~ClickLabel();

			void draw(gcn::Graphics* graphics);

			int l_getAlignment(lua_State *L);
			int l_setAlignment(lua_State *L);
			int l_getCaption(lua_State *L);
			int l_setCaption(lua_State *L);
			// -
			LUAGUI_SETFONT_IMPL(Button);
			LUAGUI_WIDGET_IMPL(Button);
			// --
			static const char className[];
			static Lunar<ClickLabel>::RegType methods[];

	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
