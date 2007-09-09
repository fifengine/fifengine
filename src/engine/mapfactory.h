/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_MAPFACTORY_H
#define FIFE_MAPFACTORY_H

#include "singleton.h"
#include <string>
#include <map>

namespace FIFE {

	class Map;
	class MapLoader;
	/** User-Interface to load any kind of supported mapfile.
	 * 
	 * The most important function is probably \c loadFile() which
	 * iterates through all supported \em MapLoader(s) until one
	 * succeeds.
	 *
	 * A \em MapLoader needs to be registered with the factory; there
	 * is probably no reason to unregister a loader... ever.
	 * 
	 * \see MapLoader
	 * \see Map
	 * 
	 * This class is derived from a \em singleton, you can access
	 * the object by calling the \c instance() member function.
	 * 
	 * @see DynamicSingleton
	 */
	class MapFactory : public DynamicSingleton<MapFactory> {
		public:
			MapFactory();
			virtual ~MapFactory();

			/** Register a format-specific loader class with the factory.
			 *
			 * @param loader Pointer to a valid instance (\b no NULL pointers, please)
			 */
			void registerLoader(MapLoader* loader);
			
			void unregisterLoader(MapLoader* loader);

			/** Attempt to load a mapfile.
			 *
			 * Tries all registered loaders until success.
			 *
			 * @param file Path to file to load.
			 * @return Pointer load \em Map instance (on success) or NULL pointer on failure.
			 *
			 * \note Exceptions from the maploaders are caught and \b not propagated.
			 * \bug Throwing an exception when all loaders fail might be a good idea.
			 */
			Map* loadFile(const std::string& path);
			
			/** Removes all registered loaders.
			 *
			 * Probably never call this yourself; maybe the engine should on exit, but then... why?
			 */
			void cleanup();

		private:

			typedef std::map<std::string, MapLoader*> type_loaders;
			type_loaders m_loaders;

	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
