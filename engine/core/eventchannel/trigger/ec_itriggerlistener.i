%module fife
%{
#include "eventchannel/trigger/ec_itriggerlistener.h"
%}

namespace FIFE {

	class Trigger;

	class ITriggerListener {
		
	public:
		virtual ~ITriggerListener() { }
		
		virtual void triggerFired() = 0;

	};
}