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

%module fifechan
%{
#include <fifechan.hpp>
%}

namespace fcn {
	class Font;
	class Image;
	class ActionListener;
	class MouseListener;
	class KeyListener;

	inline const char* getVersion();
	inline int getMajor();
	inline int getMinor();
	inline int getPatch();
	inline const char* getHash();

	%nodefaultctor;
	class Graphics {
	public:
		enum Alignment {
			Left = 0,
			Center,
			Right
		};
	};
	%clearnodefaultctor;
	
	class Color {
	public:
		Color();
		Color(int32_t color);
		Color(int32_t r, int32_t g, int32_t b, int32_t a = 255);
		Color operator+(const Color& color) const;
		Color operator-(const Color& color) const;
		Color operator*(float value) const;
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;
		int32_t r;
		int32_t g;
		int32_t b;
		int32_t a;
	};

	class Size {
	public:
		Size(int32_t width = 0, int32_t height = 0); 
		~Size();
		int32_t getWidth() const;
		int32_t getHeight() const;
		void setWidth(int32_t width);
		void setHeight(int32_t height);
	};

	class Point {
	public:
		int32_t x;
		int32_t y;
		explicit Point(int32_t _x = 0, int32_t _y = 0);
		Point(const Point& rhs);
		Point operator+(const Point& p) const;
		Point operator-(const Point& p) const;
		Point& operator+=(const Point& p);
		Point& operator-=(const Point& p);
		Point operator*(const int32_t& i) const;
		Point operator/(const int32_t& i) const;
		bool operator==(const Point& p) const;
		bool operator!=(const Point& p) const;
		int32_t length() const;
		void normalize();
		void rotate(int32_t angle);
		void rotate(const Point& origin, int32_t angle);
		void set(int32_t _x, int32_t _y);
	};

	typedef std::vector<Point> PointVector;

	class Widget {
	public:
		enum SelectionMode {
			Selection_None = 0,
			Selection_Border = 1,
			Selection_Background = 2
		};
/* 		Widget(); */
/* 		virtual ~Widget(); */
		virtual Widget* getParent() const;
		virtual void setWidth(int32_t width);
		virtual int32_t getWidth() const;
		virtual void setHeight(int32_t height);
		virtual int32_t getHeight() const;
		virtual void setSize(int32_t width, int32_t height);
		virtual void setX(int32_t x);
		virtual int32_t getX() const;
		virtual void setY(int32_t y);
		virtual int32_t getY() const;
		virtual void setPosition(int32_t x, int32_t y);
		void setOutlineSize(uint32_t size);
		uint32_t getOutlineSize() const;
		void setBorderSize(uint32_t size);
		uint32_t getBorderSize() const;
		void setMargin(int32_t margin);
		void setMarginTop(int32_t margin);
		int32_t getMarginTop() const;
		void setMarginRight(int32_t margin);
		int32_t getMarginRight() const;
		void setMarginBottom(int32_t margin);
		int32_t getMarginBottom() const;
		void setMarginLeft(int32_t margin);
		int32_t getMarginLeft() const;
		void setPadding(uint32_t padding);
		void setPaddingTop(uint32_t padding);
		uint32_t getPaddingTop() const;
		void setPaddingRight(uint32_t padding);
		uint32_t getPaddingRight() const;
		void setPaddingBottom(uint32_t padding);
		uint32_t getPaddingBottom() const;
		void setPaddingLeft(uint32_t padding);
		uint32_t getPaddingLeft() const;
		virtual void setFocusable(bool focusable);
		virtual bool isFocusable() const;
		virtual bool isFocused() const;
		virtual void setEnabled(bool enabled);
		virtual bool isEnabled() const;
		virtual void setVisible(bool visible);
		virtual bool isVisible() const;
		virtual bool isSetVisible() const;
		virtual void setBaseColor(const Color& color);
		virtual const Color& getBaseColor() const;
		virtual void setForegroundColor(const Color& color);
		virtual const Color& getForegroundColor() const;
		virtual void setBackgroundColor(const Color& color);
		virtual const Color& getBackgroundColor() const;
		virtual void setSelectionColor(const Color& color);
		virtual const Color& getSelectionColor() const;
		virtual void setOutlineColor(const Color& color);
		virtual const Color& getOutlineColor() const;
		virtual void setBorderColor(const Color& color);
		virtual const Color& getBorderColor() const;
		void setSelectionMode(SelectionMode mode);
		SelectionMode getSelectionMode() const;
		void setMinSize(const Size& size);
		const Size& getMinSize() const;
		void setMaxSize(const Size& size);
		const Size& getMaxSize() const;
		void setFixedSize(const Size& size);
		const Size& getFixedSize() const;
		bool isFixedSize() const;
		void setVerticalExpand(bool expand);
		bool isVerticalExpand() const;
		void setHorizontalExpand(bool expand);
		bool isHorizontalExpand() const;
		virtual bool isLayouted();
		virtual void adaptLayout(bool top=true);
		virtual void resizeToContent(bool recursiv=true);
		virtual void adjustSize();
		virtual void expandContent(bool recursiv=true);
		virtual void requestFocus();
		virtual void requestMoveToTop();
		virtual void requestMoveToBottom();
		virtual void setActionEventId(const std::string& actionEventId);
		virtual const std::string& getActionEventId() const;
		virtual void getAbsolutePosition(int32_t& OUTPUT, int32_t& OUTPUT) const;
		Font *getFont() const;
		static void setGlobalFont(Font* font);
		virtual void setFont(Font* font);
		virtual bool isTabInEnabled() const;
		virtual void setTabInEnabled(bool enabled);
		virtual bool isTabOutEnabled() const;
		virtual void setTabOutEnabled(bool enabled);
		virtual bool isModalFocusable() const;
		virtual bool isModalMouseInputFocusable() const;
		virtual void requestModalFocus();
		virtual void requestModalMouseInputFocus();
		virtual void releaseModalFocus();
		virtual void releaseModalMouseInputFocus();
		virtual bool isModalFocused() const;
		virtual bool isModalMouseInputFocused() const;
		virtual Widget *getWidgetAt(int32_t x, int32_t y);
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
		virtual void addWidgetListener(WidgetListener* widgetListener);
		virtual void removeWidgetListener(WidgetListener* widgetListener);
/* 	protected: */
		virtual void draw(Graphics* graphics) = 0;
	};
	
