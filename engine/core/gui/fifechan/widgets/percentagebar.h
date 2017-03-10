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

#ifndef FCN_PERCENTAGE_BAR_HPP
#define FCN_PERCENTAGE_BAR_HPP

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/image.hpp>
#include <fifechan/platform.hpp>
#include <fifechan/widget.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

namespace fcn
{
	/**
	* A simple percentage bar.
	*
	*
	*/
	class PercentageBar : public Widget
	{
	public:

		enum Orientation
		{
			HORIZONTAL = 0,
			VERTICAL
        };

		/**
		* Constructor.
		*
		*/
		PercentageBar();
		virtual ~PercentageBar() {}

		// Inherited from Widget

		virtual void draw(Graphics* graphics);

		/*
		 * Sets the image that will be displayed above the widget
		 *
		 * @param image Image to be displayed
		 */
		void setForegroundImage(Image* image);

        /**
         * Sets the orientation of the percentage bar.
         *
         * @param orientation The orientation of the percentage bar.
         * @see getOrientation
         */
        void setOrientation(Orientation orientation);

        /**
         * Gets the orientation of the percentage bar.
         *
         * @return The orientation of the percentage bar.
         * @see setOrientation
         */
        Orientation getOrientation() const;

        /**
         * Gets the value of the percentage bar
         *
         * @return The value of the percentage bar.
         */
        int32_t getValue() const;

        /**
         * Sets the value of the percentage bar
         *
         * @param value In the range of 0-100.
         */
        void setValue(int32_t value);

		// changed from private to allow derived instance access
	protected:
		Image* mImage;

        /**
         * Holds the current value of the percentage bar.
         */
        double mValue;

        /**
         * Holds the orientation of the percentage bar. A percentage bar can be drawn
         * vertically or horizontally.
         */
        Orientation mOrientation;
	};
}

#endif // end FCN_PERCENTAGE_BAR_HPP
