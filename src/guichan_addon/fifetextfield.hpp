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

#ifndef GCN_FIFETEXTFIELD_HPP
#define GCN_FIFETEXTFIELD_HPP

#include <string>

#include "guichan/widgets/textfield.hpp"
#include "graphix_primitives.h"

namespace gcn
{
    class DECLSPEC FIFETextField : public TextField
    {
    public:
        FIFETextField();

        FIFETextField(const std::string& text);

        virtual void setText(const std::string& text);

        virtual const std::string& getText() const;
    
        virtual void drawCaret(Graphics* graphics, int x);    

        virtual void adjustSize();

        virtual void adjustHeight();

        virtual void setCaretPosition(unsigned int position);

        virtual unsigned int getCaretPosition() const;

        virtual void fontChanged();
        
        virtual void draw(Graphics* graphics);

        virtual void drawBorder(Graphics* graphics);

				virtual void mousePress(int x, int y, int button);

        virtual void keyPress(const Key& key);   
        
    protected:

        void fixScroll();
    
        std::string mText;
				FIFE::Image mTextTexture;
        unsigned int mCaretPosition;
        int mXScroll;        
    };  
}

#endif // end GCN_FIFETEXTFIELD_HPP
