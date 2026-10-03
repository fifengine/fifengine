// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "floatingtextrenderer.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <format>
#include <limits>
#include <memory>
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
#include "view/renderers/speechbubblegeometry.h"
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
            assert(std::cmp_less_equal(value, std::numeric_limits<uint16_t>::max()));
            return static_cast<uint16_t>(value);
        }

        // Restores the font colour it was constructed with when leaving scope.
        struct FontColorGuard
        {
                IFont* font;
                SDL_Color previous;

                explicit FontColorGuard(IFont* f) : font(f), previous(f->getColor())
                {
                }

                ~FontColorGuard()
                {
                    font->setColor(previous.r, previous.g, previous.b, previous.a);
                }
        };
    } // namespace

    namespace
    {
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_VIEWVIEW);
            return log;
        }
    } // namespace

    FloatingTextRenderer::FloatingTextRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position), m_font(nullptr)
    {
        setEnabled(false);
    }

    FloatingTextRenderer::FloatingTextRenderer(FloatingTextRenderer const & old) :
        RendererBase(old), m_font(old.m_font), m_defaultStyle(old.m_defaultStyle)
    {
        // Per-instance styles are deliberately not copied: the clone never
        // registers as a delete listener on those instances.
        setEnabled(false);
    }

    std::unique_ptr<RendererBase> FloatingTextRenderer::clone()
    {
        return std::make_unique<FloatingTextRenderer>(*this);
    }

    FloatingTextRenderer::~FloatingTextRenderer()
    {
        clearAllStyles();
    }

    void FloatingTextRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        static_cast<void>(cam);
        static_cast<void>(layer);
        if (m_font == nullptr) {
            // no font selected.. nothing to render
            return;
        }

        auto instance_it  = instances.begin();
        uint32_t const lm = m_renderbackend->getLightingModel();

        for (; instance_it != instances.end(); ++instance_it) {
            Instance const * instance   = (*instance_it)->instance;
            std::string const * saytext = instance->getSayText();
            if (saytext == nullptr) {
                continue;
            }

            SpeechStyle const & style = getEffectiveStyle(instance);
            IFont* useFont            = style.fontOverride != nullptr ? style.fontOverride : m_font;

            // Guard first, so it restores the colour the font had on entry.
            FontColorGuard const guard(useFont);
            if (style.hasTextColorOverride) {
                useFont->setColor(
                    style.textColorOverride.getR(),
                    style.textColorOverride.getG(),
                    style.textColorOverride.getB(),
                    style.textColorOverride.getAlpha());
            }

            std::string displayText = *saytext;
            bool hasLineBreaks      = displayText.find('\n') != std::string::npos;
            if (style.maxTextWidth > 0) {
                displayText   = useFont->splitTextToWidth(*saytext, style.maxTextWidth);
                hasLineBreaks = true;
            }

            Image* img = hasLineBreaks ? useFont->getAsImageMultiline(displayText) : useFont->getAsImage(displayText);

            int32_t const imageWidth  = toScreenSize(img->getWidth());
            int32_t const imageHeight = toScreenSize(img->getHeight());
            Rect const & ir           = (*instance_it)->dimensions;

            bool const hasBubble = resolveBubbleType(style) != BubbleType::NONE;

            int32_t const pad      = std::max(0, style.bubblePadding);
            int32_t const contentW = std::max(imageWidth, style.maxTextWidth);
            Rect textRect;
            textRect.w = imageWidth;
            textRect.h = imageHeight;

            Rect bub;
            if (hasBubble) {
                // Leave a gap below the bubble for the tail to span, otherwise its
                // tip would sit above its base and the triangle would be inverted.
                int32_t const tailGap = std::max(0, style.tailSize);
                bub.w                 = contentW + 2 * pad;
                bub.h                 = imageHeight + 2 * pad;
                bub.x                 = (ir.x + (ir.w / 2)) - (bub.w / 2);
                bub.y                 = ir.y - imageHeight - pad - tailGap;

                textRect.x = bub.x + pad;
                textRect.y = bub.y + pad;
                if (style.textAlign == TextAlign::CENTER) {
                    textRect.x += (contentW - imageWidth) / 2;
                } else if (style.textAlign == TextAlign::RIGHT) {
                    textRect.x += contentW - imageWidth;
                }
            } else {
                // the center of the text rect is always aligned to the instance's rect center.
                textRect.x = (ir.x + (ir.w / 2)) - (imageWidth / 2);
                textRect.y = ir.y - imageHeight; // make the text rect floating higher than the instance.
            }

            // Without this check it can happen that changeRenderInfos() call produces an out_of_range error
            // because the image rendering can be skipped, if it's not on the screen.
            // The result is that it tried to modify more objects as exist.
            Rect const cullRect = hasBubble ? bub : textRect;
            if (cullRect.right() < 0 || std::cmp_greater(cullRect.x, m_renderbackend->getWidth()) ||
                cullRect.bottom() < 0 || std::cmp_greater(cullRect.y, m_renderbackend->getHeight())) {
                continue;
            }

            uint16_t elements = 1;
            Point const instanceAnchor(ir.x + ir.w / 2, ir.y);

            if (hasBubble) {
                elements += static_cast<uint16_t>(drawBuiltInBubble(m_renderbackend, bub, instanceAnchor, style));
            } else if (style.hasBackgroundOverride || style.hasBorderOverride) {
                Point const p(textRect.x - pad, textRect.y - pad);
                uint16_t const w = toRectExtent(textRect.w + 2 * pad);
                uint16_t const h = toRectExtent(textRect.h + 2 * pad);

                if (style.hasBackgroundOverride) {
                    Color const & c = style.backgroundColorOverride;
                    m_renderbackend->fillRectangle(p, w, h, c.getR(), c.getG(), c.getB(), c.getAlpha());
                    ++elements;
                }
                if (style.hasBorderOverride) {
                    Color const & c = style.borderColorOverride;
                    m_renderbackend->drawRectangle(p, w, h, c.getR(), c.getG(), c.getB(), c.getAlpha());
                    ++elements;
                }
            }

            img->render(textRect);
            if (lm > 0) {
                m_renderbackend->changeRenderInfos(
                    RENDER_DATA_WITHOUT_Z, elements, 4, 5, false, true, 255, REPLACE, ALWAYS);
            }
        }
    }

    BubbleType FloatingTextRenderer::resolveBubbleType(SpeechStyle const & style)
    {
        if (style.bubbleType == BubbleType::NONE || style.bubbleType == BubbleType::CLASSIC) {
            return style.bubbleType;
        }
        // ROUND/THOUGHT/SHOUT/WHISPER are only drawn by fcn::SpeechBubble so far.
        if (m_warnedBubbleTypes.insert(static_cast<uint8_t>(style.bubbleType)).second) {
            FL_WARN(
                _log(),
                std::format(
                    "SpeechStyle bubble type {} is not drawn by FloatingTextRenderer, using CLASSIC",
                    static_cast<int>(style.bubbleType)));
        }
        return BubbleType::CLASSIC;
    }

    TailDirection FloatingTextRenderer::resolveTailDirection(
        SpeechStyle const & style, Point const & anchor, Rect const & bubble)
    {
        TailDirection const inferred = detail::inferTailDirection(anchor, bubble);
        if (style.tailDir == TailDirection::AUTO || style.tailDir == inferred) {
            return inferred;
        }
        // The bubble is always drawn above the speaker, so only the downward
        // tail has a base edge facing the anchor. Any other direction would put
        // the triangle inside or across the body instead of protruding from it.
        if (m_warnedTailDirections.insert(static_cast<uint8_t>(style.tailDir)).second) {
            FL_WARN(
                _log(),
                std::format(
                    "SpeechStyle tail direction {} cannot be drawn above the speaker, using {}",
                    static_cast<int>(style.tailDir),
                    static_cast<int>(inferred)));
        }
        return inferred;
    }

    int32_t FloatingTextRenderer::drawBuiltInBubble(
        RenderBackend* rb, Rect const & bubbleRect, Point const & instanceAnchor, SpeechStyle const & style)
    {
        int32_t primitives = 0;

        // Nothing is drawn unless a colour was asked for: an implicit white fill
        // would swallow the default light text colour.
        bool const fillBody = style.hasBackgroundOverride;
        bool const drawEdge = style.hasBorderOverride;
        if (!fillBody && !drawEdge) {
            return 0;
        }

        Color bg           = style.backgroundColorOverride;
        Color const border = style.borderColorOverride;

        int32_t const x  = bubbleRect.x;
        int32_t const y  = bubbleRect.y;
        int32_t const bw = bubbleRect.w;
        int32_t const bh = bubbleRect.h;
        int32_t const cr = detail::clampCornerRadius(bubbleRect, style.cornerRadius);

        // Angles grow clockwise on screen: 0 is right, 90 down, 180 left, 270 up.
        struct Corner
        {
                Point center;
                int32_t start;
                int32_t end;
        };
        Corner const corners[4] = {
            {Point(x + cr, y + cr), 180, 270},
            {Point(x + bw - cr, y + cr), 270, 360},
            {Point(x + bw - cr, y + bh - cr), 0, 90},
            {Point(x + cr, y + bh - cr), 90, 180},
        };

        // 90 degree segments rather than full circles: together with the three
        // rectangles below they tile the shape exactly, so a translucent fill
        // never blends twice and leaves darker corners.
        auto const fillCorners = [&]() {
            for (Corner const & corner : corners) {
                rb->drawFillCircleSegment(
                    corner.center,
                    static_cast<uint32_t>(cr),
                    corner.start,
                    corner.end,
                    bg.getR(),
                    bg.getG(),
                    bg.getB(),
                    bg.getAlpha());
                ++primitives;
            }
        };

        auto const fillBodyRects = [&]() {
            rb->fillRectangle(
                Point(x + cr, y),
                toRectExtent(bw - 2 * cr),
                toRectExtent(bh),
                bg.getR(),
                bg.getG(),
                bg.getB(),
                bg.getAlpha());
            rb->fillRectangle(
                Point(x, y + cr),
                toRectExtent(cr),
                toRectExtent(bh - 2 * cr),
                bg.getR(),
                bg.getG(),
                bg.getB(),
                bg.getAlpha());
            rb->fillRectangle(
                Point(x + bw - cr, y + cr),
                toRectExtent(cr),
                toRectExtent(bh - 2 * cr),
                bg.getR(),
                bg.getG(),
                bg.getB(),
                bg.getAlpha());
            primitives += 3;
        };

        if (cr > 0 && fillBody) {
            fillCorners();
            fillBodyRects();
        } else if (fillBody) {
            rb->fillRectangle(
                Point(x, y), toRectExtent(bw), toRectExtent(bh), bg.getR(), bg.getG(), bg.getB(), bg.getAlpha());
            ++primitives;
        }

        if (drawEdge && border.getAlpha() > 0) {
            // Straight edges between the corner arcs.
            rb->drawLine(
                Point(x + cr, y),
                Point(x + bw - cr, y),
                border.getR(),
                border.getG(),
                border.getB(),
                border.getAlpha());
            rb->drawLine(
                Point(x + cr, y + bh),
                Point(x + bw - cr, y + bh),
                border.getR(),
                border.getG(),
                border.getB(),
                border.getAlpha());
            rb->drawLine(
                Point(x, y + cr),
                Point(x, y + bh - cr),
                border.getR(),
                border.getG(),
                border.getB(),
                border.getAlpha());
            rb->drawLine(
                Point(x + bw, y + cr),
                Point(x + bw, y + bh - cr),
                border.getR(),
                border.getG(),
                border.getB(),
                border.getAlpha());
            primitives += 4;

            if (cr > 0) {
                for (Corner const & corner : corners) {
                    rb->drawCircleSegment(
                        corner.center,
                        static_cast<uint32_t>(cr),
                        corner.start,
                        corner.end,
                        border.getR(),
                        border.getG(),
                        border.getB(),
                        border.getAlpha());
                    ++primitives;
                }
            }
        }

        TailDirection const dir = resolveTailDirection(style, instanceAnchor, bubbleRect);
        Point tip;
        if (dir == TailDirection::UP || dir == TailDirection::DOWN) {
            tip = Point(instanceAnchor.x + style.tailOffset, instanceAnchor.y);
        } else {
            tip = Point(instanceAnchor.x, instanceAnchor.y + style.tailOffset);
        }

        auto const tail = detail::computeTailVertices(bubbleRect, tip, dir, style.tailSize);
        if (tail[0] != tail[1] && tail[1] != tail[2]) {
            if (fillBody) {
                rb->fillTriangle(tail[0], tail[1], tail[2], bg.getR(), bg.getG(), bg.getB(), bg.getAlpha());
                ++primitives;
            }

            if (drawEdge && border.getAlpha() > 0) {
                // drawTriangle fills on the OpenGL backend and only outlines on
                // the SDL one, so the edges are stroked individually instead.
                rb->drawLine(tail[0], tail[1], border.getR(), border.getG(), border.getB(), border.getAlpha());
                rb->drawLine(tail[1], tail[2], border.getR(), border.getG(), border.getB(), border.getAlpha());
                rb->drawLine(tail[2], tail[0], border.getR(), border.getG(), border.getB(), border.getAlpha());
                primitives += 3;
            }
        }

        return primitives;
    }

    void FloatingTextRenderer::setSpeechStyle(Instance* instance, SpeechStyle const & style)
    {
        if (instance == nullptr) {
            return;
        }
        m_speechStyles[instance] = style;
        if (m_styledInstances.insert(instance).second) {
            instance->addDeleteListener(this);
        }
    }

    void FloatingTextRenderer::clearSpeechStyle(Instance* instance)
    {
        if (m_speechStyles.erase(instance) == 0) {
            return;
        }
        if (m_styledInstances.erase(instance) == 1) {
            instance->removeDeleteListener(this);
        }
    }

    void FloatingTextRenderer::clearAllStyles()
    {
        for (Instance* instance : m_styledInstances) {
            instance->removeDeleteListener(this);
        }
        m_styledInstances.clear();
        m_speechStyles.clear();
    }

    void FloatingTextRenderer::setDefaultSpeechStyle(SpeechStyle const & style)
    {
        m_defaultStyle = style;
    }

    SpeechStyle const & FloatingTextRenderer::getDefaultSpeechStyle() const
    {
        return m_defaultStyle;
    }

    SpeechStyle const & FloatingTextRenderer::getEffectiveStyle(Instance const * instance) const
    {
        auto const it = m_speechStyles.find(const_cast<Instance*>(instance));
        return it != m_speechStyles.end() ? it->second : m_defaultStyle;
    }

    void FloatingTextRenderer::onInstanceDeleted(Instance* instance)
    {
        m_speechStyles.erase(instance);
        m_styledInstances.erase(instance);
    }

    FloatingTextRenderer* FloatingTextRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<FloatingTextRenderer*>(cnt->getRenderer("FloatingTextRenderer"));
    }
} // namespace FIFE
