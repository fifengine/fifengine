/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "audio/soundclippool.h"
%}

%include "util/resource/resource.i"

namespace FIFE {
	class SoundClip;

	class SoundClipPool: public Pool {
	public:
		virtual ~SoundClipPool();
		inline SoundClip& getSoundClip(unsigned int index);
	private:
		SoundClipPool();
	};

	class SoundManager {
		public:
			SoundManager(SoundClipPool*);
			~SoundManager();
	
			void init();
			
			SoundEmitter* createEmitter();
			SoundEmitter* getEmitter(unsigned int emitterid);
			void releaseEmitter(unsigned int emitterid);
			
			ALCcontext* getContext();
			
			void setVolume(float vol);
			float getVolume() const;
			void mute();
			void unmute();
			
			void setListenerPosition(float x, float y, float z);
			void setListenerOrientation(float x, float y, float z);
	};
}
