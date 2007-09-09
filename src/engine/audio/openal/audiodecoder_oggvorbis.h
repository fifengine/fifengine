/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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
#ifndef OGGVORBISDECODER_H_
#define OGGVORBISDECODER_H_

#include <boost/shared_ptr.hpp>
#include <vorbisfile.h>

#include "audiodecoder.h"
#include "rawdata.h"

namespace FIFE {
	
	typedef ::boost::shared_ptr<RawData> RawDataPtr;
	
	/** The decoder class for OggVorbis files
	 */
	class OggVorbisDecoder : public AudioDecoder {
		public:
			OggVorbisDecoder(RawDataPtr rdp);
			virtual ~OggVorbisDecoder();
			
			
			virtual bool decodeAll();
			virtual uint32_t decodePart(uint32_t);
			virtual void *getBuffer() { return m_data; }
			virtual uint32_t getBufferSize() { return m_datasize; }
			
			/** This function checkes if rdp points to an OggVorbis file.
			 * 
			 * @param rdp A RawDataPtr that is to be checked.
			 * @return Returns true if rdp points to an OggVorbis file, false if not.
			 */
			static bool isOggVorbisFile(RawDataPtr rdp);
			
		protected:
			virtual uint32_t guessedLength();
			
		private:
			RawDataPtr m_file;
			OggVorbis_File m_ovf;
			int8_t *m_data;
			uint32_t m_datasize;
	};

}

#endif
