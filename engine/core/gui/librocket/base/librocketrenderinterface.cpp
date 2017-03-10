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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/imagemanager.h"
#include "video/opengl/glimage.h"

#include "librocketrenderinterface.h"

namespace FIFE {
	
	LibRocketRenderInterface::LibRocketRenderInterface()
	:
	m_renderBackend(RenderBackend::instance()),
	m_imageManager(ImageManager::instance()) {
	}
	
	LibRocketRenderInterface::~LibRocketRenderInterface() {
		freeTextures();
	}
	
	void LibRocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation) {
		
		GeometryCallData geometryCallData;
		
		geometryCallData.vertices.reserve(num_vertices);
		for(int i = 0; i < num_vertices; i++) {
			GuiVertex vertex;
			
			vertex.position.set(vertices[i].position.x, vertices[i].position.y);
			vertex.color.set(vertices[i].colour.red, vertices[i].colour.green, vertices[i].colour.blue, vertices[i].colour.alpha);
			vertex.texCoords.set(vertices[i].tex_coord.x, vertices[i].tex_coord.y);
			
			geometryCallData.vertices.push_back(vertex);
		}
		
		geometryCallData.indices.reserve(num_indices);
		for(int i = 0; i < num_indices; i++) {
			geometryCallData.indices.push_back(indices[i]);
		}
		
		geometryCallData.textureHandle = texture;
		geometryCallData.translation.set(translation.x, translation.y);
		
		if(m_geometryCalls.empty()) {
			GeometryCall geometryCall;
			
			geometryCall.callChain.push(geometryCallData);
			m_geometryCalls.push(geometryCall);
		} else {
			GeometryCall& geometryCall = m_geometryCalls.back();
			
			geometryCall.callChain.push(geometryCallData);
		}
	}
	
	Rocket::Core::CompiledGeometryHandle LibRocketRenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture) {
		return (Rocket::Core::CompiledGeometryHandle) NULL;
	}
	
	void LibRocketRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation) {
	}
	
	void LibRocketRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry)) {
	}
	
	void LibRocketRenderInterface::EnableScissorRegion(bool enable) {
		GeometryCall gc;
		
		gc.enableScissorTest = enable;
		
		//check if there is a previous call and has a clip area enabled
		if(!m_geometryCalls.empty()) {
			GeometryCall& prevgc = m_geometryCalls.back();
			
			if(prevgc.hasScissorArea) {
				gc.hasScissorArea = true;
				gc.scissorArea = prevgc.scissorArea;
			}
		}
		
		m_geometryCalls.push(gc);
	}
	
	void LibRocketRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
		GeometryCall gc;
		
		gc.hasScissorArea = true;
		gc.scissorArea = Rect(x, y, width, height);
		
		//check if there is a previous call and has scissors enabled
		if(!m_geometryCalls.empty()) {
			GeometryCall& prevgc = m_geometryCalls.back();
			
			gc.enableScissorTest = prevgc.enableScissorTest;
		}
		
		m_geometryCalls.push(gc);
	}
	
	bool LibRocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source) {
		
		std::string filename(source.CString());
		
		ImagePtr loadedTexture = m_imageManager->exists(filename) ? 
		                         m_imageManager->get(filename) :
		                         m_imageManager->load(filename);
		
		texture_handle = static_cast<Rocket::Core::TextureHandle>(loadedTexture->getHandle());
		texture_dimensions.x = loadedTexture->getWidth();
		texture_dimensions.y = loadedTexture->getHeight();
		
		return true;
	}
	
	bool LibRocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions) {
		
		Image* generatedTexture = m_renderBackend->createImage(source, source_dimensions.x, source_dimensions.y);
		
		generatedTexture->setState(IResource::RES_LOADED);
		m_imageManager->add(generatedTexture);
		
		texture_handle = static_cast<Rocket::Core::TextureHandle>(generatedTexture->getHandle());
		
		return true;
	}
	
	void LibRocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle) {
		
		ResourceHandle rh = static_cast<ResourceHandle>(texture_handle);
		m_freedTextures.push_back(rh);
	}
	
	void LibRocketRenderInterface::render() {
		
		while(!m_geometryCalls.empty()) {
			GeometryCall& geometryCall = m_geometryCalls.front();
			
			if(geometryCall.hasScissorArea) {
				m_renderBackend->pushClipArea(geometryCall.scissorArea, false);
			}
			
			if(geometryCall.enableScissorTest) {
				m_renderBackend->enableScissorTest();
			} else {
				m_renderBackend->disableScissorTest();
			}
			
			while(!geometryCall.callChain.empty()) {
				GeometryCallData& geometryCallData = geometryCall.callChain.front();
				
				ImagePtr img = m_imageManager->get(geometryCallData.textureHandle);
				
				m_renderBackend->renderGuiGeometry(geometryCallData.vertices, geometryCallData.indices, geometryCallData.translation, img);
				
				geometryCall.callChain.pop();
			}
			
			if(geometryCall.hasScissorArea) {
				m_renderBackend->popClipArea();
			}

			m_geometryCalls.pop();
		}
	}
	
	void LibRocketRenderInterface::freeTextures() {
		std::list<ResourceHandle>::iterator it(m_freedTextures.begin());
		std::list<ResourceHandle>::iterator end(m_freedTextures.end());
		
		for(; it != end; ++it) {
			ResourceHandle rh = (*it);
			ImagePtr texture = m_imageManager->get(rh);
			texture->free();
		}
		
		std::list<ResourceHandle> temp;
		m_freedTextures.swap(temp);
	}
};