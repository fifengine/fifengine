%module view
%{
#include "view/view.h"
#include "view/camera.h"
%}

namespace FIFE {
	class View {
	public:
		~View();
	private:
		View(SDL_Screen* screen);
	};

	class Camera {
	public:
		Camera(View* View);
		~Camera();

		void setTilt(double tilt);
		double getTilt();

		void setRotation(double rotation);
		double getRotation();

		void setZoom(double zoom);
		double getZoom();
	};
}
