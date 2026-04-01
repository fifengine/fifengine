// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

/*
 * For comments regarding functions please see the header file.
 */

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/graphics.hpp>
#include <fifechan/rectangle.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "percentagebar.h"

namespace fcn
{

    PercentageBar::PercentageBar() : mImage(nullptr), mValue(0), mOrientation(HORIZONTAL)
    {
        setOrientation(HORIZONTAL);
        setValue(0);
    }

    void PercentageBar::draw(Graphics* graphics)
    {
        graphics->setColor(getForegroundColor());

        if (getOrientation() == HORIZONTAL) {
            graphics->fillRectangle(fcn::Rectangle(0, 0, (getWidth() * mValue) / 100, getHeight()));
        } else {
            const int32_t filledHeight = (getHeight() * mValue) / 100;
            graphics->fillRectangle(fcn::Rectangle(0, getHeight() - filledHeight, getWidth(), filledHeight));
        }

        if (mImage != nullptr) {
            graphics->drawImage(mImage, 0, 0);
        }
    }

    void PercentageBar::setForegroundImage(Image* image)
    {
        mImage = image;
        if (mImage != nullptr) {
            setHeight(image->getHeight());
            setWidth(image->getWidth());
        }
    }

    void PercentageBar::setValue(int32_t value)
    {
        if (value > 100) {
            mValue = 100;
            return;
        }

        if (value < 0) {
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
} // namespace fcn
