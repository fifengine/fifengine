/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_INSTANCERENDERER_H
#define FIFE_INSTANCERENDERER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "view/rendererbase.h"

namespace FIFE {
	class Location;
	class RenderBackend;
	class ImagePool;
	class AnimationPool;
	class InstanceVisualCacheItem;

	class InstanceRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param imagepool image pool where from fetch images
		 * @param animpool animation pool where from fetch images
		 */
		InstanceRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool);
		
		InstanceRenderer(const InstanceRenderer& old);
		
		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~InstanceRenderer();
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances);
		std::string getName() { return "InstanceRenderer"; }

		/** Marks given instance to be outlined with given parameters
		 */
		void addOutlined(Instance* instance, int r, int g, int b, int width);
		
		/** Removes instance from outlining list
		 */
		void removeOutlined(Instance* instance);
		
		/** Removes all outlines
		 */
		void removeAllOutlines();
		
		/** Gets instance for interface access
		 */
		static InstanceRenderer* getInstance(IRendererContainer* cnt);
		
		void reset();

	private:
		ImagePool* m_imagepool;
		AnimationPool* m_animationpool;
		
		// contains per-instance information for outline drawing
		class OutlineInfo {
		public:
			uint8_t r;
			uint8_t g;
			uint8_t b;
			int width;
			Image* outline;
			Image* curimg;
			OutlineInfo();
			~OutlineInfo();
		};
		typedef std::map<Instance*, OutlineInfo> InstanceToOutlines_t;
		typedef std::map<Layer*, InstanceToOutlines_t> LayerToOutlineMap_t;
		// mapping of layer -> instance -> outlineinfo
		LayerToOutlineMap_t m_layer_to_outlinemap;
		
		/** Binds new outline (if needed) to the instance's OutlineInfo
		 */
		Image* bindOutline(OutlineInfo& info, InstanceVisualCacheItem& vc, Camera* cam);
	};
}

#endif
