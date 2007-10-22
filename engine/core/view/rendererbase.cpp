/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/layer.h"
#include "model/structures/elevation.h"
#include "util/logger.h"
#include "rendererbase.h"

namespace FIFE {
	static Logger _log(LM_VIEW);
	
	RendererBase::RendererBase(): 
		m_enabled(false), 
		m_pipeline_position(DEFAULT_RENDERER_POSITION),
		m_listener(NULL) {
	}
	
	void RendererBase::setPipelinePosition(int position) { 
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
	
	void RendererBase::activateAllLayers(Elevation* elevation) {
		clearActiveLayers();
		
		const std::vector<Layer*>& tmp = elevation->getLayers();
		std::vector<Layer*>::const_iterator it = tmp.begin();
		for (; it != tmp.end(); ++it) {
			addActiveLayer(*it);
		}
	}
	
}
