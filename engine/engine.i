%module engine
%{
#include <boost/shared_ptr.hpp>
#include "engine.h"
%}

namespace boost {
	template<class T> class shared_ptr {
	public:
		T * operator-> () const;
	};
}

namespace FIFE {

	namespace audio {
		class Manager;
	}
	class EventManager;
	class VFS;
	class TimeManager;
	class SettingsManager;
	class GUIManager;

	class Engine {
		public:
			Engine();
			virtual ~Engine();
			void initializePumping();
			void finalizePumping();
			void pump();

			audio::Manager* getAudioManager();
			EventManager* getEventManager();
			TimeManager* getTimeManager();
			SettingsManager* getSettingsManager();
			GUIManager* getGuiManager();
	};
}
