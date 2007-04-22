/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef FIFEVFSHOSTSYSTEM_H
#define FIFEVFSHOSTSYSTEM_H

#include "vfssource.h"

namespace FIFE {

	/** The most basic VFSSource for the "normal" filesystem
	 *
	 */
	class VFSHostSystem : public VFSSource {
		public:
			VFSHostSystem();
			virtual ~VFSHostSystem();

			virtual bool fileExists(const std::string& name) const;
			virtual RawData* open(const std::string& file) const;

	};

}

#endif
