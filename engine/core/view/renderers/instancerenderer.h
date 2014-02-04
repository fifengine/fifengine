/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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
#include "util/time/timer.h"

namespace FIFE {
	class Location;
	class RenderBackend;
	class InstanceDeleteListener;

	class InstanceRenderer: public RendererBase {
	public:
		/** constructor.
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		InstanceRenderer(RenderBackend* renderbackend, int32_t position);

		InstanceRenderer(const InstanceRenderer& old);

		RendererBase* clone();

		/** Destructor.
		 */
		virtual ~InstanceRenderer();
		void render(Camera* cam, Layer* layer, RenderList& instances);
		std::string getName() { return "InstanceRenderer"; }

		/** Marks given instance to be outlined with given parameters
		 */
		void addOutlined(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t width, int32_t threshold = 1);

		/** Marks given instance to be colored with given parameters
		 */
		void addColored(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t a = 128);

		/** Marks given instance to have a transparent area with given parameters
		 */
		void addTransparentArea(Instance* instance, const std::list<std::string> &groups, uint32_t w, uint32_t h, uint8_t trans, bool front = true);

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

		/** Removes all stuff
		 */
		void reset();

		/** Sets the interval in seconds (default is 60).
		 */
		void setRemoveInterval(uint32_t interval);

		/** Gets the interval in seconds (default is 60).
		 */
		uint32_t getRemoveInterval() const;

		/** Add properly old ImagePtr into a check list.
		  * If it is still not used after a time(interval) then it is freed.
		 */
		void addToCheck(const ImagePtr& image);

		/** Timer callback, tried to remove old effect images
		 */
		void check();

		/** Removes instance from all effects.
		  * Should only be called by delete listener!
		 */
		void removeInstance(Instance* instance);

		/** Returns true if coloring need binding, otherwise false
		 */
		bool needColorBinding() { return m_need_bind_coloring; }

	private:
		bool m_area_layer;
		uint32_t m_interval;
		bool m_timer_enabled;
		std::list<std::string> m_unlit_groups;
		bool m_need_sorting;
		bool m_need_bind_coloring;

		enum InstanceRendererEffect {
			NOTHING = 0x00,
			OUTLINE = 0x01,
			COLOR = 0x02,
			AREA = 0x04
		};
		typedef uint8_t Effect;

		// contains per-instance information for outline drawing
		class OutlineInfo {
		public:
			uint8_t r;
			uint8_t g;
			uint8_t b;
			int32_t width;
			int32_t threshold;
			bool dirty;
			ImagePtr outline;
			Image* curimg;
			InstanceRenderer* renderer;
			OutlineInfo(InstanceRenderer* r);
			~OutlineInfo();
		};
		// contains per-instance information for overlay drawing
		class ColoringInfo {
		public:
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
			bool dirty;
			ImagePtr overlay;
			Image* curimg;
			InstanceRenderer* renderer;
			ColoringInfo(InstanceRenderer* r);
			~ColoringInfo();
		};
		class AreaInfo {
		public:
			Instance* instance;
			std::list<std::string> groups;
			uint32_t w;
			uint32_t h;
			uint8_t trans;
			bool front;
			double z;
			AreaInfo();
			~AreaInfo();
		};
		typedef std::map<Instance*, OutlineInfo> InstanceToOutlines_t;
		typedef std::map<Instance*, ColoringInfo> InstanceToColoring_t;
		typedef std::map<Instance*, AreaInfo> InstanceToAreas_t;

		InstanceToOutlines_t m_instance_outlines;
		InstanceToColoring_t m_instance_colorings;
		InstanceToAreas_t m_instance_areas;

		// struct to hold the ImagePtr with a timestamp
		typedef struct {
			ImagePtr image;
			uint32_t timestamp;
		} s_image_entry;
		typedef std::list<s_image_entry> ImagesToCheck_t;
		// old effect images
		ImagesToCheck_t m_check_images;
		// timer
		Timer m_timer;

		// InstanceDeleteListener to automatically remove Instance effect (outline, coloring, ...)
		InstanceDeleteListener* m_delete_listener;
		typedef std::map<Instance*, Effect> InstanceToEffects_t;
		InstanceToEffects_t m_assigned_instances;

		void renderOverlay(RenderDataType type, RenderItem* item, uint8_t const* coloringColor, bool recoloring);

		/** Binds new outline (if needed) to the instance's OutlineInfo
		 */
		Image* bindOutline(OutlineInfo& info, RenderItem& vc, Camera* cam);
		Image* bindMultiOutline(OutlineInfo& info, RenderItem& vc, Camera* cam);
		Image* bindColoring(ColoringInfo& info, RenderItem& vc, Camera* cam);

		ImagePtr getMultiColorOverlay(const RenderItem& vc, OverlayColors* colors = 0);

		void renderUnsorted(Camera* cam, Layer* layer, RenderList& instances);
		void renderAlreadySorted(Camera* cam, Layer* layer, RenderList& instances);

		void removeFromCheck(const ImagePtr& image);
		bool isValidImage(const ImagePtr& image);
	};
}

#endif
