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

#ifndef FIFE_FLOATINGTEXTRENDERER_H
#define FIFE_FLOATINGTEXTRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		/** Constructor.
		 *
		 * @param renderbackend The renderbackend to use.
		 * @param position The position for this renderer in rendering pipeline.
		 * @ see setPipelinePosition
		 */
		FloatingTextRenderer(RenderBackend* renderbackend, int32_t position);

		/** Copy Constructor.
		 */
		FloatingTextRenderer(const FloatingTextRenderer& old);

		/** Makes copy of this renderer.
		 */
		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~FloatingTextRenderer();

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
		std::string getName() { return "FloatingTextRenderer"; }

		/** Changes default font in the renderer
		 * Note that this does not change the font ownership
		 */
		void setFont(IFont* font) { m_font = font; }

		/** Changes default font color
		 * Only useful for .ttf fonts
		 */
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		/** Set default background quad
		 * r,g,b,a values for background
		 */
		void setBackground(uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba = 255);

		/** Set default border
		 * r,g,b,a values for border
		 */
		void setBorder(uint8_t bbr, uint8_t bbg, uint8_t bbb, uint8_t bba = 255);

		/** Disable the default background
		 */
		void resetBackground();

		/** Disable the default border
		 */
		void resetBorder();

		/** Gets instance for interface access.
		 */
		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

		/** Provides access point to the RenderBackend
		 */
		RenderBackend* getRenderBackend() const {return m_renderbackend;}

	private:
		RenderBackend* m_renderbackend;
		IFont* m_font;
		bool m_font_color;
		SDL_Color m_color;
		bool m_background;
		bool m_backborder;
		SDL_Color m_backcolor;
		SDL_Color m_backbordercolor;
	};

}

#endif
