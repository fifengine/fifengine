// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "floatingtextrenderer.h"

// Standard C++ library includes
#include <cassert>
#include <limits>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include "view/camera.h"
#include "view/visual.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] int32_t toScreenSize(uint32_t const value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }

        [[nodiscard]] uint16_t toRectExtent(int32_t const value)
        {
            assert(value >= 0);
            assert(value <= std::numeric_limits<uint16_t>::max());
            return static_cast<uint16_t>(value);
        }
    } // namespace

    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position),
        m_renderbackend(renderbackend),
        m_font(nullptr),
        m_font_color(false),
        m_color{255, 255, 255, 255},
        m_background(false),
        m_backborder(false),
        m_backcolor{0, 0, 0, 255},
        m_backbordercolor{255, 255, 255, 255}
    {
        setEnabled(false);
    }

    FloatingTextRenderer::FloatingTextRenderer(FloatingTextRenderer const & old) :
        RendererBase(old),
        m_renderbackend(old.m_renderbackend),
        m_font(old.m_font),
        m_font_color(old.m_font_color),
        m_color(old.m_color),
        m_background(old.m_background),
        m_backborder(old.m_backborder),
        m_backcolor(old.m_backcolor),
        m_backbordercolor(old.m_backbordercolor)
    {
        setEnabled(false);
    }

    RendererBase* FloatingTextRenderer::clone()
    {
        return new FloatingTextRenderer(*this);
    }

    FloatingTextRenderer::~FloatingTextRenderer() = default;

    void FloatingTextRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        static_cast<void>(cam);
        static_cast<void>(layer);
        if (m_font == nullptr) {
            // no font selected.. nothing to render
            return;
        }

        auto instance_it          = instances.begin();
        uint32_t const lm         = m_renderbackend->getLightingModel();
        SDL_Color const old_color = m_font->getColor();
        if (m_font_color) {
            m_font->setColor(m_color.r, m_color.g, m_color.b, m_color.a);
        }
        for (; instance_it != instances.end(); ++instance_it) {
            Instance* instance          = (*instance_it)->instance;
            std::string const * saytext = instance->getSayText();
            if (saytext != nullptr) {
                Rect const & ir           = (*instance_it)->dimensions;
                Image* img                = m_font->getAsImageMultiline(*saytext);
                int32_t const imageWidth  = toScreenSize(img->getWidth());
                int32_t const imageHeight = toScreenSize(img->getHeight());
                Rect r;
                r.x = (ir.x + ir.w / 2) - imageWidth / 2; /// the center of the text rect is always aligned to the
                                                          /// instance's rect center.
                r.y = ir.y - imageHeight;                 /// make the text rect floating higher than the instance.
                r.w = imageWidth;
                r.h = imageHeight;
                // Without this check it can happen that changeRenderInfos() call produces an out_of_range error
                // because the image rendering can be skipped, if it's not on the screen.
                // The result is that it tried to modify more objects as exist.
                if (r.right() < 0 || std::cmp_greater(r.x, m_renderbackend->getWidth()) || r.bottom() < 0 ||
                    std::cmp_greater(r.y, m_renderbackend->getHeight())) {
                    continue;
                }
                if (m_background || m_backborder) {
                    int32_t const overdraw = 5;

                    Point const p = Point(r.x - overdraw, r.y - overdraw);

                    if (m_background) {
                        m_renderbackend->fillRectangle(
                            p,
                            toRectExtent(r.w + (2 * overdraw)),
                            toRectExtent(r.h + (2 * overdraw)),
                            m_backcolor.r,
                            m_backcolor.g,
                            m_backcolor.b,
                            m_backcolor.a);
                    }

                    if (m_backborder) {
                        m_renderbackend->drawRectangle(
                            p,
                            toRectExtent(r.w + (2 * overdraw)),
                            toRectExtent(r.h + (2 * overdraw)),
                            m_backbordercolor.r,
                            m_backbordercolor.g,
                            m_backbordercolor.b,
                            m_backbordercolor.a);
                    }
                }
                img->render(r);
                if (lm > 0) {
                    uint16_t elements = 1;
                    if (m_background) {
                        ++elements;
                    }
                    if (m_backborder) {
                        ++elements;
                    }
                    m_renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, elements, 4, 5, false, true, 255, REPLACE, ALWAYS);
                }
            }
        }
        if (m_font_color) {
            m_font->setColor(old_color.r, old_color.g, old_color.b, old_color.a);
        }
    }

    void FloatingTextRenderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_color.r = r;
        m_color.g = g;
        m_color.b = b;
        m_color.a = a;

        m_font_color = true;
    }

    void FloatingTextRenderer::setBackground(uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba)
    {
        m_backcolor.r = br;
        m_backcolor.g = bg;
        m_backcolor.b = bb;
        m_backcolor.a = ba;

        m_background = true;
    }

    void FloatingTextRenderer::setBorder(uint8_t bbr, uint8_t bbg, uint8_t bbb, uint8_t bba)
    {
        m_backbordercolor.r = bbr;
        m_backbordercolor.g = bbg;
        m_backbordercolor.b = bbb;
        m_backbordercolor.a = bba;

        m_backborder = true;
    }

    void FloatingTextRenderer::resetBackground()
    {
        m_background = false;
    }

    void FloatingTextRenderer::resetBorder()
    {
        m_backborder = false;
    }

    FloatingTextRenderer* FloatingTextRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<FloatingTextRenderer*>(cnt->getRenderer("FloatingTextRenderer"));
    }
} // namespace FIFE
