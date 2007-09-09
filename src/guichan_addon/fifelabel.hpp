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

#ifndef GCN_FIFELABEL_HPP
#define GCN_FIFELABEL_HPP

#include <string>

#include "graphix_primitives.h"

#include "guichan/graphics.hpp"
#include "guichan/platform.hpp"
#include "guichan/widget.hpp"
#include "guichan/widgets/label.hpp"

namespace gcn
{
    class DECLSPEC FIFELabel: public Label
    {
    public:
        FIFELabel();

        FIFELabel(const std::string& caption);

        virtual const std::string &getCaption() const;

        virtual void setCaption(const std::string& caption);

        virtual void setAlignment(unsigned int alignment);

        virtual unsigned int getAlignment();
        
        virtual void adjustSize();
        
        virtual void draw(Graphics* graphics);

        virtual void drawBorder(Graphics* graphics);
        
    private:
        std::string mCaption;
				FIFE::Image mImage;
        unsigned int mAlignment;        
    };  
}

#endif // end GCN_LABEL_HPP
