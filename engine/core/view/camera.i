%module camera
%{
#include "view/camera.h"
%}

%include "util/point.h"
%include "util/rect.h"

typedef signed int int32_t;

namespace FIFE { namespace map {

  class View;

	class Camera {
		public:
			~Camera();

			void setViewport(const Rect& viewport);

			void moveTo(const Point& gridPosition);
			void moveBy(const Point& delta);

		private:
			Camera(View* view);

	};
}}
