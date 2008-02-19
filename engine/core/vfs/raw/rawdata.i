/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

%module fife
%{
#include "vfs/raw/rawdata.h"
%}

%include "vfs/raw/rawdatasource.h"

namespace FIFE {
	class RawData {
		public:
			RawData(RawDataSource* datasource);
			virtual ~RawData();

			std::vector<uint8_t> getDataInBytes();
			std::vector<std::string> getDataInLines();

			unsigned int getDataLength() const;
			unsigned int getCurrentIndex() const;
			void setIndex(unsigned int index);
			void moveIndex(int offset);

			void readInto(uint8_t* buffer, size_t len);

			uint8_t read8();
			uint16_t read16Little();
			uint32_t read32Little();
			uint16_t read16Big();
			uint32_t read32Big();

			std::string readString(size_t len);
			bool getLine(std::string& buffer);
	};
}
