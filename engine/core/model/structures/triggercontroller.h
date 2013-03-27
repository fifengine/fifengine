/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_TRIGGER_CONTROLLER_H
#define FIFE_TRIGGER_CONTROLLER_H

// Standard C++ library includes
#include <vector>
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	// FORWARD REFERENCES
	class Trigger;
	class Map;
	class Layer;

	/** This class serves as a central place to manage triggers for a Map.
	 *
	 *  You should never instantiate this class directly as Map does it
	 *  when you create a new map.
	 */
	class TriggerController : public FifeClass {
	public:

	// LIFECYCLE

		/** Constructor.
		 *
		 * @param map A pointer to the map that the controller will interact with.
		 */
		TriggerController(Map* map);

		/** Destructor.
		 */
		virtual ~TriggerController();

	// OPERATORS

	// OPERATIONS

		/** Creates and adds a single cell trigger to the map on the specified layer.
		 *
		 * @param triggerName The name of the trigger.  This must be unique per map.
		 * @param layer A pointer to the layer in which to add the Trigger to
		 * @param pt1 The ModelCoordinate where the Trigger should be added.
		 */
		Trigger* addCellTriggerToLayer(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt1);

	// ACCESS

	// INQUIRY

	protected:
	private:
		/** Checks to see if the trigger name already exists.
		 *
		 * @param name The name of the trigger to check
		 *
		 * @return true if the name already exists, false otherwise.
		 */
		bool exists(const std::string& name);

		//! Pointer to the map this controller is associated with.
		Map* m_map;

		typedef std::map< std::string, Trigger* > TriggerNameMap;
		typedef std::map< std::string, Trigger*  >::iterator TriggerNameMapIterator;
		typedef std::map< std::string, Trigger*  >::const_iterator TriggerNameMapConstIterator;
		typedef std::pair< std::string, Trigger*  > TriggerNameMapPair;

		//! Trigger name map
		TriggerNameMap m_triggerNameMap;
	};

	// INLINE METHODS
	//

	// EXTERNAL REFERENCES
	//

} //FIFE

#endif
