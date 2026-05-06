// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GRIDRENDERER_H
#define FIFE_GRIDRENDERER_H

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

    class /*FIFE_API*/ GridRenderer : public RendererBase
    {
        public:
            /** constructor.
             * @param renderbackend to use
             * @param position position for this renderer in rendering pipeline
             */
            GridRenderer(RenderBackend* renderbackend, int32_t position);

            GridRenderer(GridRenderer const & old);

            RendererBase* clone();

            /** Destructor.
             */
            virtual ~GridRenderer();

            void render(Camera* cam, Layer* layer, RenderList& instances);
            std::string getName()
            {
                return "GridRenderer";
            }
            void setColor(Uint8 r, Uint8 g, Uint8 b);

            static GridRenderer* getInstance(IRendererContainer* cnt);

        private:
            SDL_Color m_color;
    };
} // namespace FIFE

#endif
