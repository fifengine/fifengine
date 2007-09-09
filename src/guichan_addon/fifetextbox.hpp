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

#ifndef GCN_FIFETEXTBOX_HPP
#define GCN_FIFETEXTBOX_HPP

#include <string>
#include <vector>

#include "guichan/widgets/textbox.hpp"
#include "graphix_primitives.h"

namespace gcn
{
	class DECLSPEC FIFETextBox : public TextBox
	{
		public:
			FIFETextBox();
	
			FIFETextBox(const std::string& text);

      ~FIFETextBox();

			virtual void setText(const std::string& text);

			virtual std::string getText() const;

			virtual const std::string& getTextRow(int row) const;

			virtual void setTextRow(int row, const std::string& text);

			virtual unsigned int getNumberOfRows() const;   

			virtual unsigned int getCaretPosition() const;

			virtual void setCaretPosition(unsigned int position);

			virtual unsigned int getCaretRow() const;

			virtual void setCaretRow(int row);
                
			virtual unsigned int getCaretColumn() const;

			virtual void setCaretColumn(int column);
        
			virtual void setCaretRowColumn(int row, int column);

			virtual void scrollToCaret();

			virtual bool isEditable() const;
        
			virtual void setEditable(bool editable);

			virtual void addRow(const std::string row);

			virtual bool isOpaque();

			virtual void setOpaque(bool opaque);

			virtual void draw(Graphics* graphics);
        
			virtual void drawBorder(Graphics* graphics);
	
			virtual void fontChanged();
        
			virtual void keyPress(const Key& key);

      virtual void mousePress(int x, int y, int button);
        
    protected:
        virtual void drawCaret(Graphics* graphics, int x, int y);
        
        virtual void adjustSize();
    
        std::vector<std::string> mTextRows;
        std::vector<FIFE::Image*> mTextRowTextures;
        int mCaretColumn;
        int mCaretRow;
        bool mEditable;
        bool mOpaque;
    };  
}

#endif // end GCN_TEXTBOX_HPP
