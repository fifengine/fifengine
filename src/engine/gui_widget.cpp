#include "gui_widget.h"
#include "engine.h"
#include "gui.h"


namespace FIFE {
	Widget::Widget() : m_attached(false) {};
	Widget::~Widget() {};

	void Widget::attach() {
		if(!m_attached) Engine::instance()->getGUI()->add( topWidget() );
		m_attached = true;
	}

	void Widget::detach() {
		if(m_attached) Engine::instance()->getGUI()->remove( topWidget() );
		m_attached = false;
	}
	
	bool Widget::isAttached() {
		return m_attached;
	}
}; //FIFE

