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

#ifndef FIFE_MODEL_METAMODEL_ACTION_H
#define FIFE_MODEL_METAMODEL_ACTION_H

// Standard C++ library includes
#include <string>
#include <vector>
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"

namespace FIFE { namespace model {

	class Action : public AttributedClass {
		public:
			/** Constructor
			 * Actions are created by calling addAction from object, thus
			 * this method should really be called only by object or test code
			 */
			Action();

			/** Destructor
			 */
			virtual ~Action();

			/** Gets index to animation closest to given angle
			 * @return animation index, -1 if no animations available
			 */
			int getAnimationIndexByAngle(unsigned int angle);

			/** Adds new animation with given angle (degrees)
			 */
			void addAnimation(unsigned int angle, int animation_index);

		private:
			typedef std::map<unsigned int, int> type_animmap;
			// animations associated with this action (handles to pool)
			//   mapping = direction -> animation
			type_animmap m_animations;
	};

}}

#endif
