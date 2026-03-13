// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_CELLSELECTIONRENDERER_H
#define FIFE_CELLSELECTIONRENDERER_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;

    /** CellSelectionRenderer renders a frame around selected cells.
     */
    class CellSelectionRenderer : public RendererBase
    {
    public:
        /** Constructor.
         *
         * @param renderbackend The renderbackend to use.
         * @param position The position for this renderer in rendering pipeline.
         * @ see setPipelinePosition
         */
        CellSelectionRenderer(RenderBackend* renderbackend, int32_t position);

        /** Copy Constructor.
         */
        CellSelectionRenderer(const CellSelectionRenderer& old);

        /** Makes copy of this renderer.
         */
        RendererBase* clone();

        /** Destructor.
         */
        virtual ~CellSelectionRenderer();

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
            return "CellSelectionRenderer";
        }

        /** Gets instance for interface access.
         */
        static CellSelectionRenderer* getInstance(IRendererContainer* cnt);

        /** Deselects all locations.
         */
        void reset();

        /** Selects given location on map.
         *
         * @param loc The location.
         */
        void selectLocation(const Location* loc);

        /** Deselects given location on map.
         *
         * @param loc The location.
         */
        void deselectLocation(const Location* loc);

        /** Returns selected locations.
         *
         * @return A vector that contains all selected locations.
         */
        const std::vector<Location>& getLocations() const
        {
            return m_locations;
        }

        /** Changes the used color.
         *
         * @param r The value for red, range 0-255.
         * @param g The value for green, range 0-255.
         * @param b The value for blue, range 0-255.
         */
        void setColor(uint8_t r, uint8_t g, uint8_t b);

    private:
        //! selected locations
        std::vector<Location> m_locations;

        //! currently used color
        SDL_Color m_color;
    };

} // namespace FIFE

#endif
