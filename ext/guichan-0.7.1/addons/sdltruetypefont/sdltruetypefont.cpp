/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004, 2005, 2006 Olof Naessén and Per Larsson
 *
 *                                                         Js_./
 * Per Larsson a.k.a finalman                          _RqZ{a<^_aa
 * Olof Naessén a.k.a jansem/yakslem                _asww7!uY`>  )\a//
 *                                                 _Qhm`] _f "'c  1!5m
 * Visit: http://guichan.darkbits.org             )Qk<P ` _: :+' .'  "{[
 *                                               .)j(] .d_/ '-(  P .   S
 * License: (BSD)                                <Td/Z <fP"5(\"??"\a.  .L
 * Redistribution and use in source and          _dV>ws?a-?'      ._/L  #'
 * binary forms, with or without                 )4d[#7r, .   '     )d`)[
 * modification, are permitted provided         _Q-5'5W..j/?'   -?!\)cam'
 * that the following conditions are met:       j<<WP+k/);.        _W=j f
 * 1. Redistributions of source code must       .$%w\/]Q  . ."'  .  mj$
 *    retain the above copyright notice,        ]E.pYY(Q]>.   a     J@\
 *    this list of conditions and the           j(]1u<sE"L,. .   ./^ ]{a
 *    following disclaimer.                     4'_uomm\.  )L);-4     (3=
 * 2. Redistributions in binary form must        )_]X{Z('a_"a7'<a"a,  ]"[
 *    reproduce the above copyright notice,       #}<]m7`Za??4,P-"'7. ).m
 *    this list of conditions and the            ]d2e)Q(<Q(  ?94   b-  LQ/
 *    following disclaimer in the                <B!</]C)d_, '(<' .f. =C+m
 *    documentation and/or other materials      .Z!=J ]e []('-4f _ ) -.)m]'
 *    provided with the distribution.          .w[5]' _[ /.)_-"+?   _/ <W"
 * 3. Neither the name of Guichan nor the      :$we` _! + _/ .        j?
 *    names of its contributors may be used     =3)= _f  (_yQmWW$#(    "
 *    to endorse or promote products derived     -   W,  sQQQQmZQ#Wwa]..
 *    from this software without specific        (js, \[QQW$QWW#?!V"".
 *    prior written permission.                    ]y:.<\..          .
 *                                                 -]n w/ '         [.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT       )/ )/           !
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY         <  (; sac    ,    '
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING,               ]^ .-  %
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF            c <   r
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR            aga<  <La
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          5%  )P'-3L
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR        _bQf` y`..)a
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          ,J?4P'.P"_(\?d'.,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES               _Pa,)!f/<[]/  ?"
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT      _2-..:. .r+_,.. .
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     ?a.<%"'  " -'.a_ _,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION)                     ^
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For comments regarding functions please see the header file. 
 */

#include "sdltruetypefont.hpp"

#include "guichan/exception.hpp"
#include "guichan/image.hpp"

#include <guichan/sdl/sdlgraphics.hpp>

namespace gcn
{    
    SDLTrueTypeFont::SDLTrueTypeFont (const std::string& filename, int size)
    {
        mRowSpacing = 0;
        mGlyphSpacing = 0;
        mAntiAlias = true;        
        mFilename = filename;
        mFont = NULL;
        
        mFont = TTF_OpenFont(filename.c_str(), size);
        
        if (mFont == NULL)
        {
            throw GCN_EXCEPTION("SDLTrueTypeFont::SDLTrueTypeFont. "+std::string(TTF_GetError()));
        }
    }
    
    SDLTrueTypeFont::~SDLTrueTypeFont()
    {
        TTF_CloseFont(mFont);
    }
  
    int SDLTrueTypeFont::getWidth(const std::string& text) const
    {
        int w, h;
        TTF_SizeText(mFont, text.c_str(), &w, &h);
        
        return w;
    }

    int SDLTrueTypeFont::getHeight() const
    {
        return TTF_FontHeight(mFont) + mRowSpacing;
    }
    
    void SDLTrueTypeFont::drawString(Graphics* graphics, const std::string& text, const int x, const int y)
    {
        if (text == "")
        {
            return;
        }
        
        SDLGraphics *sdlGraphics = dynamic_cast<SDLGraphics *>(graphics);

        if (sdlGraphics == NULL)
        {
            throw GCN_EXCEPTION("SDLTrueTypeFont::drawString. Graphics object not an SDL graphics object!");
            return;
        }
        
        // This is needed for drawing the Glyph in the middle if we have spacing
        int yoffset = getRowSpacing() / 2;
        
        Color col = sdlGraphics->getColor();

        SDL_Color sdlCol;
        sdlCol.b = col.b;
        sdlCol.r = col.r;
        sdlCol.g = col.g;

        SDL_Surface *textSurface;
        if (mAntiAlias)
        {
            textSurface = TTF_RenderText_Blended(mFont, text.c_str(), sdlCol);
        }
        else
        {
            textSurface = TTF_RenderText_Solid(mFont, text.c_str(), sdlCol);
        }
        
        SDL_Rect dst, src;
        dst.x = x;
        dst.y = y + yoffset;
        src.w = textSurface->w;
        src.h = textSurface->h;
        src.x = 0;
        src.y = 0;
        
        sdlGraphics->drawSDLSurface(textSurface, src, dst);
        SDL_FreeSurface(textSurface);        
    }
    
    void SDLTrueTypeFont::setRowSpacing(int spacing)
    {
        mRowSpacing = spacing;
    }

    int SDLTrueTypeFont::getRowSpacing()
    {
        return mRowSpacing;
    }
    
    void SDLTrueTypeFont::setGlyphSpacing(int spacing)
    {
        mGlyphSpacing = spacing;
    }
    
    int SDLTrueTypeFont::getGlyphSpacing()
    {
        return mGlyphSpacing;
    }

    void SDLTrueTypeFont::setAntiAlias(bool antiAlias)
    {
        mAntiAlias = antiAlias;
    }

    bool SDLTrueTypeFont::isAntiAlias()
    {
        return mAntiAlias;        
    }    
}

