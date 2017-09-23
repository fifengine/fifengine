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

#ifndef FIFE_SOUNDSOURCE_H
#define FIFE_SOUNDSOURCE_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class ActionAudio;
	class Instance;
	class SoundChangeListener;
	class SoundEmitter;

	/** Interface class between Instance / ActionAudio and SoundEmitter.
	 */
	class SoundSource {
	public:

		SoundSource(Instance* instance);
		~SoundSource();

		/** Sets the ActionAudio. Owned by Object.
		 */
		void setActionAudio(ActionAudio* audio);

		/** Return ActionAudio. Owned by Object.
		 */
		ActionAudio* getActionAudio() const;

		/** Sets the positon of the SoundEmitter, called from Instance.
		 */
		void setPosition();

		/** Sets the direction of the SoundEmitter, called from Instance.
		 */
		void setDirection();
	
	private:
		/** Moves data from ActionAudio to SoundEmitter.
		 */
		void updateSoundEmitter();
		
		//! Associated Instance
		Instance* m_instance;
		//! Actual ActionAudio
		ActionAudio* m_audio;
		//! Related SoundEmitter
		SoundEmitter* m_emitter;
		//! InstanceChangeListener for position and direction
		SoundChangeListener* m_listener;


	};
}

#endif