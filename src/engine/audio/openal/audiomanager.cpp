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

#include <vorbisfile.h>
#include <stdio.h>


#include "audiomanager.h"
#include "audioconfig.h"
#include "audiodecoder.h"
#include "exception.h"
#include "log.h"

/* Changelog:
 * -Try to use Log more often
 * 
 * ToDo:
 * -Remove the exception from the constructor. IIRC stro doesn't like them
 */

namespace FIFE {

	typedef ::boost::shared_ptr<AudioDecoder> audiodec_sp;

	void AudioManager::setAmbientSound(const std::string &name) {
		if (m_sound_disabled) {
			return;
		}

		// Turn the background music off
		if (name == "" ) {
			delete m_bgsound;
			m_bgsound=0;
			return;
		}
		
		if (!m_bgsound) {
			m_bgsound = new AudioSource();
			m_bgsound->setPosition(0.0, 0.0);
			m_bgsound->playLoop();
		}

		m_bgsound->loadAudioFile(name);
		m_bgsound->enableAudio();
	}

	type_audiobufptr AudioManager::getBufferFromFile(const std::string &name) {
		// Check if we have a buffer with this file already
		std::vector<boost::weak_ptr<AudioBuffer> >::const_iterator end = m_audiobufvec.end();
		for (std::vector<boost::weak_ptr<AudioBuffer> >::iterator i = m_audiobufvec.begin(); i != end; ++i) {
			type_audiobufptr locked = (*i).lock();
			if (locked) {
				if (locked->m_name == name) {
					if (locked->isStreaming()) // We cannot reuse a streaming AudioBuffer
						continue;
					return locked;
				}
			} else {
				m_audiobufvec.erase(i);
			}
		}

		// The file is either not reusable or not in memory. We have to create a
		// new AudioBuffer.
		audiodec_sp ad(AudioDecoder::createDecoder(name));
		if (!ad) {
			std::cerr << "Unknown music format" << std::endl;
			return type_audiobufptr();
		}

		type_audiobufptr asp;
		asp.reset(AudioBuffer::createAudioBuffer(name, ad));

		::boost::weak_ptr<AudioBuffer> wp(asp);
		m_audiobufvec.push_back(wp);

		return asp;
	}

	float AudioManager::m_savedvolume = 1.0;

	AudioManager::AudioManager() : m_sound_disabled(false), m_bgsound(0), m_audiocontext(NULL), m_audiodevice(alcOpenDevice(NULL)) {
		if (!m_audiodevice) {
			// Error
			//Log("AudioManager", Log::LEVEL_WARN) << "Couldn't open audio device - disabling sound";
			// FIFE::Log is not available at creation time :-(
			std::cerr << "Error: could not open audio device - disabling sound!" << std::endl;
			m_sound_disabled = true;
			return;
		}

		m_audiocontext = alcCreateContext(m_audiodevice, NULL);

		if (alcGetError(m_audiodevice) != ALC_NO_ERROR || !m_audiocontext) {
			// Error
			throw Exception("Couldn't create audio context");
		}

		alcMakeContextCurrent(m_audiocontext);

		if (alcGetError(m_audiodevice) != ALC_NO_ERROR) {
			// Error
			throw Exception("Couldn't change current audio context");
		}

		changeListenerOrientation(1.0, 0.0);
	}

	AudioManager::~AudioManager() {
		m_audiobufvec.clear();
		if (m_sound_disabled)
			return;

		//alcMakeContextCurrent(NULL);
		/* I think I read somewhere that you should NOT do above call on exit */

		alcDestroyContext(m_audiocontext);
		alcCloseDevice(m_audiodevice);
	}

}//FIFE
