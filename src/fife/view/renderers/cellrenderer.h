// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_CELLRENDERER_H
#define FIFE_CELLRENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <set>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "targetrenderer.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;
    class Cell;
    class IFont;

    class FIFE_API CellRenderer : public RendererBase
    {
        public:
            /** Constructor.
             *
             * @param renderbackend The renderbackend to use.
             * @param position The position for this renderer in rendering pipeline.
             * @ see setPipelinePosition
             */
            CellRenderer(RenderBackend* renderbackend, int32_t position);

            /** Copy Constructor.
             */
            CellRenderer(CellRenderer const & old);

            /** Makes copy of this renderer.
             */
            RendererBase* clone();

            /** Destructor.
             */
            virtual ~CellRenderer();

            /** Returns the renderer name.
             *
             * @return The name as string.
             */
            std::string getName();

            /** This method is called by the view to ask renderer to draw its rendering aspect based on
             * given parameters.
             *
             * @param cam Camera view to draw
             * @param layer Current layer to be rendered
             * @param instances Instances on the current layer
             */
            void render(Camera* cam, Layer* layer, RenderList& instances);

            /** Sets color that is used to visualize blocker.
             *
             * @param r The value for red, range 0-255.
             * @param g The value for green, range 0-255.
             * @param b The value for blue, range 0-255.
             */
            void setBlockerColor(uint8_t r, uint8_t g, uint8_t b);

            /** Sets color that is used to visualize paths.
             *
             * @param r The value for red, range 0-255.
             * @param g The value for green, range 0-255.
             * @param b The value for blue, range 0-255.
             */
            void setPathColor(uint8_t r, uint8_t g, uint8_t b);

            /** Enables blocking visualization.
             *
             * @param enabled A bool that enables or disables the visualization.
             */
            void setEnabledBlocking(bool enabled);

            /** Gets whether blocking visualization is enabled.
             *
             * @return A bool, true if visualization is enabled, otherwise false.
             */
            bool isEnabledBlocking() const;

            /** Enables path visualization.
             *
             * @param enabled A bool that enables or disables the visualization.
             */
            void setEnabledPathVisual(bool enabled);

            /** Gets whether path visualization is enabled.
             *
             * @return A bool, true if visualization is enabled, otherwise false.
             */
            bool isEnabledPathVisual() const;

            /** Adds a instance to path visualization. The current path is automatically rendered.
             *
             * @param instance The instance's path is visualized.
             */
            void addPathVisual(Instance* instance);

            /** Removes a instance from path visualization.
             *
             * @param instance The instance's path is visualized.
             */
            void removePathVisual(Instance* instance);

            /** Enables cost visualization.
             *
             * @param costId The cost identifier as string.
             * @param enabled A bool that enables or disables the visualization.
             */
            void setEnabledCost(std::string const & costId, bool enabled);

            /** Gets whether a cost visualization is enabled.
             *
             * @param costId The cost identifier as string.
             * @return A bool, true if visualization is enabled, otherwise false.
             */
            bool isEnabledCost(std::string const & costId);

            /** Sets the font.
             *
             * @param font The used font.
             */
            void setFont(IFont* font);

            /** Returns the font.
             *
             * @return The used font.
             */
            IFont* getFont();

            /** Gets instance for interface access.
             */
            static CellRenderer* getInstance(IRendererContainer* cnt);

        private:
            SDL_Color m_blockerColor;
            SDL_Color m_pathColor;
            bool m_blockingEnabled;
            bool m_pathVisualEnabled;
            IFont* m_font;

            std::vector<Instance*> m_visualPaths;
            std::set<std::string> m_visualCosts;
    };
} // namespace FIFE

#endif
