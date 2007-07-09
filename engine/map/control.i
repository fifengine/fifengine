%module mapcontrol
%{
#include "map/control.h"
%}

namespace FIFE { 
	class CannotOpenFile;
	namespace map {

	class MapLoader;

	class Control {
		public:
			Control();
			~Control();

			void addMapLoader(MapLoader* loader);

			void load(const std::string& filename) throw(CannotOpenFile);
      
			void start();
			void stop();
			bool isRunning();

			size_t getCurrentElevation();
			void setElevation(size_t);

		private:
	};
}}
