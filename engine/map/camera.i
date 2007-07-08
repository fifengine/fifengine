%module camera
%{
#include "map/camera.h"
%}

%include "util/point.h"
%include "util/rect.h"

namespace FIFE { namespace map {

  class Control;

	class Camera {
		public:
			Camera(Control* map_control);
			~Control();

			void setViewport(const Rect& viewport);

			void moveTo(const Point& gridPosition);
			void moveBy(const Point& delta);

			void render();

		private:
	};
}}
