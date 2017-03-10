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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/layer.h"
#include "model/structures/map.h"
#include "util/log/logger.h"
#include "rendererbase.h"

namespace FIFE {
	static Logger _log(LM_VIEW);
	
	// use some big value, so that non-positioned renderers show on top
	const int32_t DEFAULT_RENDERER_POSITION = 1000;	
	
	RendererBase::RendererBase(RenderBackend* renderbackend, int32_t position): 
		m_renderbackend(renderbackend),
		m_enabled(false),
		m_pipeline_position(DEFAULT_RENDERER_POSITION),
		m_listener(NULL) {
		setPipelinePosition(position);
	}
	
	RendererBase::RendererBase(const RendererBase& old):
		m_renderbackend(old.m_renderbackend),
		m_enabled(old.m_enabled),
		m_pipeline_position(old.m_pipeline_position),
		m_listener(NULL) {
		setPipelinePosition(old.m_pipeline_position);
	}
	
	RendererBase::RendererBase():
		m_renderbackend(NULL),
		m_enabled(false),
		m_pipeline_position(DEFAULT_RENDERER_POSITION),
		m_listener(NULL) {
	}
	
	void RendererBase::setPipelinePosition(int32_t position) { 
		if (position !=m_pipeline_position) { 
			m_pipeline_position = position; 
			if (m_listener) {
				m_listener->onRendererPipelinePositionChanged(this);
			}
		}
	}
	
	void RendererBase::setEnabled(bool enabled) { 
		if (m_enabled != enabled) {
			m_enabled = enabled;
			if (m_listener) {
				m_listener->onRendererEnabledChanged(this);
			}
		}
	}
	
	void RendererBase::addActiveLayer(Layer* layer) {
		if (std::find(m_active_layers.begin(), m_active_layers.end(), layer) == m_active_layers.end()) {
			m_active_layers.push_back(layer);
		}
	}
	
	void RendererBase::removeActiveLayer(Layer* layer) {
		m_active_layers.remove(layer);
	}
	
	void RendererBase::clearActiveLayers() {
		m_active_layers.clear();
	}
	
	bool RendererBase::isActivedLayer(Layer* layer) {
		return std::find(m_active_layers.begin(), m_active_layers.end(), layer) != m_active_layers.end();
	}
	
	void RendererBase::activateAllLayers(Map* map) {
		clearActiveLayers();
		
		const std::list<Layer*>& tmp = map->getLayers();
		std::list<Layer*>::const_iterator it = tmp.begin();
		for (; it != tmp.end(); ++it) {
			addActiveLayer(*it);
		}
	}
	
}
