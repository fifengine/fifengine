/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_CELLSELECTIONRENDERER_H
#define FIFE_CELLSELECTIONRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"
#include "model/structures/location.h"

namespace FIFE {
	class RenderBackend;

	/** CellSelectionRenderer renders a frame around selected cells.
	 */
	class CellSelectionRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 */
		CellSelectionRenderer(RenderBackend* renderbackend, int position);
		
		CellSelectionRenderer(const CellSelectionRenderer& old);
		
		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~CellSelectionRenderer();

		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances);

		/** Returns the renderer name */
		std::string getName() { return "CellSelectionRenderer"; }

		/** returns instance used in given view
		 */
		static CellSelectionRenderer* getInstance(IRendererContainer* cnt);
		
		/** Deselects all locations */
		void reset();
		
		/** Selects given location on map
		 */
		void selectLocation(const Location* loc);

		/** Deselects given location on map
		 */
		void deselectLocation(const Location* loc);

		/** Returns selected locations
		 */
		const std::vector<Location> getLocations() const { return m_locations; }
		
	private:
		// selected locations
		std::vector<Location> m_locations;
	};

}

#endif
