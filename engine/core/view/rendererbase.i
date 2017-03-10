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
#include "view/rendererbase.h"
%}

namespace FIFE {
	class Camera;
	class Layer;
	class Instance;
	class Map;
	
	class RendererBase {
	public:
		RendererBase();
		virtual ~RendererBase();
		virtual std::string getName() = 0;
		int32_t getPipelinePosition() { return m_position; }
		void setPipelinePosition(int32_t position);
		void setEnabled(bool enabled);
		bool isEnabled();
		void addActiveLayer(Layer* layer);
		void removeActiveLayer(Layer* layer);
		void clearActiveLayers();
		void activateAllLayers(Map* map);
		std::list<Layer*> getActiveLayers() const {return m_active_layers;}
	};
	
	class IRendererContainer {
	public:
		virtual ~IRendererContainer() {}
		virtual RendererBase* getRenderer(const std::string& renderername) = 0;
	};
	
}
