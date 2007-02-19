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
#include <cmath>

// 3rd party library includes
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "timemanager.h"

#include "camera.h"
#include "control.h"
#include "view.h"
#include "elevation.h"
#include "grid.h"
#include "geometry.h"

namespace FIFE { namespace map {

	Camera::Camera(Control* map_control) {
		m_control = map_control;
		m_control->addCamera(this);

		m_timer.setInterval(50);
		m_timer.setCallback( boost::bind( &Camera::update, this ) );

		reset();
	}

	Camera::~Camera() {
		if( m_control )
			m_control->removeCamera(this);
	}

	void Camera::update() {
		Point mappos(m_view->getXPos(), m_view->getYPos());
		Point dist = m_next_position - mappos;
		
		uint32_t length = uint32_t(sqrt(static_cast<float>(dist.x * dist.x + 
		                                                   dist.y * dist.y)));
		if (length < 50) {
			m_timer.stop();
			return;
		}
		
		Debug("map_camera") 
			<< "Zooming from " << mappos
			<< " to " << m_next_position
			<< " by " << (dist*50) / length;
		
		mappos += (dist*50) / length;

		m_view->setXPos(mappos.x);
		m_view->setYPos(mappos.y);
	}

	void Camera::zoomTo(const Point& position) {
		if (!m_grid) {
			return;
		}
		// center on screen
		Point offset = Point(m_view->m_rect.w, m_view->m_rect.h) / 2;
		m_next_position = m_grid->getGeometry()->toScreen(position) - offset;
		m_timer.start();

		Debug("map_camera") 
			<< "Setting Next position:" << m_next_position;
	}

	void Camera::reset() {
		assert( m_control );
		m_timer.stop();
		m_view = m_control->getView();
		if (!m_view->getCurrentElevation()) {
			m_grid = 0;
		} else {
			m_grid = m_view->getCurrentElevation()->getReferenceGrid();
		}
	}

	void Camera::controlDeleted() {
		assert( m_control );
		m_timer.stop();
		m_control = 0;
	}
} } // FIFE::map
