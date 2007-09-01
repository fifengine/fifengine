%module camera
%{
#include "view/camera.h"
%}

namespace FIFE {
	class Camera {
	public:
		Camera();
		~Camera();
		void setTilt(double tilt);
		double getTilt() const;
		void setRotation(double rotation);
		double getRotation() const;
		void setZoom(double zoom);
		double getZoom() const;
		void setLocation(Location location);
		const Location& getLocation() const;
		void setViewPort(const Rect& viewport);
		const Rect& getViewPort() const;
		void setScreenCellWidth(unsigned int width);
	};
}
