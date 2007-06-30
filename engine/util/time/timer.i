%module(directors="1") timer
%{
#include "util/time/timer.h"
%}

namespace FIFE {
	%feature("director") TimerListener;
	class TimerListener {
		public:
			virtual void onTimer();
			virtual ~TimerListener();
	};
	class Timer {
		public:
			typedef boost::function0<void> type_callback;
			Timer();
			virtual ~Timer(); 
			void setInterval(unsigned long msec);
			void start();
			void stop();
			void setCallback(const type_callback& callback);
			void setListener(TimerListener* listener);
	};
}
