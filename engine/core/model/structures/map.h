/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_MAP_MAP_H
#define FIFE_MAP_MAP_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"
#include "model/metamodel/timeprovider.h"

#include "location.h"

namespace FIFE {

	class Layer;
	class CellGrid;

	class Dataset;

	/** A container of \c Layer(s).
	 *
	 * The actual data is contained in \c Layer objects
	 *
	 * @see Layer
	 * @see MapView
	 * @see MapLoader
	 */
	class Map : public AttributedClass {
		public:

			/** Construct a map
			 * To add map to model, one should call Model::addMap (otherwise
			 * map is not registered with the engine properly)
			 */
			Map(const std::string& identifier, TimeProvider* tp_master=NULL);

			/** Destructor
			 */
			~Map();

			/** Adds dataset into map. Does not transfer ownership
			 */
			void addDataset(Dataset* dataset);

			/** Get the datasets used by this map.
			 */
			std::list<Dataset*> getDatasets();

			/** Get the datasets used by this map, and recursively get any
			 * datasets referred to by these datasets.
			 */
			std::list<Dataset*>  getDatasetsRec();

			/** Add a Layer to this Map. Map owns
			 * the returned pointer to the new Layer, so don't
			 * delete it!
			 */
			Layer* createLayer(const std::string& identifier, CellGrid* grid);

			/** Delete a layer from the map
			 */
			void deleteLayer(Layer*);

			/** Get the layers on this map.
			 */
			std::list<Layer*> getLayers() const;

			/** Get a set of layers by a value.
			 *
			 * @param the field to search on
			 * @param the value to be searched for in the field
			 */
			std::list<Layer*> getLayers(const std::string& field, const std::string& value) const;

			/** Get all layers
			 */
			const std::vector<Layer*>& getLayers() { return m_layers; }

			/** Get the overall number of layers
			 */
			size_t getNumLayers() const;

			/** Delete all layers from the map
			 */
			void deleteLayers();

			/** Maps coordinate from one layer to another
			 */
			void getMatchingCoordinates(const ModelCoordinate& coord_to_map, const Layer* from_layer, 
				const Layer* to_layer, std::vector<ModelCoordinate>& matching_coords) const;

			/** Called periodically to update events on map
			 */
			void update();
			
			/** Sets speed for the map. See Model::setTimeMultiplier.
			 */
			void setTimeMultiplier(float multip) { m_timeprovider.setMultiplier(multip); }
			
			/** Gets model speed. @see setTimeMultiplier.
			 */
			float getTimeMultiplier() const { return m_timeprovider.getMultiplier(); }
			
			/** Gets timeprovider used in the map
			 */
			TimeProvider* getTimeProvider() { return &m_timeprovider; }

		private:
			std::string m_mapname;
			std::vector<Dataset*> m_datasets;
			std::vector<Layer*> m_layers;
			TimeProvider m_timeprovider;
			
			Map(const Map& map);
			Map& operator=(const Map& map);
	};

} //FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
