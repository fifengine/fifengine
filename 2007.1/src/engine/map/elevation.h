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

#ifndef FIFE_MAP_ELEVATION_H
#define FIFE_MAP_ELEVATION_H

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "attributedclass.h"

namespace FIFE { namespace map {

	class ObjectInfo;
	class Layer;
	typedef boost::shared_ptr<Layer> LayerPtr;

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;
	typedef boost::weak_ptr<Elevation> ElevationWeakPtr;

	class Map;
	typedef boost::shared_ptr<Map> MapPtr;
	typedef boost::weak_ptr<Map> MapWeakPtr;

	/** Contains a game level.
	 * 
	 * Tiles are only graphics and they are on a separate Layer.
	 *
	 * \invariant getLayer(index)->getLayerNumber() == index
	 * \invariant elevation->addLayer(layer); layer->getElevation() == elevation
	 * \invariant elevation->insertLayer(index,layer); layer->getElevation() == elevation
	 * \invariant elevation->removeLayer(layer->getLayerNumber()); 
	 *  layer->getElevation() == ElevationPtr()
	 */
	class Elevation : public AttributedClass {
		public:

			/** Constructs a elevation instance
			 */
			static ElevationPtr create();

			/** Destructor
			 */
			~Elevation();

			/** Retrieve the map this elevation is contained in
			 */
			MapPtr getMap();

			/** Add a Layer at the top
			 * The elevation now owns the Layer.
			 * Increases num Layers by one.
			 */
			void addLayer(LayerPtr layer);

			/** Get a layer by its index
			 */
			LayerPtr getLayer(size_t);

			/** Get the overall number of layers
			 */
			size_t getNumLayers() const;

			/** Insert a layer to the elevation
			 */
			void insertLayer(size_t index, LayerPtr layer);

			/** Remove a layer from the elevation
			 */
			void removeLayer(size_t index);

			/** Remove all layers from the elevation
			 */
			void clearLayers();

			/** Apply a visitor to each layer
			 */
			template<typename T>
			void forEachLayer(T visitor) {
				std::for_each(m_layers.begin(),m_layers.end(),visitor);
			}

			/** Set the 'reference' Layer
			 */
			void setReferenceLayer(size_t layer);

			/** Get the 'reference' Layer
			 */
			LayerPtr getReferenceLayer();

			/** Try to calculate a valid nice starting position
			 */
			Point centerOfMass();

			/** Get total number of elevations
			 */
			static long globalCount();
		private:
			/** Constructor
			 */
			Elevation();

			void resetLayerNumbers();

			ElevationWeakPtr m_self;
			MapWeakPtr m_map;
			static long m_count;

			typedef std::vector<LayerPtr> type_layers;
			type_layers m_layers;

			int m_width;
			int m_height;
			size_t m_reference_layer;

			// Not copyable.
			Elevation(const Elevation&);
			Elevation& operator=(const Elevation&);

			friend class Map;
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
