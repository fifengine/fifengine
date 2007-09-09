/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef FIFEMFFALLOUTPROVIDERDAT1_H
#define FIFEMFFALLOUTPROVIDERDAT1_H

#include "vfssourceprovider.h"

namespace FIFE {

	class MFFalloutProviderDAT1 : public VFSSourceProvider {
		public:
			MFFalloutProviderDAT1();
			virtual ~MFFalloutProviderDAT1();

			virtual bool isReadable(const std::string& file) const;
			virtual VFSSource* createSource(const std::string& file) const;
	};

}

#endif
