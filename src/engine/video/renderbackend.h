// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKEND_H
#define FIFE_VIDEO_RENDERBACKEND_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <vector>

#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

// FIFE includes
#include "color.h"
#include "image.h"
#include "util/base/singleton.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "video/devicecaps.h"

#ifdef HAVE_OPENGL
    #include <stack>

    #include "video/opengl/fife_opengl.h"
#endif

namespace FIFE
{

    class Image;

#ifdef HAVE_OPENGL
    enum GLConstants : uint16_t
    {
        KEEP     = GL_KEEP,
        ZERO     = GL_ZERO,
        REPLACE  = GL_REPLACE,
        INCR     = GL_INCR,
        DECR     = GL_DECR,
        INVERT   = GL_INVERT,
        NEVER    = GL_NEVER,
        LESS     = GL_LESS,
        LEQUAL   = GL_LEQUAL,
        GREATER  = GL_GREATER,
        GEQUAL   = GL_GEQUAL,
        EQUAL    = GL_EQUAL,
        NOTEQUAL = GL_NOTEQUAL,
        ALWAYS   = GL_ALWAYS
    };
#else
    enum GLConstants : uint16_t
    {
        KEEP = 0,
        ZERO,
        REPLACE,
        INCR,
        DECR,
        INVERT,
        NEVER,
        LESS,
        LEQUAL,
        GREATER,
        GEQUAL,
        EQUAL,
        NOTEQUAL,
        ALWAYS
    };
#endif

    enum OverlayType : uint8_t
    {
        OVERLAY_TYPE_NONE                = 0,
        OVERLAY_TYPE_COLOR               = 1,
        OVERLAY_TYPE_COLOR_AND_TEXTURE   = 2,
        OVERLAY_TYPE_TEXTURES_AND_FACTOR = 3
    };

    enum TextureFiltering : uint8_t
    {
        TEXTURE_FILTER_NONE        = 0,
        TEXTURE_FILTER_BILINEAR    = 1,
        TEXTURE_FILTER_TRILINEAR   = 2,
        TEXTURE_FILTER_ANISOTROPIC = 3
    };

    enum RenderDataType : uint8_t
    {
        RENDER_DATA_WITHOUT_Z      = 0,
        RENDER_DATA_TEXTURE_Z      = 1,
        RENDER_DATA_TEXCOLOR_Z     = 2,
        RENDER_DATA_MULTITEXTURE_Z = 3
    };

    class /*FIFE_API*/ GuiVertex
    {
        public:
            DoublePoint position;
            Color color;
            DoublePoint texCoords;
    };

    /** Abstract interface for all the renderbackends. */
    class /*FIFE_API*/ RenderBackend : public DynamicSingleton<RenderBackend>
    {
        public:
            /** Constructor.
             * @param colorkey The colorkey to use.
             */
            explicit RenderBackend(SDL_Color const & colorkey);

            /** Destructor.
             */
            virtual ~RenderBackend();

            RenderBackend(RenderBackend const &)            = delete;
            RenderBackend& operator=(RenderBackend const &) = delete;

            /** The name of the renderbackend.
             * @return The name of this renderbackend.
             */
            virtual std::string const & getName() const = 0;

            /** Called when a new frame starts.
             */
            virtual void startFrame();

            /** Called when a frame is finished and ready to be displayed.
             */
            virtual void endFrame();

            /** Initializes the backend.
             */
            virtual void init(std::string const & driver) = 0;

            /** Forces a clear of the backbuffer
             */
            virtual void clearBackBuffer() = 0;

            /** Initializes the light.
             */
            virtual void setLightingModel(uint32_t lighting) = 0;

            /** Gets the current light model.
             */
            virtual uint32_t getLightingModel() const = 0;

            /** Set colors for lighting
             */
            virtual void setLighting(float red, float green, float blue) = 0;

            /** Reset lighting with default values.
             */
            virtual void resetLighting() = 0;

            /** Reset stencil buffer with given value.
             */
            virtual void resetStencilBuffer(uint8_t buffer) = 0;

