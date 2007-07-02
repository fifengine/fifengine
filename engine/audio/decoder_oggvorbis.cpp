/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log.h"

#include "decoder_oggvorbis.h"

namespace FIFE { namespace audio {

	namespace OGG_detail {
		static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource) {
			RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
			size_t restlen = rdp->getDataLength()-rdp->getCurrentIndex();
			size_t len = (restlen<=size*nmemb)?restlen:size*nmemb;
			if (len) {
				rdp->readInto(reinterpret_cast<uint8_t *>(ptr), len);
			}
			return len;
		}

		static int seek(void *datasource, ogg_int64_t offset, int whence) {
			RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
			switch (whence) {
				case SEEK_SET:
					(*rdp).setIndex(offset);
					break;
				case SEEK_CUR:
					(*rdp).moveIndex(offset);
					break;
				case SEEK_END:
					Debug("decoder_ogg")
						<< "Seek_END: " << offset;
					(*rdp).setIndex( (*rdp).getDataLength() -1 + offset);
					break;
				default:
					Debug("decoder_ogg") // or Warn?
						<< "Error in seek_ogg";
			}
			return -1;
		}
		
		static int close(void *datasource) { return 0; }
		
		static long tell(void *datasource) {
			RawDataPtr rdp = *(reinterpret_cast<RawDataPtr *>(datasource));
			return (*rdp).getCurrentIndex();
		}
	}

	bool OggVorbisDecoder::isOggVorbisFile(RawDataPtr rdp) {
		ov_callbacks ocb = { 
			OGG_detail::read, OGG_detail::seek, 
			OGG_detail::close, OGG_detail::tell
		};
		OggVorbis_File ovf;
		if (0 == ov_test_callbacks(&rdp, &ovf, 0, 0, ocb)) {
			rdp->setIndex(0);
			ov_clear(&ovf);
			return true;
		}
		rdp->setIndex(0);
		return false;
	}

	OggVorbisDecoder::OggVorbisDecoder(RawDataPtr rdp) : 
		Decoder(), m_file(rdp), m_ovf(), m_data(0), m_datasize(0) {
		// FIXME: throw exceptions?
		ov_callbacks ocb = {
			OGG_detail::read, OGG_detail::seek, OGG_detail::close, OGG_detail::tell
		};
		if (0 > ov_open_callbacks(&m_file, &m_ovf, 0, 0, ocb)) {
			Warn("decoder_ogg")
				<< "Error opening OggVorbis file";
			return;
		}
		
		vorbis_info *vi = ov_info(&m_ovf, -1);
		if (!vi) {
			Warn("decoder_ogg")
				<< "Error fetching oggvorbis info";
			return;
		}
		
		m_isstereo = vi->channels == 2;
		m_samplerate = vi->rate;
	}
	
	bool OggVorbisDecoder::decodeHelper(uint32_t len, int& stream, int& ret) {
		delete[] m_data;
		m_data = new int8_t[len];
		m_datasize = 0;
		do {
			ret = ov_read(&m_ovf, reinterpret_cast<char *>(&m_data[m_datasize]), 
			              len-m_datasize, 0, 2, 1, &stream);
			if (ret > 0) {
				m_datasize += ret;
			}
		} while (len-m_datasize > 0 && ret > 0);
		return (len-m_datasize == 0 && ret != 0);
	}
	
	bool OggVorbisDecoder::decodeAll() {
		int stream = 0, ret;

		if (decodeHelper(guessedLength(), stream, ret)) {
			// If the loop ended because with a nonzero ret
			char buf;
			if (0 < ov_read(&m_ovf, &buf, 1, 0, 2, 1, &stream)) {
				Warn("decoder_ogg")
					<< "Warn: We guessed the wrong length of the decoded"
					<< "OggVorbis stream. Please report to the FIFE team!";
			} else {
				ret = 0;
			}
		}

		return ret == 0;
	}

	uint32_t OggVorbisDecoder::decodePart(uint32_t len) {
		int stream = 0, ret;
		
		decodeHelper(len, stream, ret); // ignore return value
		
		return m_datasize;
	}

	uint32_t OggVorbisDecoder::guessedLength() { 
		return ov_pcm_total(&m_ovf, -1) * 4;
	}
} } // FIFE::audio
