/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <boost/filesystem/convenience.hpp>
#include <boost/scoped_array.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"
#include "video/image.h"
#include "video/pixelbuffer.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "debugutils.h"
#include "util/exception.h"
#include "log.h"

#include "imagecache.h"
#include "sdlimagefont.hpp"

namespace FIFE {

	SDLImageFont::SDLImageFont() : FontBase() {
	}

	SDLImageFont::~SDLImageFont() {
		type_glyphs::iterator i = m_glyphs.begin();
		for(; i != m_glyphs.end(); ++i) {
			SDL_FreeSurface(i->second.surface);
		}
		
	}

	int SDLImageFont::getWidth(const std::string& text) const {
		int w = 0;

		for(size_t i=0; i!= text.size(); ++i) {
			type_glyphs::const_iterator it = m_glyphs.find( text[i] );

			if( it != m_glyphs.end() ) {
				w += it->second.surface->w + getGlyphSpacing();
				continue;
			}

			if( m_placeholder.surface ) {
				w += m_placeholder.surface->w + getGlyphSpacing();
			}
		}
		return w;
	}

	int SDLImageFont::getHeight() const {
		return mHeight;
	}

	SDL_Surface *SDLImageFont::renderString(const std::string& text) {
		SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			getWidth(text),getHeight(),32,
			0xff000000,0x00ff0000,0x0000ff00,0x000000ff);

		SDL_FillRect(surface,0,0x00000000);

		SDL_Rect dst;
		dst.x = dst.y = 0;
		s_glyph *glyph = 0;

		for(size_t i=0; i!= text.size(); ++i) {
			type_glyphs::iterator it = m_glyphs.find( text[i] );

			if( it == m_glyphs.end() ) {
				if( !m_placeholder.surface ) {
					continue;
				}
				glyph = &m_placeholder;
			} else {
				glyph = &(it->second);
			}
			dst.y  = glyph->offset.y;
			dst.x += glyph->offset.x;

			SDL_BlitSurface(glyph->surface,0,surface,&dst);
			dst.x += glyph->surface->w + getGlyphSpacing();
		}

		return surface;
	}

	void SDLImageFont::setColor(Uint8 r, Uint8 g, Uint8 b) {
	}
}
