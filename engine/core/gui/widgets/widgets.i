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

%module fife
%{
#include <guichan.hpp>
#include <guichan/mouseevent.hpp>
#include "gui/widgets/utf8textfield.h"
#include "gui/widgets/utf8textbox.h"
#include "gui/widgets/twobutton.h"
#include "gui/widgets/togglebutton.h"
#include "gui/widgets/clicklabel.h"
#include "gui/widgets/icon2.hpp"
%}

namespace gcn {
	class Font;
	class Image;
	class ActionListener;
	class MouseListener;
	class KeyListener;

	%nodefaultctor;
	class Graphics {
	public:
		enum Alignment {
			LEFT = 0,
			CENTER,
			RIGHT
		};
	};
	%clearnodefaultctor;

	
	class Color {
	public:
		Color();
		Color(int color);
		Color(int r, int g, int b, int a = 255);
		Color operator+(const Color& color) const;
		Color operator-(const Color& color) const;
		Color operator*(float value) const;
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;
		int r;
		int g;
		int b;
		int a;
	};

	class Widget {
	public:
/* 		Widget(); */
/* 		virtual ~Widget(); */
		virtual void setWidth(int width);
		virtual int getWidth() const;
		virtual void setHeight(int height);
		virtual int getHeight() const;
		virtual void setSize(int width, int height);
		virtual void setX(int x);
		virtual int getX() const;
		virtual void setY(int y);
		virtual int getY() const;
		virtual void setPosition(int x, int y);
		virtual void setFrameSize(unsigned int frameSize);
		virtual unsigned int getFrameSize() const;
		virtual void setFocusable(bool focusable);
		virtual bool isFocusable() const;
		virtual bool isFocused() const;
		virtual void setEnabled(bool enabled);
		virtual bool isEnabled() const;
		virtual void setVisible(bool visible);
		virtual bool isVisible() const;
		virtual void setBaseColor(const Color& color);
		virtual const Color& getBaseColor() const;
		virtual void setForegroundColor(const Color& color);
		virtual const Color& getForegroundColor() const;
		virtual void setBackgroundColor(const Color& color);
		virtual const Color& getBackgroundColor() const;
		virtual void setSelectionColor(const Color& color);
		virtual const Color& getSelectionColor() const;
		virtual void requestFocus();
		virtual void requestMoveToTop();
		virtual void requestMoveToBottom();
		virtual void setActionEventId(const std::string& actionEventId);
		virtual const std::string& getActionEventId() const;
		virtual void getAbsolutePosition(int& x, int& y) const;
		Font *getFont() const;
		static void setGlobalFont(Font* font);
		virtual void setFont(Font* font);
		virtual bool isTabInEnabled() const;
		virtual void setTabInEnabled(bool enabled);
		virtual bool isTabOutEnabled() const;
		virtual void setTabOutEnabled(bool enabled);
		virtual void requestModalFocus();
		virtual void requestModalMouseInputFocus();
		virtual void releaseModalFocus();
		virtual void releaseModalMouseInputFocus();
		virtual bool isModalFocused() const;
		virtual bool isModalMouseInputFocused() const;
		virtual Widget *getWidgetAt(int x, int y);
		virtual void moveToTop(Widget* widget) { };
		virtual void moveToBottom(Widget* widget) { };
		virtual void focusNext() { };
		virtual void focusPrevious() { };
		virtual void addActionListener(ActionListener* actionListener);
		virtual void removeActionListener(ActionListener* actionListener);
		virtual void addMouseListener(MouseListener* actionListener);
		virtual void removeMouseListener(MouseListener* actionListener);
		virtual void addKeyListener(KeyListener* actionListener);
		virtual void removeKeyListener(KeyListener* actionListener);
/* 	protected: */
		virtual void draw(Graphics* graphics) = 0;
	};
	
	%feature("notabstract") Container;
	class Container: public Widget {
	public:
		Container();
		virtual ~Container();
		virtual void setOpaque(bool opaque);
		virtual bool isOpaque() const;
		virtual void add(Widget* widget);
		virtual void add(Widget* widget, int x, int y);
		virtual void remove(Widget* widget);
		virtual void clear();
	};
	
	%feature("notabstract") CheckBox;
	class CheckBox: public Widget {
	public:
		CheckBox();
		virtual ~CheckBox();
		virtual bool isSelected() const;
		virtual void setSelected(bool marked);
		virtual const std::string &getCaption() const;
		virtual void setCaption(const std::string& caption);
		virtual void adjustSize();
	};

