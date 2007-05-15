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

#ifndef FIFE_MAP_LOADERS_UTIL_COMPLEXANIMATION_PROVIDER_H
#define FIFE_MAP_LOADERS_UTIL_COMPLEXANIMATION_PROVIDER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderable_provider.h"

class TiXmlElement;

namespace FIFE { 
	class ComplexAnimation;
	class PartialAction;

namespace video { namespace loaders {
	typedef boost::shared_ptr<PartialAction> PartialActionPtr;

	/** Complex Animation Provider 
	 *  This reads the action->animation mappings from a xml description
	 *  file that was set as location.
	 *
	 *  @see ComplexAnimation
	 */
	class ComplexAnimationProvider : public RenderableProvider {
		public:
			/** Constructor
			 * @param location The XML Filename Location to read from
			 */
			ComplexAnimationProvider(const RenderableLocation& location) : 
				RenderableProvider(location) {}

			/** Create a Complex Animation
			 */
			virtual RenderAble* createRenderable();

		private:
			/** Read the action elements
			 *  Reads the action elements and builds
			 *  the complex animation by registering
			 *  the animations.
			 *  Calls readPartialActions() in turn
			 */
			void readActions(TiXmlElement* actionElement);

			/** Read the partial-action elements
			 *  Reads the partial-action elements
			 *  of an action and registers them within
			 *  the complex animation.
			 *  It also registers them in the temporary
			 *  m_partialActions list for the second
			 *  parsing pass
			 */
			void readPartialActions(uint8_t action, uint8_t direction,
				TiXmlElement* actionElement);

			/** Build the continuations from the next-partial-action elements
			 *  Reads the next-partial-action elements
			 *  and thus builds the connections between the partial
			 *  actions ... this only loops through the actions
			 *  and partial-actions ... the next-partial-actions
			 *  is then actually read by readNextPartialActionsElement
			 */
			void readNextPartialActions(TiXmlElement* actionElement);

			void readNextPartialActionsElement(PartialActionPtr& partialAction,
				TiXmlElement* nextPartialActionElement);

			PartialActionPtr findPartialAction(uint8_t action, 
				uint8_t direction, uint8_t number);

			ComplexAnimation* m_complexAnimation;
			std::vector<PartialActionPtr>    m_partialActions;
	};
} } }
#endif