	%feature("notabstract") Spacer;
	class Spacer: public Widget {
	public:
		Spacer();
		virtual ~Spacer();
		virtual void resizeToContent(bool recursiv=true);
	};

	%feature("notabstract") Container;
	class Container: public Widget {
	public:
		Container();
		virtual ~Container();
		virtual void setOpaque(bool opaque);
		virtual bool isOpaque() const;
		virtual void add(Widget* widget);
		virtual void add(Widget* widget, int32_t x, int32_t y);
		virtual void remove(Widget* widget);
		virtual void clear();
		enum LayoutPolicy {
			Absolute,
			Vertical,
			Horizontal,
			Circular
		};
		void setLayout(LayoutPolicy policy);
		LayoutPolicy getLayout() const;
		virtual void setUniformSize(bool uniform);
		virtual bool isUniformSize() const;
		virtual void setVerticalSpacing(uint32_t spacing);
		virtual uint32_t getVerticalSpacing() const;
		virtual void setHorizontalSpacing(uint32_t spacing);
		virtual uint32_t getHorizontalSpacing() const;
		void setBackgroundWidget(Widget* widget);
		Widget* getBackgroundWidget();
	};
	
	%feature("notabstract") Button;
	class Button: public Widget {
	public:
		Button();
		Button(const std::string& caption);
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		void setActive(bool state);
		bool isActive() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		void setDownXOffset(int32_t offset);
		int32_t getDownXOffset() const;
		void setDownYOffset(int32_t offset);
		int32_t getDownYOffset() const;
		void setDownOffset(int32_t x, int32_t y);
		virtual void adjustSize();
		/*virtual bool isPressed() const;*/
	};

