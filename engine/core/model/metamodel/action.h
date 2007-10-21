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
#include "util/angles.h"

#include "abstractvisual.h"

namespace FIFE {

	class Action : public AttributedClass {
	public:
		/** Constructor
		 * Actions are created by calling addAction from object, thus
		 * this method should really be called only by object or test code
		 */
		Action(const std::string& identifier);

		/** Destructor
		 */
		virtual ~Action();

		/** Sets the duration for this action
		 */
		void setDuration(unsigned int duration) { m_duration = duration; }

		/** Gets the duration of this action
		 */
		unsigned int getDuration() { return m_duration; }

		/** Sets visualization to be used. Transfers ownership.
		 */
		void setVisual(AbstractVisual* visual) { m_visual = visual; }
		
		/** Gets used visualization
		 */
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }


	private:
		// duration of the action
		unsigned int m_duration;
		// visualization for action
		AbstractVisual* m_visual;
	};

}

#endif