            /** Change the Blendingmodel.
             */
            virtual void changeBlending(int32_t scr, int32_t dst) = 0;

            /** Performs cleanup actions.
             */
            void deinit();

            /** Creates the mainscreen (the display window).
             * @param mode The ScreenMode to use.  @see FIFE::ScreenMode.
             * @param title The window title to use.
             * @param icon The window icon to use.
             */
            virtual void createMainScreen(
                ScreenMode const & mode, std::string const & title, std::string const & icon) = 0;

            /** Sets the mainscreen display mode.
             * @param mode The ScreenMode to change the display to.  @see FIFE::ScreenMode.
             */
            virtual void setScreenMode(ScreenMode const & mode) = 0;

            virtual Image* createImage(IResourceLoader* loader = nullptr)                           = 0;
            virtual Image* createImage(std::string const & name, IResourceLoader* loader = nullptr) = 0;

            /** Creates an Image suitable for this renderbackend.
             * @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
             * @param width Width of the image.
             * @param height Height of the image.
             * @return The new Image.
             */
            virtual Image* createImage(uint8_t const * data, uint32_t width, uint32_t height) = 0;
            virtual Image* createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) = 0;

            /** Helper function to create images from SDL_Surfaces.
             * Takes ownership over the surface.
             * @param surface The surface to convert.
             * @return The new Image.
             */
            virtual Image* createImage(SDL_Surface* surface)                           = 0;
            virtual Image* createImage(std::string const & name, SDL_Surface* surface) = 0;

            /** Render the Vertex Arrays, only for primitives (points, lines,...)
             */
            virtual void renderVertexArrays() = 0;

            /** Add the Image data to the array
             */
            virtual void addImageToArray(
                uint32_t id, Rect const & rec, float const * st, uint8_t alpha, uint8_t const * rgba) = 0;

            /** Dirty helper function to change the render infos
             */
            virtual void changeRenderInfos(
                RenderDataType type,
                uint16_t elements,
                int32_t src,
                int32_t dst,
                bool light,
                bool stentest,
                uint8_t stenref,
                GLConstants stenop,
                GLConstants stenfunc,
                OverlayType otype = OVERLAY_TYPE_NONE) = 0;

            /** Creates a Screenshot and saves it to a file.
             */
            virtual void captureScreen(std::string const & filename) = 0;

            /** Creates a Screenshot with the given size(w,h) and saves it to a file.
             */
            virtual void captureScreen(std::string const & filename, uint32_t width, uint32_t height) = 0;

            SDL_Window* getWindow()
            {
                return m_window;
            }

            /** Get current screen mode
             * @return The current screen mode
             */
            ScreenMode const & getCurrentScreenMode() const;

            uint32_t getWidth() const;
            uint32_t getHeight() const;
            uint32_t getScreenWidth() const
            {
                return getWidth();
            }
            uint32_t getScreenHeight() const
            {
                return getHeight();
            }
            Rect const & getArea() const;

            /** Pushes clip area to clip stack
             *  Clip areas define which area is drawn on screen. Usable e.g. with viewports
             *  note that previous items in stack do not affect the latest area pushed
             */
            void pushClipArea(Rect const & cliparea, bool clear = true);

            /** Pops clip area from clip stack
             *  @see pushClipArea
             */
            void popClipArea();

            /** Gets the current clip area
             *  @see pushClipArea
             */
            Rect const & getClipArea() const;

