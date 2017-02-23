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
#include "model/structures/instance.h"
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "actionaudio.h"
#include "soundmanager.h"
#include "soundemitter.h"
#include "soundsource.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	class SoundChangeListener : public InstanceChangeListener {
	public:
		SoundChangeListener(SoundSource* source)	{
			m_source = source;
		}
		virtual ~SoundChangeListener() {}

		virtual void onInstanceChanged(Instance* instance, InstanceChangeInfo info) {
			if ((info & ICHANGE_LOC) == ICHANGE_LOC) {
				m_source->setPosition();
			} else if ((info & ICHANGE_ROTATION) == ICHANGE_ROTATION) {
				m_source->setDirection();
			}
		}

	private:
		SoundSource* m_source;
	};

	SoundSource::SoundSource(Instance* instance) :
		m_instance(instance),
		m_audio(NULL) {

		m_emitter = SoundManager::instance()->createEmitter();
		m_listener = new SoundChangeListener(this);
		m_instance->addChangeListener(m_listener);

		// inital data
		setPosition();

	}

	SoundSource::~SoundSource() {
		m_instance->removeChangeListener(m_listener);
		delete m_listener;
		SoundManager::instance()->releaseEmitter(m_emitter->getId());
	}

	void SoundSource::setActionAudio(ActionAudio* audio) {
		if (audio != m_audio) {
			if (m_audio) {
				m_emitter->stop();
			}

			m_audio = audio;
			if (m_audio) {
				updateSoundEmitter();
				m_emitter->play();
			} else {
				m_emitter->reset();
			}
		} else if (audio && !m_emitter->isLooping()) {
			m_emitter->rewind();
			m_emitter->play();
		}
	}

	ActionAudio* SoundSource::getActionAudio() const {
		return m_audio;
	}

	void SoundSource::setPosition() {
		if (m_audio) {
			m_emitter->setPosition(m_instance->getLocationRef().getMapCoordinates());
		}
	}

	void SoundSource::setDirection() {
		if (m_audio && m_audio->isDirection()) {
			m_emitter->setDirection(m_instance->getFacingLocation().getMapCoordinates());
		}
	}

	void SoundSource::updateSoundEmitter() {
		m_emitter->setGain(m_audio->getGain());
		m_emitter->setMaxGain(m_audio->getMaxGain());
		m_emitter->setMinGain(m_audio->getMinGain());
		m_emitter->setReferenceDistance(m_audio->getReferenceDistance());
		m_emitter->setMaxDistance(m_audio->getMaxDistance());
		m_emitter->setRolloff(m_audio->getRolloff());
		m_emitter->setPitch(m_audio->getPitch());
		m_emitter->setConeInnerAngle(m_audio->getConeInnerAngle());
		m_emitter->setConeOuterAngle(m_audio->getConeOuterAngle());
		m_emitter->setConeOuterGain(m_audio->getConeOuterGain());
		m_emitter->setVelocity(m_audio->getVelocity());
		m_emitter->setLooping(m_audio->isLooping());
		m_emitter->setPositioning(m_audio->isPositioning());
		m_emitter->setPosition(m_instance->getLocationRef().getMapCoordinates());
		m_emitter->setSoundClip(m_audio->getSoundFileName());
		m_emitter->setGroup(m_audio->getGroupName());
	}

}
