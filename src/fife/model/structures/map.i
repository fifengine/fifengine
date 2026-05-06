// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/structures/map.h"
%}

%include "util/base/utilbase.i"

namespace FIFE {
  class Layer;
  class Camera;
}

%template(LayerList) std::list<FIFE::Layer*>;
%template(LayerVector) std::vector<FIFE::Layer*>;
%template(CameraVector) std::vector<FIFE::Camera*>;

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
			Camera* addCamera(const std::string& id, const Rect& viewport);
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