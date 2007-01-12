/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_MAP_LOADERS_XML_IMGCACHE_H
#define FIFE_MAP_LOADERS_XML_IMGCACHE_H

// Standard C++ library includes
#include <sys/types.h>
#include <map>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/shared_ptr.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "tinyxml/tinyxml.h"

namespace FIFE { namespace map { namespace loaders { namespace xml {

	/** Graphics-loader & simple ImageCache Interface.
	 *
	 * This was designed as a helper for the xml maploader; it contains a bunch
	 * of low-level access functions and can access the ImageCache.
	 *
	 * \see ImageCache
	 *
	 * \todo Access to FRM format
	 *
	 * \note This class uses the VFS for IO.
	 * @bug Style: A number of functions return false values on success.
	 * @bug The ID to ImageID mapping seems to be functionality, that should belond in some
	 * kind of data repository and not in a helper class. The same goes for the loading
	 * of tilesets.
	 */
	class Imgcache {
		public:
			typedef std::map<int, size_t> type_imageIndexMap;
			typedef boost::shared_ptr<type_imageIndexMap> type_imageIndexMapPtr;

		private:
			/// The possibly shared global index to image id map.
			type_imageIndexMapPtr m_imageIndexMap;
			/// True, if sharing a global id map
			bool m_isSharing;

			SDL_Surface* getSubImage(SDL_Surface* src, uint8_t w, uint8_t h,
			                         uint8_t xStep, uint8_t yStep);
			SDL_RWops* createRWops(const char*);

		public:
			/** Base constructor - Independent index-map.
			 *
			 * This is probably what you want.
			 */
			Imgcache();
			
			/** Advanced usage constructor - shared index-map
			 *
			 * @param Pointer to valid index map
			 *
			 * \note This functionality is not tested so far. 
			 */
			Imgcache(type_imageIndexMapPtr thisIndex);
			
			/** Access to the (internal) index-map.
			 *
			 * For advanced constructor usage.
			 *
			 * @return Pointer to the index-map of this instance.
			 */
			type_imageIndexMapPtr getIndices();

			/** Parse a <tileset> xml-subtree and registers the graphics.
			 *
			 * @param Pointer into a tinyxml tree.
			 * @return Zero on success, true value on error
			 *
			 * \todo Exceptions?
			 * 
			 * @see getImageForId
			 *
			 * See also: map.dtd - XML map definition (includes tileset for now)
			 */
			int loadTileset(TiXmlElement* element);

			int loadAnimation(TiXmlElement* element);
			
			/** Create an Image object which can later be accessed by id.
			 *
			 * @param filename VFS path to a SDL_Image-supported format.
			 * @param id Index for this image
			 * @return Zero on success, true value on error.
			 *
			 * \note The Image is created in a way that the ImageCache will never
			 * automatically remove it.
			 *
			 * \see ImageCache
			 *
			 * \bug Check if requested index is already in use.
			 * I think this was once here already }:o (Fixed)
			 *
			 */
			int loadImageAs(const char* filename, int id);
			
			/** For direct access to the pixels.
			 *
			 * @param filename VFS path to a SDL_Image-supported format
			 * @return Pointer to SDL_Surface or NULL on failure.
			 *
			 * \note This does not hit the cache, you have to delete it yourself!
			 */
			SDL_Surface* loadImageAsSurface(const char* filename);
			
			/** Re-index one integer to another.
			 *
			 * Holy batman, you found the most important functions of them all.
			 *
			 * @param Index into the index map of this instance (you define 
			 * these indices in the xml or define them with \c loadImageAs)
			 * @return Index into the ImageCache (for rendering) or UNDEFINED_ID
			 * for referencing a dummy image, if the index is wrong.
			 */
			size_t getImageForId(int);
			
			/** Clears the indices.
			 *
			 * \note This does \b not clear the actual data in the ImageCache!
			 */
			void clear();
	};
} } } }
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
