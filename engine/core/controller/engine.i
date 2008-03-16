/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
%module fife
%{
#include "controller/engine.h"
%}

namespace FIFE {

	class SoundManager;
	class EventManager;
	class TimeManager;
	class GUIManager;
	class ImagePool;
	class AnimationPool;
	class SoundClipPool;
	class RenderBackend;
	class View;
	class Model;
	class LogManager;
	class GuiFont;
	class VFS;
	class Cursor;

	class EngineSettings {
	public:
		~EngineSettings();
		void validate() const throw(FIFE::NotSet);
		void setBitsPerPixel(unsigned int bitsperpixel) throw(FIFE::NotSupported);
		unsigned int getBitsPerPixel() const;
		std::vector<unsigned int> getPossibleBitsPerPixel() const;
		void setFullScreen(bool fullscreen);
		bool isFullScreen() const;
		void setInitialVolume(float volume) throw(FIFE::NotSupported);
		float getInitialVolume() const;
		float getMaxVolume() const;
		void setRenderBackend(const std::string& renderbackend) throw(FIFE::NotSupported);
		const std::string getRenderBackend() const;
		std::vector<std::string> getPossibleRenderBackends();
		void setSDLRemoveFakeAlpha(bool sldremovefakealpha);
		bool isSDLRemoveFakeAlpha(bool sldremovefakealpha) const;
		void setScreenWidth(unsigned int screenwidth);
		unsigned int getScreenWidth() const;
		void setScreenHeight(unsigned int screenheight);
		unsigned int getScreenHeight() const;
		void setDefaultFontPath(const std::string& defaultfontpath) throw(FIFE::NotFound);
		std::string getDefaultFontPath() const;
		void setDefaultFontSize(const unsigned int defaultfontsize);
		unsigned int getDefaultFontSize() const;
		void setDefaultFontGlyphs(const std::string& defaultfontglyphs);
		std::string getDefaultFontGlyphs() const;
		void setImageChunkingSize(unsigned int size);
		unsigned int getImageChunkingSize();
		
	private:
		EngineSettings();
	};	
	
	class Engine {
	public:
		Engine();
		virtual ~Engine();
		void initializePumping();
		void finalizePumping();
		void pump();

		EngineSettings& getSettings();
		void init() throw(FIFE::NotSet);

		SoundManager* getSoundManager();
		EventManager* getEventManager();
		TimeManager* getTimeManager();
		GUIManager* getGuiManager();
		ImagePool* getImagePool();
		AnimationPool* getAnimationPool();
		SoundClipPool* getSoundClipPool();
		RenderBackend* getRenderBackend();
		View* getView();
		Model* getModel();
		LogManager* getLogManager();
		GuiFont* getDefaultFont();
		VFS* getVFS();
		Cursor* getCursor();
	};
}
