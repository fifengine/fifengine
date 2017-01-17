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

#ifndef FIFE_GUI_LIBROCKETMANAGER_H
#define FIFE_GUI_LIBROCKETMANAGER_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes
#include <Rocket/Core/SystemInterface.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "gui/guimanager.h"

namespace Rocket {
	
	namespace Core {
		class Context;
		class ElementDocument;
	}
};

namespace FIFE {
	
	class LibRocketInputProcessor;
	class LibRocketRenderInterface;
	
	class LibRocketManager : 
	public Rocket::Core::SystemInterface,
	public IGUIManager, 
	public DynamicSingleton<LibRocketManager> {

	public:
		
		/** Constructor.
		 */
		LibRocketManager();

		/**
		 * Destructor.
		 */
		virtual ~LibRocketManager();
		
		/**
		 * Initializes the librocket manager.
		 */
		void init(const std::string& backend, int32_t screenWidth, int32_t screenHeight);
		
		/**
		 * NOTE 
		 * There is an inconsistency in the naming of this method.
		 * It's in upper camelcase and should have been in lower camelcase,
		 * but it overrides Rocket::Core::SystemInterface::GetElapsedTime.
		 * 
		 * @return The number of seconds elapsed since the start of the application.
		 */
		virtual float GetElapsedTime();
		
		/** Updates and renders the gui.
		 */
		virtual void turn();

		/** Resizes the top container.
		 * 
		 * @param x The new starting X coordinate.
		 * @param y The new starting Y coordinate.
		 * @param width The new width.
		 * @param height The new height.
		 */
		virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		
		/** Loads a rocket .rml file and shows it.
		 * 
		 * @param id Id of the document chosen by the user.
		 * @param documentPath Path to the file.
		 * @return A document if the .rml file exists, NULL otherwise.
		 */
		virtual Rocket::Core::ElementDocument* loadDocument(const std::string& id, const std::string& documentPath);
		
		/** @return Document with id.
		 */
		virtual Rocket::Core::ElementDocument* getDocument(const std::string& id);
		
		/** Unloads a rocket document.
		 */
		virtual void unloadDocument(Rocket::Core::ElementDocument* document);
		
		/** Unloads a rocket document with given id.
		 */
		virtual void unloadDocument(const std::string& id);
		
		/**
		 * Loads a font to be used by librocket.
		 * 
		 * @param filepath Path of the font.
		 */
		virtual void loadFont(const std::string& filepath);
		
		/** Receives input and converts it to librocket format, then it forwards it
		 * to librocket.
		 */
		virtual bool onSdlEvent(SDL_Event& evt);
		
		/** Shows the librocket debugger.
		 */
		void showDebugger() const;
		
		/**
		 * Hides the librocket debugger.
		 */
		void hideDebugger() const;
		
	private:
		
		/**
		 * Unloads documents opened by librocket.
		 */
		void unloadDocuments();
		
		/** Librocket's context.
		 */
		Rocket::Core::Context* m_context;
		
		/** Render Interface for librocket.
		 */
		LibRocketRenderInterface* m_renderInterface;
		
		/** Input processor for librocket.
		 */
		LibRocketInputProcessor* m_inputProcessor;
		
		/** A set of all open documents.
		 */
		std::map<std::string, Rocket::Core::ElementDocument*> m_openDocuments;

	};
};

#endif //FIFE_GUI_LIBROCKETMANAGER_H