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

#ifndef FIFE_MAP_MAP_H
#define FIFE_MAP_MAP_H

// Standard C++ library includes
#include <list>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"
#include "util/resource/resource.h"
#include "model/metamodel/timeprovider.h"
#include "util/structures/rect.h"

#include "location.h"

namespace FIFE {

	class RendererBase;
	class RenderBackend;
	class Layer;
	class CellGrid;
	class Map;
	class Camera;
	class Instance;
	class TriggerController;

	/** Listener interface for changes happening on map
	 */
	class MapChangeListener {
	public:
		virtual ~MapChangeListener() {};

		/** Called when some layer is changed on map. @see LayerChangeListener
		 * Layer is effectively changed, in case some of its instances
		 * is created, deleted or changed during latest update cycle
		 * @param map where change occurred
		 * @param changedLayers list of layers containing some changes
		 * @note Does not report layer creations and deletions
		 */
		virtual void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) = 0;

		/** Called when some layer gets created on the map
		 * @param map where change occurred
		 * @param layer which got created
		 */
		virtual void onLayerCreate(Map* map, Layer* layer) = 0;

		/** Called when some instance gets deleted on layer
		 * @param map where change occurred
		 * @param layer which will be deleted
		 * @note right after this call, layer actually gets deleted!
		 */
		virtual void onLayerDelete(Map* map, Layer* layer) = 0;
	};

	/** A container of \c Layer(s).
	 *
	 * The actual data is contained in \c Layer objects
	 * @see Layer
	 */
	class Map : public FifeClass {
		public:

			/** Construct a map
			 * To add map to model, one should call Model::addMap (otherwise
			 * map is not registered with the engine properly)
			 */
			Map(const std::string& identifier, RenderBackend* renderbackend,
				const std::vector<RendererBase*>& renderers, TimeProvider* tp_master=NULL);

			/** Destructor
			 */
			~Map();

			/** Get the identifier for this map.
			 */
			const std::string& getId() const { return m_id; }

			/** Sets the identifier for this map.
			 */
			void setId(const std::string& id) { m_id = id; }

			/** Add a Layer to this Map. Map owns the returned pointer to the new Layer, so don't delete it!
			 */
			Layer* createLayer(const std::string& identifier, CellGrid* grid);

			/** Delete a layer from the map
			 */
			void deleteLayer(Layer*);

			/** Get the layers on this map.
			 */
			const std::list<Layer*>& getLayers() const { return m_layers; }

			/** Get the layer with the given id.
			 */
			Layer* getLayer(const std::string& identifier);

			/** Get the overall number of layers
			 */
			uint32_t getLayerCount() const;

			/** Delete all layers from the map
			 */
			void deleteLayers();

			/** Maps coordinate from one layer to another
			 */
			void getMatchingCoordinates(const ModelCoordinate& coord_to_map, const Layer* from_layer,
				const Layer* to_layer, std::vector<ModelCoordinate>& matching_coords) const;

			/** Retrieves the minimum/maximum coordinates of instances on the map.
			 * @param min A reference to a ExactModelCoordinate that will hold the minimum coordinate.
			 * @param max A reference to a ExactModelCoordinate that will hold the maximum coordinate.
			 */
			void getMinMaxCoordinates(ExactModelCoordinate& min, ExactModelCoordinate& max);

			/** Called periodically to update events on map
			 * @returns true, if map was changed
			 */
			bool update();

			/** Sets speed for the map. See Model::setTimeMultiplier.
			 */
			void setTimeMultiplier(float multip) { m_timeProvider.setMultiplier(multip); }

			/** Gets model speed. @see setTimeMultiplier.
			 */
			float getTimeMultiplier() const { return m_timeProvider.getMultiplier(); }

			/** Gets timeprovider used in the map
			 */
			TimeProvider* getTimeProvider() { return &m_timeProvider; }

			/** Adds new change listener
			* @param listener to add
			*/
			void addChangeListener(MapChangeListener* listener);

			/** Removes associated change listener
			* @param listener to remove
			*/
			void removeChangeListener(MapChangeListener* listener);

			/** Returns true, if map information was changed during previous update round
			*/
			bool isChanged() { return !m_changedLayers.empty(); }

			/** Returns layers that were changed during previous update round
			*/
			std::vector<Layer*>& getChangedLayers() { return m_changedLayers; }

			/** Adds camera to the map. The Map takes ownership of the camera
				so don't delete it.
			*/
			Camera* addCamera(const std::string& id, Layer *layer, const Rect& viewport);

			/** Removes a camera from the map
			*/
			void removeCamera(const std::string& id);

			/** Get a camera by its identifier.
			*/
			Camera* getCamera(const std::string& id);

			/** Get a list containing all cameras.
			*/
			const std::vector<Camera*>& getCameras() const;

			/** Return the number of enabled cameras in this map.
			 */
			uint32_t getActiveCameraCount() const;

			void setFilename(const std::string& file) { m_filename = file; }
			const std::string& getFilename() const { return m_filename; }

			/** Adds instance that is to be transferred to another layer.
			* @param instance A pointer to the instance that is to be transferred.
			* @param target A const reference to the target location.
			*/
			void addInstanceForTransfer(Instance* instance, const Location& target);

			/** Removes instance that should be transferred to another layer.
			* @param instance A pointer to the instance that should be transferred.
			*/
			void removeInstanceForTransfer(Instance* instance);

			/** Creates cellcaches for this map. Called from maploader.
			*/
			void initializeCellCaches();

			/** Creates cellcaches for this map. Called from maploader.
			*/
			void finalizeCellCaches();

			/**
			 */
			TriggerController* getTriggerController() const { return m_triggerController; };

		private:
			std::string m_id;
			std::string m_filename;

			std::list<Layer*> m_layers;
			TimeProvider m_timeProvider;

			Map(const Map& map);
			Map& operator=(const Map& map);

			//! listeners for map changes
			std::vector<MapChangeListener*> m_changeListeners;

			//! holds changed layers after each update
			std::vector<Layer*> m_changedLayers;

			//! holds the cameras attached to this map
			std::vector<Camera*> m_cameras;

			//! pointer to renderbackend
			RenderBackend* m_renderBackend;

			//! holds handles to all created renderers
			std::vector<RendererBase*> m_renderers;

			//! true, if something was changed on map during previous update (layer change, creation, deletion)
			bool m_changed;

			//! holds instances which should be transferred on the next update
			std::map<Instance*, Location> m_transferInstances;

			TriggerController* m_triggerController;
	};

} //FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
