/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naessén and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naessén a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GCN_CONTRIB_OGLFTFONT_HPP
#define GCN_CONTRIB_OGLFTFONT_HPP

#include <string>

#include <oglft.h>

#include "guichan/font.hpp"
#include "guichan/graphics.hpp"
#include "guichan/platform.hpp"

namespace gcn
{
    namespace contrib
    {
        /**
         * OpenGL True Type Font implementation of Font. It uses the OGLFT library
         * to display True Type Fonts with OpenGL.
         *
         * @author Verneuil Julien
         */
        class GCN_EXTENSION_DECLSPEC OGLFTFont: public Font
        {
        public:

            /**
             * Constructor.
             *
             * @param filename the filename of the True Type Font.
             * @param size the size the font should be in.
             */
            GLFont (const std::string& filename, int size);

            /**
             * Destructor.
             */
            virtual ~GLFont();

            /**
             * Sets the spacing between rows in pixels. Default is 0 pixels.
             * The spacing can be negative.
             *
             * @param spacing the spacing in pixels.
             */
            virtual void setRowSpacing (int spacing);

            /**
             * Gets the spacing between rows in pixels.
             *
             * @return the spacing.
             */
            virtual int getRowSpacing();

            /**
             * Gets the actual color of the font.
             *
             * @return color the actual color of the font.
             */
            virtual gcn::Color getColor();

            /**
             * Sets the actual color of the font.
             *
             * @param color the color of the font.
             */
            virtual void setColor(gcn::Color color);

        
            // Inherited from Font

            virtual int getWidth(const std::string& text) const;

            virtual int getHeight() const;

            virtual void drawString(Graphics* graphics, const std::string& text, int x, int y);

        protected:
            OGLFT::TranslucentTexture* mFont;
            std::string mFilename;
            gcn::Color mFontColor;
            int mSize;
            int mRowSpacing;
        };
    }
}

#endif
