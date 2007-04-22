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

/*
	This represents an Object on the map. To get an idea what is an object
	in fife, toggle it's drawing by pressing 'o'. Currently only it's
	visual representation is attached to it. Later other stuff, like
	scripts, it's in game value, etc. will follow.
		-phoku
*/
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <SDL_video.h>

#include "video/rect.h"

namespace FIFE {

	class RenderAble;
	/** A generic game-object.
	 *
	 * At this time this class stores a position (on the fallout object hex-grid)
	 * and has functionality to draw some kind of graphic (on the map).
	 *
	 * \see RenderAble
	 *
	 * An object is contained inside a \c MapElevation; \see MapElevation
	 * 
	 * \note I still think it might be a good idea to write derived classes for certain
	 * item types (critter, weapon, ... in fallout-terms); though only splitting in the
	 * scriptengine might be possible, too.
	 *
	 * \bug Objects can currently not be created in-game, only from (within) the respective
	 * maploaders; this is a maploader-design bug, but I thought I mention it here, too.
	 * That statement is somewhat wrong. Create an object by calling new Object(name,pos).
	 * It's a lack of features that we currently can't load objects from the fallout data. There need
	 * to be some kind of data repository. Won't fix till 2006.3.
	 *
	 * \bug An Object can also between two positions on a map (traveling from a to b); to
	 * represent movement (graphically) I suggest a second position (m_next_position) and
	 * a float value (m_move_progess) between 0.0 - 1.0 and linear interpolate between the
	 * renderpositions. m_next_position should never be more then 1 hex away from m_position!
	 *
	 * \bug This class does not carry all the information (=variables) that (for example) Fallout
	 * defines for an object; and it shouldn't (because those are game-specific). But:
	 * I *need* access to some of the missing values in-game. So either derived classes
	 * for different loaders (messes with my different classes for different types idea - above)
	 * or some member-pointer to another structure (maybe into the maploader; that data still exists).
	 *
	 * Please kind in mind that multiple inheritance is difficult with the current Lua-approach.
	 */
	class Object {
		public:

			/** Magic value for (newly created) objects that are not yet positioned.
			 */
			// changed from ~0 to -1; I think this is easier to read
			static const int POS_NOT_ON_MAP = -1;

			/** Construct an Object.
			 * 
			 * @param name Type/Class-name; not a character-name
			 * @param position Initial position on map; defaults to POS_NOT_ON_MAP.
			 * 
			 * \note The \c Object isn't finished; at least you need to call 
			 * \c setVisual() on it.
			 */
			Object(const std::string& name, int position = POS_NOT_ON_MAP);
			
			virtual ~Object();

			const std::string& getName() const;

			int getPosition() const;

			void setPosition(int pos);

			void addToInventory(Object* obj);

			/** Mutator for the visual-representation of this object.
			 *
			 * @param idx Index into the \c ImageCache.
			 *
			 * \see getVisual function for more information.
			 */
			void setVisual(size_t);

			/** Accessor to the visual-representation of this object.
			 *
			 * More precisely an index into the \c ImageCache; this can
			 * be any type implementing \c RenderAble.
			 *
			 * \return idx Index for ImageCache::getImage().
			 * 
			 * \see ImageCache
			 * \see RenderAble
			 * \see Image
			 * \see Animation
			 */
			size_t getVisual();
			
			/** Used for rendering, range [0,255] where 255 means fully opaque and 0 fully transparent, def 255
			 */
			unsigned char m_opacity;

		private:
			std::string m_name;
			int m_position;
			size_t m_visual;

			Rect m_renderpos;

			typedef std::vector<Object*> type_inventory;
			type_inventory m_inventory;

			void init();
			void cleanup();

			Object(const Object&);
			Object& operator=(const Object&);
	};

}//FIFE

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
