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

#ifndef FIFE_GUI_LIBROCKETINPUTPROCESSOR_H
#define FIFE_GUI_LIBROCKETINPUTPROCESSOR_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL/SDL_events.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace Rocket {
	namespace Core {
		class Context;
	}
}

namespace FIFE {
	
	class LibRocketInputProcessor { 
	public:
		
		/** Constructor
		 */
		LibRocketInputProcessor(Rocket::Core::Context* context);
		
		/** Destructor
		 */
		~LibRocketInputProcessor();
		
		/**
		 * Processes SDL input and converts it to librocket input, then forwards it to
		 * the librocket context.
		 * 
		 * @param evt The SDL input.
		 * @return A boolean value indicating if the event was consumed by librocket or not.
		 */
		bool onSdlEvent(SDL_Event &evt);
		
	private:
		
		Rocket::Core::Context* m_context;
	};
	
};

#endif //FIFE_GUI_LIBROCKETINPUTPROCESSOR_H