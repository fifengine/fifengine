%module(directors="1") widgets
%{
#include <guichan.hpp>
%}


namespace gcn {
	%feature("director") ListModel;
	class ListModel
	{
	public:
		virtual ~ListModel() { }
		virtual int getNumberOfElements() = 0;
		virtual std::string getElementAt(int i) = 0;
	};

	class Color;
	class Font;
    class Widget {
    public:
        Widget();
        virtual ~Widget();
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
        virtual void setBorderSize(unsigned int borderSize);
        virtual unsigned int getBorderSize() const;
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
        virtual bool hasModalFocus() const;
        virtual bool hasModalMouseInputFocus() const;
        virtual Widget *getWidgetAt(int x, int y);
        virtual void moveToTop(Widget* widget) { };
        virtual void moveToBottom(Widget* widget) { };
        virtual void focusNext() { };
        virtual void focusPrevious() { };
	private:
		virtual void draw(Graphics* graphics) = 0;
	};

/*	class BasicContainer: public Widget { 
	public:
        BasicContainer();
        virtual ~BasicContainer();
	};
*/
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

	%feature("notabstract") Label;
	class Label: public Widget {
	public:
		Label();
		Label(const std::string& caption);
		virtual const std::string &getCaption() const;
		virtual void setCaption(const std::string& caption);
		virtual void setAlignment(unsigned int alignment);
		virtual unsigned int getAlignment();
		virtual void adjustSize();
	};
}

