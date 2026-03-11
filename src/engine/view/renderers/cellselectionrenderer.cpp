// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <vector>

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

#include "cellselectionrenderer.h"
#include "view/camera.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    CellSelectionRenderer::CellSelectionRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position)
    {
        setEnabled(false);
        m_color.r = 255;
        m_color.g = 0;
        m_color.b = 0;
    }

    CellSelectionRenderer::CellSelectionRenderer(const CellSelectionRenderer& old) :
        RendererBase(old), m_color(old.m_color)
    {
        setEnabled(false);
    }

    RendererBase* CellSelectionRenderer::clone()
    {
        return new CellSelectionRenderer(*this);
    }

    CellSelectionRenderer::~CellSelectionRenderer() = default;

    CellSelectionRenderer* CellSelectionRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<CellSelectionRenderer*>(cnt->getRenderer("CellSelectionRenderer"));
    }

    void CellSelectionRenderer::reset()
    {
        m_locations.clear();
    }

    void CellSelectionRenderer::selectLocation(const Location* loc)
    {
        if (loc != nullptr) {
            auto it = m_locations.begin();
            for (; it != m_locations.end(); it++) {
                if ((*it).getLayerCoordinates() == loc->getLayerCoordinates()) {
                    return;
                }
            }

            m_locations.emplace_back(*loc);
        }
    }

    void CellSelectionRenderer::deselectLocation(const Location* loc)
    {
        if (loc != nullptr) {
            auto it = m_locations.begin();
            for (; it != m_locations.end(); it++) {
                if ((*it).getLayerCoordinates() == loc->getLayerCoordinates()) {
                    m_locations.erase(it);
                    break;
                }
            }
        }
    }

    void CellSelectionRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        if (m_locations.empty()) {
            return;
        }

        auto locit = m_locations.begin();
        for (; locit != m_locations.end(); locit++) {
            const Location loc = *locit;
            if (layer != loc.getLayer()) {
                continue;
            }

            CellGrid* cg = layer->getCellGrid();
            if (cg == nullptr) {
                FL_WARN(_log, "No cellgrid assigned to layer, cannot draw selection");
                continue;
            }

            std::vector<ExactModelCoordinate> vertices;
            cg->getVertices(vertices, loc.getLayerCoordinates());
            auto it             = vertices.begin();
            ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
            Point pt1(firstpt.x, firstpt.y);
            Point pt2;
            ++it;
            for (; it != vertices.end(); it++) {
                ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
                pt2.x           = pts.x;
                pt2.y           = pts.y;
                Point cpt1      = pt1;
                Point cpt2      = pt2;
                m_renderbackend->drawLine(cpt1, cpt2, m_color.r, m_color.g, m_color.b);
                pt1 = pt2;
            }
            m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_color.r, m_color.g, m_color.b);
        }
    }

    void CellSelectionRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
    {
        m_color.r = r;
        m_color.g = g;
        m_color.b = b;
    }
} // namespace FIFE
