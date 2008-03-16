%module fife
%{
#include "eventchannel/trigger/ec_itriggercontroller.h"
%}

namespace FIFE {

	class Trigger;

	class ITriggerController {
		
	public:
		virtual ~ITriggerController() { }

		virtual void registerTrigger(Trigger& trigger) = 0;
		
		virtual void unregisterTrigger(Trigger& trigger) = 0;

	};
}