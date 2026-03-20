// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/renderbackend.h"

#include "model/structures/instancetree.h"
#include "quadtreerenderer.h"
#include "util/structures/quadtree.h"
#include "view/camera.h"

// credit to phoku for his NodeDisplay example which the visitor code is adapted from ( he coded the quadtree after all
// )

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    QuadTreeRenderer::QuadTreeRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position)
    {
        setEnabled(false);
    }

    QuadTreeRenderer::QuadTreeRenderer(const QuadTreeRenderer& old) : RendererBase(old)
    {
        setEnabled(false);
    }

    RendererBase* QuadTreeRenderer::clone()
    {
        return new QuadTreeRenderer(*this);
    }

    QuadTreeRenderer::~QuadTreeRenderer() = default;
    RenderVisitor::RenderVisitor(RenderBackend* rb, Layer* layer, Camera* camera) :
        visited(0), m_renderbackend(rb), m_layer(layer), m_camera(camera)
    {
    }

    template <typename T>
    bool RenderVisitor::visit(QuadNode<T, InstanceTree::MIN_TREE_SIZE>* node, int32_t d)
    {

        if (d == 0) {
            visited = 0;
        }

        int32_t x    = node->x();
        int32_t y    = node->y();
        int32_t size = node->size();

        ++visited;
        CellGrid* cg = m_layer->getCellGrid(); /// we have checked for null pointer in  quadtreerenderer::render().. no
                                               /// need to check again

        ExactModelCoordinate emc = cg->toMapCoordinates(ExactModelCoordinate(x, y)); // 0.5 for each cell's half-width
        ScreenPoint scrpt1       = m_camera->toScreenCoordinates(emc);
        emc = cg->toMapCoordinates(ExactModelCoordinate(x, y + size)); // this size usage is wrong.. me thinks
        ScreenPoint scrpt2 = m_camera->toScreenCoordinates(emc);
        emc                = cg->toMapCoordinates(ExactModelCoordinate(x + size, y));
        ScreenPoint scrpt3 = m_camera->toScreenCoordinates(emc);
        emc                = cg->toMapCoordinates(ExactModelCoordinate(x + size, y + size));
        ScreenPoint scrpt4 = m_camera->toScreenCoordinates(emc);

        m_renderbackend->drawLine(Point(scrpt1.x, scrpt1.y), Point(scrpt2.x, scrpt2.y), 255, 255, 255);
        m_renderbackend->drawLine(Point(scrpt1.x, scrpt1.y), Point(scrpt3.x, scrpt3.y), 255, 255, 255);
        m_renderbackend->drawLine(Point(scrpt3.x, scrpt3.y), Point(scrpt4.x, scrpt4.y), 255, 255, 255);
        m_renderbackend->drawLine(Point(scrpt2.x, scrpt2.y), Point(scrpt4.x, scrpt4.y), 255, 255, 255);

        return true;
    }

    void QuadTreeRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        const CellGrid* cg = layer->getCellGrid();
        if (cg == nullptr) {
            FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
            return;
        }
        InstanceTree* itree = layer->getInstanceTree();
        RenderVisitor VIPguess(m_renderbackend, layer, cam);
        itree->applyVisitor(VIPguess);
    }

} // namespace FIFE