	%feature("notabstract") UTF8TextField;
	%rename(TextField) UTF8TextField;
	class UTF8TextField: public Widget {
	public:
		UTF8TextField();
		UTF8TextField(const std::string& text);
		virtual void setText(const std::string& text);
		virtual const std::string& getText() const;
		virtual void adjustSize();
		virtual void adjustHeight();
		virtual void setCaretPosition(unsigned int position);
		virtual unsigned int getCaretPosition() const;
	};

	%feature("notabstract") Button;
	class Button: public Widget {
	public:
		Button();
		Button(const std::string& caption);
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		virtual void adjustSize();
		/*virtual bool isPressed() const;*/
	};
		
	%feature("notabstract") TwoButton;
	class TwoButton: public Widget {
	public:
		TwoButton(Image *up_image = 0, Image *down_image = 0, Image *hover_image = 0, const char * caption = "");
		~TwoButton();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		void setUpImage(Image* image);
		void setDownImage(Image* image);
		void setHoverImage(Image* image);
		void setDownOffset(int x, int y);
		int getDownXOffset();
		int getDownYOffset();
	};
	
	%feature("notabstract") ToggleButton;
	class ToggleButton: public Widget {
	public:
		ToggleButton(Image *up_image = 0, Image *down_image = 0, Image *hover_image = 0, const char * caption = "", const char * group = "");
		~ToggleButton();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		void setSpacing(unsigned int spacing);
		unsigned int getSpacing() const;
		void setUpImage(Image* image);
		void setDownImage(Image* image);
		void setHoverImage(Image* image);
		void setDownOffset(int x, int y);
		int getDownXOffset() const;
		int getDownYOffset() const;
		bool isToggled() const;
		void setToggled(bool toggled);
		void setGroup(const std::string &group);
		const std::string &getGroup() const;
	};

	%feature("notabstract") ScrollArea;
	class ScrollArea: public Widget {
	public:
		ScrollArea();
		ScrollArea(Widget *content);
		ScrollArea(Widget *content,ScrollArea::ScrollPolicy hPolicy,ScrollArea::ScrollPolicy vPolicy);
		virtual ~ScrollArea();
		virtual void setContent(Widget* widget);
		virtual Widget* getContent();
		virtual void setHorizontalScrollPolicy(ScrollArea::ScrollPolicy hPolicy);
		virtual ScrollArea::ScrollPolicy getHorizontalScrollPolicy();
		virtual void setVerticalScrollPolicy(ScrollArea::ScrollPolicy vPolicy);
		virtual ScrollArea::ScrollPolicy getVerticalScrollPolicy();
		virtual void setScrollPolicy(ScrollArea::ScrollPolicy hPolicy, ScrollArea::ScrollPolicy vPolicy);
		virtual void setVerticalScrollAmount(int vScroll);
		virtual int getVerticalScrollAmount();
		virtual void setHorizontalScrollAmount(int hScroll);
		virtual int getHorizontalScrollAmount();
		virtual void setScrollAmount(int hScroll, int vScroll);
		virtual int getHorizontalMaxScroll();
		virtual int getVerticalMaxScroll();
		virtual void setScrollbarWidth(int width);
		virtual int getScrollbarWidth();
		virtual void setLeftButtonScrollAmount(int amount);
		virtual void setRightButtonScrollAmount(int amount);
		virtual void setUpButtonScrollAmount(int amount);
		virtual void setDownButtonScrollAmount(int amount);
		virtual int getLeftButtonScrollAmount();
		virtual int getRightButtonScrollAmount();
		virtual int getUpButtonScrollAmount();
		virtual int getDownButtonScrollAmount();
		enum ScrollPolicy
		{
			SHOW_ALWAYS,
			SHOW_NEVER,
			SHOW_AUTO
		};
	};
	
	
	%feature("notabstract") UTF8TextBox;
	%rename(TextBox) UTF8TextBox;
	class UTF8TextBox: public Widget {
	public:
		UTF8TextBox();
		UTF8TextBox(const std::string& text);
		virtual void setText(const std::string& text);
		virtual std::string getText() const;
		virtual const std::string& getTextRow(int row) const;
		virtual void setTextRow(int row, const std::string& text);
		virtual unsigned int getNumberOfRows() const;
		virtual unsigned int getCaretPosition() const;
		virtual void setCaretPosition(unsigned int position);
		virtual unsigned int getCaretRow() const;
		virtual void setCaretRow(int row);
		virtual unsigned int getCaretColumn() const;
		virtual void setCaretColumn(int column);
		virtual void setCaretRowColumn(int row, int column);
		virtual void scrollToCaret();
		virtual bool isEditable() const;
		virtual void setEditable(bool editable);
		virtual void addRow(const std::string row);
		virtual bool isOpaque();
		virtual void setOpaque(bool opaque);
	};

