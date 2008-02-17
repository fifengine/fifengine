/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
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

#ifndef FIFE_TIME_PROVIDER_H
#define FIFE_TIME_PROVIDER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"

namespace FIFE {

	/** Timeprovider is an utility providing time management functionality
	 * You can have hierarchy of time providers, whose multiplier can be changed independently.
	 * Changing providers upper in the hierarchy, changes also all subproviders with the same ratio
	 */
	class TimeProvider {
	public:
		/** Constructor
		 * In case there there is no provider master, you can use NULL
		 */
		TimeProvider(TimeProvider* master);
		~TimeProvider();
		void setMultiplier(float multiplier);
		float getMultiplier() const;
		float getTotalMultiplier() const;
		unsigned int getGameTicks() const;
		
	private:
		TimeProvider* m_master;
		float m_multiplier;
	};
	
	unsigned int scaleTime(float multiplier, unsigned int ticks);

} //FIFE
#endif

