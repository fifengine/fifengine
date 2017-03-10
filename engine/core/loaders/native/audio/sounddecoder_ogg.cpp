/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "sounddecoder_ogg.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_AUDIO);

	/* OggVorbis Callback functions
	 */
	namespace OGG_cb {
		static size_t read(void *ptr, size_t size, size_t nmemb, void *datasource) {
			RawData* rdp = reinterpret_cast<RawData*>(datasource);
			size_t restlen = rdp->getDataLength()-rdp->getCurrentIndex();
			size_t len = (restlen<=size*nmemb)?restlen:size*nmemb;
			if (len) {
				rdp->readInto(reinterpret_cast<uint8_t *>(ptr), len);
			}
			return len;
		}

		static int32_t seek(void *datasource, ogg_int64_t offset, int32_t whence) {
			RawData* rdp = reinterpret_cast<RawData*>(datasource);
			switch (whence) {
				case SEEK_SET:
					(*rdp).setIndex(static_cast<uint32_t>(offset));
					return 0;
				case SEEK_CUR:
					(*rdp).moveIndex(static_cast<uint32_t>(offset));
					return 0;
				case SEEK_END:
					(*rdp).setIndex( (*rdp).getDataLength() -1 + static_cast<uint32_t>(offset));
					return 0;
			}
			return -1;
		}

		static int32_t close(void *datasource) { return 0; }

		static long tell(void *datasource) {
			RawData* rdp = reinterpret_cast<RawData*>(datasource);
			return (*rdp).getCurrentIndex();
		}
	}

	SoundDecoderOgg::SoundDecoderOgg(RawData* rdp) : m_file(rdp) {

		ov_callbacks ocb = {
			OGG_cb::read, OGG_cb::seek, OGG_cb::close, OGG_cb::tell
		};

		if (0 > ov_open_callbacks(m_file.get(), &m_ovf, 0, 0, ocb)) {
			throw InvalidFormat("Error opening OggVorbis file");
		}


		vorbis_info *vi = ov_info(&m_ovf, -1);
		if (!vi) {
			throw InvalidFormat("Error fetching OggVorbis info");
		}

		if (!ov_seekable(&m_ovf)) {
			throw InvalidFormat("OggVorbis file has to be seekable");
		}

		m_isstereo = vi->channels == 2;
		m_samplerate = vi->rate;
		m_is8bit = false;
		m_declength = (m_isstereo ? 2 : 1) * 2 * ov_pcm_total(&m_ovf, -1);
		m_datasize = 0;
		m_data = NULL;
	}

	bool SoundDecoderOgg::decode(uint64_t length) {
		int32_t stream = 0;
		int32_t ret = 0;

		// release buffer and allocate new memory
		releaseBuffer();
		m_data = new char[length];

		// decode the stream
		m_datasize = 0;
		do {
			ret = ov_read(&m_ovf, m_data + m_datasize,
			              length-m_datasize, 0, 2, 1, &stream);
			if (ret > 0) {
				m_datasize += ret;
			}

		} while (length-m_datasize > 0 && ret > 0);

		return m_datasize == 0;
	}

	bool SoundDecoderOgg::setCursor(uint64_t pos) {

		if (ov_pcm_seek(&m_ovf, pos / ((m_isstereo ? 2 : 1) * 2)) == 0) {
			return true;
		}
		return false;
	}
}
