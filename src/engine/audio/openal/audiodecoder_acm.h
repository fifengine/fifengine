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
#ifndef AUDIO_ACM_H_
#define AUDIO_ACM_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "rawdata.h"
#include "audiodecoder.h"

namespace ACMDecoder {
	class CACMUnpacker;
}

namespace FIFE {
	
	typedef ::boost::shared_ptr<RawData> RawDataPtr;
	
	/** The decoder class for the Fallout own audio format ACM.
	 * 
	 * This is just a wrapper class for the actual decoding code in 
	 * ACMDecoder::CACMUnpacker (in audiodecoder_acm.cpp). .
	 */
	class ACMDecoder : public AudioDecoder {
		public:
			ACMDecoder(RawDataPtr, const std::string &name="");
			virtual ~ACMDecoder();

			virtual bool decodeAll();
			virtual uint32_t decodePart(uint32_t);
			virtual void *getBuffer() { return m_data; }
			virtual uint32_t getBufferSize() { return m_datasize; }
			
			static bool isACMFile(RawDataPtr);
		protected:
			virtual uint32_t guessedLength();
		private:
			static bool decideChannels(const std::string &);
		
			::ACMDecoder::CACMUnpacker *m_acm;
			uint8_t *m_data;
			uint32_t m_datasize, m_gsize;	
	};
}

#endif
