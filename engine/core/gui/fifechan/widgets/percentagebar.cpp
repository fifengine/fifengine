/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

/*
 * For comments regarding functions please see the header file.
 */

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/rectangle.hpp>
#include <fifechan/graphics.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "percentagebar.h"

namespace fcn
{

    PercentageBar::PercentageBar()
    {
		mImage = 0;

		setOrientation(HORIZONTAL);
        setValue(0);
    }
    
    void PercentageBar::draw(Graphics* graphics)
    {
		graphics->setColor(getForegroundColor());

        if (getOrientation() == HORIZONTAL)
        {
			graphics->fillRectangle(fcn::Rectangle(0,0,getWidth() * mValue/100,getHeight()));
		}
		else
		{
			graphics->fillRectangle(fcn::Rectangle(0,getHeight()-getHeight() * mValue/100,getWidth(),getHeight() * mValue/100));
		}


        if ( mImage )
            graphics->drawImage(mImage, 0, 0);

    }

    void PercentageBar::setForegroundImage(Image* image)
    {
        mImage = image;
        if( mImage ) {
            setHeight(image->getHeight());
            setWidth(image->getWidth());
        }
    }

    void PercentageBar::setValue(int32_t value)
    {
        if (value > 100)
        {
            mValue = 100;
            return;
        }

        if (value < 0)
        {
            mValue = 0;
            return;
        }

        mValue = value;
    }

   int32_t PercentageBar::getValue() const
    {
        return mValue;
    }

    void PercentageBar::setOrientation(PercentageBar::Orientation orientation)
	{
	    mOrientation = orientation;
	}

	PercentageBar::Orientation PercentageBar::getOrientation() const
	{
	    return mOrientation;
    }
}
