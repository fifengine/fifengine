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
%module engine
%{
#include "controller/engine.h"
%}

namespace FIFE {

	namespace audio {
		class Manager;
	}
	class EventManager;
	class TimeManager;
	class SettingsManager;
	class GUIManager;
	class ImagePool;
	class AnimationPool;
	class RenderBackend;
	class View;
	class Model;

	class Engine {
		public:
			Engine(bool use_miniwindow=false);
			virtual ~Engine();
			void initializePumping();
			void finalizePumping();
			void pump();

			audio::Manager* getAudioManager();
			EventManager* getEventManager();
			TimeManager* getTimeManager();
			SettingsManager* getSettingsManager();
			GUIManager* getGuiManager();
			ImagePool* getImagePool();
			AnimationPool* getAnimationPool();
			RenderBackend* getRenderBackend();
			View* getView();
			Model* getModel();
	};
}
