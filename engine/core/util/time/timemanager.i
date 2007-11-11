%module fife
%{
#include "util/time/timemanager.h"
%}

namespace FIFE {
	class TimeEvent;
	class TimeManager {
	public:
		TimeManager();
		virtual ~TimeManager();
		void update();
		unsigned long getTime() const;
		unsigned long getTimeDelta() const;
		double getAverageFrameTime() const;
		void printStatistics() const;
		void registerEvent(TimeEvent* event);
		void unregisterEvent(TimeEvent* event);
        };
}
