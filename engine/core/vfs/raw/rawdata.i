/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

%module fife
%{
#include "vfs/raw/rawdata.h"
%}

%include "vfs/raw/rawdatasource.h"

namespace FIFE {
	%apply std::string &OUTPUT { std::string& outbuffer };
	%apply uint8_t *OUTPUT { uint8_t* buffer };
	
	class RawData {
		public:
			RawData(RawDataSource* datasource);
			virtual ~RawData();

			std::vector<uint8_t> getDataInBytes();
			std::vector<std::string> getDataInLines();

			uint32_t getDataLength() const;
			uint32_t getCurrentIndex() const;
			void setIndex(uint32_t index);
			void moveIndex(int32_t offset);

			void readInto(uint8_t* buffer, size_t len);
			uint8_t read8();
			uint16_t read16Little();
			uint32_t read32Little();
			uint16_t read16Big();
			uint32_t read32Big();

			void read(std::string& outbuffer, int32_t size=-1);
			std::string readString(size_t len);
			bool getLine(std::string& buffer);
	};
	%clear std::string& outbuffer;
}
