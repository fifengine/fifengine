/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/imagemanager.h"

#include "librocketrenderinterface.h"

namespace FIFE {
	
	LibRocketRenderInterface::LibRocketRenderInterface()
	:
	m_renderBackend(RenderBackend::instance()),
	m_imageManager(ImageManager::instance()) {
	}
	
	LibRocketRenderInterface::~LibRocketRenderInterface() {
	}
	
	void LibRocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation) {
	}
	
	Rocket::Core::CompiledGeometryHandle LibRocketRenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture) {
		return (Rocket::Core::CompiledGeometryHandle) NULL;
	}
	
	void LibRocketRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation) {
	}
	
	void LibRocketRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry)) {
		
	}
	
	void LibRocketRenderInterface::EnableScissorRegion(bool enable)
	{
	}
	
	void LibRocketRenderInterface::SetScissorRegion(int x, int y, int width, int height)
	{
	}
	
	bool LibRocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source) {
	}
	
	bool LibRocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions) {
	}
	
	void LibRocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle) {
	}
};