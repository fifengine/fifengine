/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2007 Josh Matthews
 *
 *                                                         Js_./
 *                                                     _RqZ{a<^_aa
 *                                                  _asww7!uY`>  )\a//
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

#ifndef ADJUSTINGCONTAINER_HPP
#define ADJUSTINGCONTAINER_HPP

#include <vector>

namespace gcn
{
    /**
     * Self-adjusting Container class.  Add widgets to it, set the number of
     * columns, padding, alignment and spacing, and it'll arrange all of the
     * elements.
     *
     * @author Josh Matthews
     */
    class AdjustingContainer : public gcn::Container
    {
    public:
        /**
         * Constructor.
         */
        AdjustingContainer();
        
        /**
         * Destructor.
         */
        virtual ~AdjustingContainer();
        
        /**
         * Set the number of columns to divide the widgets into.
         * The number of rows is derived automatically from the number
         * of widgets based on the number of columns.  Default column
         * alignment is left.
         *
         * @param numberOfColumns the number of columns.
         */
        virtual void setNumberOfColumns(unsigned int numberOfColumns);
        
        /**
         * Set a specific column's alignment.
         *
         * @param column the column number, starting from 0.
         * @param alignment the column's alignment. See enum with alignments.
         */
        virtual void setColumnAlignment(unsigned int column, unsigned int alignment);
        
        /**
         * Set the padding for the sides of the container.
         *
         * @param paddingLeft left padding.
         * @param paddingRight right padding.
         * @param paddingTop top padding.
         * @param paddingBottom bottom padding.
         */
        virtual void setPadding(unsigned int paddingLeft,
                                unsigned int paddingRight,
                                unsigned int paddingTop,
                                unsigned int paddingBottom);
        
        /**
         * Set the spacing between rows.
         *
         * @param verticalSpacing spacing in pixels.
         */
        virtual void setVerticalSpacing(unsigned int verticalSpacing);
        
        /**
         * Set the horizontal spacing between columns.
         *
         * @param horizontalSpacing spacing in pixels.
         */
        virtual void setHorizontalSpacing(unsigned int horizontalSpacing);
        
        /**
         * Rearrange the widgets and resize the container.
         */
        virtual void adjustContent();


        // Inherited from Container

        virtual void logic();
        
        virtual void add(Widget *widget);

        virtual void add(Widget *widget, int x, int y);

        virtual void remove(Widget *widget);

        virtual void clear();
               
        /**
         * Possible alignment values for each column.
         *
         * LEFT   - Align content to the left of the column.
         * MIDDLE - Align content to the middle of the column.
         * RIGHT  - Align content to the right of the column.
         */
        enum
        {
            LEFT = 0,
            CENTER,
            RIGHT
        };

    protected:
        
        /**
         * Adjust the size of the container to fit all the widgets.
         */
        virtual void adjustSize();

        std::vector<Widget*> mContainedWidgets;
        std::vector<unsigned int> mColumnWidths;
        std::vector<unsigned int> mColumnAlignment;
        std::vector<unsigned int> mRowHeights;
        unsigned int mWidth;
        unsigned int mHeight;
        unsigned int mNumberOfColumns;
        unsigned int mNumberOfRows;
        unsigned int mPaddingLeft;
        unsigned int mPaddingRight;
        unsigned int mPaddingTop;
        unsigned int mPaddingBottom;
        unsigned int mVerticalSpacing;
        unsigned int mHorizontalSpacing;
    };
}

#endif // ADJUSTINGCONTAINER_HPP
