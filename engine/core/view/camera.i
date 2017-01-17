/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

%module fife
%{
#include "view/camera.h"
#include "view/rendererbase.h"
%}

%include "view/rendererbase.i"

namespace FIFE {
	typedef Point3D ScreenPoint;
	%template(ScreenPoint) PointType3D<int32_t>;
	
	%apply std::list<Instance*> &OUTPUT { std::list<Instance*>& instances };
	class Camera: public IRendererContainer {
	public:
		~Camera();
		const std::string& getId() const;
		void setId(const std::string& id);
		void setTilt(double tilt);
		double getTilt() const;
		void setRotation(double rotation);
		double getRotation() const;
		void setZoom(double zoom);
		double getZoom() const;
		double getOriginalZToY() const;
		void setZToY(double zToY);
		double getZToY() const;
		void setZToYEnabled(bool enabled);
		bool isZToYEnabled() const;
		void setLocation(Location location);
		Location getLocation() const;
		Location& getLocationRef();
		void attach(Instance *instance);
		void detach();
		Instance* getAttached() const;
		void setViewPort(const Rect& viewport);
		const Rect& getViewPort() const;
		const Rect& getMapViewPort();
		Rect getLayerViewPort(Layer* layer);
		void setCellImageDimensions(uint32_t width, uint32_t height);
		Point getCellImageDimensions();
		Point3D getZOffset(Layer* layer);
		ScreenPoint toScreenCoordinates(const ExactModelCoordinate& map_coords);
		ExactModelCoordinate toMapCoordinates(ScreenPoint screen_coords, bool z_calculated=true);
		void setEnabled(bool enabled);
		bool isEnabled();
		
		void getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances, uint8_t alpha = 0);
		void getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances, uint8_t alpha = 0);
		void getMatchingInstances(Location& loc, std::list<Instance*>& instances, bool use_exactcoordinates=false);
		RendererBase* getRenderer(const std::string& name);
		void resetRenderers();
		
		void setLightingColor(float red, float green, float blue);
		void resetLightingColor();
		std::vector<float> getLightingColor();

		void setOverlayColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
		std::vector<uint8_t> getOverlayColor();
		void resetOverlayColor();
		void setOverlayImage(int32_t id, bool fill = false);
		int32_t getOverlayImage();
		void resetOverlayImage();
		void setOverlayAnimation(AnimationPtr anim, bool fill = false);
		AnimationPtr getOverlayAnimation();
		void resetOverlayAnimation();
	
		void refresh();
	private:
		Camera();
	};
	%clear std::list<Instance*>& instances;
}

%clear InstanceLst& instances;
