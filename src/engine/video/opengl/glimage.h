// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_OPENGL_GLIMAGE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
#include "video/image.h"

#include "fife_opengl.h"

namespace FIFE
{

    /** Implements an Image using OpenGL.
     *
     * This class contains a texture handle bound to the data given to the constructor;
     * it draws a textured Quad to the screen.
     *
     * @see Image
     * @note Width and height are not limited to powers of two;
     *       non-power of two images will be converted internally,
     *       if they are not supported by the hardware (ARB_texture_non_power_of_two).
     * @todo Check the correctness of the generateTexture function on big endian systems (ppc)
     */
    class /*FIFE_API*/ GLImage : public Image
    {
    public:
        explicit GLImage(IResourceLoader* loader = nullptr);
        explicit GLImage(const std::string& name, IResourceLoader* loader = nullptr);
        explicit GLImage(SDL_Surface* surface);
        GLImage(const std::string& name, SDL_Surface* surface);
        explicit GLImage(const uint8_t* data, uint32_t width, uint32_t height);
        GLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);

        virtual ~GLImage();
        virtual void invalidate();
        virtual void setSurface(SDL_Surface* surface);
        virtual void render(const Rect& rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr);
        virtual void render(
            const Rect& rect, const ImagePtr& overlay, uint8_t alpha = 255, uint8_t const * rgb = nullptr);

        virtual void renderZ(const Rect& rect, float vertexZ, uint8_t alpha = 255, uint8_t const * rgb = nullptr);
        virtual void renderZ(
            const Rect& rect,
            float vertexZ,
            const ImagePtr& overlay,
            uint8_t alpha       = 255,
            uint8_t const * rgb = nullptr);

        virtual void useSharedImage(const ImagePtr& shared, const Rect& region);
        virtual void forceLoadInternal();
        virtual void copySubimage(uint32_t xoffset, uint32_t yoffset, const ImagePtr& img);
        virtual void load();
        virtual void free();

        GLuint getTexId() const;
        const GLfloat* getTexCoords() const;
        bool isCompressed() const
        {
            return m_compressed;
        }
        void setCompressed(bool compressed)
        {
            m_compressed = compressed;
        }

    private:
        // Holds Atlas Name if this is a shared image
        std::string m_atlas_name;
        // Holds Atlas ImagePtr if this is a shared image
        ImagePtr m_atlas_img;

        // texture coords to use
        GLfloat m_tex_coords[4];

        //     [0]    [2]    ->(x)
        // [1]  +------+
        //      |      |
        //      |      |
        // [3]  +------+
        //
        //      |
        //      v
        //     (y)
        // To map these indices with previous one:
        // [0]:=[1]:=0.0f, [2]:=m_col_tex_coords, [3]:=m_row_tex_coords

        /** Frees allocated memory and calls resetGlImage
         */
        void cleanup();

        /** Resets GLImage variables
         */
        void resetGlimage();

        /** Generates the GL Texture for use when rendering.
         */
        void generateGLTexture();
        void generateGLSharedTexture(const GLImage* shared, const Rect& region);
        void validateShared();

        GLImage* m_shared_img;

        /** Holds texture ids that are used to access textures in GL rendering context
         */
        GLuint m_texId;

        uint32_t m_chunk_size_w;
        uint32_t m_chunk_size_h;

        SDL_Color m_colorkey;

        // Was this image compressed by OpenGL driver during loading ?
        bool m_compressed;
    };

} // namespace FIFE

#endif
