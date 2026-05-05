// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_COORDINATERENDERER_H
#define FIFE_COORDINATERENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/structures/rect.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;
    class IFont;

    class /*FIFE_API*/ CoordinateRenderer : public RendererBase
    {
        public:
            /** Constructor.
             *
             * @param renderbackend The renderbackend to use.
             * @param position The position for this renderer in rendering pipeline.
             * @ see setPipelinePosition
             */
            CoordinateRenderer(RenderBackend* renderbackend, int32_t position);

            /** Copy Constructor.
             */
            CoordinateRenderer(CoordinateRenderer const & old);

            /** Makes copy of this renderer.
             */
            RendererBase* clone();

            /** Destructor.
             */
            virtual ~CoordinateRenderer();

            /** This method is called by the view to ask renderer to draw its rendering aspect based on
             * given parameters.
             *
             * @param cam Camera view to draw
             * @param layer Current layer to be rendered
             * @param instances Instances on the current layer
             */
            void render(Camera* cam, Layer* layer, RenderList& instances);

            /** Returns the renderer name.
             *
             * @return The name as string.
             */
            std::string getName()
            {
                return "CoordinateRenderer";
            }

            /** Changes the used color.
             *
             * @param r The value for red, range 0-255.
             * @param g The value for green, range 0-255.
             * @param b The value for blue, range 0-255.
             */
            void setColor(uint8_t r, uint8_t g, uint8_t b);

            /** Changes default font in the renderer
             * @note that this does not change the font ownership
             *
             * @param font Pointer to the font
             */
            void setFont(IFont* font)
            {
                m_font = font;
            }

            /** Enables / disables zooming for coordinate images, by default it is enabled.
             */
            void setZoom(bool enable)
            {
                m_zoom = enable;
            }

            /** Gets instance for interface access.
             */
            static CoordinateRenderer* getInstance(IRendererContainer* cnt);

        private:
            void adjustLayerArea();

            Rect m_layer_area;
            Location m_tmploc;
            ExactModelCoordinate m_c;
            IFont* m_font;
            bool m_font_color;
            SDL_Color m_color;
            bool m_zoom;
    };
} // namespace FIFE

#endif