	%feature("notabstract") ImageButton;
	class ImageButton: public Button {
	public:
		ImageButton();
		ImageButton(const Image* image);
		virtual ~ImageButton();
		void setUpImage(const Image* image);
		const Image* getUpImage() const;
		void setDownImage(const Image* image);
		const Image* getDownImage() const;
		void setHoverImage(const Image* image);
		const Image* getHoverImage() const;
		void setInactiveUpImage(const Image* image);
		const Image* getInactiveUpImage() const;
		void setInactiveDownImage(const Image* image);
		const Image* getInactiveDownImage() const;
		void setInactiveHoverImage(const Image* image);
		const Image* getInactiveHoverImage() const;
		virtual void resizeToContent(bool recursiv=true);
		virtual void adjustSize();
	};

	%feature("notabstract") ToggleButton;
	class ToggleButton: public ImageButton {
	public:
		ToggleButton();
		ToggleButton(const std::string &caption, const std::string &group, bool selected = false);
		~ToggleButton();
		virtual bool isSelected() const;
		virtual void setSelected(bool selected);
		virtual void toggleSelected();
		void setGroup(const std::string& group);
		const std::string &getGroup() const;
	};

	%feature("notabstract") CheckBox;
	class CheckBox: public ImageButton {
	public:
		enum MarkerStyle {
			Marker_Checkmark = 0,
			Marker_Cross,
			Marker_Dot,
			Marker_Rhombus,
			Marker_Image
		};
		CheckBox();
		virtual ~CheckBox();
		virtual bool isSelected() const;
		virtual void setSelected(bool marked);
		virtual void toggleSelected();
		void setBackgroundImage(const std::string& filename);
		void setBackgroundImage(const Image* image);
		const Image* getBackgroundImage() const;
		MarkerStyle getMarkerStyle() const;
		void setMarkerStyle(MarkerStyle mode);
		virtual void adjustSize();
	};
	
	%feature("notabstract") Icon;
	class Icon: public Widget {
	public:
		Icon(Image* image);
		virtual ~Icon();
		void setImage(Image* image);
		const Image* getImage() const;
		bool isScaling() const;
		void setScaling(bool scale);
		bool isTiling() const;
		void setTiling(bool tile);
		void setOpaque(bool opaque);
		bool isOpaque() const;
		virtual void adjustSize();
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
		virtual void setVerticalScrollAmount(int32_t vScroll);
		virtual int32_t getVerticalScrollAmount();
		virtual void setHorizontalScrollAmount(int32_t hScroll);
		virtual int32_t getHorizontalScrollAmount();
		virtual void setScrollAmount(int32_t hScroll, int32_t vScroll);
		virtual int32_t getHorizontalMaxScroll();
		virtual int32_t getVerticalMaxScroll();
		virtual void setScrollbarWidth(int32_t width);
		virtual int32_t getScrollbarWidth();
		virtual void setLeftButtonScrollAmount(int32_t amount);
		virtual void setRightButtonScrollAmount(int32_t amount);
		virtual void setUpButtonScrollAmount(int32_t amount);
		virtual void setDownButtonScrollAmount(int32_t amount);
		virtual int32_t getLeftButtonScrollAmount();
		virtual int32_t getRightButtonScrollAmount();
		virtual int32_t getUpButtonScrollAmount();
		virtual int32_t getDownButtonScrollAmount();
		enum ScrollPolicy
		{
			ShowAlways,
			ShowNever,
			ShowAuto
		};
	};
	

	%feature("director") ListModel;
	class ListModel
	{
	public:
		virtual ~ListModel() { }
		virtual int32_t getNumberOfElements() = 0;
		virtual std::string getElementAt(int32_t i) = 0;
	};

	%feature("notabstract") ListBox;
	class ListBox: public Widget {
	public:
		ListBox();
		ListBox(ListModel *listModel);
		virtual ~ListBox() { }
		virtual int32_t getSelected();
		virtual void setSelected(int32_t selected);
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
		virtual int32_t getSelected();
		virtual void setSelected(int32_t selected);
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
		virtual void setMarkerLength(int32_t length);
		virtual int32_t getMarkerLength() const;
		virtual void setOrientation(Slider::Orientation orientation);
		virtual Slider::Orientation getOrientation() const;
		virtual void setStepLength(double length);
		virtual double getStepLength() const;
		enum Orientation
		{
			Horizontal = 0,
			Vertical
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
		virtual void setTitleBarHeight(uint32_t height);
		virtual uint32_t getTitleBarHeight();
		virtual void setMovable(bool movable);
		virtual bool isMovable() const;
		virtual void setOpaque(bool opaque);
		virtual bool isOpaque();
		virtual void resizeToContent();
	};

