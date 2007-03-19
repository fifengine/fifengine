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

	/** Contains a game level.
	 * 
	 * Tiles are only graphics and they are on a separate Layer.
	 *
	 * Objects can be: critters, items, walls, scenery and non-visible 
	 * entities (blockers).
	 *
	 */
	class Elevation : public AttributedClass {
		public:

			/** Constructor
			 */
			Elevation();

			/** Destructor
			 */
			~Elevation();

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

		private:
			typedef std::vector<LayerPtr> type_layers;
			type_layers m_layers;

			int m_width;
			int m_height;
			size_t m_reference_layer;

			// Not copyable.
			Elevation(const Elevation&);
			Elevation& operator=(const Elevation&);
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
