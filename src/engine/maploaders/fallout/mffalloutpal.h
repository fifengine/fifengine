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
#ifndef MFFALLOUTPAL_H
#define MFFALLOUTPAL_H

#include <string>
#include <stdint.h>

namespace FIFE {

	class MFFalloutPAL {
		public:
			MFFalloutPAL(const std::string& file);
			~MFFalloutPAL();

			uint8_t getRed(uint8_t index) const;
			uint8_t getGreen(uint8_t index) const;
			uint8_t getBlue(uint8_t index) const;

			uint8_t get(size_t index) const;
			uint32_t operator[](uint8_t) const;
			uint32_t getRGB(uint8_t index) const;

		private:
			uint8_t* m_palette;

			MFFalloutPAL(const MFFalloutPAL&);
			MFFalloutPAL& operator=(const MFFalloutPAL&);
	};

}//FIFE

#endif