            /** Writes pixel to given position. Returns true, if pixel was written (not out of bounds)
             */
            virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws line between given points with given RGBA
             */
            virtual void drawLine(
                Point const & p1, Point const & p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws line between given points with given RGBA and width.
             */
            virtual void drawThickLine(
                Point const & p1,
                Point const & p2,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws lines between given points with given RGBA and width.
             */
            virtual void drawPolyLine(
                std::vector<Point> const & points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws bezier curve between given points with given RGBA and width.
             */
            virtual void drawBezier(
                std::vector<Point> const & points,
                int32_t steps,
                uint8_t width,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws triangle between given points with given RGBA
             */
            virtual void drawTriangle(
                Point const & p1,
                Point const & p2,
                Point const & p3,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws an axis parallel rectangle.
             */
            virtual void drawRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws a filled axis parallel rectangle.
             */
            virtual void fillRectangle(
                Point const & p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws quad between given points with given RGBA
             */
            virtual void drawQuad(
                Point const & p1,
                Point const & p2,
                Point const & p3,
                Point const & p4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws a quad that represents a vertex with given RGBA
             */
            virtual void drawVertex(
                Point const & p, uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws a circle.
             */
            virtual void drawCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws a filled circle.
             */
            virtual void drawFillCircle(
                Point const & p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

            /** Draws a circle segment.
             * Note: The start angle must be less than the end angle. 0 angle is right side.
             */
            virtual void drawCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws a filled circle segment. 0 angle is right side.
             * Note: The start angle must be less than the end angle.
             */
            virtual void drawFillCircleSegment(
                Point const & p,
                uint32_t radius,
                int32_t sangle,
                int32_t eangle,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255) = 0;

            /** Draws a light primitive that based on a triangle fan
             */
            virtual void drawLightPrimitive(
                Point const & p,
                uint8_t intensity,
                float radius,
                int32_t subdivisions,
                float xstretch,
                float ystretch,
                uint8_t red,
                uint8_t green,
                uint8_t blue) = 0;

            /** Enables scissor test on the render backend.
             */
            virtual void enableScissorTest() = 0;

            /** Disables scissor test on the render backend.
             */
            virtual void disableScissorTest() = 0;

            /** Enable or disable the alpha 'optimizing' code
             *
             * @param enabled Optimize whether the image shall be analysed for 'fake' alpha images.
             * Only implemented by and useful for the SDL backend at the moment.
             */
            void setAlphaOptimizerEnabled(bool enabled)
            {
                m_isalphaoptimized = enabled;
            }

            /** @see setAlphaOptimizerEnabled
             */
            bool isAlphaOptimizerEnabled() const
            {
                return m_isalphaoptimized;
            }

            /** Enables or disable compressing images by video driver.
             * @remarks This is relevant for in OpenGL renderbackend
             */
            void setImageCompressingEnabled(bool enabled)
            {
                m_compressimages = enabled;
            }

            /** @see setImageCompressingEnabled
             */
            bool isImageCompressingEnabled() const
            {
                return m_compressimages;
            }

            /** Enables or disable the usage of the framebuffer, if available
             */
            void setFramebufferEnabled(bool enabled)
            {
                m_useframebuffer = enabled;
            }

            /** @see setFramebufferEnabled
             */
            bool isFramebufferEnabled() const
            {
                return m_useframebuffer;
            }

            /** Enables or disable the usage of npot, if available
             */
            void setNPOTEnabled(bool enabled)
            {
                m_usenpot = enabled;
            }

            /** @see setNPOTEnabled
             */
            bool isNPOTEnabled() const
            {
                return m_usenpot;
            }

            /** Sets the texture filtering method.
             * Supports none, bilinear, trilinear and anisotropic filtering.
             * Note! Works only for OpenGL backends.
             * @see TextureFiltering
             */
            void setTextureFiltering(TextureFiltering filter);

            /** @see setTextureFiltering
             */
            TextureFiltering getTextureFiltering() const;

            /** Enables or disables the usage of mipmapping.
             * Note! Works only for OpenGL backends.
             */
            void setMipmappingEnabled(bool enabled);

            /** @see setMipmappingEnabled
             */
            bool isMipmappingEnabled() const;

            /** Gets max antisotropy for antisotropic filtering.
             */
            int32_t getMaxAnisotropy() const;

            /** Enables or disables monochrome rendering.
             * Note! Works only for OpenGL backends.
             */
            void setMonochromeEnabled(bool enabled);

            /** @see setMonochromeEnabled
             */
            bool isMonochromeEnabled() const;

            /** Enables or disables depth buffer rendering.
             * Note! Works only for OpenGL backend.
             */
            void setDepthBufferEnabled(bool enabled);

            /** @see setMonochromeEnabled
             */
            bool isDepthBufferEnabled() const;

            /** Sets the value for alpha test.
             * Discards a fragment if its value is not greater. Only used if depth buffer is enabled.
             * Note! Works only for OpenGL backend.
             */
            void setAlphaTestValue(float alpha);

            /** @see setAlphaValue
             */
            float getAlphaTestValue() const;

            /** Sets whether to use the colorkey feature
             */
            void setColorKeyEnabled(bool colorkeyenable);

            /** Gets whether the colorkey feature is in use
             */
            bool isColorKeyEnabled() const;

            /** Sets the global colorkey to use for images
             */
            void setColorKey(SDL_Color const & colorkey);

            /** Gets the global colorkey setting
             */
            SDL_Color const & getColorKey() const;

            /** Set the background color
             */
            void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);

            /** Reset the background color to black
             */
            void resetBackgroundColor();

            /** Gets the current screen rgba format
             */
            SDL_PixelFormat getPixelFormat() const;

            /** Sets whether to use VSync
             */
            void setVSyncEnabled(bool vsync);

            /** Gets whether VSync is in use
             */
            bool isVSyncEnabled() const;

            /** Sets whether to use the frame limiter
             */
            void setFrameLimitEnabled(bool limited);

            /** Gets whether the frame limiter is in use
             */
            bool isFrameLimitEnabled() const;

            /** Sets the frame limit
             */
            void setFrameLimit(uint16_t framelimit);

            /** Gets the frame limit
             */
            uint16_t getFrameLimit() const;

            /** Returns screen render surface
             */
            SDL_Surface* getScreenSurface();

            /** Returns currently attached render surface
             */
            SDL_Surface* getRenderTargetSurface();

            /** Attaches given image as a new render surface
             */
            virtual void attachRenderTarget(ImagePtr& img, bool discard) = 0;

            /** Detaches current render surface
             */
            virtual void detachRenderTarget() = 0;

            /** Renders geometry required by gui.
             */
            virtual void renderGuiGeometry(
                std::vector<GuiVertex> const & vertices,
                std::vector<int> const & indices,
                DoublePoint const & translation,
                ImagePtr texture) = 0;

            /** Helper that returns an interpolated Point
             */
            Point getBezierPoint(std::vector<Point> const & points, int32_t elements, float t);

            /** Helper that adds the control points for bezier curves.
             */
            void addControlPoints(std::vector<Point> const & points, std::vector<Point>& newPoints);

        protected:
            /** Sets given clip area into image
             *  @see pushClipArea
             */
            virtual void setClipArea(Rect const & cliparea, bool clear) = 0;

            SDL_Window* m_window;
            SDL_Surface* m_screen;
            SDL_Surface* m_target;
            bool m_compressimages;
            bool m_useframebuffer;
            bool m_usenpot;
            bool m_isalphaoptimized;
            bool m_iscolorkeyenabled;
            SDL_Color m_colorkey;
            ScreenMode m_screenMode;
            SDL_PixelFormatDetails m_rgba_format;

            bool m_isbackgroundcolor;
            SDL_Color m_backgroundcolor;

            // mipmapping
            bool m_isMipmapping;
            // texture filter
            TextureFiltering m_textureFilter;
            // max anisotropy
            int32_t m_maxAnisotropy;
            // monochrome rendering
            bool m_monochrome;
            // depth buffer rendering
            bool m_isDepthBuffer;
            // alpha test value
            float m_alphaValue;
            // vsync value
            bool m_vSync;

            /** Clears any possible clip areas
             *  @see pushClipArea
             */
            void clearClipArea();

            class /*FIFE_API*/ ClipInfo
            {
                public:
                    Rect r;
                    bool clearing{false};
            };
            std::stack<ClipInfo> m_clipstack;

            ClipInfo m_guiClip;

        private:
            bool m_isframelimit;
            uint32_t m_frame_start;
            uint16_t m_framelimit;
    };
} // namespace FIFE

#endif
