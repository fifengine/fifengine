%module engine
%{
#include "engine.h"
%}


namespace FIFE {

	namespace audio {
		class Manager;
	}
	class EventManager;
	class VFS;
	class TimeManager;
	class SettingsManager;

	class Engine {
		public:
			Engine();
			virtual ~Engine();
			void pump();

			audio::Manager* getAudioManager();
			EventManager* getEventManager();
			VFS* getVFS();
			TimeManager* getTimeManager();
			SettingsManager* getSettingsManager();
	};
}
