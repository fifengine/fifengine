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

/*
 * For comments regarding functions please see the header file. 
 */

#include "guichan/contrib/opengl/oglftfont.hpp"

#include "guichan/exception.hpp"
#include "guichan/opengl/openglgraphics.hpp"

namespace gcn
{
    namespace contrib
    {
        OGLFTFont::OGLFTFont (const std::string& filename, int size)
        {
            mRowSpacing = 0;
            mFilename = filename;
            mFont = NULL;
            mSize = size;

            mFont = new OGLFT::TranslucentTexture(filename.c_str(), size, 72);

            if(mFont == NULL || !mFont->isValid())
            {
                throw GCN_EXCEPTION("Invalid True Type Font.");
            }

            glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

            mFontColor = gcn::Color(0, 0, 0, 0);
        }

        OGLFTFont::~OGLFTFont()
        {
            delete mFont;
        }

        int OGLFTFont::getWidth(const std::string& text) const
        {
            OGLFT::BBox bbox = mFont->measure(text.c_str());

            return (int)bbox.x_max_ + (int)bbox.x_min_;
        }

        int OGLFTFont::getHeight() const
        {
            return mSize - 1 + mRowSpacing;
        }

        void OGLFTFont::setRowSpacing(int spacing)
        {
            mRowSpacing = spacing;
        }

        int OGLFTFont::getRowSpacing()
        {
            return mRowSpacing;
        }

        gcn::Color OGLFTFont::getColor()
        {
            return mFontColor;
        }

        void OGLFTFont::setColor(gcn::Color color)
        {
            mFontColor = color;
        }

        void OGLFTFont::drawString(gcn::Graphics* graphics, const std::string& text, int x, int y)
        {
            if (text == "")
            {
                return;
            }

            gcn::OpenGLGraphics* glGraphics = dynamic_cast<gcn::OpenGLGraphics *>(graphics);

            if(glGraphics == NULL)
            {
                throw GCN_EXCEPTION("Graphics object not an OpenGL graphics object!");
            }

            const gcn::ClipRectangle& top = glGraphics->getCurrentClipArea();

            mFont->setForegroundColor(mFontColor.r/255, mFontColor.g/255, mFontColor.b/255);

            glPushMatrix();
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glTranslated(x + top.xOffset, y + top.yOffset + (mSize/2)+5, 0.);
            glRotatef(180., 1., 0., 0.);

            mFont->draw(text.c_str());

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
            glPopMatrix();
        }
    }
}
