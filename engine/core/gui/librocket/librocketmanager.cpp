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

// 3rd party library includes
#include <Rocket/Core.h>
#include <Rocket/Controls.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/time/timemanager.h"

#include "librocketmanager.h"
#include "base/librocketinputprocessor.h"
#include "base/librocketrenderinterface.h"

namespace FIFE {
	
	LibRocketManager::LibRocketManager()
	:
	m_renderInterface(new LibRocketRenderInterface),
	m_inputProcessor(NULL) {
		Rocket::Core::SetSystemInterface(this);
		Rocket::Core::SetRenderInterface(m_renderInterface);
		Rocket::Core::Initialise();
		Rocket::Controls::Initialise();
	}
	
	LibRocketManager::~LibRocketManager() {
		unloadDocuments();
		m_context->RemoveReference();
		Rocket::Core::Shutdown();
		
		delete m_renderInterface;
		delete m_inputProcessor;
	}
	
	void LibRocketManager::init(const std::string& backend, int32_t screenWidth, int32_t screenHeight) {
		
		m_context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(screenWidth, screenHeight));
		
		m_inputProcessor = new LibRocketInputProcessor(m_context);
	}
	
	float LibRocketManager::GetElapsedTime() {
		static TimeManager *timeManager = TimeManager::instance();
		
		return timeManager->getTime() / 1000.0f;
	}
	
	void LibRocketManager::turn() {
		m_inputProcessor->turn();
		
		m_context->Update();
		m_context->Render();
		
		m_renderInterface->render();
		m_renderInterface->freeTextures();
	}
	
	void LibRocketManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		m_context->SetDimensions(Rocket::Core::Vector2i(width, height));
	}
	
	Rocket::Core::ElementDocument* LibRocketManager::loadDocument(const std::string& documentPath) {
		Rocket::Core::String rocketDocumentPath(documentPath.c_str());
		
		Rocket::Core::ElementDocument* document = m_context->LoadDocument(rocketDocumentPath);
		
		if(document != NULL) {
			m_openDocuments.insert(document);
		}
		
		return document;
	}
	
	void LibRocketManager::unloadDocument(Rocket::Core::ElementDocument* document) {
		std::set<Rocket::Core::ElementDocument*>::iterator doc(m_openDocuments.find(document));
		
		if(doc != m_openDocuments.end()) {
			document->GetContext()->UnloadDocument(document);
			m_openDocuments.erase(doc);
		}
	}
	
	void LibRocketManager::loadFont(const std::string& filepath) {
		Rocket::Core::String rocketFontPath(filepath.c_str());
		
		bool succeeded = Rocket::Core::FontDatabase::LoadFontFace(rocketFontPath);
		if(!succeeded) {
			throw GuiException("Librocket cannot open font: " + filepath);
		}
	}
	
	bool LibRocketManager::onSdlEvent(SDL_Event& evt) {
		return (m_inputProcessor != NULL) ? m_inputProcessor->onSdlEvent(evt)
		                                   : false;
	}
	
	void LibRocketManager::unloadDocuments() {
		
		std::set<Rocket::Core::ElementDocument*>::iterator currDoc(m_openDocuments.begin());
		std::set<Rocket::Core::ElementDocument*>::iterator endDocs(m_openDocuments.end());
		
		for(; currDoc != endDocs; ++currDoc) {
			unloadDocument(*currDoc);
		}
		
		m_openDocuments.clear();
	}
};