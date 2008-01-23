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

#ifndef FIFE_INSTANCEGROUP_H
#define FIFE_INSTANCEGROUP_H

// Standard C++ library includes
#include <algorithm>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
/*#include "model/metamodel/modelcoords.h"
#include "util/attributedclass.h"
#include "model/metamodel/object.h"*/

#include "instance.h"

namespace FIFE {

	/*class Elevation;
	class Selection;
	class CellGrid;
	class Object;
	class InstanceTree;*/
  class Instance;
  class InstanceGroup;

  typedef std::list<InstanceGroup*> InstanceGroupList;
  typedef std::list<Instance*> InstanceList;

  /** Instance groups are managed by this class.
   */
  class InstanceGroupManager {
    public:
      /** Constructor
       */
      InstanceGroupManager() {};

      /** Destructor
       */
      ~InstanceGroupManager();

      /** Adds a group as child. Do NOT delete a group from outside since InstanceGroupManager handles
       * this!
       *
       * @param group Pointer to child group
       */
      InstanceGroup *addGroup( InstanceGroup *group );

      /** Creates a new group -- unless added to the manager, you have to take of deleting it yourself!
       *
       * This is mostly used from Python.
       * @param tmpl Instance group to be created
       */
      InstanceGroup *createGroup( InstanceGroup &group );

      /** Removes a child group.
       * @param group Pointer to group to be removed
       */
      void removeGroup( InstanceGroup *group );

      /** Removes all groups.
       */
      void clear();

      /** Gets child group by Id.
       * @param id Group Id of group to fetch
       * @param recursive Search recursive in childs
       * @return Pointer to child group, NULL if not a child of this group
       */
      InstanceGroup *getGroup( const std::string &id, bool recursive = false ) const;

      /** Checks if a given group is a child group.
       * @param group Pointer to group to be checked.
       * @return true when group is a child
       */
      bool isChild( InstanceGroup *group ) const;

      /** Gets child groups.
       * @return Reference to child group list
       */
      InstanceGroupList &getGroups() { return m_childgroups; }

    protected:
      InstanceGroupList m_childgroups;
  };

	/** An instance group can contain any amount of instances and child groups.
	 */
	class InstanceGroup : public InstanceGroupManager, public AttributedClass {
		public:
			/** Constructor
       * @param identifier Identifier
       * @param offset Position offset for the group
       * @param parent Add the group to a parent group
       */
      InstanceGroup( const std::string &identifier, DoublePoint3D &offset, InstanceGroup *parent = NULL );

			/** Destructor
			 */
      ~InstanceGroup();

      /** Adds an instance to the group.
       * @param instance Pointer to instance to be added
       */
      void addInstance( Instance *instance );

      /** Removes an instance from the group.
       * @param instance Pointer to instance to be removed
       */
      void removeInstance( Instance *instance );

      /** Gets reference to instance list.
       */
      InstanceList &getMembers() { return m_instances; }

      /** Checks if an instance belongs to this group.
       * @param instance Pointer to instance to be checked
       * @return true if instance is member of group
       */
      bool isMember( Instance *instance ) const;

      /** Sets the offset for the group.
       * @param offset DoublePoint3D containing new offset
       */
      void setOffset( DoublePoint3D &offset );

      /** Gets current offset.
       * @return DoublePoint3D with offset
       */
      const DoublePoint3D getOffset() const { return m_offset; }

		protected:
      DoublePoint3D     m_offset;
      InstanceList      m_instances;
	};

} // FIFE

#endif
