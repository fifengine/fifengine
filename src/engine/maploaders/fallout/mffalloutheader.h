#ifndef IANMFFALLOUTHEADER_H
#define IANMFFALLOUTHEADER_H

#include <string>
#include <stdint.h>
#include "vfs.h"

namespace FIFE {

	class RawData;
	class MFFalloutHeader {
		public:
			MFFalloutHeader(RawDataPtr data);
			~MFFalloutHeader();

			void loadHeader(RawDataPtr data);

			const std::string& getMapName() const;
			uint32_t getVersion() const;
			uint32_t getElevations() const;		// Anchorite: For proper map loading
			uint32_t getNumLocalVars() const;
			uint32_t getNumGlobalVars() const;
			bool hasElevation(unsigned int num) const;

			unsigned int getNumElevations() const;

		private:
			std::string m_mapname;

			uint32_t m_version;
			int32_t m_defaultPlayerPos;
			int32_t m_defaultElevation;
			int32_t m_defaultOrientation;
			uint32_t m_num_localvars;
			uint32_t m_scriptid;
			uint32_t m_elevations;
			uint32_t m_num_globalvars;
			uint32_t m_mapID;
			uint32_t m_timestamp;

			void checkSanity();
	};

}//FIFE

#endif
