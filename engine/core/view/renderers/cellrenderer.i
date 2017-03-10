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
#include "view/renderers/cellrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class IFont;

	class CellRenderer: public RendererBase {
	public:
		virtual ~CellRenderer();
		std::string getName();
		void setBlockerColor(uint8_t r, uint8_t g, uint8_t b);
		void setPathColor(uint8_t r, uint8_t g, uint8_t b);
		void setEnabledBlocking(bool enabled);
		bool isEnabledBlocking();
		void setEnabledFogOfWar(bool enabled);
		bool isEnabledFogOfWar();
		void setEnabledPathVisual(bool enabled);
		bool isEnabledPathVisual();
		void setFogOfWarLayer(Layer* layer);
		void setConcealImage(ImagePtr image);
		void setMaskImage(ImagePtr image);
		void addPathVisual(Instance* instance);
		void removePathVisual(Instance* instance);
		void setEnabledCost(const std::string& costId, bool enabled);
		bool isEnabledCost(const std::string& costId);
		void setFont(IFont* font);
		IFont* getFont();
		
		static CellRenderer* getInstance(IRendererContainer* cnt);
		
	private:
		CellRenderer(RenderBackend* renderbackend, int32_t position);
	};
}

