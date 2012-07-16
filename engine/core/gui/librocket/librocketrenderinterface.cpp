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
		freeTextures();
	}
	
	void LibRocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation) {
		GeometryCallData geometryCallData;
		
		for(int i = 0; i < num_vertices; i++) {
			geometryCallData.vertices.push_back(vertices[i]);
		}
		
		for(int i = 0; i < num_indices; i++) {
			geometryCallData.indices.push_back(indices[i]);
		}
		
		geometryCallData.textureHandle = texture;
		geometryCallData.translation = translation;
		
		if(m_geometryCalls.empty()) {
			GeometryCall geometryCall;
			geometryCall.callChain.push(geometryCallData);
			
			m_geometryCalls.push(geometryCall);
		} else {
			GeometryCall& geometryCall = m_geometryCalls.front();
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
		if(enable)
			m_renderBackend->enableScissorTest();
		else
			m_renderBackend->disableScissorTest();
	}
	
	void LibRocketRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
		GeometryCall gc;
		
		gc.hasScissorArea = true;
		gc.scissorArea = Rect(x, y, width, height);
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
			
			GeometryCall& currentCall = m_geometryCalls.front();
			
			bool clipped = currentCall.hasScissorArea;
			
			if(clipped)
				m_renderBackend->pushClipArea(currentCall.scissorArea, false);
			
			while(!currentCall.callChain.empty()) {
				
				GeometryCallData& currentCallData = currentCall.callChain.front();
				
				glPushMatrix();
				glTranslatef(currentCallData.translation.x, currentCallData.translation.y, 0);

				glVertexPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &currentCallData.vertices[0].position);
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rocket::Core::Vertex), &currentCallData.vertices[0].colour);

				if (!currentCallData.textureHandle)
				{
					glDisable(GL_TEXTURE_2D);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, (GLuint) currentCallData.textureHandle);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &currentCallData.vertices[0].tex_coord);
				}

				glDrawElements(GL_TRIANGLES, currentCallData.indices.size(), GL_UNSIGNED_INT, &currentCallData.indices[0]);

				glPopMatrix();
				
				currentCall.callChain.pop();
			}
			
			if(clipped)
				m_renderBackend->popClipArea();
			
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
		
		m_freedTextures.clear();
	}
};