	%feature("director") ListModel;
	class ListModel
	{
	public:
		virtual ~ListModel() { }
		virtual int getNumberOfElements() = 0;
		virtual std::string getElementAt(int i) = 0;
	};

	%feature("notabstract") ListBox;
	class ListBox: public Widget {
	public:
		ListBox();
		ListBox(ListModel *listModel);
		virtual ~ListBox() { }
		virtual int getSelected();
		virtual void setSelected(int selected);
		virtual void setListModel(ListModel *listModel);
		virtual ListModel *getListModel();
		virtual void adjustSize();
		virtual bool isWrappingEnabled();
		virtual void setWrappingEnabled(bool wrapping);
	};

	%feature("notabstract") DropDown;
	class DropDown: public Widget {
	public:
		DropDown(ListModel *listModel = NULL,
		         ScrollArea *scrollArea = NULL,
		         ListBox *listBox = NULL);
		virtual ~DropDown();
		virtual int getSelected();
		virtual void setSelected(int selected);
		virtual void setListModel(ListModel *listModel);
		virtual ListModel *getListModel();
		virtual void adjustHeight();
		virtual void setBaseColor(const Color& color);
		virtual const Color& getBaseColor() const;
		virtual void setForegroundColor(const Color& color);
		virtual const Color& getForegroundColor() const;
		virtual void setBackgroundColor(const Color& color);
		virtual const Color& getBackgroundColor() const;
		virtual void setSelectionColor(const Color& color);
		virtual const Color& getSelectionColor() const;
	};

	%feature("notabstract") RadioButton;
	class RadioButton: public Widget {
	public:
		RadioButton();
		RadioButton(const std::string &caption,
					const std::string &group,
					bool marked=false);
		virtual ~RadioButton();
		virtual bool isSelected() const;
		virtual void setSelected(bool marked);
		virtual const std::string &getCaption() const;
		virtual void setCaption(const std::string caption);
		virtual void setGroup(const std::string &group);
		virtual const std::string &getGroup() const;
		virtual void adjustSize();
	};

	%feature("notabstract") Slider;
	class Slider: public Widget {
	public:
		Slider(double scaleEnd = 1.0);
		Slider(double scaleStart, double scaleEnd);
		virtual ~Slider() { }
		virtual void setScale(double scaleStart, double scaleEnd);
		virtual double getScaleStart() const;
		virtual void setScaleStart(double scaleStart);
		virtual double getScaleEnd() const;
		virtual void setScaleEnd(double scaleEnd);
		virtual double getValue() const;
		virtual void setValue(double value);
		virtual void setMarkerLength(int length);
		virtual int getMarkerLength() const;
		virtual void setOrientation(Slider::Orientation orientation);
		virtual Slider::Orientation getOrientation() const;
		virtual void setStepLength(double length);
		virtual double getStepLength() const;
		enum Orientation
		{
			HORIZONTAL = 0,
			VERTICAL
		};
	};

	%feature("notabstract") Window;
	class Window: public Container {
	public:
		Window();
		Window(const std::string& caption);
		virtual ~Window();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		virtual void setPadding(unsigned int padding);
		virtual unsigned int getPadding() const;
		virtual void setTitleBarHeight(unsigned int height);
		virtual unsigned int getTitleBarHeight();
		virtual void setMovable(bool movable);
		virtual bool isMovable() const;
		virtual void setOpaque(bool opaque);
		virtual bool isOpaque();
		virtual void resizeToContent();
	};

	%feature("notabstract") ClickLabel;
	%rename(Label) ClickLabel;
	class ClickLabel: public Widget {
	public:
		ClickLabel();
		ClickLabel(const std::string& caption);
		virtual ~ClickLabel();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		bool isTextWrapping() const;
		void setTextWrapping(bool);
		virtual void setWidth(int width);
		virtual void adjustSize();
	};

	%feature("notabstract") Icon2;
	%rename(Icon) Icon2;
	class Icon2: public Widget {
	public:
		Icon2(Image* image);
		virtual ~Icon2();
		void setImage(Image* image);
	};
}



