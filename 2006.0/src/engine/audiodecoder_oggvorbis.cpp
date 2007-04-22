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

#include "audiodecoder_oggvorbis.h"

namespace FIFE {

	static size_t read_ogg(void *ptr, size_t size, size_t nmemb, void *datasource) {
		RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
		size_t restlen = rdp->getDataLength()-rdp->getCurrentIndex();
		size_t len = (restlen<=size*nmemb)?restlen:size*nmemb;
		if (len) {
			rdp->readInto(reinterpret_cast<uint8_t *>(ptr), len);
		}
		return len;
	}

	static int seek_ogg(void *datasource, ogg_int64_t offset, int whence) {
		RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
		switch (whence) {
			case SEEK_SET:
				(*rdp).setIndex(offset);
				break;
			case SEEK_CUR:
				(*rdp).moveIndex(offset);
				break;
			case SEEK_END:
				std::cerr << "Seek_END:" << offset << std::endl;
				(*rdp).setIndex( (*rdp).getDataLength() -1 + offset);
				break;
			default:
				std::cerr << "Error in seek_ogg" << std::endl;
		}
		return -1;
	}
	
	static int close_ogg(void *datasource) { return 0;}
	
	static long tell_ogg(void *datasource) {
		RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
		return (*rdp).getCurrentIndex();
	}



	bool OggVorbisDecoder::isOggVorbisFile(RawDataPtr rdp) {
		ov_callbacks ocb = { read_ogg, seek_ogg, close_ogg, tell_ogg };
		OggVorbis_File ovf;
		if (0 == ov_test_callbacks(&rdp, &ovf, 0, 0, ocb)) {
			rdp->setIndex(0);
			ov_clear(&ovf);
			return true;
		}
		rdp->setIndex(0);
		return false;
	}

	OggVorbisDecoder::OggVorbisDecoder(RawDataPtr rdp) : AudioDecoder(), m_file(rdp), m_ovf(), m_data(0), m_datasize(0) {
		ov_callbacks ocb = { read_ogg, seek_ogg, close_ogg, tell_ogg };
		if (0>ov_open_callbacks(&m_file, &m_ovf, 0, 0, ocb)) {
			std::cerr << "Error opening OggVorbis file" << std::endl;
			return;
		}
		
		vorbis_info *vi = ov_info(&m_ovf, -1);
		if (!vi) {
			std::cerr << "Error fetching oggvorbis info" << std::endl;
			return;
		}
		m_isstereo = (vi->channels==2)?true:false;
		m_samplerate = vi->rate;
		
	}
	
	OggVorbisDecoder::~OggVorbisDecoder() {
		ov_clear(&m_ovf);
		delete [] m_data;
	}
	
	bool OggVorbisDecoder::decodeAll() {
		delete [] m_data;
		const uint32_t len =guessedLength();
		m_data = new int8_t[len];
		int stream=0, ret;
		m_datasize=0;
		do {
			ret = ov_read(&m_ovf, reinterpret_cast<char *>(&m_data[m_datasize]), len-m_datasize, 0, 2, 1, &stream);
			if (ret>0)
				m_datasize+=ret;
		} while (len-m_datasize>0 && ret>0);

		if (len-m_datasize==0 && ret!=0) {
			char buf;
			if (0<ov_read(&m_ovf, &buf, 1, 0, 2, 1, &stream))
				std::cerr << "Warn: We guessed the wrong length of the decoded OggVorbis stream. Please report to the FIFE team!" << std::endl;
			else
				ret = 0;
		}
	
		return (ret == 0)?true:false;
	}

	uint32_t OggVorbisDecoder::decodePart(uint32_t len) {
		delete [] m_data;
		m_data = new int8_t[len];
		int stream=0, ret;
		m_datasize=0;
		do {
			ret = ov_read(&m_ovf, reinterpret_cast<char *>(&m_data[m_datasize]), len-m_datasize, 0, 2, 1, &stream);
			if (ret>0)
				m_datasize+=ret;
		} while (len>m_datasize && ret>0);
		
		return m_datasize;
	}

	uint32_t OggVorbisDecoder::guessedLength() { return ov_pcm_total(&m_ovf, -1)*4; }
}
