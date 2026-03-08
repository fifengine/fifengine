// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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