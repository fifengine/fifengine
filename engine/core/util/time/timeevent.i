%module fife
%{
#include "util/time/timeevent.h"
#include "util/fifeclass.h"
%}

namespace FIFE {
	%feature("director") TimeEvent;
	class TimeEvent: public FifeClass {
	public:
		TimeEvent(int period = -1);
		virtual ~TimeEvent();
		virtual void updateEvent(unsigned long time) = 0;
		void setPeriod(int period);
		int getPeriod();
	};
}
