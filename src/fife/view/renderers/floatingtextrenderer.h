// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FLOATINGTEXTRENDERER_H
#define FIFE_FLOATINGTEXTRENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

// 3rd party library includes

// FIFE includes
#include "model/structures/instance.h"
#include "util/structures/rect.h"
#include "view/rendererbase.h"
#include "view/renderers/speechstyle.h"

namespace FIFE
{
    class RenderBackend;
    class IFont;

    class FIFE_API FloatingTextRenderer : public RendererBase, public InstanceDeleteListener
    {
        public:
            /** Constructor.
             *
             * @param renderbackend The renderbackend to use.
             * @param position The position for this renderer in rendering pipeline.
             * @ see setPipelinePosition
             */
            FloatingTextRenderer(RenderBackend* renderbackend, int32_t position);

            /** Copy Constructor.
             */
            FloatingTextRenderer(FloatingTextRenderer const & old);

            /** Makes copy of this renderer.
             */
            std::unique_ptr<RendererBase> clone() override;

            /** Destructor.
             */
            ~FloatingTextRenderer() override;

            /** This method is called by the view to ask renderer to draw its rendering aspect based on
             * given parameters.
             *
             * @param cam Camera view to draw
             * @param layer Current layer to be rendered
             * @param instances Instances on the current layer
             */
            void render(Camera* cam, Layer* layer, RenderList& instances) override;

            /** Returns the renderer name.
             *
             * @return The name as string.
             */
            std::string getName() override
            {
                return "FloatingTextRenderer";
            }

            /** Changes default font in the renderer
             * Note that this does not change the font ownership
             */
            void setFont(IFont* font)
            {
                m_font = font;
            }

            /** Gets instance for interface access.
             */
            static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

            /** Provides access point to the RenderBackend
             */
            RenderBackend* getRenderBackend() const
            {
                return RendererBase::m_renderbackend;
            }

            /**
             * Sets the speech style for a single instance.
             *  Registers as delete listener on the instance while a style is held.
             *  @param instance instance to style, must outlive the style registration
             *  @param style style to apply, copied
             */
            void setSpeechStyle(Instance* instance, SpeechStyle const & style);

            /**
             * Drops the per-instance style, falling back to the default style.
             */
            void clearSpeechStyle(Instance* instance);

            /**
             * Drops all per-instance styles and unregisters all delete listeners.
             */
            void clearAllStyles();

            /**
             * Sets the style used for instances without a per-instance style.
             */
            void setDefaultSpeechStyle(SpeechStyle const & style);

            /**
             * Returns the style used for instances without a per-instance style.
             */
            SpeechStyle const & getDefaultSpeechStyle() const;

            /**
             * Returns the effective style: per-instance if set, otherwise the default.
             */
            SpeechStyle const & getEffectiveStyle(Instance const * instance) const;

            /**
             * Drops the style of a destroyed instance.
             */
            void onInstanceDeleted(Instance* instance) override;

        private:
            /**
             * Maps a requested bubble type onto one this renderer can draw.
             */
            BubbleType resolveBubbleType(SpeechStyle const & style);

            /**
             * Maps a requested tail direction onto one this renderer can draw.
             */
            TailDirection resolveTailDirection(SpeechStyle const & style, Point const & anchor, Rect const & bubble);

            /**
             * Draws the bubble body, border and tail. Returns the queued primitive count.
             */
            int32_t drawBuiltInBubble(
                RenderBackend* rb,
                Rect const & bubbleRect,
                Point const & instanceAnchor,
                SpeechStyle const & style);

            IFont* m_font;
            std::unordered_map<Instance*, SpeechStyle> m_speechStyles;
            std::unordered_set<Instance*> m_styledInstances;
            std::set<uint8_t> m_warnedBubbleTypes;
            std::set<uint8_t> m_warnedTailDirections;
            SpeechStyle m_defaultStyle;
    };

} // namespace FIFE

#endif
