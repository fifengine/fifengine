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

%module soundmanager
%{
#include "audio/soundmanager.h"
%}

%include "model/metamodel/modelcoords.i"
%include "util/resource/resource.i"

namespace FIFE {

	enum SoundDistanceModelType {
		SD_DISTANCE_NONE,
		SD_DISTANCE_INVERSE,
		SD_DISTANCE_INVERSE_CLAMPED,
		SD_DISTANCE_LINEAR,
		SD_DISTANCE_LINEAR_CLAMPED,
		SD_DISTANCE_EXPONENT,
		SD_DISTANCE_EXPONENT_CLAMPED
	};

	class SoundClip;

	class SoundManager {
		public:
			SoundManager();
			~SoundManager();
	
			void init();
			
			SoundEmitter* createEmitter();
			SoundEmitter* createEmitter(const std::string& name);
			SoundEmitter* getEmitter(uint32_t emitterId);
			void releaseEmitter(uint32_t emitterId);
			
			ALCcontext* getContext();
			
			void setVolume(float vol);
			float getVolume() const;
			void mute();
			void unmute();
			
			void setDistanceModel(SoundDistanceModelType model);
			SoundDistanceModelType getDistanceModel() const;

			void setListenerPosition(const AudioSpaceCoordinate& position);
			AudioSpaceCoordinate getListenerPosition() const;
			void setListenerOrientation(const AudioSpaceCoordinate& orientation);
			AudioSpaceCoordinate getListenerOrientation() const;
			void setListenerVelocity(const AudioSpaceCoordinate& velocity);
			AudioSpaceCoordinate getListenerVelocity() const;
			void setDopplerFactor(float factor);
			float getDopplerFactor() const;
	};
}
