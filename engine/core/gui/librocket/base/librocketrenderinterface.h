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

#ifndef FIFE_GUI_LIBROCKETRENDERINTERFACE_H
#define FIFE_GUI_LIBROCKETRENDERINTERFACE_H

// Standard C++ library includes
#include <list>
#include <queue>
#include <vector>

// 3rd party library includes
#include <Rocket/Core/RenderInterface.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "util/resource/resource.h"
#include "util/structures/rect.h"
#include "util/structures/point.h"
#include "video/color.h"
#include "video/renderbackend.h"

namespace FIFE {
	
	class ImageManager;
	class RenderBackend;
	
	class LibRocketRenderInterface : public Rocket::Core::RenderInterface {
	public:
		
		/** Constructor.
		 */
		LibRocketRenderInterface();
		
		/** Destructor.
		 */
		virtual ~LibRocketRenderInterface();
		
		/** Called by Rocket when it wants to render geometry that it does not wish to optimise.
		 */
		virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);

		/** Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
		 */
		virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture);

		/** Called by Rocket when it wants to render application-compiled geometry.
		 */
		virtual void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation);
		
		/** Called by Rocket when it wants to release application-compiled geometry.
		 */
		virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

		/** Called by Rocket when it wants to enable or disable scissoring to clip content.
		 */
		virtual void EnableScissorRegion(bool enable);
		
		/** Called by Rocket when it wants to change the scissor region.
		 */
		virtual void SetScissorRegion(int x, int y, int width, int height);

		/** Called by Rocket when a texture is required by the library.
		 */
		virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
		
		/** Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
		 */
		virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
		
		/** Called by Rocket when a loaded texture is no longer required.
		 */
		virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);
		
		/**
		 * Renders librocket gui.
		 */
		void render();
		
		/**
		 * Frees all textures that are no longer needed by librocket.
		 */
		void freeTextures();
		
	private:
		
		RenderBackend* m_renderBackend;
		
		ImageManager* m_imageManager;
		
		class GeometryCallData {
		public:
			std::vector<GuiVertex> vertices;
			std::vector<int> indices;
			ResourceHandle textureHandle;
			DoublePoint translation;
		};
		
		typedef std::queue<GeometryCallData> GeometryCallDataChain;
		
		class GeometryCall {
		public:
			
			GeometryCall() 
			: 
			enableScissorTest(false),
			hasScissorArea(false)
		    {
			}
			
			GeometryCallDataChain callChain;
			Rect scissorArea;
			bool enableScissorTest;
			bool hasScissorArea;
		};
		
		std::queue<GeometryCall> m_geometryCalls;
		
		std::list<ResourceHandle> m_freedTextures;
	};
	
};

#endif // FIFE_GUI_LIBROCKETRENDERINTERFACE_H