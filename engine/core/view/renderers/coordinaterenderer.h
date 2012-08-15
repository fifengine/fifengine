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

#ifndef FIFE_COORDINATERENDERER_H
 #define FIFE_COORDINATERENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"
#include "util/structures/rect.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class CoordinateRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		CoordinateRenderer(RenderBackend* renderbackend, int32_t position);

		CoordinateRenderer(const CoordinateRenderer& old);

		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~CoordinateRenderer();

		void render(Camera* cam, Layer* layer, RenderList& instances);
		std::string getName() { return "CoordinateRenderer"; }
		void setColor(uint8_t r, uint8_t g, uint8_t b);

		/** Changes default font in the renderer
		 * @note that this does not change the font ownership
		 *
		 * @param font Pointer to the font
		 */
		void setFont(IFont* font) { m_font = font; }

		/** Enables / disables zooming for coordinate images, by default it is enabled.
		 */
		void setZoom(bool enable) { m_zoom = enable; }

		static CoordinateRenderer* getInstance(IRendererContainer* cnt);

	private:
		void adjustLayerArea();

		Rect m_layer_area;
		Location m_tmploc;
		ExactModelCoordinate m_c;
		IFont* m_font;
		bool m_font_color;
		SDL_Color m_color;
		bool m_zoom;
	};
}

#endif
