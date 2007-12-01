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

#include "guichan/openlayer/openlayerimage.hpp"

#include "guichan/exception.hpp"

namespace gcn
{
    OpenLayerImage::OpenLayerImage(BITMAP* allegroBitmap, bool autoFree)
            : mOpenLayerBitmap(NULL),
              mAllegroBitmap(allegroBitmap),
              mAutoFree(autoFree)
        
    {
        if (mAllegroBitmap == NULL)
        {
            throw GCN_EXCEPTION(std::string("Unable to load bitmap."));
        }
    }

    OpenLayerImage::~OpenLayerImage()
    {
        if (mAutoFree)
        {
            free();
        }
    }

    ol::Bitmap* OpenLayerImage::getBitmap() const
    {
        return mOpenLayerBitmap;
    }

    void OpenLayerImage::free()
    {
        if (mOpenLayerBitmap != NULL)
        {
            delete mOpenLayerBitmap;
            mOpenLayerBitmap = NULL;
        }
        
        if (mAllegroBitmap != NULL)
        {
            destroy_bitmap(mAllegroBitmap);
            mAllegroBitmap = NULL;
        }
    }

    int OpenLayerImage::getWidth() const
    {
        if (mAllegroBitmap == NULL && mOpenLayerBitmap == NULL)
        {
            throw GCN_EXCEPTION("Trying to get the width of a non loaded image.");
        }

        if (mOpenLayerBitmap == NULL)
        {
            return mAllegroBitmap->w;
        }

        return mOpenLayerBitmap->Width();
    }

    int OpenLayerImage::getHeight() const
    {
        if (mAllegroBitmap == NULL && mOpenLayerBitmap == NULL)
        {
            throw GCN_EXCEPTION("Trying to get the height of a non loaded image.");
        }
        
        if (mOpenLayerBitmap == NULL)
        {
            return mAllegroBitmap->h;
        }

        return mOpenLayerBitmap->Height();
    }

    Color OpenLayerImage::getPixel(int x, int y)
    {
        if (mAllegroBitmap == NULL && mOpenLayerBitmap == NULL)
        {
            throw GCN_EXCEPTION("Trying to get a pixel from a non loaded image.");
        }

        if (mOpenLayerBitmap == NULL)
        {
            int color = getpixel(mAllegroBitmap, x, y);
            return Color(getr(color), getg(color), getb(color), 255);
        }
        
        return Color(mOpenLayerBitmap->GetPixelPacked(x, y));

    }

    void OpenLayerImage::putPixel(int x, int y, const Color& color)
    {
        if (mAllegroBitmap == NULL && mOpenLayerBitmap == NULL)
        {
            throw GCN_EXCEPTION("Trying to put a pixel in a non loaded image.");
        }

        if (mAllegroBitmap == NULL)
        {
            return;
        }

        int c = makeacol_depth(32, color.r, color.g, color.b, color.a);

        putpixel(mAllegroBitmap, x, y, c);
    }

    void OpenLayerImage::convertToDisplayFormat()
    {
        if (mAllegroBitmap == NULL)
        {
            throw GCN_EXCEPTION("Trying to convert a non loaded image to display format.");
        }

        if (mOpenLayerBitmap != NULL)
        {
            throw GCN_EXCEPTION("Trying to convert an image to display format which has already been converted.");
        }

        mOpenLayerBitmap = new ol::Bitmap(mAllegroBitmap, false, true);
        mOpenLayerBitmap->SendToGPU();
        
        destroy_bitmap(mAllegroBitmap);
        mAllegroBitmap = NULL;
    }
}
