// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_QUADTREERENDERER_H
#define FIFE_QUADTREERENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "model/structures/instancetree.h"
#include "util/structures/quadtree.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;

    class FIFE_API RenderVisitor
    {
        public:
            int32_t visited;
            RenderBackend* m_renderbackend;
            Layer* m_layer;
            Camera* m_camera;
            RenderVisitor(RenderBackend* rb, Layer* layer, Camera* camera);
            ~RenderVisitor() = default;
            template <typename T>
            bool visit(QuadNode<T, InstanceTree::MIN_TREE_SIZE>* node, int32_t d);
    };

    class FIFE_API QuadTreeRenderer : public RendererBase
    {
        public:
            /** constructor.
             * @param renderbackend to use
             * @param position position for this renderer in rendering pipeline
             */
            QuadTreeRenderer(RenderBackend* renderbackend, int32_t position);

            QuadTreeRenderer(QuadTreeRenderer const & old);

            RendererBase* clone();

            /** Destructor.
             */
            virtual ~QuadTreeRenderer();

            void render(Camera* cam, Layer* layer, RenderList& instances);

            std::string getName()
            {
                return "QuadTreeRenderer";
            }
    };
} // namespace FIFE

#endif
