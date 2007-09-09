#include "gui_label.h"
#include "gui.h"
#include "engine.h"
#include "exception.h"
#include <guichan.hpp>

#include "log.h"

namespace FIFE {

Label::Label() : m_label(0), m_attached(false)
{
	m_label = new gcn::FIFELabel("");
	m_label->setPosition(10,10);
};

Label::~Label(){
  delete m_label;
};

gcn::Widget* Label::topWidget()
{
	return m_label;
};

void Label::setLabel(const std::string& caption)
{
	if (m_label)
		m_label->setCaption(caption);
};

void Label::setPosition(int x, int y)
{
	m_label->setPosition(x,y);
};

}; //FIFE
