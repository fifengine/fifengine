%module mapcontrol
%{
#include "map/control.h"
%}

namespace FIFE { namespace map {

  class MapLoader;

	class Control {
		public:
			Control();
			~Control();

			void addMapLoader(MapLoader* loader);

			void load(const std::string& filename);
      
			void start();
			void stop();
			bool isRunning();
			void turn();

			size_t getCurrentElevation();
			void setElevation(size_t);

		private:
	};
}}
