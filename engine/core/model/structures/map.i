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
#include "model/structures/map.h"
%}

%include "util/base/utilbase.i"

namespace FIFE {
  class Layer;
  class Camera;
}

namespace std {
  %template(LayerList) list<FIFE::Layer*>;
  %template(LayerVector) vector<FIFE::Layer*>;
  %template(CameraVector) vector<FIFE::Camera*>;
}

namespace FIFE {

	class Map;
	class Rect;
	class TriggerController;

	%feature("director") MapChangeListener;
	class MapChangeListener {
	public:
		virtual ~MapChangeListener() {};
		virtual void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) = 0;
		virtual void onLayerCreate(Map* map, Layer* layer) = 0;
		virtual void onLayerDelete(Map* map, Layer* layer) = 0;
	};

	class Map : public FifeClass {
		public:

			Map(const std::string& identifier, RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers, TimeProvider* tp_master=NULL);
			~Map();

			const std::string& getId() const;
			void setId(const std::string& id);

			Layer* createLayer(const std::string& identifier, CellGrid* grid);
			void deleteLayer(Layer* index);

			std::list<Layer*> getLayers() const;
			Layer* getLayer(const std::string& id);

			uint32_t getLayerCount() const;
			void deleteLayers();

			void getMinMaxCoordinates(ExactModelCoordinate& min, ExactModelCoordinate& max);

			void setTimeMultiplier(float multip);
			double getTimeMultiplier() const;
			
			void addChangeListener(MapChangeListener* listener);
			void removeChangeListener(MapChangeListener* listener);
			bool isChanged();
			std::vector<Layer*>& getChangedLayers();
			Camera* addCamera(const std::string& id, Layer *layer, const Rect& viewport);
			void removeCamera(const std::string& id);
			Camera* getCamera(const std::string& id);
			std::vector<Camera*>& getCameras();
			
			void setFilename(const std::string& file);
			const std::string& getFilename() const;

			void initializeCellCaches();
			void finalizeCellCaches();

			TriggerController* getTriggerController() const;
	};
}
