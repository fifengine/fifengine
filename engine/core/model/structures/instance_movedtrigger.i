%module fife
%{
#include "model/structures/instance_movedtrigger.h"
%}

%include "eventchannel/trigger/ec_trigger.i"

%include "instance.i"
%include "location.i"

namespace FIFE {

  class InstanceMovedTrigger : public Trigger {

  public:
	InstanceMovedTrigger(ITriggerListener& listener, Instance* instance);

	bool requirementsMet();

  protected:
	Instance* m_instance;
	Location m_lastlocation;
  };

}