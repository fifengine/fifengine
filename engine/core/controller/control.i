%module mapcontrol
%{
#include "controller/control.h"
%}

%include "util/point.h"
%include "model/structures/objectinfo.i"

namespace FIFE { 
	class CannotOpenFile;

	namespace map {

	class Camera;
	class MapLoader;

	class Control {
		public:
			Control();
			~Control();

			void addMapLoader(MapLoader* loader);

			void load(const std::string& filename) throw(CannotOpenFile);
      
			void start();
			void update();
			void stop();
			bool isRunning();

			Point select(const Point& mousept, size_t layer);

			ObjectInfo* createDynamicObject(size_t layer, Point p, char* prototype);

			Camera* createCamera();
			void deleteCamera(Camera* camera);

		private:
	};
}}
