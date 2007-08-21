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

#include "layer.h"

namespace FIFE { namespace model {

	class Layer;
	class Map;

	class GeometryType;

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

			/** Destructor
			 */
			~Elevation();

			/** Retrieve the map this elevation is contained in
			 */
			Map* getMap();

			/** Add a Layer to this Elevation. Elevation owns
			 * the returned pointer to the new Layer, so don't
			 * delete it!
			 */
			Layer* addLayer(GeometryType* geometry);

			/** Remove a layer from the elevation
			 */
			void removeLayer(Layer*);

			/** Get a set of layers by a value.
			 *
			 * @param the field to search on
			 * @param the value to be searched for in the field
			 */
			template<typename T>
			std::list<Layer*> getLayers(const std::string& field, const T& value) const {
				std::list<Layer*> matches;

				std::vector<Layer*>::const_iterator it = m_layers.begin();
				for(; it != m_layers.end(); ++it) {
					if((*it)->get<T>(field) == value)
						matches.push_back(*it);
				}

				return matches;
			}


			/** Get the overall number of layers
			 */
			size_t getNumLayers() const;

			/** Remove all layers from the elevation
			 */
			void clearLayers();

			/** Set the 'reference' Layer
			 * @note Is this actually used? --jwt
			 */
			void setReferenceLayer(Layer* layer);

			/** Get the 'reference' Layer
			 */
			Layer* getReferenceLayer();

			/** Try to calculate a valid nice starting position
			 */
			Point centerOfMass();

			/** Called periodically to update events on elevation
			 */
			void update();

		private:

			/** Constructor (Elevations are created by Maps)
			 */
			Elevation();
		
			std::string m_name;

			Map* m_map;

			std::vector<Layer*> m_layers;

			int m_width;
			int m_height;
			Layer* m_reference_layer;

			// Not copyable.
			Elevation(const Elevation&);
			Elevation& operator=(const Elevation&);

			friend class Map;
	};

} } //FIFE::model

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
