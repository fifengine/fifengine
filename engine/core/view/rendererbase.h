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

#ifndef FIFE_RENDERERBASE_H
#define FIFE_RENDERERBASE_H

// Standard C++ library includes
#include <list>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "renderitem.h"

namespace FIFE {
	class Camera;
	class Layer;
	class Map;
	class Instance;
	class RenderBackend;
	
	class RendererBase;
	/** RendererListener allows reaction to changes in renderer
	 *  Having this implemented via callback mechanism should speed up the rendering a bit
	 *  (e.g. no need to sort pipeline order per each frame)
	 */
	class IRendererListener {
	public:
		virtual ~IRendererListener() {}
		
		/** Renderer's pipeline position has been changed
		 */
		virtual void onRendererPipelinePositionChanged(RendererBase* renderer) = 0;
		
		/** Renderer is enabled / disabled
		 */
		virtual void onRendererEnabledChanged(RendererBase* renderer) = 0;
	};
	
	/** Interface to class owning the renderers
	 * Used to get correct subclass of renderer in scripting side (via renderer's static method)
	 */
	class IRendererContainer {
	public:
		virtual ~IRendererContainer() {}
		
		/** Returns renderer with given name
		 */
		virtual RendererBase* getRenderer(const std::string& renderername) = 0;
	};
	
	/** Base class for all view renderers
	 * View renderer renders one aspect of the view shown on screen
	 */
	class RendererBase {
	public:
		/** Constructor
		 * @param renderbackend to use
		 * @param position position for this renderer in rendering pipeline
		 */
		RendererBase(RenderBackend* renderbackend, int32_t position);
		
		/** Copy Constructor
		 */
		RendererBase(const RendererBase& old);
		
		/** Makes copy of this renderer
		 */
		virtual RendererBase* clone() = 0;
		
		/** Destructor
		 */
		virtual ~RendererBase() {};
		
		/** This method is called by the view to ask renderer to draw its rendering aspect based on
		 * given parameters. Renderers receive non-clipped instance stack since there is no
		 * way to know which information is relevant for the renderer. E.g. effect renderer
		 * might need to know offscreen instance locations to be able to draw radiation coming from
		 * some instance not visible on the screen.
		 *
		 * @param cam camera view to draw
		 * @param layer current layer to be rendered
		 * @param instances instances on the current layer
		 * @ see setPipelinePosition
		 */
		virtual void render(Camera* cam, Layer* layer, RenderList& instances) = 0;
		
		/** Name of the renderer
		 */
		virtual std::string getName() = 0;
		
		/** Gets renderer position in the rendering pipeline
		 */
		int32_t getPipelinePosition() const { return m_pipeline_position; }
		
		/** Sets renderer position in the rendering pipeline
		 *  Pipeline position defines in which order view calls the renderers when update occurs
		 *  Note that renderers are called once per rendered layer, thus to update the
		 *  whole screen, renderer might receive multiple calls
		 */
		void setPipelinePosition(int32_t position);
		
		/** Enables renderer
		 */
		virtual void setEnabled(bool enabled);
		
		/** Resets information in the renderer
		 */
		virtual void reset() {}
		
		/** Is renderer enabled
		 */
		bool isEnabled() const { return m_enabled; }
		
		/** Sets listener for renderer
		 */
		void setRendererListener(IRendererListener* listener) { m_listener = listener; }
		
		/** Adds active layer to renderer. Only active layers are rendered
		 */
		void addActiveLayer(Layer* layer);
		
		/** Removes active layer from renderer.
		 */
		void removeActiveLayer(Layer* layer);
		
		/** Clears all active layers from renderer
		 */
		void clearActiveLayers();
		
		/** Activates all layers from given elevation
		 */
		void activateAllLayers(Map* elevation);
		
		/** Returns if given layer is currently activated
		 */
		bool isActivedLayer(Layer* layer);

		/** Returns list of activated layer
		 */
		std::list<Layer*> getActiveLayers() const {return m_active_layers;}
		
	
	protected:
		RendererBase();
		
		std::list<Layer*> m_active_layers;
		RenderBackend* m_renderbackend;
	
	private:
		bool m_enabled;
		int32_t m_pipeline_position;
		IRendererListener* m_listener;
	};
}

#endif
