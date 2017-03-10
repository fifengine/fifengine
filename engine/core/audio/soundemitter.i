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

%module soundemitter
%{
#include "audio/soundemitter.h"
%}

namespace FIFE {

	enum SoundPositionType {
		SD_SAMPLE_POS,
		SD_TIME_POS,
		SD_BYTE_POS
	};

	class SoundDecoder;
	class SoundManager;

	class SoundEmitter {
	public:
		SoundEmitter(SoundManager* manager, uint32_t uid);
		~SoundEmitter();

		uint32_t getId() const;

		void setSoundClip(SoundClipPtr soundclip);
		SoundClipPtr getSoundClip();
		void reset(bool defaultall = false);
		void release();

		void play();
		void pause();
		void stop();

		void setLooping(bool loop);
		void setRolloff(float rolloff);
		void setPositioning(bool relative);
		void setPosition(float x, float y, float z);
		void setVelocity(float x, float y, float z);
		void setGain(float gain);
		float getGain();

		bool isStereo();
		int16_t getBitResolution();
		uint64_t getSampleRate();
		uint64_t getDecodedLength();
		uint64_t getDuration();

		void setCursor(SoundPositionType type, float value);
		float getCursor(SoundPositionType type);
	};
}

