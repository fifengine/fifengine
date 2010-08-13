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
	class AbstractFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 * @param font default font used to render the texts
		 */
		FloatingTextRenderer(RenderBackend* renderbackend, int position, AbstractFont* font);
		
		FloatingTextRenderer(const FloatingTextRenderer& old);
		
		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~FloatingTextRenderer();

		void render(Camera* cam, Layer* layer, RenderList& instances);

		std::string getName() { return "FloatingTextRenderer"; }
		
		/** Changes default font in the renderer
		 * Note that this does not change the font ownership
		 */
		void changeDefaultFont(AbstractFont* font) { m_font = font; }
		
		/** Changes default font color
		 * Only usefull for .ttf fonts
		 */
		void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

		/** Set default background quad
		 * r,g,b,a values for background
		 */
		void setBackground(Uint8 br, Uint8 bg, Uint8 bb, Uint8 ba = 255);

		/** Set default border
		 * r,g,b,a values for border
		 */
		void setBorder(Uint8 bbr, Uint8 bbg, Uint8 bbb, Uint8 bba = 255);
		
		/** Disable the default background
		 */
		void resetBackground();

		/** Disable the default border
		 */
		void resetBorder();

		/** returns instance used in given view
		 */
		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

	private:
		RenderBackend* m_renderbackend;
		AbstractFont* m_font;
		bool m_font_color;
		SDL_Color m_color;
		bool m_background;
		bool m_backborder;
		SDL_Color m_backcolor;
		SDL_Color m_backbordercolor;
	};

}

#endif
