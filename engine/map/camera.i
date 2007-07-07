%module camera
%{
#include "map/camera.h"
%}

%include "util/point.h"

namespace FIFE { namespace map {

  class Control;

	class Camera {
		public:
			Camera(Control* map_control);
			~Control();

			void moveTo(const Point& gridPosition);
			void moveBy(const Point& delta);

			void render();

		private:
	};
}}
