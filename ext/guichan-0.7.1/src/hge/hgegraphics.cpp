/*     _______   __   __   __   ______   __   __   _______   __   __
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

#include "guichan/hge/hgegraphics.hpp"

#include "guichan/cliprectangle.hpp"
#include "guichan/exception.hpp"
#include "guichan/rectangle.hpp"
#include "guichan/hge/hgeimage.hpp"

namespace gcn
{
    HGE *HGEGraphics::mHGE = NULL;

    HGEGraphics::HGEGraphics()
    {
        mHGE = hgeCreate(HGE_VERSION);

        mHardwareColor = 0;
    }

    HGEGraphics::~HGEGraphics()
    {
        mHGE->Release();
    }

    bool HGEGraphics::pushClipArea(Rectangle area)
    {
        bool result = Graphics::pushClipArea(area);

        ClipRectangle carea = mClipStack.top();

        // To avoid the odd thing. When the width or the height of the clip area is zero,
        // Gfx_SetClipping will fail and set the clip area as the fullscreen
        // (or the full window).
        if (carea.width == 0)
        {
            carea.width++;
        }
        if (carea.height == 0)
        {
            carea.height++;
        }

        mHGE->Gfx_SetClipping(carea.x - 1, 
                              carea.y - 1, 
                              carea.width, 
                              carea.height);
        return result;
    }

    void HGEGraphics::popClipArea()
    {
        Graphics::popClipArea();

        if (mClipStack.empty())
        {
            mHGE->Gfx_SetClipping();

            return;
        }

        ClipRectangle carea = mClipStack.top();

        if (carea.width == 0)
        {
            carea.width++;
        }
        if (carea.height == 0)
        {
            carea.height++;
        }

        mHGE->Gfx_SetClipping(carea.x - 1, 
                              carea.y - 1, 
                              carea.width, 
                              carea.height);
    }

    void HGEGraphics::drawImage(const Image *image, 
                                int srcX, 
                                int srcY, 
                                int dstX, 
                                int dstY, 
                                int width, 
                                int height)
    {
        const HGEImage *hgeImage = static_cast<const HGEImage*>(image);

        if (hgeImage == NULL)
        {
            throw GCN_EXCEPTION("Trying to draw an image of unknown format, must be an HGEImage.");
        }

        ClipRectangle const top = mClipStack.top();

        dstX += top.xOffset;
        dstY += top.yOffset;

        hgeImage->getSprite()->SetTextureRect(srcX, srcY, width, height);
        hgeImage->getSprite()->Render(dstX, dstY);
    }

    void HGEGraphics::drawImage(const Image *image, int dstX, int dstY)
    {
        drawImage(image, 0, 0, dstX, dstY, image->getWidth(), image->getHeight());
    }

    void HGEGraphics::drawPoint(int x, int y)
    {
        ClipRectangle const top = mClipStack.top();

        x += top.xOffset;
        y += top.yOffset;

        mHGE->Gfx_RenderLine(x, y, x + 1, y, mHardwareColor);
    }

    void HGEGraphics::drawLine(int x1, int y1, int x2, int y2)
    {
        // To avoid some strange things.
        // HGE::Gfx_RenderLine will omit the last point 
        // of the line, so we should adjust the coordinates here.
        if (y1 == y2)
        {
            if (x1 < x2)
            {
                x1--;
            }
            else
            {
                x2--;
            }
        }
        else
        if (x1 == x2)
        {
            if (y1 < y2)
            {
                y1--;
            }
            else
            {
                y2--;
            }
        }
        else
        if (x1 != x2 && y1 != y2)
        {
            if (x1 < x2)
            {
                x1--;
            }
            else
            {
                x2--;
            }
            if (y1 < y2)
            {
                y1--;
            }
            else
            {
                y2--;
            }
         } 

        ClipRectangle const top = mClipStack.top();

        x1 += top.xOffset;
        y1 += top.yOffset;
        x2 += top.xOffset;
        y2 += top.yOffset;

        mHGE->Gfx_RenderLine(x1, y1, x2, y2, mHardwareColor);
    }

    void HGEGraphics::drawRectangle(const Rectangle &rectangle)
    {
        int x1 = rectangle.x;
        int y1 = rectangle.y;
        int x2 = rectangle.x + rectangle.width - 1 ;
        int y2 = rectangle.y + rectangle.height - 1;

        ClipRectangle const top = mClipStack.top();

        x1 += top.xOffset;
        y1 += top.yOffset;
        x2 += top.xOffset;
        y2 += top.yOffset;
     
        mHGE->Gfx_RenderLine(x1, y1, x2, y1, mHardwareColor);
        mHGE->Gfx_RenderLine(x2, y1, x2, y2, mHardwareColor);
        mHGE->Gfx_RenderLine(x2, y2, x1, y2, mHardwareColor);
        mHGE->Gfx_RenderLine(x1, y2, x1, y1 - 1, mHardwareColor);
    }

    void HGEGraphics::fillRectangle(const Rectangle &rectangle)
    {
        int x1 = rectangle.x - 1;
        int y1 = rectangle.y - 1;
        int x2 = rectangle.x + rectangle.width - 1;
        int y2 = rectangle.y + rectangle.height - 1;

        ClipRectangle const top = mClipStack.top();

        x1 += top.xOffset;
        y1 += top.yOffset;
        x2 += top.xOffset;
        y2 += top.yOffset;

        hgeQuad quad;

        quad.tex = NULL;

        quad.v[0].x   = x1;
        quad.v[0].y   = y1;
        quad.v[0].col = mHardwareColor;

        quad.v[1].x   = x2;
        quad.v[1].y   = y1;
        quad.v[1].col = mHardwareColor;

        quad.v[2].x   = x2;
        quad.v[2].y   = y2;
        quad.v[2].col = mHardwareColor;

        quad.v[3].x   = x1;
        quad.v[3].y   = y2;
        quad.v[3].col = mHardwareColor;

        int i;
        for (i = 0; i < 4; ++i)
        {
            quad.v[i].z = 0.5f;
        }

        quad.blend = BLEND_DEFAULT;

        mHGE->Gfx_RenderQuad(&quad);

    }

    void HGEGraphics::drawText(const std::string &text, int x, int y, unsigned int alignment)
    {
        ClipRectangle const top = mClipStack.top();

        x += top.xOffset;
        y += top.yOffset;
       
        Graphics::drawText(text, x, y, alignment);
    }

    void HGEGraphics::setColor(const Color &color)
    {
        mColor = color;

        mHardwareColor = ARGB(color.a, color.r, color.g, color.b);  
    }

    const Color& HGEGraphics::getColor() const
    {
        return  mColor;
    }
}
