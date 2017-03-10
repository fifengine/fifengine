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
#include "util/math/angles.h"
#include "util/base/fifeclass.h"

#include "ivisual.h"

namespace FIFE {

	class Action : public FifeClass {
	public:
		/** Constructor
		 * Actions are created by calling addAction from object, thus
		 * this method should really be called only by object or test code
		 */
		Action(const std::string& identifier);

		/** Destructor
		 */
		virtual ~Action();

		/** Get the identifier for this action.
		 */
		const std::string& getId() { return m_id; }

		/** Sets the duration for this action
		 */
		void setDuration(uint32_t duration) { m_duration = duration; }

		/** Gets the duration of this action
		 */
		uint32_t getDuration() { return m_duration; }

		/** Sets visualization to be used. Transfers ownership.
		 */
		void adoptVisual(IVisual* visual) { m_visual = visual; }

		/** Gets used visualization
		 */
		template<typename T> T* getVisual() const { return reinterpret_cast<T*>(m_visual); }


	private:
		std::string m_id;

		// duration of the action
		uint32_t m_duration;
		// visualization for action
		IVisual* m_visual;
	};

}

#endif
