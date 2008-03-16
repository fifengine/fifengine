%module fife
%{
#include "eventchannel/trigger/ec_trigger.h"
%}

%include "ec_trigger.i"
%include "ec_itriggercontroller.i"
%include "ec_itriggerlistener.i"

namespace FIFE {

  class Trigger : public ITriggerController {

  public:

	Trigger(ITriggerListener& listener);

	void registerTrigger(Trigger& trigger);

	void unregisterTrigger(Trigger& trigger);

	void registerListener(ITriggerListener& triggerlistener);

	void unregisterListener(ITriggerListener& triggerlistener);
  };
}