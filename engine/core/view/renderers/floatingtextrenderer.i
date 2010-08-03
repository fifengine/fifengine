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

%module fife
%{
#include "view/renderers/floatingtextrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class AbstractFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		virtual ~FloatingTextRenderer();
		void changeDefaultFont(AbstractFont* font);
		void setColor(Uint8 r, Uint8 g, Uint8 b);
		void setDefaultBackground(Uint8 br, Uint8 bg, Uint8 bb, Uint8 bbr, Uint8 bbg, Uint8 bbb);
		void resetDefaultBackground();

		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

	private:
		FloatingTextRenderer(RenderBackend* renderbackend, int position, AbstractFont* font);
	};


}
