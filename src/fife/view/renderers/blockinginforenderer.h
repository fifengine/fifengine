// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_BLOCKINGINFORENDERER_H
#define FIFE_BLOCKINGINFORENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;

    class FIFE_API BlockingInfoRenderer : public RendererBase
    {
        public:
            /** Constructor.
             *
             * @param renderbackend The renderbackend to use.
             * @param position The position for this renderer in rendering pipeline.
             * @ see setPipelinePosition
             */
            BlockingInfoRenderer(RenderBackend* renderbackend, int32_t position);

            /** Copy Constructor.
             */
            BlockingInfoRenderer(BlockingInfoRenderer const & old);

            /** Makes copy of this renderer.
             */
            RendererBase* clone();

            /** Destructor.
             */
            virtual ~BlockingInfoRenderer();

            /** Returns the renderer name.
             *
             * @return The name as string.
             */
            std::string getName()
            {
                return "BlockingInfoRenderer";
            }

            /** This method is called by the view to ask renderer to draw its rendering aspect based on
             * given parameters.
             *
             * @param cam Camera view to draw
             * @param layer Current layer to be rendered
             * @param instances Instances on the current layer
             */
            void render(Camera* cam, Layer* layer, RenderList& instances);

            /** Changes the used color.
             *
             * @param r The value for red, range 0-255.
             * @param g The value for green, range 0-255.
             * @param b The value for blue, range 0-255.
             */
            void setColor(uint8_t r, uint8_t g, uint8_t b);

            /** Gets instance for interface access.
             */
            static BlockingInfoRenderer* getInstance(IRendererContainer* cnt);

        private:
            //! currently used color
            SDL_Color m_color;
    };
} // namespace FIFE

#endif