	%feature("notabstract") AdjustingContainer;
	class AdjustingContainer: public Container {
	public:
		AdjustingContainer();
		virtual ~AdjustingContainer();
		
		virtual void setNumberOfColumns(uint32_t numberOfColumns);
		virtual uint32_t getNumberOfColumns() const;
		virtual void setColumnAlignment(uint32_t column, uint32_t alignment);
		virtual uint32_t getColumnAlignment(uint32_t column) const;
		virtual void adjustContent();

		enum {
			LEFT = 0,
			CENTER,
			RIGHT
		};
	};

	%feature("notabstract") FlowContainer;
	class FlowContainer: public Container {
	public:
		FlowContainer();
		virtual ~FlowContainer();
		void setAlignment(FlowContainer::Alignment alignment);
		FlowContainer::Alignment getAlignment() const;
		void adjustContent();

		enum Alignment {
			Left = 0,
			Right,
			Top,
			Bottom,
			Center
		};
	};

	%feature("notabstract") TextBox;
	class TextBox: public Widget {
	public:
		TextBox();
		TextBox(const std::string& text);
		virtual void setText(const std::string& text);
		virtual std::string getText() const;
		virtual std::string getTextRow(int32_t row) const;
		virtual void setTextRow(int32_t row, const std::string& text);
		virtual int32_t getNumberOfRows() const;
		virtual int32_t getCaretPosition() const;
		virtual void setCaretPosition(int32_t position);
		virtual int32_t getCaretRow() const;
		virtual void setCaretRow(int32_t row);
		virtual int32_t getCaretColumn() const;
		virtual void setCaretColumn(int32_t column);
		virtual void setCaretRowColumn(int32_t row, int32_t column);
		virtual void scrollToCaret();
		virtual bool isEditable() const;
		virtual void setEditable(bool editable);
		virtual void addRow(const std::string row);
		virtual bool isOpaque();
		virtual void setOpaque(bool opaque);
	};
	
	
	%feature("notabstract") TextField;
	class TextField: public Widget {
	public:
		TextField();
		TextField(const std::string& text);
		virtual ~TextField();
		virtual void setText(const std::string& text);
		virtual std::string getText() const;
		virtual void adjustSize();
		virtual void adjustHeight();
		virtual void setCaretPosition(uint32_t position);
		virtual uint32_t getCaretPosition() const;
	};
	
	%feature("notabstract") PasswordField;
	class PasswordField : public TextField {
	public:
		PasswordField(const std::string& text = "");
		virtual ~PasswordField();
	};
	
	%feature("notabstract") IconProgressBar;
	class IconProgressBar : public Widget {
	public:
		IconProgressBar();
		IconProgressBar(Image *image, int32_t maxIcons);
		virtual ~IconProgressBar();
		void setImage(Image* image);
		const Image* getImage() const;
		void setMaxIcons(int32_t maxIcons);
		int32_t getMaxIcons() const;
		void setOrientation(IconProgressBar::Orientation orientation);
		IconProgressBar::Orientation getOrientation() const;
		void advance();
		void reset();
		void setIconCount(int icons);
		int getIconCount() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
		
		enum Orientation
		{
			HORIZONTAL = 0,
			VERTICAL
		};
	};

	%feature("notabstract") ImageProgressBar;
	class ImageProgressBar : public Widget {
	public:
		ImageProgressBar();
		ImageProgressBar(Image *image, int32_t maxValue);
		virtual ~ImageProgressBar();
		void setBarImage(Image* image);
		const Image* getBarImage() const;
		void setForegroundImage(Image* image);
		const Image* getForegroundImage() const;
		void setMaxValue(int32_t maxValue);
		int32_t getMaxValue() const;
		void setValue(int32_t value);
		int32_t getValue() const;
		void setOrientation(ImageProgressBar::Orientation orientation);
		ImageProgressBar::Orientation getOrientation() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
		
		enum Orientation {
			HORIZONTAL = 0,
			VERTICAL
		};
	};

	%feature("notabstract") Tab;
	class Tab: public Container {
	public:
		Tab();
		virtual ~Tab();
		void setTabbedArea(fcn::TabbedArea* tabbedArea);
		fcn::TabbedArea* getTabbedArea();
	};

