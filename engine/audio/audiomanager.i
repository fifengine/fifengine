%module audiomanager
%{
#include "audio/audiomanager.h"
%}

namespace FIFE { namespace audio {

	%rename(AudioManager) Manager;
	class Manager {
		public:
			Manager();
			~Manager();
			void setAmbientSound(const std::string &file);
			static void setVolume(float vol);
	};
}}