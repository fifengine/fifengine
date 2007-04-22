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

#include "guichan/basiccontainer.hpp"
#include "guichan/keyinput.hpp"
#include "guichan/mouseinput.hpp"
#include "guichan/widgets/scrollarea.hpp"
#include "fifetextbox.hpp"
#include "sdltruetypefont.hpp"
#include "sdlfifegraphics.hpp"
#ifdef HAVE_OPENGL
  #include "openglfifegraphics.hpp"
#endif
#include "log.h"

#include <SDL.h>

#include <typeinfo>


namespace gcn
{
	FIFETextBox::FIFETextBox()
	{
		mCaretColumn = 0;
		mCaretRow = 0;
		mEditable = true;
		mOpaque = true;
		
		setFocusable(true);
	
		//addMouseListener(this);
		//addKeyListener(this);
		adjustSize();
		setBorderSize(1);    
	}
	
	FIFETextBox::FIFETextBox(const std::string& text)
	{
		mCaretColumn = 0;
		mCaretRow = 0;
		mEditable = true;
		mOpaque = true;
		
		setText(text);
	
		setFocusable(true);    
	
		//addMouseListener(this);
		//addKeyListener(this);
		adjustSize();
		setBorderSize(1);    
	}
	
	FIFETextBox::~FIFETextBox() {
		std::vector<FIFE::Image*>::iterator i = mTextRowTextures.begin();
		for(;i != mTextRowTextures.end(); ++i)
			delete *i;
		mTextRowTextures.clear();
	}
	