	%feature("notabstract") TabbedArea;
	class TabbedArea: public Widget {
	public:
		TabbedArea();
		virtual ~TabbedArea();
		void setOpaque(bool opaque);
		bool isOpaque() const;
		virtual void addTab(fcn::Tab* tab, Widget* widget);
		virtual void removeTabWithIndex(uint32_t index);
		virtual void removeTab(fcn::Tab* tab);
		uint32_t getNumberOfTabs() const;
		virtual bool isTabSelected(uint32_t index) const;
		virtual bool isTabSelected(fcn::Tab* tab) const;
		virtual void setSelectedTab(uint32_t index);
		virtual void setSelectedTab(fcn::Tab* tab);
		virtual uint32_t getSelectedTabIndex() const;
		fcn::Tab* getSelectedTab() const;
		void setBaseColor(const Color& color);
		void setBackgroundWidget(Widget* widget);
		Widget* getBackgroundWidget();
		void setLayout(Container::LayoutPolicy policy);
		Container::LayoutPolicy getLayout() const;
		virtual void setUniformSize(bool uniform);
		virtual bool isUniformSize() const;
		virtual void setVerticalSpacing(uint32_t spacing);
		virtual uint32_t getVerticalSpacing() const;
		virtual void setHorizontalSpacing(uint32_t spacing);
		virtual uint32_t getHorizontalSpacing() const;
	};

	%feature("notabstract") BarGraph;
	class BarGraph: public Widget {
	public:
		BarGraph();
		BarGraph(int32_t x, int32_t y, int32_t w, int32_t h);
		virtual ~BarGraph();
		void setBarX(int32_t x);
		int32_t getBarX() const;
		void setBarY(int32_t y);
		int32_t getBarY() const;
		void setBarPosition(int32_t x, int32_t y);
		void setBarPosition(const Point& pos);
		void setBarWidth(int32_t w);
		int32_t getBarWidth() const;
		void setBarHeight(int32_t h);
		int32_t getBarHeight() const;
		void setBarSize(int32_t w, int32_t h);
		void setOpaque(bool opaque);
		bool isOpaque() const;
	};

	%feature("notabstract") CurveGraph;
	class CurveGraph: public Widget {
	public:
		CurveGraph();
		CurveGraph(const PointVector& data);
		virtual ~CurveGraph();
		void setPointVector(const PointVector& data);
		const PointVector& getPointVector() const;
		void resetPointVector();
		void setThickness(uint32_t thickness);
		uint32_t getThickness() const;
		void setAutomaticControllPoints(bool acp);
		bool isAutomaticControllPoints() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
	};

	%feature("notabstract") LineGraph;
	class LineGraph: public Widget {
	public:
		LineGraph();
		LineGraph(const PointVector& data);
		virtual ~LineGraph();
		void setPointVector(const PointVector& data);
		const PointVector& getPointVector() const;
		void resetPointVector();
		void setThickness(uint32_t thickness);
		uint32_t getThickness() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
	};

	%feature("notabstract") PieGraph;
	class PieGraph: public Widget {
	public:
		PieGraph();
		PieGraph(const Point& center);
		virtual ~PieGraph();
		void setCenterX(int32_t x);
		void setCenterY(int32_t y);
		void setCenter(int32_t x, int32_t y);
		int32_t getCenterX() const;
		int32_t getCenterY() const;
		void setCenter(const Point& center);
		const Point& getCenter() const;
		void setRadius(int32_t radius);
		int32_t getRadius() const;
		void addSegment(int32_t startAngle, int32_t stopAngle, const Color& color);
		void clearSegments();
		void setOpaque(bool opaque);
		bool isOpaque() const;
	};

	%feature("notabstract") PointGraph;
	class PointGraph: public Widget {
	public:
		PointGraph();
		PointGraph(const PointVector& data);
		virtual ~PointGraph();
		void setPointVector(const PointVector& data);
		const PointVector& getPointVector() const;
		void resetPointVector();
		void setThickness(uint32_t thickness);
		uint32_t getThickness() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
	};
}
namespace std {
	%template(FcnPointVector) vector<fcn::Point>;
}
