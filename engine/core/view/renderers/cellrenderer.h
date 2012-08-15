/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
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
	class Cell;
	class IFont;

	class CellRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 */
		CellRenderer(RenderBackend* renderbackend, int32_t position);
		
		CellRenderer(const CellRenderer& old);

		RendererBase* clone();
		
		/** Destructor.
		 */
		virtual ~CellRenderer();

		std::string getName();
		void render(Camera* cam, Layer* layer, RenderList& instances);
		void setBlockerColor(uint8_t r, uint8_t g, uint8_t b);
		void setPathColor(uint8_t r, uint8_t g, uint8_t b);
		
		void setEnabledBlocking(bool enabled);
		bool isEnabledBlocking();
		void setEnabledFogOfWar(bool enabled);
		bool isEnabledFogOfWar();
		void setEnabledPathVisual(bool enabled);
		bool isEnabledPathVisual();
		
		void setFogOfWarLayer(Layer* layer);
		void setConcealImage(ImagePtr image);
		void setMaskImage(ImagePtr image);
		
		void addPathVisual(Instance* instance);
		void removePathVisual(Instance* instance);

		void createFowMap(Camera* cam, Layer* layer);
		void addConcealImageToMap(Camera* cam, Point center, ImagePtr image);
		void addMaskImageToMap(Camera* cam, Point center, ImagePtr image);
		Layer* getFowLayer();

		void setEnabledCost(const std::string& costId, bool enabled);
		bool isEnabledCost(const std::string& costId);
		void setFont(IFont* font);
		IFont* getFont();
		void updateFowMap();

		static CellRenderer* getInstance(IRendererContainer* cnt);

	private:
		SDL_Color m_blockerColor;
		SDL_Color m_pathColor;
		bool m_blockingEnabled;
		bool m_fowEnabled;
		bool m_pathVisualEnabled;
		// FoW stuff
		TargetRenderer* m_targetRenderer;
		RenderTargetPtr m_fowTarget;
		Layer* m_fowLayer;
		ImagePtr m_concealImage;
		ImagePtr m_maskImage;
		ImagePtr m_fowImage;
		IFont* m_font;
		
		std::vector<Instance*> m_visualPaths;
		std::set<std::string> m_visualCosts;
	};
}

#endif
