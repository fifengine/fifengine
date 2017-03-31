/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_CELLRENDERER_H
#define FIFE_CELLRENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"

#include "targetrenderer.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class CellRenderer: public RendererBase {
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
		CellRenderer(const CellRenderer& old);

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

		/** Enables Fog of War visualization.
		 *
		 * @param enabled A bool that enables or disables the visualization.
		 */
		void setEnabledFogOfWar(bool enabled);

		/** Gets whether Fog of War visualization is enabled.
		 *
		 * @return A bool, true if visualization is enabled, otherwise false.
		 */
		bool isEnabledFogOfWar();
		
		/** Sets the layer that is used for Fog of War visualization.
		 *
		 * @param layer The layer that is used for visualization.
		 */
		void setFogOfWarLayer(Layer* layer);

		/** Sets the image that is used for concealed spaces.
		 *
		 * @param image The used image.
		 */
		void setConcealImage(ImagePtr image);

		/** Sets the fog image.
		 *
		 * @param image The used image.
		 */
		void setMaskImage(ImagePtr image);
		
		/** Creates render target for Fog of War.
		 *
		 * @param cam The used camera.
		 * @param layer The used layer.
		 */
		void createFowMap(Camera* cam, Layer* layer);

		/** Adds image to FoW render target.
		 *
		 * @param cam The used camera.
		 * @param center The coordinates.
		 * @param image The used image.
		 * @param id The id as string.
		 */
		void addImageToMap(Camera* cam, Point center, ImagePtr image, const std::string& id);

		/** Returns the layer that is used for Fog of War visualization.
		 *
		 * @return The used layer.
		 */
		Layer* getFowLayer();

		/** Enables cost visualization.
		 *
		 * @param costId The cost identifier as string.
		 * @param enabled A bool that enables or disables the visualization.
		 */
		void setEnabledCost(const std::string& costId, bool enabled);

		/** Gets whether a cost visualization is enabled.
		 *
		 * @param costId The cost identifier as string.
		 * @return A bool, true if visualization is enabled, otherwise false.
		 */
		bool isEnabledCost(const std::string& costId);

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

		/** Updates render target.
		 */
		void updateFowMap();

		/** Gets instance for interface access.
		 */
		static CellRenderer* getInstance(IRendererContainer* cnt);

	private:
		/** Updates FoW.
		 */
		void updateFoW(Camera* cam, Layer* layer);

		/** Updates cached cell costs.
		 */
		void updateCellCosts(Camera* cam, Layer* layer);

		/** Sends the buffered cell cost data to the backend.
		 *
		 * @param layer Current layer to be rendered
		 */
		void renderBuffer(Layer* layer);

		// FoW stuff
		bool m_fowEnabled;
		TargetRenderer* m_targetRenderer;
		RenderTargetPtr m_fowTarget;
		Layer* m_fowLayer;
		ImagePtr m_concealImage;
		ImagePtr m_maskImage;
		ImagePtr m_fowImage;
		IFont* m_font;
		
		// Visualize cell costs
		bool m_costsUpdate;
		std::set<std::string> m_visualCosts;

		struct Data {
			Rect rect;
			Image* image;
		};
		//! Buffers the cost images per Layer.
		std::map<Layer*, std::vector<Data> > m_bufferMap;
	};
}

#endif
