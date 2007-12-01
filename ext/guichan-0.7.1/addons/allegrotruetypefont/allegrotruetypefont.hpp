/*      _______   __   __   __   ______   __   __   _______   __   __
*     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
*    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
*   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
*  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
* /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
* \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
*
* Copyright (c) 2004, 2005, 2006, 2007 Olof Naessén and Per Larsson
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

#ifndef GCN_ALLEGROTRUETYPEFONT_HPP
#define GCN_ALLEGROTRUETYPEFONT_HPP

#include "guichan/font.hpp"
#include "guichan/graphics.hpp"
#include "guichan/image.hpp"
#include "guichan/platform.hpp"

struct GLYPH_FACE;
struct GLYPH_REND;
struct GLYPH_KEEP;

namespace gcn
{
    /**
     * True Type Font support using GlyphKeeper and Allegro. 
     *
     * Note: GlyphKeeper must be compiled for allegro (define 
     *       GLYPH_TARGET=GLYPH_TARGET_ALLEGRO) and must be 
     *       initialized before using this class. 
     *
     * There's only basic functionality implemented here, so 
     * you can add more and advanced functionality by deriving 
     * from this class. 
     *
     * @author Tobias Scheuer. 
     */
    class GCN_EXTENSION_DECLSPEC AllegroTrueTypeFont : public Font
    {
    public:
        /**
         * Constructor.
         *     
         * @param filename the filename of the True Type Font.
         * @param size the size the font should be in.
         */
        AllegroTrueTypeFont(const std::string& filename, int size);

        /**
         * Constructor.
         *     
         * @param filename the filename of the True Type Font.
         * @param width the width the font should be in.
         * @param height the height the font should be in.
         */
        AllegroTrueTypeFont(const std::string& filename, int width, int height);

        /**
         * Destructor.
         */
        ~AllegroTrueTypeFont();


        // Inherited from Font

        virtual int getWidth(const std::string& text) const;

        virtual int getHeight() const;

        virtual void drawString(Graphics* graphics, const std::string& text, int x, int y);

    protected:
        void load(const std::string& filename, int w, int h);
        GLYPH_FACE* mFace;
        GLYPH_REND* mRend;
        GLYPH_KEEP* mKeeper;
    };
}

#endif // GCN_ALLEGROTRUETYPEFONT_HPP
