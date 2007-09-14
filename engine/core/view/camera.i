%module camera
%{
#include "view/camera.h"
%}

namespace FIFE {
	typedef Point3D ScreenPoint;
	%template(ScreenPoint) PointType3D<int>;
	
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
		void setCellImageDimensions(unsigned int width, unsigned int height);
		ScreenPoint toScreenCoordinates(ExactModelCoordinate elevation_coords);
		ExactModelCoordinate toElevationCoordinates(ScreenPoint screen_coords);
	};
}
