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
#include <fifechan.hpp>
#include "gui/fifechan/widgets/utf8textfield.h"
#include "gui/fifechan/widgets/utf8textbox.h"
#include "gui/fifechan/widgets/twobutton.h"
#include "gui/fifechan/widgets/togglebutton.h"
#include "gui/fifechan/widgets/clicklabel.h"
#include "gui/fifechan/widgets/icon2.h"
#include "gui/fifechan/widgets/percentagebar.h"
%}

namespace fcn {
	class ActionListener;
	class Color;
	class Font;
	class Image;
	class KeyListener;
	class MouseListener;
	class WidgetListener;
	class Widget;

	%feature("notabstract") UTF8TextField;
	%rename(TextField) UTF8TextField;
	class UTF8TextField: public Widget {
	public:
		UTF8TextField();
		UTF8TextField(const std::string& text);
		virtual void setText(const std::string& text);
		virtual std::string getText() const;
		virtual void adjustSize();
		virtual void adjustHeight();
		virtual void setCaretPosition(uint32_t position);
		virtual uint32_t getCaretPosition() const;
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
		void setDownOffset(int32_t x, int32_t y);
		int32_t getDownXOffset();
		int32_t getDownYOffset();
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
		void setSpacing(uint32_t spacing);
		uint32_t getSpacing() const;
		void setUpImage(Image* image);
		void setDownImage(Image* image);
		void setHoverImage(Image* image);
		void setDownOffset(int32_t x, int32_t y);
		int32_t getDownXOffset() const;
		int32_t getDownYOffset() const;
		bool isToggled() const;
		void setToggled(bool toggled);
		void setGroup(const std::string &group);
		const std::string &getGroup() const;
	};
	
	
	%feature("notabstract") UTF8TextBox;
	%rename(TextBox) UTF8TextBox;
	class UTF8TextBox: public Widget {
	public:
		UTF8TextBox();
		UTF8TextBox(const std::string& text);
		virtual void setText(const std::string& text);
		virtual std::string getText() const;
		virtual std::string getTextRow(int row) const;
		virtual void setTextRow(int32_t row, const std::string& text);
		virtual uint32_t getNumberOfRows() const;
		virtual uint32_t getCaretPosition() const;
		virtual void setCaretPosition(uint32_t position);
		virtual uint32_t getCaretRow() const;
		virtual void setCaretRow(int32_t row);
		virtual uint32_t getCaretColumn() const;
		virtual void setCaretColumn(int32_t column);
		virtual void setCaretRowColumn(int32_t row, int32_t column);
		virtual void scrollToCaret();
		virtual bool isEditable() const;
		virtual void setEditable(bool editable);
		virtual void addRow(const std::string row);
		virtual bool isOpaque();
		virtual void setOpaque(bool opaque);
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
		virtual void setWidth(int32_t width);
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
	
	%feature("notabstract") PercentageBar;
	class PercentageBar: public Widget {
	public:
		PercentageBar();
		virtual ~PercentageBar();
		virtual void setForegroundImage(Image* image);
        virtual void setOrientation(PercentageBar::Orientation orientation);
        virtual PercentageBar::Orientation getOrientation() const;
        virtual int32_t getValue() const;
        virtual void setValue(int32_t value);
		
		enum Orientation
		{
			HORIZONTAL = 0,
			VERTICAL
		};
	};
}



