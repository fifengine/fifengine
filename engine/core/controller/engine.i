/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
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
	class Model;
	class LogManager;
	class GuiFont;
	class VFS;
	class Cursor;
	class RendererBase;
	class DeviceCaps;
	class ScreenMode;
	class Image;

	class EngineSettings {
	public:
		~EngineSettings();
		void validate() const;
		void setBitsPerPixel(uint8_t bitsperpixel);
		uint16_t getBitsPerPixel() const;
		std::vector<uint16_t> getPossibleBitsPerPixel() const;
		std::vector<std::pair<uint16_t, uint16_t> > getPossibleResolutions() const;
		void setFullScreen(bool fullscreen);
		bool isFullScreen() const;
		void setInitialVolume(float volume);
		float getInitialVolume() const;
		float getMaxVolume() const;
		void setRenderBackend(const std::string& renderbackend);
		const std::string getRenderBackend() const;
		std::vector<std::string> getPossibleRenderBackends();
		void setSDLRemoveFakeAlpha(bool sdlremovefakealpha);
		bool isSDLRemoveFakeAlpha(bool sdlremovefakealpha) const;
		void setScreenWidth(uint16_t screenwidth);
		uint16_t getScreenWidth() const;
		void setScreenHeight(uint16_t screenheight);
		uint16_t getScreenHeight() const;
		void setDefaultFontPath(const std::string& defaultfontpath);
		std::string getDefaultFontPath() const;
		void setDefaultFontSize(uint16_t defaultfontsize);
		uint16_t getDefaultFontSize() const;
		void setDefaultFontGlyphs(const std::string& defaultfontglyphs);
		std::string getDefaultFontGlyphs() const;
		void setWindowTitle(const std::string& title);
		std::string getWindowTitle() const;
		void setWindowIcon(const std::string& icon);
		std::string getWindowIcon() const;
		void setColorKeyEnabled(bool colorkeyenable);
		bool isColorKeyEnabled() const;
		void setColorKey(uint8_t r, uint8_t g, uint8_t b);
		const SDL_Color& getColorKey() const;
		void setVideoDriver(const std::string& driver);
		const std::string& getVideoDriver() const;
		void setLightingModel(unsigned int lighting);
		unsigned int getLightingModel() const;
		
	private:
		EngineSettings();
	};	
	
	%feature("director") IEngineChangeListener;
	class IEngineChangeListener {
	public:
		virtual ~IEngineChangeListener() {}
		virtual void onScreenModeChanged(const ScreenMode& newmode) = 0;
	};

	class Engine {
	public:
		Engine();
		virtual ~Engine();
		void initializePumping();
		void finalizePumping();
		void pump();

		EngineSettings& getSettings();
		const DeviceCaps& getDeviceCaps() const;
		
		Image* changeScreenMode(const ScreenMode& mode);
		
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
		Model* getModel();
		LogManager* getLogManager();
		GuiFont* getDefaultFont();
		VFS* getVFS();
		Cursor* getCursor();
		
		void addChangeListener(IEngineChangeListener* listener);
		void removeChangeListener(IEngineChangeListener* listener);
	};
}
