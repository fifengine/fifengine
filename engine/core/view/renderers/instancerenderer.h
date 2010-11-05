/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_INSTANCERENDERER_H
#define FIFE_INSTANCERENDERER_H

// Standard C++ library includes
#include <string>
#include <list>

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
		void render(Camera* cam, Layer* layer, RenderList& instances);
		std::string getName() { return "InstanceRenderer"; }

		/** Marks given instance to be outlined with given parameters
		 */
		void addOutlined(Instance* instance, int r, int g, int b, int width);
		
		/** Marks given instance to be colored with given parameters
		 */
		void addColored(Instance* instance, int r, int g, int b);

		/** Marks given instance to have an transparent area with given paramters
		 */
		void addTransparentArea(Instance* instance, const std::list<std::string> &groups, unsigned int w, unsigned int h, unsigned char trans, bool front = true);
		
		/** Removes instance from outlining list
		 */
		void removeOutlined(Instance* instance);
		
		/** Removes instance from coloring list
		 */
		void removeColored(Instance* instance);

		/** Removes instance form area list
		 */
		void removeTransparentArea(Instance* instance);
		
		/** Removes all outlines
		 */
		void removeAllOutlines();
		
		/** Removes all coloring
		 */
		void removeAllColored();

		/** Removes all transparent areas
		 */
		void removeAllTransparentAreas();
		
		/** Add groups(Namespaces) into a list. All instances, whose namespace is in the list
		 *  will not lighted from the LightRenderer.
		 */
		void addIgnoreLight(const std::list<std::string> &groups);

		/** Removes groups(Namespaces) from the list
		 */
		void removeIgnoreLight(const std::list<std::string> &groups);

		/** Removes all groups(Namespaces)
		 */
		void removeAllIgnoreLight();

		/** Gets instance for interface access
		 */
		static InstanceRenderer* getInstance(IRendererContainer* cnt);

		/** Provides access point to the RenderBackend
		 */
		RenderBackend* getRenderBackend() const {return m_renderbackend;}
		
		void reset();

	private:
		ImagePool* m_imagepool;
		AnimationPool* m_animationpool;
		bool m_area_layer;
		std::list<std::string> m_unlit_groups;
		
		// contains per-instance information for outline drawing
		class OutlineInfo {
		public:
			uint8_t r;
			uint8_t g;
			uint8_t b;
			int width;
			bool dirty;
			Image* outline;
			Image* curimg;
			OutlineInfo();
			~OutlineInfo();
		};
		// contains per-instance information for overlay drawing
		class ColoringInfo {
		public:
			uint8_t r;
			uint8_t g;
			uint8_t b;
			bool dirty;
			Image* overlay;
			Image* curimg;
			ColoringInfo();
			~ColoringInfo();
		};
		class AreaInfo {
		public:
			Instance* instance;
			//std::string groups;
			std::list<std::string> groups;
			unsigned int w;
			unsigned int h;
			unsigned char trans;
			bool front;
			float z;
			AreaInfo();
			~AreaInfo();
		};
		typedef std::map<Instance*, OutlineInfo> InstanceToOutlines_t;
		typedef std::map<Instance*, ColoringInfo> InstanceToColoring_t;
		typedef std::map<Instance*, AreaInfo> InstanceToAreas_t;
		
		InstanceToOutlines_t m_instance_outlines;
		InstanceToColoring_t m_instance_colorings;
		InstanceToAreas_t m_instance_areas;
		
		/** Binds new outline (if needed) to the instance's OutlineInfo
		 */
		Image* bindOutline(OutlineInfo& info, RenderItem& vc, Camera* cam);
		Image* bindColoring(ColoringInfo& info, RenderItem& vc, Camera* cam);
	};
}

#endif
