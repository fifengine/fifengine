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

#ifndef FIFE_QUADTREERENDERER_H
#define FIFE_QUADTREERENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"
#include "util/structures/quadtree.h"
#include "model/structures/instancetree.h"

namespace FIFE {
	class RenderBackend;

	class RenderVisitor {
		public:
			int32_t visited;
			RenderBackend * m_renderbackend;
			Layer * m_layer;
			Camera *m_camera;
			RenderVisitor(RenderBackend * rb, Layer * layer, Camera *camera);
			~RenderVisitor();
			template<typename T> bool visit(QuadNode<T,InstanceTree::MIN_TREE_SIZE>* node, int32_t d);

	};

	class QuadTreeRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		QuadTreeRenderer(RenderBackend* renderbackend, int32_t position);

		QuadTreeRenderer(const QuadTreeRenderer& old);

		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~QuadTreeRenderer();

		void render(Camera* cam, Layer* layer, RenderList& instances);

		std::string getName() {
			return "QuadTreeRenderer";
		}
	};
}

#endif
