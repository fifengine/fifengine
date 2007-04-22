/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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

#include "guichan/keyinput.hpp"
#include "guichan/mouseinput.hpp"
#include "fifetextfield.hpp"
#include "sdltruetypefont.hpp"
#include "sdlfifegraphics.hpp"
#ifdef HAVE_OPENGL
  #include "openglfifegraphics.hpp"
#endif
#include "log.h"

namespace gcn
{
    FIFETextField::FIFETextField()
    {
        mCaretPosition = 0;
        mXScroll = 0;

        setFocusable(true);

        //addMouseListener(this);
        //addKeyListener(this);
        adjustHeight();
        setBorderSize(1); 
    }
  
    FIFETextField::FIFETextField(const std::string& text)
    {
        mCaretPosition = 0;
        mXScroll = 0;
    
        mText = text;
        adjustSize();
        setBorderSize(1);
        
        setFocusable(true);    
  
        //addMouseListener(this);
        //addKeyListener(this);  
				
				setText(text);
    }

    void FIFETextField::setText(const std::string& text)
    {
        if(text.size() < mCaretPosition )
        {
            mCaretPosition = text.size();
        }
    
        mText = text;
				SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());
				mTextTexture.setSurface(font->renderString(mText));    
    }
  
    void FIFETextField::draw(Graphics* graphics)
    {
        Color faceColor = getBackgroundColor();
        graphics->setColor(faceColor);
        graphics->fillRectangle(Rectangle(0, 0, getWidth(), getHeight()));
    
        if (hasFocus())
        {      
            drawCaret(graphics, getFont()->getWidth(mText.substr(0, mCaretPosition)) - mXScroll);
        }

				SDLFIFEGraphics *sdlGraphics;
#ifdef HAVE_OPENGL
				OpenGLFIFEGraphics *glGraphics;
#endif
				bool using_gl = true;
					
				sdlGraphics = dynamic_cast<SDLFIFEGraphics *>(graphics);
#ifdef HAVE_OPENGL
				glGraphics = dynamic_cast<OpenGLFIFEGraphics *>(graphics);
#endif
				if (sdlGraphics) using_gl = false;
    
				SDL_Surface* s = mTextTexture.getSurface();
				if (!s) {
					return;
				}
				
				int xpos = 1 + mXScroll;
				int ypos = 1;
				int width = mTextTexture.getSurface()->w + 2;
				int height = mTextTexture.getSurface()->h + 2;
					
				if (!using_gl) {
					sdlGraphics->drawFIFEImage(&mTextTexture, xpos, ypos, width, height, getForegroundColor());       
#ifdef HAVE_OPENGL
				} else {
					glGraphics->drawFIFEImage(&mTextTexture, xpos, ypos, width, height, getForegroundColor());
#endif
				}
    }

    void FIFETextField::drawBorder(Graphics* graphics)
    {
        Color faceColor = getBaseColor();
        Color highlightColor, shadowColor;
        int alpha = getBaseColor().a;
        int width = getWidth() + getBorderSize() * 2 - 1;
        int height = getHeight() + getBorderSize() * 2 - 1;
        highlightColor = faceColor + 0x303030;
        highlightColor.a = alpha;
        shadowColor = faceColor - 0x303030;
        shadowColor.a = alpha;
        
        unsigned int i;
        for (i = 0; i < getBorderSize(); ++i)
        {
            graphics->setColor(shadowColor);
            graphics->drawLine(i,i, width - i, i);
            graphics->drawLine(i,i + 1, i, height - i - 1);
            graphics->setColor(highlightColor);
            graphics->drawLine(width - i,i + 1, width - i, height - i); 
            graphics->drawLine(i,height - i, width - i - 1, height - i);
        }
    }
    
    void FIFETextField::drawCaret(Graphics* graphics, int x)
    {
        graphics->setColor(getForegroundColor());
        graphics->drawLine(x, getHeight() - 2, x, 1);    
    }
  
    void FIFETextField::mousePress(int x, int y, int button)
    {
        if (hasMouse() && button == MouseInput::LEFT)
        {
            mCaretPosition = getFont()->getStringIndexAt(mText, x + mXScroll);
            fixScroll();
        }      
    }
  
    void FIFETextField::keyPress(const Key& key)
    {
			SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());
        if (key.getValue() == Key::LEFT && mCaretPosition > 0)
        {
            --mCaretPosition;
        }

        else if (key.getValue() == Key::RIGHT && mCaretPosition < mText.size())
        {
            ++mCaretPosition;
        }

        else if (key.getValue() == Key::DELETE && mCaretPosition < mText.size())
        {
            mText.erase(mCaretPosition, 1);
						mTextTexture.setSurface(font->renderString(mText));    
        }

        else if (key.getValue() == Key::BACKSPACE && mCaretPosition > 0)
        {
            mText.erase(mCaretPosition - 1, 1);
            --mCaretPosition;
						mTextTexture.setSurface(font->renderString(mText));    
        }

        else if (key.getValue() == Key::ENTER)
        {
            generateAction();
        }

        else if (key.getValue() == Key::HOME)
        {
            mCaretPosition = 0;
        }    

        else if (key.getValue() == Key::END)
        {
            mCaretPosition = mText.size();
        }    

        else if (key.isCharacter())
        {
            mText.insert(mCaretPosition, std::string(1,static_cast<char>(key.getValue())));
            ++mCaretPosition;
						mTextTexture.setSurface(font->renderString(mText));    
        }

        fixScroll();    
    }

    void FIFETextField::adjustSize()
    {
        setWidth(getFont()->getWidth(mText) + 4);
        adjustHeight();

        fixScroll();    
    }
  
    void FIFETextField::adjustHeight()
    {
        setHeight(getFont()->getHeight() + 2);    
    }

    void FIFETextField::fixScroll()
    {
        if (hasFocus())
        {
            int caretX = getFont()->getWidth(mText.substr(0, mCaretPosition));

            if (caretX - mXScroll > getWidth() - 4)
            {
                mXScroll = caretX - getWidth() + 4;
            }
            else if (caretX - mXScroll < getFont()->getWidth(" "))
            {
                mXScroll = caretX - getFont()->getWidth(" ");
        
                if (mXScroll < 0)
                {
                    mXScroll = 0;
                }
            }
        }
    }

    void FIFETextField::setCaretPosition(unsigned int position)
    {
        if (position > mText.size())
        {
            mCaretPosition = mText.size();
        }
        else
        {    
            mCaretPosition = position;
        }

        fixScroll();    
    }

    unsigned int FIFETextField::getCaretPosition() const
    {
        return mCaretPosition;    
    }

    const std::string& FIFETextField::getText() const
    {
        return mText;    
    }
  
    void FIFETextField::fontChanged()
    {
        fixScroll();
				SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());
				mTextTexture.setSurface(font->renderString(mText));
    }
}