	void FIFETextBox::setText(const std::string& text)
	{
		mCaretColumn = 0;
		mCaretRow = 0;
	
		mTextRows.clear();
	
		std::vector<FIFE::Image*>::iterator i = mTextRowTextures.begin();
		for(;i != mTextRowTextures.end(); ++i)
			delete *i;

		mTextRowTextures.clear();
					
		std::string::size_type pos, lastPos = 0;
		int length;
		do
		{
			pos = text.find("\n", lastPos);
		
			if (pos != std::string::npos) {
				length = pos - lastPos;
			} else 	{
				length = text.size() - lastPos;
			}
			std::string sub = text.substr(lastPos, length);
			mTextRows.push_back(sub);

			SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());

			SDL_Surface* fs = font->renderString(sub);
			
			FIFE::Image *img = new FIFE::Image();
			img->setSurface(fs);
			
			mTextRowTextures.push_back(img);
			
			lastPos = pos + 1;
		
		} while (pos != std::string::npos);
		adjustSize();
	}
	
	void FIFETextBox::draw(Graphics* graphics)
	{
		unsigned int i;
	
		if (mOpaque) {            
			graphics->setColor(getBackgroundColor());    
			graphics->fillRectangle(Rectangle(0, 0, getWidth(), getHeight()));
		}
		
		if (hasFocus() && isEditable())  {      
			drawCaret(graphics, getFont()->getWidth(mTextRows[mCaretRow].substr(0, mCaretColumn)),
					mCaretRow * getFont()->getHeight());
		}
	
		//graphics->setColor(getForegroundColor());
		//graphics->setFont(getFont());
	
		for (i = 0; i < mTextRowTextures.size(); i++)
		{
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
			
			FIFE::Image* img = mTextRowTextures[i];
			if (!img) {
				FIFE::Log("FIFETextBox::draw():") << "FIFE::Image missing in &(mTextRowTextures[i])";
				continue;
			}
			
			SDL_Surface *s = img->getSurface();
			if (!s) {
				continue;
			}
			
			int xpos = 1;
			int ypos = i * getFont()->getHeight();
			int width = s->w;
			int height = getFont()->getHeight();
			
			if (!using_gl) {
				sdlGraphics->drawFIFEImage(img, xpos, ypos, width, height, getForegroundColor());       
#ifdef HAVE_OPENGL
			} else {
				glGraphics->drawFIFEImage(img, xpos, ypos, width, height, getForegroundColor());
#endif
			}
		}    
	}
	
	void FIFETextBox::drawBorder(Graphics* graphics) {
		int width = getWidth() + getBorderSize() * 2 - 1;
		int height = getHeight() + getBorderSize() * 2 - 1;
	
		graphics->setColor(getBackgroundColor());
	
		unsigned int i;
		for (i = 0; i < getBorderSize(); ++i) {
			graphics->drawLine(i,i, width - i, i);
			graphics->drawLine(i,i + 1, i, height - i - 1);
			graphics->drawLine(width - i,i + 1, width - i, height - i); 
			graphics->drawLine(i,height - i, width - i - 1, height - i);
		}
	}
	
	void FIFETextBox::drawCaret(Graphics* graphics, int x, int y) {
		graphics->setColor(getForegroundColor());
		graphics->drawLine(x, getFont()->getHeight() + y, x, y);
	}
	
	void FIFETextBox::mousePress(int x, int y, int button) {       
		if (hasMouse() && button == MouseInput::LEFT) {
			mCaretRow = y / getFont()->getHeight();
			if (mCaretRow >= static_cast<int>(mTextRows.size()))
				mCaretRow = mTextRows.size() - 1;
			mCaretColumn = getFont()->getStringIndexAt(mTextRows[mCaretRow], x);
		}
	}
	
	void FIFETextBox::keyPress(const Key& key) {    
		// FIXME: This is a MESS!
		SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());

		if (key.getValue() == Key::LEFT) {
			--mCaretColumn;
			if (mCaretColumn < 0) {
				--mCaretRow;
		
				if (mCaretRow < 0) {
					mCaretRow = 0;
					mCaretColumn = 0;
				} else {
					mCaretColumn = mTextRows[mCaretRow].size();
				}
			}
		} else if (key.getValue() == Key::RIGHT)  {
			++mCaretColumn;
			if (mCaretColumn > static_cast<int>(mTextRows[mCaretRow].size()))
			{
				++mCaretRow;
		
				if (mCaretRow >= static_cast<int>( mTextRows.size())) {
					mCaretRow = mTextRows.size() - 1;
					if (mCaretRow < 0)
						mCaretRow = 0;
					mCaretColumn = mTextRows[mCaretRow].size();
				} else 	{
					mCaretColumn = 0;
				}
			}
		} else if (key.getValue() == Key::DOWN) {
			setCaretRow(mCaretRow + 1);
		} else if (key.getValue() == Key::UP) {
			setCaretRow(mCaretRow - 1);
		} else if (key.getValue() == Key::HOME) {
			mCaretColumn = 0;
		} else if (key.getValue() == Key::END) {
			mCaretColumn = mTextRows[mCaretRow].size();
		} else if (key.getValue() == Key::ENTER && mEditable) {
			mTextRows.insert(mTextRows.begin() + mCaretRow + 1,
			mTextRows[mCaretRow].substr(mCaretColumn, mTextRows[mCaretRow].size() - mCaretColumn));
								
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow].substr(
					mCaretColumn, mTextRows[mCaretRow].size() - mCaretColumn));
			FIFE::Image *img = new FIFE::Image();
			img->setSurface(fs);
			mTextRowTextures.insert(mTextRowTextures.begin() + mCaretRow + 1, img);
	
			mTextRows[mCaretRow].resize(mCaretColumn);
			mTextRowTextures[mCaretRow]->setSurface(font->renderString(mTextRows[mCaretRow]));
			++mCaretRow;
			mCaretColumn = 0;
		} else if (key.getValue() == Key::BACKSPACE
			&& mCaretColumn != 0
			&& mEditable) {
			mTextRows[mCaretRow].erase(mCaretColumn - 1, 1);
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow]);
			mTextRowTextures[mCaretRow]->setSurface(fs);
			--mCaretColumn;
		} else if (key.getValue() == Key::BACKSPACE 
			&& mCaretColumn == 0
			&& mCaretRow != 0
			&& mEditable) {
			mCaretColumn = mTextRows[mCaretRow - 1].size();
			mTextRows[mCaretRow - 1] += mTextRows[mCaretRow];
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow - 1]);
			mTextRowTextures[mCaretRow - 1]->setSurface(fs);
			mTextRows.erase(mTextRows.begin() + mCaretRow);
			mTextRowTextures.erase(mTextRowTextures.begin() + mCaretRow);
			--mCaretRow;
		} else if (key.getValue() == Key::DELETE
			&& mCaretColumn < static_cast<int>(mTextRows[mCaretRow].size())
			&& mEditable) {
			mTextRows[mCaretRow].erase(mCaretColumn, 1);
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow]);
			mTextRowTextures[mCaretRow]->setSurface(fs);
		} else if (key.getValue() == Key::DELETE
			&& mCaretColumn == static_cast<int>(mTextRows[mCaretRow].size())
			&& mCaretRow < (static_cast<int>(mTextRows.size()) - 1)
			&& mEditable) {
			mTextRows[mCaretRow] += mTextRows[mCaretRow + 1];
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow]);
			mTextRowTextures[mCaretRow]->setSurface(fs);
			mTextRows.erase(mTextRows.begin() + mCaretRow + 1);
			mTextRowTextures.erase(mTextRowTextures.begin() + mCaretRow + 1);
		} else if(key.getValue() == Key::PAGE_UP) {
			int w, h, rowsPerPage;
			getParent()->getDrawSize(w, h, this);
			rowsPerPage = h / getFont()->getHeight();
			mCaretRow -= rowsPerPage;
		
			if (mCaretRow < 0)
				mCaretRow = 0;
		} else if(key.getValue() == Key::PAGE_DOWN) {
			int w, h, rowsPerPage;
			getParent()->getDrawSize(w, h, this);
			rowsPerPage = h / getFont()->getHeight();
			mCaretRow += rowsPerPage;
		
			if (mCaretRow >= static_cast<int>(mTextRows.size()))
			{
				mCaretRow = mTextRows.size() - 1;
			}
		} else if(key.getValue() == Key::TAB
			&& mEditable) {
			mTextRows[mCaretRow].insert(mCaretColumn,std::string("    "));
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow]);
			mTextRowTextures[mCaretRow]->setSurface(fs);
			mCaretColumn += 4;      
		} else if (key.isCharacter()
			&& mEditable) {
			mTextRows[mCaretRow].insert(mCaretColumn,std::string(1,static_cast<char>(key.getValue())));
			SDL_Surface *fs = font->renderString(mTextRows[mCaretRow]);
			mTextRowTextures[mCaretRow]->setSurface(fs);
			++mCaretColumn;
		};
	
		adjustSize();
		scrollToCaret();    
	};
	
	void FIFETextBox::adjustSize()
	{
		unsigned int i;
		int width = 0;
		for (i = 0; i < mTextRows.size(); ++i)
		{
			int w = getFont()->getWidth(mTextRows[i]);
			if (width < w)
			{
				width = w;
			}
		}
	
		setWidth(width + 1);
		setHeight(getFont()->getHeight() * mTextRows.size());
	}
		
	void FIFETextBox::setCaretPosition(unsigned int position)
	{
		int row;
	
		for (row = 0; row < static_cast<int>(mTextRows.size()); row++) {
			if (position <= mTextRows[row].size()) {
				mCaretRow = row;
				mCaretColumn = position;
				return; // we are done
			} else {
				position--;        
			}
		}
	
		// position beyond end of text
		mCaretRow = mTextRows.size() - 1;
		mCaretColumn = mTextRows[mCaretRow].size();    
	}
	
	unsigned int FIFETextBox::getCaretPosition() const
	{
		int pos = 0, row;
		for (row = 0; row < mCaretRow; row++){
			pos += mTextRows[row].size();
		}
		return pos + mCaretColumn;    
	}

	void FIFETextBox::setCaretRowColumn(int row, int column)
	{
		setCaretRow(row);
		setCaretColumn(column);
	}
	
	void FIFETextBox::setCaretRow(int row)
	{    
		mCaretRow = row;
	
		if (mCaretRow >= static_cast<int>(mTextRows.size())) {
			mCaretRow = mTextRows.size() - 1;
		}
	
		if (mCaretRow < 0)
			mCaretRow = 0;
		setCaretColumn(mCaretColumn);    
	}
	
	unsigned int FIFETextBox::getCaretRow() const
	{
		return mCaretRow;    
	}
	
	void FIFETextBox::setCaretColumn(int column)
	{
		mCaretColumn = column;
	
		if (mCaretColumn > static_cast<int>(mTextRows[mCaretRow].size()))
			mCaretColumn = mTextRows[mCaretRow].size();
	
		if (mCaretColumn < 0)
			mCaretColumn = 0;
	}
	
	unsigned int FIFETextBox::getCaretColumn() const
	{
		return mCaretColumn;
	}
	
	const std::string& FIFETextBox::getTextRow(int row) const
	{
		return mTextRows[row];
	}

	void FIFETextBox::setTextRow(int row, const std::string& text)
	{
		SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());
	
		mTextRows[row] = text;
		SDL_Surface *fs = font->renderString(mTextRows[row]);
		mTextRowTextures[row]->setSurface(fs);

		if (mCaretRow == row)
			setCaretColumn(mCaretColumn);

		adjustSize();
	}

	unsigned int FIFETextBox::getNumberOfRows() const
	{
		return mTextRows.size();
	}
	
	std::string FIFETextBox::getText() const
	{
		if (mTextRows.size() == 0)
			return std::string("");
	
		int i;
		std::string text;
	
		for (i = 0; i < static_cast<int>(mTextRows.size() - 1); ++i)
			text = text + mTextRows[i] + "\n";
		text = text + mTextRows[i];
		return text;
	}
	
	void FIFETextBox::fontChanged()
	{
		SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());
			
		adjustSize();
		for (unsigned int i = 0; i < mTextRowTextures.size(); i++) {
			SDL_Surface *fs = font->renderString(mTextRows[i]);
			mTextRowTextures[i]->setSurface(fs);
		}
	}
	
	void FIFETextBox::scrollToCaret()
	{
		Widget *par = getParent();
		if (par == NULL)
			return;
		
		ScrollArea* scrollArea = dynamic_cast<ScrollArea *>(par);
		if (scrollArea != NULL) {
			Rectangle scroll;
			scroll.x = getFont()->getWidth(mTextRows[mCaretRow].substr(0, mCaretColumn));
			scroll.y = getFont()->getHeight() * mCaretRow;
			scroll.width = 6;
			scroll.height = getFont()->getHeight() + 2; // add 2 for some extra space
			scrollArea->scrollToRectangle(scroll);
		}
	}
	
	void FIFETextBox::setEditable(bool editable)
	{
		mEditable = editable;
	}
	
	bool FIFETextBox::isEditable() const
	{
		return mEditable;
	}

	void FIFETextBox::addRow(const std::string row)
	{
		SDLTrueTypeFont *font = dynamic_cast<SDLTrueTypeFont *>(getFont());

		mTextRows.push_back(row);
		SDL_Surface *fs = font->renderString(row);
		FIFE::Image *img = new FIFE::Image();
		img->setSurface(fs);
		mTextRowTextures.push_back(img);
		adjustSize();
	}
	
	bool FIFETextBox::isOpaque()
	{
		return mOpaque;
	}
	
	void FIFETextBox::setOpaque(bool opaque)
	{
		mOpaque = opaque;
	}    
}
