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

/*
 * For comments regarding functions please see the header file.
 */

#include "guichan/allegro/allegrographics.hpp"
#include "guichan/allegro/allegroimage.hpp"
#include "guichan/rectangle.hpp"
#include "guichan/exception.hpp"
#include "guichan/cliprectangle.hpp"
#include "guichan/color.hpp"

namespace gcn
{
    AllegroGraphics::AllegroGraphics()
    {
        mTarget = NULL;
        mClipNull = false;
    }

    AllegroGraphics::AllegroGraphics(BITMAP *target)
    {
        mTarget = target;
    }

    AllegroGraphics::~AllegroGraphics()
    {
    }

    void AllegroGraphics::setTarget(BITMAP *target)
    {
        mTarget = target;
    }

    BITMAP *AllegroGraphics::getTarget()
    {
        return mTarget;
    }

    void AllegroGraphics::_beginDraw()
    {
        if (mTarget == NULL)
        {
            throw GCN_EXCEPTION("Target BITMAP is null, set it with setTarget first.");
        }

        // push a clip area the size of the target bitmap
        pushClipArea(Rectangle(0, 0, mTarget->w, mTarget->h));
    }

    void AllegroGraphics::_endDraw()
    {
        // pop the clip area pushed in _beginDraw
        popClipArea();
    }

    bool AllegroGraphics::pushClipArea(Rectangle area)
    {
        bool result = Graphics::pushClipArea(area);

        ClipRectangle cr = mClipStack.top();

        // Allegro won't let you set clip areas
        // that have zero width or height
        // so we have to check for that.
        if (cr.width == 0 || cr.height == 0)
        {
            mClipNull = true;
        }
        else
        {
            mClipNull = false;
#if ALLEGRO_VERSION == 4 && ALLEGRO_SUB_VERSION == 0
            set_clip(mTarget, cr.x, cr.y, cr.x + cr.width - 1, cr.y + cr.height - 1);
#else
            set_clip_rect(mTarget, cr.x, cr.y, cr.x + cr.width - 1, cr.y + cr.height - 1);
#endif
        }

        return result;
    }

    void AllegroGraphics::popClipArea()
    {
        Graphics::popClipArea();

        if (mClipStack.empty())
        {
            return;
        }

        ClipRectangle cr = mClipStack.top();

        // Allegro won't let you set clip areas
        //that have zero width or height
        // so we have to check for that.
        if (cr.width == 0 || cr.height == 0)
        {
            mClipNull = true;
        }
        else
        {
            mClipNull = false;
#if ALLEGRO_VERSION == 4 && ALLEGRO_SUB_VERSION == 0
            set_clip(mTarget, cr.x, cr.y, cr.x + cr.width - 1, cr.y + cr.height - 1);
#else
            set_clip_rect(mTarget, cr.x, cr.y, cr.x + cr.width - 1, cr.y + cr.height - 1);
#endif
        }
    }

    void AllegroGraphics::drawImage(const Image* image,
                                    int srcX, int srcY,
                                    int dstX, int dstY,
                                    int width, int height)
    {
        if (mClipNull)
        {
            return;
        }

        dstX += mClipStack.top().xOffset;
        dstY += mClipStack.top().yOffset;

        const AllegroImage* srcImage = dynamic_cast<const AllegroImage*>(image);

        if (srcImage == NULL)
        {
            throw GCN_EXCEPTION("Trying to draw an image of unknown format, must be an AllegroImage.");
        }

        masked_blit(srcImage->getBitmap(), mTarget, srcX, srcY, dstX, dstY, width, height);
    }

    void AllegroGraphics::drawPoint(int x, int y)
    {
        if (mClipNull)
        {
            return;
        }

        int xOffset = mClipStack.top().xOffset;
        int yOffset = mClipStack.top().yOffset;

        putpixel(mTarget,
                 x + xOffset,
                 y + yOffset,
                 mAlColor);
    }

    void AllegroGraphics::drawLine(int x1, int y1, int x2, int y2)
    {
        if (mClipNull)
        {
            return;
        }

        int xOffset = mClipStack.top().xOffset;
        int yOffset = mClipStack.top().yOffset;

        line(mTarget,
             x1 + xOffset,
             y1 + yOffset,
             x2 + xOffset,
             y2 + yOffset,
             mAlColor);
    }

    void AllegroGraphics::drawRectangle(const Rectangle& rectangle)
    {
        if (mClipNull)
        {
            return;
        }

        int xOffset = mClipStack.top().xOffset;
        int yOffset = mClipStack.top().yOffset;

        rect(mTarget,
             rectangle.x + xOffset,
             rectangle.y + yOffset,
             rectangle.x + rectangle.width - 1 + xOffset,
             rectangle.y + rectangle.height - 1 + yOffset,
             mAlColor);
    }

    void AllegroGraphics::fillRectangle(const Rectangle& rectangle)
    {
        if (mClipNull)
        {
            return;
        }

        int xOffset = mClipStack.top().xOffset;
        int yOffset = mClipStack.top().yOffset;

        rectfill(mTarget,
                 rectangle.x + xOffset,
                 rectangle.y + yOffset,
                 rectangle.x + rectangle.width - 1 + xOffset,
                 rectangle.y + rectangle.height - 1 + yOffset,
                 mAlColor);
    }

    void AllegroGraphics::setColor(const Color& color)
    {
        mColor = color;
        mAlColor = makecol(color.r, color.g, color.b);

        if (color.a != 255)
        {
            set_trans_blender(255, 255, 255, color.a);
            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
        }
        else
        {
            solid_mode();
        }
    }

    const Color& AllegroGraphics::getColor()
    {
        return mColor;
    }
}
