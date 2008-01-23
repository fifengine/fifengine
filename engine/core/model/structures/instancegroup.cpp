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

// Standard C++ library includes

// 3rd party library includes
//#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
/*#include "util/purge.h"

#include "layer.h"
#include "elevation.h"
#include "instancetree.h"*/
#include "instancegroup.h"
#include "instance.h"

namespace FIFE {
	InstanceGroup::InstanceGroup( const std::string &identifier, DoublePoint3D &offset, InstanceGroup *parent ) :
		AttributedClass( identifier ),
		m_offset( offset )
	{
		if( parent ) {
			parent->addGroup( this );
		}
	}

	InstanceGroup::~InstanceGroup() {
		m_instances.clear();
		m_childgroups.clear();
	}

	void InstanceGroup::addInstance( Instance *instance ) {
		if( isMember( instance )) {
			throw NameClash( "Duplicate instance registered." );
		}

		m_instances.push_back( instance );
	}

	void InstanceGroup::removeInstance( Instance *instance ) {
		InstanceList::iterator iter = std::find( m_instances.begin(), m_instances.end(), instance );
		if( iter != m_instances.end()) {
			m_instances.erase( iter );
		}
	}

	bool InstanceGroup::isMember( Instance *instance ) const {
		InstanceList::const_iterator iter = std::find( m_instances.begin(), m_instances.end(), instance );
		return iter != m_instances.end();
	}

	void InstanceGroup::setOffset( DoublePoint3D &offset ) {
		m_offset = offset;
	}


	InstanceGroupManager::~InstanceGroupManager() {
		clear();
	}

	InstanceGroup *InstanceGroupManager::addGroup( InstanceGroup *group ) {
		if( isChild( group )) {
			throw NameClash( "Duplicate child group registered." );
		}

		m_childgroups.push_back( group );
		return group;
	}

	InstanceGroup *InstanceGroupManager::createGroup( InstanceGroup &group ) {
		return new InstanceGroup( group );
	}

	void InstanceGroupManager::removeGroup( InstanceGroup *group ) {
		InstanceGroupList::iterator iter = std::find( m_childgroups.begin(), m_childgroups.end(), group );
		if( iter != m_childgroups.end()) {
			m_childgroups.erase( iter );
		}
	}

	void InstanceGroupManager::clear() {
		InstanceGroupList::iterator iter = m_childgroups.begin();
		for(; iter != m_childgroups.end(); iter++ ) {
			delete *iter;
		}
	}

	bool InstanceGroupManager::isChild( InstanceGroup *group ) const {
		InstanceGroupList::const_iterator iter = std::find( m_childgroups.begin(), m_childgroups.end(), group );
		return iter != m_childgroups.end();
	}

	InstanceGroup *InstanceGroupManager::getGroup( const std::string &id, bool recursive ) const {
		InstanceGroupList::const_iterator iter = m_childgroups.begin();
		InstanceGroup *group;

		for(; iter != m_childgroups.end(); iter++ ) {
			if( (*iter)->Id() == id ) {
				return *iter;
			}

			if( recursive ) {
				group = (*iter)->getGroup( id, true );
				if( group ) {
					return group;
				}
			}
		}

		return NULL;
	}
} // FIFE
