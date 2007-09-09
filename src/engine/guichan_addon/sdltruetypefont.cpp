/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004, 2005 darkbits                        Js_./
 * Per Larsson a.k.a finalman                          _RqZ{a<^_aa
 * Olof Naess� a.k.a jansem/yakslem                _asww7!uY`>  )\a//
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

#include "sdltruetypefont.hpp"
#include "SDL.h"

#include "guichan/exception.hpp"
#include "guichan/image.hpp"

#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "video/rect.h"
#include "video/image.h"
#include "video/screen.h"

#include "log.h"

namespace gcn {

	SDLTrueTypeFont::SDLTrueTypeFont (const std::string& filename, int size) : 
      mColor() {
		mRowSpacing = 0;
		mGlyphSpacing = 0;
		mAntiAlias = true;
		mFilename = filename;
		mFont = NULL;

		mFont = TTF_OpenFont(filename.c_str(), size);

		if (mFont == NULL) {
			throw GCN_EXCEPTION("SDLTrueTypeFont::SDLTrueTypeFont. "+std::string(TTF_GetError()));
		}
    mColor.r = mColor.g = mColor.b = 255;
	}

	SDLTrueTypeFont::~SDLTrueTypeFont() {
		TTF_CloseFont(mFont);
	}

	int SDLTrueTypeFont::getWidth(const std::string& text) const {
		int w, h;
		TTF_SizeText(mFont, text.c_str(), &w, &h);

		return w;
	}

	int SDLTrueTypeFont::getHeight() const {
		return TTF_FontHeight(mFont) + mRowSpacing;
	}

	// we should add a cache or something like that.. this can't be fast..
	void SDLTrueTypeFont::drawString(Graphics* graphics, const std::string& text, const int x, const int y) {
		if (text == "") {
			return;
		}

		int yoffset = getRowSpacing() / 2;

		SDL_Surface* textSurface = renderString(text);
		FIFE::RenderBackend* rbackend = FIFE::RenderManager::instance()->getCurrentBackend();
		FIFE::Image* image = rbackend->createStaticImageFromSDL(textSurface);
		const gcn::ClipRectangle& clip = rbackend->getMainScreen()->getCurrentClipArea();

		FIFE::Rect rect;
		rect.x = x + clip.xOffset;
		rect.y = y + clip.yOffset + yoffset;
		rect.w = image->getWidth();
		rect.h = image->getHeight();
		image->render(rect, rbackend->getMainScreen());
		delete image;
	}

	SDL_Surface* SDLTrueTypeFont::renderString(const std::string& text) {
		if (mAntiAlias) {
			return TTF_RenderText_Blended(mFont, text.c_str(), mColor);
		} else {
			return TTF_RenderText_Solid(mFont, text.c_str(), mColor);
		}
	}

	void SDLTrueTypeFont::setRowSpacing(int spacing) {
		mRowSpacing = spacing;
	}

	int SDLTrueTypeFont::getRowSpacing() {
		return mRowSpacing;
	}

	void SDLTrueTypeFont::setGlyphSpacing(int spacing) {
		mGlyphSpacing = spacing;
	}

	int SDLTrueTypeFont::getGlyphSpacing() {
		return mGlyphSpacing;
	}

	void SDLTrueTypeFont::setAntiAlias(bool antiAlias) {
		mAntiAlias = antiAlias;
	}

	bool SDLTrueTypeFont::isAntiAlias() {
		return mAntiAlias;
	}

	int SDLTrueTypeFont::getStringIndexAt(const std::string &text, int x) {
		for (int i = 0; i < static_cast<int>(text.size()); ++i) {
			if (getWidth(text.substr(0,i)) > x) {
				return i-1;
			}
		}
		return text.length();
	}

	void SDLTrueTypeFont::setColor(Uint8 r, Uint8 g, Uint8 b) {
		mColor.r = r;
		mColor.g = g;
		mColor.b = b;
	}
}
