// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_QUADTREERENDERER_H
#define FIFE_QUADTREERENDERER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/instancetree.h"
#include "util/structures/quadtree.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;

    class RenderVisitor
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

    class QuadTreeRenderer : public RendererBase
    {
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

        std::string getName()
        {
            return "QuadTreeRenderer";
        }
    };
} // namespace FIFE

#endif
