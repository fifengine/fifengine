// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/action.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "coordinaterenderer.h"
#include "view/camera.h"
#include "view/visual.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] int32_t toScreenSize(const uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }
    } // namespace

    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    CoordinateRenderer::CoordinateRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position),

        m_font(nullptr),
        m_font_color(false),
        m_zoom(true)
    {
        setEnabled(false);
    }

    CoordinateRenderer::CoordinateRenderer(const CoordinateRenderer& old) :
        RendererBase(old),

        m_font(old.m_font),
        m_font_color(false),
        m_color(old.m_color),
        m_zoom(old.m_zoom)
    {
        setEnabled(false);
    }

    RendererBase* CoordinateRenderer::clone()
    {
        return new CoordinateRenderer(*this);
    }

    CoordinateRenderer::~CoordinateRenderer() = default;

    CoordinateRenderer* CoordinateRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<CoordinateRenderer*>(cnt->getRenderer("CoordinateRenderer"));
    }

    void CoordinateRenderer::adjustLayerArea()
    {
        m_tmploc.setMapCoordinates(m_c);
        ModelCoordinate c = m_tmploc.getLayerCoordinates();
        m_layer_area.x    = std::min(c.x, m_layer_area.x);
        m_layer_area.w    = std::max(c.x, m_layer_area.w);
        m_layer_area.y    = std::min(c.y, m_layer_area.y);
        m_layer_area.h    = std::max(c.y, m_layer_area.h);
    }

    const int32_t MIN_COORD = -9999999;
    const int32_t MAX_COORD = 9999999;
    void CoordinateRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        static_cast<void>(instances);
        if (m_font == nullptr) {
            // no font selected.. nothing to render
            return;
        }
        Rect r            = Rect();
        const bool zoomed = (!Mathd::Equal(1.0, cam->getZoom()) && m_zoom);
        Rect cv           = cam->getViewPort();

        m_tmploc.setLayer(layer);
        m_layer_area.x = MAX_COORD;
        m_layer_area.y = MAX_COORD;
        m_layer_area.w = MIN_COORD;
        m_layer_area.h = MIN_COORD;

        m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y), false);
        adjustLayerArea();
        m_c = cam->toMapCoordinates(ScreenPoint(cv.x + cv.w, cv.y), false);
        adjustLayerArea();
        m_c = cam->toMapCoordinates(ScreenPoint(cv.x, cv.y + cv.h), false);
        adjustLayerArea();
        m_c = cam->toMapCoordinates(ScreenPoint(cv.x + cv.w, cv.y + cv.h), false);
        adjustLayerArea();

        SDL_Color old_color = m_font->getColor();
        if (old_color.r != m_color.r || old_color.g != m_color.g || old_color.b != m_color.b) {
            m_font->setColor(m_color.r, m_color.g, m_color.b);
            m_font_color = true;
        }
        for (int32_t x = m_layer_area.x - 1; x < m_layer_area.w + 1; x++) {
            for (int32_t y = m_layer_area.y - 1; y < m_layer_area.h + 1; y++) {
                ModelCoordinate mc(x, y);
                m_tmploc.setLayerCoordinates(mc);
                ScreenPoint drawpt = cam->toScreenCoordinates(m_tmploc.getMapCoordinates());
                if (drawpt.x < cv.x || drawpt.x > cv.x + cv.w || drawpt.y < cv.y || drawpt.y > cv.y + cv.h) {
                    continue;
                }

                // we split the stringstream in three images, because so the TextRenderPool can reuse the most images.
                // more to render but less images to generate
                std::stringstream sts;
                sts << mc.x;
                Image* imgx = m_font->getAsImage(sts.str());
                sts.str(",");
                Image* imgc = m_font->getAsImage(sts.str());
                sts.str("");
                sts << mc.y;
                Image* imgy              = m_font->getAsImage(sts.str());
                const int32_t widthX     = toScreenSize(imgx->getWidth());
                const int32_t widthComma = toScreenSize(imgc->getWidth());
                const int32_t widthY     = toScreenSize(imgy->getWidth());
                const int32_t heightX    = toScreenSize(imgx->getHeight());

                if (zoomed) {
                    const double zoom = cam->getZoom();
                    r.x = static_cast<int32_t>(round(drawpt.x - ((widthX + widthComma + widthY) / 2.0) * zoom));
                    r.y = static_cast<int32_t>(round(drawpt.y - (heightX / 2.0) * zoom));
                    r.w = static_cast<int32_t>(round(widthX * zoom));
                    r.h = static_cast<int32_t>(round(heightX * zoom));
                    imgx->render(r);
                    r.x += r.w;
                    r.w = static_cast<int32_t>(round(widthComma * zoom));
                    imgc->render(r);
                    r.x += r.w;
                    r.w = static_cast<int32_t>(round(widthY * zoom));
                    imgy->render(r);
                } else {
                    r.x = drawpt.x - (widthX + widthComma + widthY) / 2;
                    r.y = drawpt.y - heightX / 2;
                    r.w = widthX;
                    r.h = heightX;
                    imgx->render(r);
                    r.x += r.w;
                    r.w = widthComma;
                    imgc->render(r);
                    r.x += r.w;
                    r.w = widthY;
                    imgy->render(r);
                }
            }
        }
        if (m_font_color) {
            m_font->setColor(old_color.r, old_color.g, old_color.b);
            m_font_color = false;
        }
    }

    void CoordinateRenderer::setColor(uint8_t r, uint8_t g, uint8_t b)
    {
        m_color.r = r;
        m_color.g = g;
        m_color.b = b;
    }
} // namespace FIFE
