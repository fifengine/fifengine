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
#include "video/renderbackend.h"
#include "video/image.h"
#include "video/imagepool.h"
#include "util/logger.h"

#include "util/fife_math.h"
#include "util/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/action.h"
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "view/visual.h"
#include "coordinaterenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	CoordinateRenderer::CoordinateRenderer(RenderBackend* renderbackend, ImagePool* imagepool):
		m_renderbackend(renderbackend),
		m_imagepool(imagepool),
		m_layer_area(),
		m_tmploc(),
		m_c() {
		setPipelinePosition(2);
		setEnabled(false);
	}
	
	CoordinateRenderer::~CoordinateRenderer() {
	}
	
	
	void CoordinateRenderer::adjustLayerArea() {
		m_tmploc.setElevationCoordinates(m_c);
		ModelCoordinate c = m_tmploc.getLayerCoordinates();
		m_layer_area.x = std::min(c.x, m_layer_area.x);
		m_layer_area.w = std::max(c.x, m_layer_area.w);
		m_layer_area.y = std::min(c.y, m_layer_area.y);
		m_layer_area.h = std::max(c.y, m_layer_area.h);
	}
	
	const int MIN_COORD = -9999999;
	const int MAX_COORD = 9999999;
	void CoordinateRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, int stackpos) {
		m_layer_area.x = MAX_COORD;
		m_layer_area.y = MAX_COORD;
		m_layer_area.w = MIN_COORD;
		m_layer_area.h = MIN_COORD;
		
		m_tmploc.setLayer(layer);
		m_c = cam->toElevationCoordinates(ScreenPoint(0, 0));
		adjustLayerArea();
		m_c = cam->toElevationCoordinates(ScreenPoint(m_renderbackend->getScreenWidth(), 0));
		adjustLayerArea();
		m_c = cam->toElevationCoordinates(ScreenPoint(0, m_renderbackend->getScreenHeight()));
		adjustLayerArea();
		m_c = cam->toElevationCoordinates(ScreenPoint(m_renderbackend->getScreenWidth(), m_renderbackend->getScreenHeight()));
		adjustLayerArea();
		
		Rect cv = cam->getViewPort();
		for (int x = m_layer_area.x-1; x < m_layer_area.w+1; x++) {
			for (int y = m_layer_area.y-1; y < m_layer_area.h+1; y++) {
				ModelCoordinate mc(x, y);
				m_tmploc.setLayerCoordinates(mc);
				ScreenPoint drawpt = cam->toScreenCoordinates(m_tmploc.getElevationCoordinates());
				if ((drawpt.x >= cv.x) || (drawpt.x <= cv.w) || 
				    (drawpt.y >= cv.y) || (drawpt.y <= cv.h)) {
					;
				}
				
			}
		}
	}
}
