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
		void validate() const;
		void setBitsPerPixel(unsigned int bitsperpixel);
		unsigned int getBitsPerPixel() const;
		std::vector<unsigned int> getPossibleBitsPerPixel() const;
		std::vector<std::pair<unsigned int, unsigned int> > getPossibleResolutions() const;
		void setFullScreen(bool fullscreen);
		bool isFullScreen() const;
		void setInitialVolume(float volume);
		float getInitialVolume() const;
		float getMaxVolume() const;
		void setRenderBackend(const std::string& renderbackend);
		const std::string getRenderBackend() const;
		std::vector<std::string> getPossibleRenderBackends();
		void setSDLRemoveFakeAlpha(bool sldremovefakealpha);
		bool isSDLRemoveFakeAlpha(bool sldremovefakealpha) const;
		void setScreenWidth(unsigned int screenwidth);
		unsigned int getScreenWidth() const;
		void setScreenHeight(unsigned int screenheight);
		unsigned int getScreenHeight() const;
		void setDefaultFontPath(const std::string& defaultfontpath);
		std::string getDefaultFontPath() const;
		void setDefaultFontSize(const unsigned int defaultfontsize);
		unsigned int getDefaultFontSize() const;
		void setDefaultFontGlyphs(const std::string& defaultfontglyphs);
		std::string getDefaultFontGlyphs() const;
		void setImageChunkingSize(unsigned int size);
		unsigned int getImageChunkingSize();
		void setWindowTitle(const std::string& title);
		std::string getWindowTitle() const;
		void setWindowIcon(const std::string& icon);
		std::string getWindowIcon() const;
		
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
		void init();
		void destroy();

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
