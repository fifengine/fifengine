// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "soundmanager.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "audio/effects/soundeffectmanager.h"
#include "soundclipmanager.h"
#include "soundemitter.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/vfs.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_AUDIO);

    SoundManager::SoundManager() :
        m_context(nullptr),
        m_device(nullptr),
        m_muteVol(0),
        m_volume(1.0),
        m_maxDistance(50.0),
        m_distanceModel(SD_DISTANCE_INVERSE_CLAMPED),
        m_state(SM_STATE_INACTIV),
        m_sources(),
        m_createdSources(0),
        m_effectManager(nullptr)
    {
    }

    SoundManager::~SoundManager()
    {
        // delete all soundemitters
        for (auto& it : m_emitterVec) {
            delete it;
        }
        m_emitterVec.clear();
        // delete all sources
        alDeleteSources(m_createdSources, &m_sources[0]);
        // delete effect manager
        delete m_effectManager;

        if (m_device != nullptr) {
            alcDestroyContext(m_context);
            alcCloseDevice(m_device);
            m_device = nullptr;
        }

        if (alcGetError(nullptr) != ALC_NO_ERROR) {
            LMsg msg;
            msg << "error closing openal device";
            FL_ERR(_log, msg);
        }
    }

    void SoundManager::init()
    {
        m_device = alcOpenDevice(nullptr);

        if ((m_device == nullptr) || alcGetError(m_device) != ALC_NO_ERROR) {
            LMsg msg;
            msg << "Could not open audio device - deactivating audio module";
            FL_ERR(_log, msg);
            m_device = nullptr;
            return;
        }

        m_context = alcCreateContext(m_device, nullptr);
        if ((m_context == nullptr) || alcGetError(m_device) != ALC_NO_ERROR) {
            LMsg msg;
            msg << "Couldn't create audio context - deactivating audio module";
            FL_ERR(_log, msg);
            m_device = nullptr;
            return;
        }

        alcMakeContextCurrent(m_context);
        if (alcGetError(m_device) != ALC_NO_ERROR) {
            LMsg msg;
            msg << "Couldn't change current audio context - deactivating audio module";
            FL_ERR(_log, msg);
            m_device = nullptr;
            return;
        }
        // create and initialize the effect manager
        m_effectManager = new SoundEffectManager();
        m_effectManager->init(m_device);

        // set listener position
        alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
        ALfloat const vec1[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
        alListenerfv(AL_ORIENTATION, &vec1[0]);

        // set volume
        alListenerf(AL_GAIN, m_volume);

        // create max sources
        for (unsigned int& m_source : m_sources) {
            alGenSources(1, &m_source);
            if (alGetError() != AL_NO_ERROR) {
                break;
            }

            m_freeSources.push(m_source);
            m_createdSources++;
        }
        m_state = SM_STATE_PLAY;
    }

    bool SoundManager::isActive() const
    {
        return m_state != SM_STATE_INACTIV;
    }

    ALCcontext* SoundManager::getContext() const
    {
        return m_context;
    }

    void SoundManager::setVolume(float vol)
    {
        m_volume  = vol;
        m_muteVol = vol;
        if (isActive()) {
            alListenerf(AL_GAIN, vol);
        }
    }

    float SoundManager::getVolume() const
    {
        return m_volume;
    }

    void SoundManager::mute()
    {
        if (isActive()) {
            alGetListenerf(AL_GAIN, &m_muteVol);
            alListenerf(AL_GAIN, 0);
        }
    }

    void SoundManager::unmute() const
    {
        if (isActive()) {
            alListenerf(AL_GAIN, m_muteVol);
        }
    }

    void SoundManager::play()
    {
        m_state = SM_STATE_PLAY;
        for (auto* emitter : m_emitterVec) {
            if (emitter == nullptr) {
                continue;
            }
            emitter->play();
        }
    }

    void SoundManager::pause()
    {
        m_state = SM_STATE_PAUSE;
        for (auto* emitter : m_emitterVec) {
            if (emitter == nullptr) {
                continue;
            }
            emitter->pause();
        }
    }

    void SoundManager::stop()
    {
        m_state = SM_STATE_STOP;
        for (auto* emitter : m_emitterVec) {
            if (emitter == nullptr) {
                continue;
            }
            emitter->stop();
        }
    }

    void SoundManager::rewind()
    {
        for (auto* emitter : m_emitterVec) {
            if (emitter == nullptr) {
                continue;
            }
            emitter->rewind();
        }
    }

    void SoundManager::setDistanceModel(SoundDistanceModelType model)
    {
        m_distanceModel = model;
        if (!isActive()) {
            return;
        }

        switch (m_distanceModel) {
        case SD_DISTANCE_NONE:
            alDistanceModel(AL_NONE);
            break;
        case SD_DISTANCE_INVERSE:
            alDistanceModel(AL_INVERSE_DISTANCE);
            break;
        case SD_DISTANCE_INVERSE_CLAMPED:
            alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
            break;
        case SD_DISTANCE_LINEAR:
            alDistanceModel(AL_LINEAR_DISTANCE);
            break;
        case SD_DISTANCE_LINEAR_CLAMPED:
            alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
            break;
        case SD_DISTANCE_EXPONENT:
            alDistanceModel(AL_EXPONENT_DISTANCE);
            break;
        case SD_DISTANCE_EXPONENT_CLAMPED:
            alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
            break;
        default:
            break;
        }
    }

    SoundDistanceModelType SoundManager::getDistanceModel() const
    {
        return m_distanceModel;
    }

    void SoundManager::setListenerPosition(AudioSpaceCoordinate const & position) const
    {
        if (isActive()) {
            alListener3f(
                AL_POSITION,
                static_cast<ALfloat>(position.x),
                static_cast<ALfloat>(position.y),
                static_cast<ALfloat>(position.z));
        }
    }

    AudioSpaceCoordinate SoundManager::getListenerPosition() const
    {
        if (isActive()) {
            ALfloat vec[3];
            alGetListenerfv(AL_POSITION, &vec[0]);
            return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
        }
        return AudioSpaceCoordinate();
    }

    void SoundManager::setListenerOrientation(AudioSpaceCoordinate const & orientation) const
    {
        if (isActive()) {
            ALfloat const vec[6] = {
                static_cast<ALfloat>(orientation.x),
                static_cast<ALfloat>(orientation.y),
                static_cast<ALfloat>(orientation.z),
                0.0,
                0.0,
                1.0};
            alListenerfv(AL_ORIENTATION, &vec[0]);
        }
    }

    AudioSpaceCoordinate SoundManager::getListenerOrientation() const
    {
        if (isActive()) {
            ALfloat vec[6];
            alGetListenerfv(AL_ORIENTATION, &vec[0]);
            return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
        }
        return AudioSpaceCoordinate();
    }

    void SoundManager::setListenerVelocity(AudioSpaceCoordinate const & velocity) const
    {
        if (isActive()) {
            alListener3f(
                AL_VELOCITY,
                static_cast<ALfloat>(velocity.x),
                static_cast<ALfloat>(velocity.y),
                static_cast<ALfloat>(velocity.z));
        }
    }

    AudioSpaceCoordinate SoundManager::getListenerVelocity() const
    {
        if (isActive()) {
            ALfloat vec[3];
            alGetListenerfv(AL_VELOCITY, &vec[0]);
            return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
        }
        return AudioSpaceCoordinate();
    }

    void SoundManager::setDopplerFactor(float factor) const
    {
        if (isActive()) {
            if (factor >= 0.0) {
                alDopplerFactor(factor);
            }
        }
    }

    float SoundManager::getDopplerFactor() const
    {
        if (isActive()) {
            return alGetFloat(AL_DOPPLER_FACTOR);
        }
        return 0.0;
    }

    void SoundManager::setListenerMaxDistance(float distance)
    {
        m_maxDistance = distance;
    }

    float SoundManager::getListenerMaxDistance() const
    {
        return m_maxDistance;
    }

    void SoundManager::update()
    {
        if (m_state != SM_STATE_PLAY) {
            return;
        }
        AudioSpaceCoordinate const listenerPos = getListenerPosition();
        auto maxDistance                       = static_cast<double>(m_maxDistance);

        // first check emitters
        for (auto* emitter : m_emitterVec) {
            if (emitter == nullptr) {
                continue;
            }
            emitter->setCheckDifference();

            bool const active = emitter->isActive();
            bool const clip   = static_cast<bool>(emitter->getSoundClip());
            bool const plays  = !emitter->isFinished();
            // remove active without clip or stopped
            if (!clip || !plays) {
                if (active) {
                    emitter->update();
                    releaseSource(emitter);
                }
                continue;
            }

            bool inRange = true;
            if (emitter->isPosition()) {
                AudioSpaceCoordinate const emitterPos = emitter->getPosition();
                double const rx                       = listenerPos.x - emitterPos.x;
                double const ry                       = listenerPos.y - emitterPos.y;
                double const rz                       = listenerPos.z - emitterPos.z;
                inRange                               = maxDistance >= Mathd::Sqrt((rx * rx) + (ry * ry) + (rz * rz));
            }
            // remove active not in range
            if (!inRange) {
                if (active) {
                    releaseSource(emitter);
                }
                continue;
            }
            if (!active && !m_freeSources.empty()) {
                setEmitterSource(emitter);
            }
        }
        // then update active
        for (auto& m_activeEmitter : m_activeEmitters) {
            m_activeEmitter.first->update();
        }
    }

    SoundEmitter* SoundManager::getEmitter(uint32_t emitterId) const
    {
        return m_emitterVec.at(emitterId);
    }

    SoundEmitter* SoundManager::createEmitter()
    {
        SoundEmitter* ptr = nullptr;
        for (size_t index = 0; index < m_emitterVec.size(); ++index) {
            if (m_emitterVec.at(index) == nullptr) {
                assert(index <= std::numeric_limits<uint32_t>::max());
                uint32_t const emitterIndex = static_cast<uint32_t>(index);
                ptr                         = new SoundEmitter(this, emitterIndex);
                m_emitterVec.at(index)      = ptr;
                break;
            }
        }
        if (ptr == nullptr) {
            assert(m_emitterVec.size() <= std::numeric_limits<uint32_t>::max());
            ptr = new SoundEmitter(this, static_cast<uint32_t>(m_emitterVec.size()));
            m_emitterVec.push_back(ptr);
        }
        return ptr;
    }

    SoundEmitter* SoundManager::createEmitter(std::string const & name)
    {
        SoundEmitter* emitter = createEmitter();
        emitter->setSoundClip(SoundClipManager::instance()->get(name));
        return emitter;
    }

    void SoundManager::releaseEmitter(uint32_t emitterId)
    {
        SoundEmitter** ptr = &m_emitterVec.at(emitterId);
        if ((*ptr)->isActive()) {
            releaseSource(*ptr);
        }
        delete *ptr;
        *ptr = nullptr;
    }

    void SoundManager::deleteEmitter(SoundEmitter* emitter)
    {
        releaseEmitter(emitter->getId());
    }

    void SoundManager::setEmitterSource(SoundEmitter* emitter)
    {
        std::pair<std::map<SoundEmitter*, ALuint>::iterator, bool> ret;
        ret = m_activeEmitters.insert(std::pair<SoundEmitter*, ALuint>(emitter, m_freeSources.front()));
        if (!ret.second) {
            LMsg msg;
            msg << "SoundEmitter already have an source handler";
            FL_WARN(_log, msg);
        }
        emitter->setSource(m_freeSources.front());
        m_freeSources.pop();
    }

    void SoundManager::releaseSource(SoundEmitter* emitter)
    {
        if (emitter->isActive()) {
            auto it = m_activeEmitters.find(emitter);
            if (it != m_activeEmitters.end()) {
                m_freeSources.push(it->second);
                m_activeEmitters.erase(it);
                emitter->setSource(0);
            } else {
                LMsg msg;
                msg << "SoundEmitter can not release source handler";
                FL_WARN(_log, msg);
            }
        }
    }

    SoundEffect* SoundManager::createSoundEffect(SoundEffectType type)
    {
        return m_effectManager->createSoundEffect(type);
    }

    SoundEffect* SoundManager::createSoundEffectPreset(SoundEffectPreset type)
    {
        return m_effectManager->createSoundEffectPreset(type);
    }

    void SoundManager::deleteSoundEffect(SoundEffect* effect)
    {
        m_effectManager->deleteSoundEffect(effect);
    }

    void SoundManager::enableSoundEffect(SoundEffect* effect)
    {
        m_effectManager->enableSoundEffect(effect);
    }

    void SoundManager::disableSoundEffect(SoundEffect* effect)
    {
        m_effectManager->disableSoundEffect(effect);
    }

    void SoundManager::addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        m_effectManager->addEmitterToSoundEffect(effect, emitter);
    }

    void SoundManager::removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        m_effectManager->removeEmitterFromSoundEffect(effect, emitter);
    }

    void SoundManager::addSoundFilterToSoundEffect(SoundEffect* effect, SoundFilter* filter)
    {
        m_effectManager->addSoundFilterToSoundEffect(effect, filter);
    }

    void SoundManager::removeSoundFilterFromSoundEffect(SoundEffect* effect, SoundFilter* filter)
    {
        m_effectManager->removeSoundFilterFromSoundEffect(effect, filter);
    }

    void SoundManager::activateEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        m_effectManager->activateEffect(effect, emitter);
    }

    void SoundManager::deactivateEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        m_effectManager->deactivateEffect(effect, emitter);
    }

    SoundFilter* SoundManager::createSoundFilter(SoundFilterType type)
    {
        return m_effectManager->createSoundFilter(type);
    }

    void SoundManager::deleteSoundFilter(SoundFilter* filter)
    {
        m_effectManager->deleteSoundFilter(filter);
    }

    void SoundManager::enableDirectSoundFilter(SoundFilter* filter)
    {
        m_effectManager->enableDirectSoundFilter(filter);
    }

    void SoundManager::disableDirectSoundFilter(SoundFilter* filter)
    {
        m_effectManager->disableDirectSoundFilter(filter);
    }

    void SoundManager::addEmitterToDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        m_effectManager->addEmitterToDirectSoundFilter(filter, emitter);
    }

    void SoundManager::removeEmitterFromDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        m_effectManager->removeEmitterFromDirectSoundFilter(filter, emitter);
    }

    void SoundManager::activateFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        m_effectManager->activateFilter(filter, emitter);
    }

    void SoundManager::deactivateFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        m_effectManager->deactivateFilter(filter, emitter);
    }

    void SoundManager::addToGroup(SoundEmitter* emitter)
    {
        if (!emitter->getGroup().empty()) {
            m_groups[emitter->getGroup()].push_back(emitter);
        }
    }

    void SoundManager::removeFromGroup(SoundEmitter* emitter)
    {
        std::string const group = emitter->getGroup();
        if (group.empty()) {
            return;
        }
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "SoundEmitter can not removed from unknown group";
            FL_WARN(_log, msg);
            return;
        }
        bool found      = false;
        auto emitterIt  = groupIt->second.begin();
        auto emitterEnd = groupIt->second.end();
        while (emitterIt != emitterEnd) {
            if ((*emitterIt) == emitter) {
                groupIt->second.erase(emitterIt++);
                found = true;
            } else {
                ++emitterIt;
            }
        }
        if (!found) {
            LMsg msg;
            msg << "SoundEmitter could not be found in the given group.";
            FL_WARN(_log, msg);
            return;
        }
    }

    void SoundManager::removeGroup(std::string const & group)
    {
        if (group.empty()) {
            return;
        }
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "SoundEmitter can not remove unknown group";
            FL_WARN(_log, msg);
            return;
        }
        std::vector<SoundEmitter*> emitters = groupIt->second;
        for (auto& emitter : emitters) {
            emitter->setGroup("");
        }
        m_groups.erase(group);
    }

    void SoundManager::removeAllGroups()
    {
        std::vector<std::string> groups;
        groups.reserve(m_groups.size());

        std::ranges::transform(m_groups, std::back_inserter(groups), [](auto const & groupPair) -> std::string const & {
            return groupPair.first;
        });

        for (auto const & group : groups) {
            removeGroup(group);
        }

        m_groups.clear();
    }

    void SoundManager::play(std::string const & group)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not played";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->play();
        }
    }

    void SoundManager::pause(std::string const & group)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not paused";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->pause();
        }
    }

    void SoundManager::stop(std::string const & group)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not stopped";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->stop();
        }
    }

    void SoundManager::rewind(std::string const & group)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not rewinded";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->rewind();
        }
    }

    void SoundManager::setGain(std::string const & group, float gain)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not set gain";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->setGain(gain);
        }
    }

    void SoundManager::setMaxGain(std::string const & group, float gain)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not set max gain";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->setMaxGain(gain);
        }
    }

    void SoundManager::setMinGain(std::string const & group, float gain)
    {
        auto groupIt = m_groups.find(group);
        if (groupIt == m_groups.end()) {
            LMsg msg;
            msg << "Unknown group can not set min gain";
            FL_WARN(_log, msg);
            return;
        }
        auto emitterIt = groupIt->second.begin();
        for (; emitterIt != groupIt->second.end(); ++emitterIt) {
            (*emitterIt)->setMinGain(gain);
        }
    }

} // namespace FIFE
