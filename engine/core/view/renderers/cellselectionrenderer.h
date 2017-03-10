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
		/** Constructor.
		 *
		 * @param renderbackend The renderbackend to use.
		 * @param position The position for this renderer in rendering pipeline.
		 * @ see setPipelinePosition
		 */
		CellSelectionRenderer(RenderBackend* renderbackend, int32_t position);

		/** Copy Constructor.
		 */
		CellSelectionRenderer(const CellSelectionRenderer& old);

		/** Makes copy of this renderer.
		 */
		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~CellSelectionRenderer();

		/** This method is called by the view to ask renderer to draw its rendering aspect based on
		 * given parameters.
		 *
		 * @param cam Camera view to draw
		 * @param layer Current layer to be rendered
		 * @param instances Instances on the current layer
		 */
		void render(Camera* cam, Layer* layer, RenderList& instances);

		/** Returns the renderer name.
		 *
		 * @return The name as string.
		 */
		std::string getName() { return "CellSelectionRenderer"; }

		/** Gets instance for interface access.
		 */
		static CellSelectionRenderer* getInstance(IRendererContainer* cnt);

		/** Deselects all locations.
		 */
		void reset();

		/** Selects given location on map.
		 *
		 * @param loc The location.
		 */
		void selectLocation(const Location* loc);

		/** Deselects given location on map.
		 *
		 * @param loc The location.
		 */
		void deselectLocation(const Location* loc);

		/** Returns selected locations.
		 *
		 * @return A vector that contains all selected locations.
		 */
		const std::vector<Location> getLocations() const { return m_locations; }

		/** Changes the used color.
		 *
		 * @param r The value for red, range 0-255.
		 * @param g The value for green, range 0-255.
		 * @param b The value for blue, range 0-255.
		 */
		void setColor(uint8_t r, uint8_t g, uint8_t b);

	private:
		//! selected locations
		std::vector<Location> m_locations;

		//! currently used color
		SDL_Color m_color;
	};

}

#endif
