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

#ifndef FIFE_ELEVATION_H
#define FIFE_ELEVATION_H

// Standard C++ library includes
#include <algorithm>
#include <vector>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"

namespace FIFE { namespace model {

	class ObjectInfo;
	class Layer;
	class Map;

	/** Contains a game level.
	 * 
	 * \invariant getLayer(index)->getLayerNumber() == index
	 * \invariant elevation->addLayer(layer); layer->getElevation() == elevation
	 * \invariant elevation->insertLayer(index,layer); layer->getElevation() == elevation
	 * \invariant elevation->removeLayer(layer->getLayerNumber()); 
	 *  layer->getElevation() == ElevationPtr()
	 */
	class Elevation : public AttributedClass {
		public:

			/** Constructor
			 */
			Elevation();

			/** Destructor
			 */
			~Elevation();

			/** Set the map of this elevation
			 */
			void setMap();

			/** Retrieve the map this elevation is contained in
			 */
			Map* getMap();

			/** Set this elevation's name
			 */
			void setName(const std::string& name);

			/** Get this elevation's name
			 */
			const std::string& getName() const;

			/** Add a Layer at the top
			 * The elevation now owns the Layer.
			 * Increases num Layers by one.
			 */
			void addLayer(Layer* layer);

			/** Get a layer by its index
			 */
			Layer* getLayer(size_t) const;

			/** Get a layer by its name
			 */
			Layer* getLayer(const std::string& name) const;

			/** Get the overall number of layers
			 */
			size_t getNumLayers() const;

			/** Insert a layer to the elevation
			 */
			void insertLayer(size_t index, Layer* layer);

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
			Layer* getReferenceLayer();

			/** Try to calculate a valid nice starting position
			 */
			Point centerOfMass();

		private:
		
			std::string m_name;

			Map* m_map;

			std::vector<Layer*> m_layers;

			int m_width;
			int m_height;
			size_t m_reference_layer;

			// Not copyable.
			Elevation(const Elevation&);
			Elevation& operator=(const Elevation&);

			friend class Map;
	};

} } //FIFE::model

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
