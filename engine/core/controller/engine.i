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
%module fife
%{
#include "controller/engine.h"
%}


namespace FIFE {

	class SoundManager;
	class EventManager;
	class TimeManager;
	class IGUIManager;
	class GUIChanManager;
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
	class ImageManager;
	class AnimationManager;
	class SoundClipManager;
	class OffRenderer;
	class TargetRenderer;

	class EngineSettings {
	public:
		~EngineSettings();
		void setBitsPerPixel(uint8_t bitsperpixel);
		uint8_t getBitsPerPixel() const;
		std::vector<uint8_t> getPossibleBitsPerPixel() const;
		void setFullScreen(bool fullscreen);
		bool isFullScreen() const;
		void setRefreshRate(uint16_t rate);
		uint16_t getRefreshRate() const;
		void setDisplay(uint8_t display);
		uint8_t getDisplay() const;
		void setVSync(bool vsync);
		bool isVSync() const;
		void setSDLDriver(const std::string& driver);
		const std::string& getSDLDriver() const;
		void setInitialVolume(float volume);
		float getInitialVolume() const;
		float getMaxVolume() const;
		void setRenderBackend(const std::string& renderbackend);
		const std::string& getRenderBackend() const;
		std::vector<std::string> getPossibleRenderBackends();
		void setSDLRemoveFakeAlpha(bool sdlremovefakealpha);
		bool isSDLRemoveFakeAlpha() const;
		void setGLCompressImages(bool oglcompressimages);
		bool isGLCompressImages() const;
		void setGLUseFramebuffer(bool ogluseframebuffer);
		bool isGLUseFramebuffer() const;
		void setGLUseNPOT(bool oglusenpot);
		bool isGLUseNPOT() const;
		void setGLTextureFiltering(FIFE::TextureFiltering filter);
		FIFE::TextureFiltering getGLTextureFiltering() const;
		void setGLUseMipmapping(bool mipmapping);
		bool isGLUseMipmapping() const;
		void setGLUseMonochrome(bool monochrome);
		bool isGLUseMonochrome() const;
		void setGLUseDepthBuffer(bool buffer);
		bool isGLUseDepthBuffer() const;
		void setGLAlphaTestValue(float alpha);
		float getGLAlphaTestValue() const;
		void setScreenWidth(uint16_t screenwidth);
		uint16_t getScreenWidth() const;
		void setScreenHeight(uint16_t screenheight);
		uint16_t getScreenHeight() const;
		void setDefaultFontPath(const std::string& defaultfontpath);
		const std::string& getDefaultFontPath() const;
		void setDefaultFontSize(uint16_t defaultfontsize);
		uint16_t getDefaultFontSize() const;
		void setDefaultFontGlyphs(const std::string& defaultfontglyphs);
		const std::string& getDefaultFontGlyphs() const;
		void setWindowTitle(const std::string& title);
		const std::string& getWindowTitle() const;
		void setWindowIcon(const std::string& icon);
		const std::string& getWindowIcon() const;
		void setColorKeyEnabled(bool colorkeyenable);
		bool isColorKeyEnabled() const;
		void setColorKey(uint8_t r, uint8_t g, uint8_t b);
		const SDL_Color& getColorKey() const;
		void setVideoDriver(const std::string& driver);
		const std::string& getVideoDriver() const;
		void setLightingModel(uint32_t lighting);
		uint32_t getLightingModel() const;
		void setFrameLimitEnabled(bool limited);
		bool isFrameLimitEnabled() const;
		void setFrameLimit(uint16_t framelimit);
		uint16_t getFrameLimit() const;
		void setMouseSensitivity(float sens);
		float getMouseSensitivity() const;
		void setMouseAccelerationEnabled(bool acceleration);
		bool isMouseAccelerationEnabled() const;
		
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
		
		void changeScreenMode(const ScreenMode& mode);

		void init();
		void destroy();

		SoundManager* getSoundManager();
		EventManager* getEventManager();
		TimeManager* getTimeManager();
		void setGuiManager(IGUIManager* guimanager);
		IGUIManager* getGuiManager();
		ImageManager* getImageManager();
		AnimationManager* getAnimationManager();
		SoundClipManager* getSoundClipManager();
		RenderBackend* getRenderBackend();
		Model* getModel();
		LogManager* getLogManager();
		VFS* getVFS();
		Cursor* getCursor();
		OffRenderer* getOffRenderer();
		TargetRenderer* getTargetRenderer();
		
		void addChangeListener(IEngineChangeListener* listener);
		void removeChangeListener(IEngineChangeListener* listener);
	};
}
