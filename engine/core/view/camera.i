%module fife
%{
#include "view/camera.h"
#include "view/rendererbase.h"
%}

%include "view/rendererbase.i"

namespace FIFE {
	typedef Point3D ScreenPoint;
	%template(ScreenPoint) PointType3D<int>;
	
	%apply std::list<Instance*> &OUTPUT { std::list<Instance*>& instances };
	class Camera: public IRendererContainer {
	public:
		~Camera();
		const std::string& getId();
		void setTilt(double tilt);
		double getTilt() const;
		void setRotation(double rotation);
		double getRotation() const;
		void setZoom(double zoom);
		double getZoom() const;
		void setLocation(Location location);
		Location getLocation() const;
		Location& getLocationRef();
		void attach(Instance *instance);
		void detach();
		Instance* getAttached() const;
		void setViewPort(const Rect& viewport);
		const Rect& getViewPort() const;
		void setCellImageDimensions(unsigned int width, unsigned int height);
		Point getCellImageDimensions();
		ScreenPoint toScreenCoordinates(ExactModelCoordinate map_coords);
		ExactModelCoordinate toMapCoordinates(ScreenPoint screen_coords, bool z_calculated=true);
		void setEnabled(bool enabled);
		bool isEnabled();
		
		void getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances);
		RendererBase* getRenderer(const std::string& name);
		void resetRenderers();

		void refresh();
	private:
		Camera();
	};
	%clear std::list<Instance*>& instances;
}

%clear InstanceLst& instances;
