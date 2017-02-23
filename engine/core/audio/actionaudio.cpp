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

#include "actionaudio.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	ActionAudio::ActionAudio() :
		m_name(""),
		m_group(""),
		m_volume(1.0),
		m_maxVolume(1.0),
		m_minVolume(0.0),
		m_refDistance(1.0),
		m_maxDistance(1000000.0),
		m_rolloff(1.0),
		m_pitch(1.0),
		m_coneInnerAngle(360.0),
		m_coneOuterAngle(360.0),
		m_coneOuterGain(0.0),
		m_looping(false),
		m_positioning(false),
		m_direction(false) {

		m_velocity = AudioSpaceCoordinate(0.0, 0.0, 0.0);

	}

	ActionAudio::~ActionAudio() {
	}

	void ActionAudio::setSoundFileName(const std::string& name) {
		m_name = name;
	}

	const std::string& ActionAudio::getSoundFileName() const {
		return m_name;
	}

	void ActionAudio::setGroupName(const std::string& name) {
		m_group = name;
	}

	const std::string& ActionAudio::getGroupName() const {
		return m_group;
	}

	void ActionAudio::setGain(float gain) {
		m_volume = gain;
	}

	float ActionAudio::getGain() const {
		return m_volume;
	}

	void ActionAudio::setMaxGain(float gain) {
		m_maxVolume = gain;
	}

	float ActionAudio::getMaxGain() const {
		return m_maxVolume;
	}

	void ActionAudio::setMinGain(float gain) {
		m_minVolume = gain;
	}

	float ActionAudio::getMinGain() const {
		return m_minVolume;
	}

	void ActionAudio::setReferenceDistance(float distance) {
		m_refDistance = distance;
	}

	float ActionAudio::getReferenceDistance() const {
		return m_refDistance;
	}

	void ActionAudio::setMaxDistance(float distance) {
		m_maxDistance = distance;
	}

	float ActionAudio::getMaxDistance() const {
		return m_maxDistance;
	}

	void ActionAudio::setRolloff(float rolloff) {
		m_rolloff = rolloff;
	}

	float ActionAudio::getRolloff() const {
		return m_rolloff;
	}

	void ActionAudio::setPitch(float pitch) {
		m_pitch = pitch;
	}

	float ActionAudio::getPitch() const {
		return m_pitch;
	}

	void ActionAudio::setConeInnerAngle(float inner) {
		m_coneInnerAngle = inner;
	}

	float ActionAudio::getConeInnerAngle() const {
		return m_coneInnerAngle;
	}

	void ActionAudio::setConeOuterAngle(float outer) {
		m_coneOuterAngle = outer;
	}

	float ActionAudio::getConeOuterAngle() const {
		return m_coneOuterAngle;
	}

	void ActionAudio::setConeOuterGain(float gain) {
		m_coneOuterGain = gain;
	}

	float ActionAudio::getConeOuterGain() const {
		return m_coneOuterGain;
	}

	void ActionAudio::setVelocity(const AudioSpaceCoordinate& velocity) {
		m_velocity = velocity;
	}

	const AudioSpaceCoordinate& ActionAudio::getVelocity() const {
		return m_velocity;
	}

	void ActionAudio::setLooping(bool loop) {
		m_looping = loop;
	}

	bool ActionAudio::isLooping() const {
		return m_looping;
	}

	void ActionAudio::setPositioning(bool relative) {
		m_positioning = relative;
	}

	bool ActionAudio::isPositioning() const {
		return m_positioning;
	}

	void ActionAudio::setDirection(bool direction) {
		m_direction = direction;
	}

	bool ActionAudio::isDirection() const {
		return m_direction;
	}
}
