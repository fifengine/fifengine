// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FCN_PERCENTAGE_BAR_HPP
#define FCN_PERCENTAGE_BAR_HPP

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes
#include <fifechan/image.hpp>
#include <fifechan/platform.hpp>
#include <fifechan/widget.hpp>

// FIFE includes
#include "util/base/fife_stdint.h"

namespace fcn
{
    /**
     * A simple percentage bar.
     *
     *
     */
    class FIFE_API PercentageBar : public Widget
    {
        public:
            enum Orientation : uint8_t
            {
                HORIZONTAL = 0,
                VERTICAL
            };

            /**
             * Constructor.
             *
             */
            PercentageBar();
            virtual ~PercentageBar() = default;

            PercentageBar(PercentageBar const &)            = delete;
            PercentageBar& operator=(PercentageBar const &) = delete;

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
            int32_t mValue;

            /**
             * Holds the orientation of the percentage bar. A percentage bar can be drawn
             * vertically or horizontally.
             */
            Orientation mOrientation;
    };
} // namespace fcn

#endif // end FCN_PERCENTAGE_BAR_HPP
