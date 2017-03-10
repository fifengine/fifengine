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

%module fife
%{
#include "view/renderers/floatingtextrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class IFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		virtual ~FloatingTextRenderer();
		void setFont(IFont* font);
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setBackground(uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba = 255);
		void setBorder(uint8_t bbr, uint8_t bbg, uint8_t bbb, uint8_t bba = 255);
		void resetBackground();
		void resetBorder();

		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

	private:
		FloatingTextRenderer(RenderBackend* renderbackend, int32_t position, IFont* font);
	};


}
