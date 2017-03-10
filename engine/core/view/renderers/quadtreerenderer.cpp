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
#include "video/renderbackend.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "quadtreerenderer.h"
#include "model/structures/instancetree.h"
#include "util/structures/quadtree.h"

//credit to phoku for his NodeDisplay example which the visitor code is adapted from ( he coded the quadtree after all )

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	QuadTreeRenderer::QuadTreeRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
	}

 	QuadTreeRenderer::QuadTreeRenderer(const QuadTreeRenderer& old):
		RendererBase(old) {
		setEnabled(false);
	}

	RendererBase* QuadTreeRenderer::clone() {
		return new QuadTreeRenderer(*this);
	}

	QuadTreeRenderer::~QuadTreeRenderer() {	}
	RenderVisitor::RenderVisitor(RenderBackend * rb, Layer * layer, Camera *camera) {

		m_renderbackend = rb;
		m_layer = layer;
		m_camera = camera;
	}

	RenderVisitor::~RenderVisitor() {}

	template<typename T> bool RenderVisitor::visit(QuadNode<T,InstanceTree::MIN_TREE_SIZE>* node, int32_t d) {

		if (d==0)
			visited = 0;

		int32_t x = node->x();
		int32_t y = node->y();
		int32_t size = node->size();

		++visited;
		CellGrid *cg = m_layer->getCellGrid(); ///we have checked for null pointer in  quadtreerenderer::render().. no need to check again


		ExactModelCoordinate emc= cg->toMapCoordinates(ExactModelCoordinate( x,y) );//0.5 for each cell's half-width
		ScreenPoint scrpt1 =m_camera->toScreenCoordinates( emc );
		emc= cg->toMapCoordinates(ExactModelCoordinate( x,y+size) );// this size usage is wrong.. me thinks
		ScreenPoint scrpt2 =m_camera->toScreenCoordinates( emc );
		emc= cg->toMapCoordinates(ExactModelCoordinate( x+size,y) );
		ScreenPoint scrpt3 =m_camera->toScreenCoordinates( emc );
		emc= cg->toMapCoordinates(ExactModelCoordinate( x+size,y+size) );
		ScreenPoint scrpt4 =m_camera->toScreenCoordinates( emc );

		m_renderbackend->drawLine(Point(scrpt1.x,scrpt1.y), Point(scrpt2.x,scrpt2.y), 255, 255, 255);
		m_renderbackend->drawLine(Point(scrpt1.x,scrpt1.y), Point(scrpt3.x,scrpt3.y), 255, 255, 255);
		m_renderbackend->drawLine(Point(scrpt3.x,scrpt3.y), Point(scrpt4.x,scrpt4.y), 255, 255, 255);
		m_renderbackend->drawLine(Point(scrpt2.x,scrpt2.y), Point(scrpt4.x,scrpt4.y), 255, 255, 255);

		return true;
	}


	void QuadTreeRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}
		InstanceTree * itree = layer->getInstanceTree();
		RenderVisitor VIPguess(m_renderbackend, layer,cam);
		itree->applyVisitor(VIPguess);
	}

}

