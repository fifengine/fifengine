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
#include <cstdio>

// 3rd party library includes
#include <vorbisfile.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/log.h"

#include "config.h"
#include "decoder.h"
#include "audiomanager.h"

/* Changelog:
 * -Try to use Log more often
 * 
 * ToDo:
 * -Remove the exception from the constructor. IIRC stro doesn't like them
 */

namespace FIFE { namespace audio {

	typedef ::boost::shared_ptr<Decoder> type_decptr;

	void Manager::setAmbientSound(const std::string &name) {
		if (m_sound_disabled) {
			return;
		}

		// Turn the background music off
		if (name == "") {
			delete m_bgsound;
			m_bgsound = 0;
			return;
		}
		
		if (!m_bgsound) {
			m_bgsound = new Source();
			m_bgsound->setPosition(0.0, 0.0);
			m_bgsound->playLoop();
		}

		try {
			m_bgsound->loadFile(name);
		} catch( NotFound& ) {
			// VFS already warns ... why?
			Warn("audio_manager")
				<< "Ambient music file " << name << " not found.";
			delete m_bgsound;
			m_bgsound = 0;
			return; // NOTE It might be cleaner to re-throw the exception here.
		}
		m_bgsound->enable();
	}

	type_bufptr Manager::getBufferFromFile(const std::string &name) {
		// Check if we have a buffer with this file already
		type_bufvec::const_iterator end = m_bufvec.end();
		for (type_bufvec::iterator i = m_bufvec.begin(); i != end; ++i) {
			type_bufptr locked = i->lock();
			if (locked) {
				if (locked->m_name == name) {
					if (locked->isStreaming()) // We cannot reuse a streaming Buffer
						continue;
					return locked;
				}
			} else {
				m_bufvec.erase(i);
			}
		}

		// The file is either not reusable or not in memory. We have to create a
		// new Buffer.
		type_decptr ad(Decoder::create(name));
		if (!ad) {
			Warn("audio_manager") 
				<< "Unknown music format: " 
				<< name;
			return type_bufptr();
		}

		type_bufptr asp;
		asp.reset(Buffer::create(name, ad));

		m_bufvec.push_back(::boost::weak_ptr<Buffer>(asp));

		return asp;
	}

	float Manager::m_savedvolume = 1.0;

	Manager::Manager() : m_sound_disabled(false), m_bgsound(0), 
	                     m_context(NULL), m_device(alcOpenDevice(NULL)) {
		if (!m_device) {
			Warn("audio_manager") 
				<< "Error: could not open audio device - disabling sound!";
			m_sound_disabled = true;
			return;
		}

		m_context = alcCreateContext(m_device, NULL);

		if (alcGetError(m_device) != ALC_NO_ERROR || !m_context) {
			// Error
			throw Exception("Couldn't create audio context");
		}

		alcMakeContextCurrent(m_context);

		if (alcGetError(m_device) != ALC_NO_ERROR) {
			// Error
			throw Exception("Couldn't change current audio context");
		}

		changeListenerOrientation(1.0, 0.0);
	}
} } //FIFE::audio
