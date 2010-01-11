/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004 - 2008 Olof Naessén and Per Larsson
 *
 *
 * Per Larsson a.k.a finalman
 * Olof Naessén a.k.a jansem/yakslem
 *
 * Visit: http://guichan.sourceforge.net
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Guichan nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * For comments regarding functions please see the header file.
 */

#include "guichan/widgets/listbox.hpp"

#include "guichan/basiccontainer.hpp"
#include "guichan/font.hpp"
#include "guichan/graphics.hpp"
#include "guichan/key.hpp"
#include "guichan/listmodel.hpp"
#include "guichan/mouseinput.hpp"
#include "guichan/selectionlistener.hpp"

namespace gcn
{
    ListBox::ListBox()
        : mSelected(-1),
          mListModel(NULL),
          mWrappingEnabled(false)
    {
        setWidth(100);
        setFocusable(true);

        addMouseListener(this);
        addKeyListener(this);
    }

    ListBox::ListBox(ListModel *listModel)
        : mSelected(-1),
          mWrappingEnabled(false)
    {
        setWidth(100);
        setListModel(listModel);
        setFocusable(true);

        addMouseListener(this);
        addKeyListener(this);
    }

    void ListBox::draw(Graphics* graphics)
    {
        graphics->setColor(getBackgroundColor());
        graphics->fillRectangle(Rectangle(0, 0, getWidth(), getHeight()));

        if (mListModel == NULL)
        {
            return;
        }

        graphics->setColor(getForegroundColor());
        graphics->setFont(getFont());
         
        // Check the current clip area so we don't draw unnecessary items
        // that are not visible.
        const ClipRectangle currentClipArea = graphics->getCurrentClipArea();
        int rowHeight = getRowHeight();
        
		// Calculate the number of rows to draw by checking the clip area.
		// The addition of two makes covers a partial visible row at the top
		// and a partial visible row at the bottom.
		int numberOfRows = currentClipArea.height / rowHeight + 2;

        if (numberOfRows > mListModel->getNumberOfElements())
        {
            numberOfRows = mListModel->getNumberOfElements();
        }

		// Calculate which row to start drawing. If the list box 
		// has a negative y coordinate value we should check if
		// we should drop rows in the begining of the list as
		// they might not be visible. A negative y value is very
		// common if the list box for instance resides in a scroll
		// area and the user has scrolled the list box downwards.
		int startRow;    	
		if (getY() < 0)
		{
			startRow = -1 * (getY() / rowHeight);
		}
		else
		{
			startRow = 0;
		}

		int i;
		// The y coordinate where we start to draw the text is
		// simply the y coordinate multiplied with the font height.
		int y = rowHeight * startRow;
        for (i = startRow; i < startRow + numberOfRows; ++i)
        {
            if (i == mSelected)
            {
                graphics->setColor(getSelectionColor());
                graphics->fillRectangle(Rectangle(0, y, getWidth(), rowHeight));
                graphics->setColor(getForegroundColor());
            }
			
			// If the row height is greater than the font height we
			// draw the text with a center vertical alignment.
			if (rowHeight > getFont()->getHeight())
			{
				graphics->drawText(mListModel->getElementAt(i), 1, y + rowHeight / 2 - getFont()->getHeight() / 2);
			}
			else
			{
				graphics->drawText(mListModel->getElementAt(i), 1, y);
			}

            y += rowHeight;
        }
    }

    void ListBox::logic()
    {
        adjustSize();
    }

    int ListBox::getSelected() const
    {
        return mSelected;
    }

    void ListBox::setSelected(int selected)
    {
        if (mListModel == NULL)
        {
            mSelected = -1;
        }
        else
        {
            if (selected < 0)
            {
                mSelected = -1;
            }
            else if (selected >= mListModel->getNumberOfElements())
            {
                mSelected = mListModel->getNumberOfElements() - 1;
            }
            else
            {
                mSelected = selected;
            }
        }
        
        Rectangle scroll;

        if (mSelected < 0)
        {
            scroll.y = 0;
        }
        else
        {
            scroll.y = getRowHeight() * mSelected;
        }

        scroll.height = getRowHeight();
        showPart(scroll);

        distributeValueChangedEvent();
    }

    void ListBox::keyPressed(KeyEvent& keyEvent)
    {
        Key key = keyEvent.getKey();

        if (key.getValue() == Key::ENTER || key.getValue() == Key::SPACE)
        {
            distributeActionEvent();
            keyEvent.consume();
        }
        else if (key.getValue() == Key::UP)
        {
            setSelected(mSelected - 1);

            if (mSelected == -1)
            {
                if (mWrappingEnabled)
                {
                    setSelected(getListModel()->getNumberOfElements() - 1);
                }
                else
                {
                    setSelected(0);
                }
            }
            
            keyEvent.consume();
        }
        else if (key.getValue() == Key::DOWN)
        {
            if (mWrappingEnabled
                && getSelected() == getListModel()->getNumberOfElements() - 1)
            {
                setSelected(0);
            }
            else
            {
                setSelected(getSelected() + 1);
            }
            
            keyEvent.consume();
        }
        else if (key.getValue() == Key::HOME)
        {
            setSelected(0);
            keyEvent.consume();
        }
        else if (key.getValue() == Key::END)
        {
            setSelected(getListModel()->getNumberOfElements() - 1);
            keyEvent.consume();
        }
    }

    void ListBox::mousePressed(MouseEvent& mouseEvent)
    {
        if (mouseEvent.getButton() == MouseEvent::LEFT)
        {
            setSelected(mouseEvent.getY() / getRowHeight());
            distributeActionEvent();
        }
    }

    void ListBox::mouseWheelMovedUp(MouseEvent& mouseEvent)
    {
        if (isFocused())
        {
            if (getSelected() > 0 )
            {
                setSelected(getSelected() - 1);
            }

            mouseEvent.consume();
        }
    }

    void ListBox::mouseWheelMovedDown(MouseEvent& mouseEvent)
    {
        if (isFocused())
        {
            setSelected(getSelected() + 1);

            mouseEvent.consume();
        }
    }

    void ListBox::mouseDragged(MouseEvent& mouseEvent)
    {
        mouseEvent.consume();
    }

    void ListBox::setListModel(ListModel *listModel)
    {
        mSelected = -1;
        mListModel = listModel;
        adjustSize();
    }

    ListModel* ListBox::getListModel()
    {
        return mListModel;
    }

    void ListBox::adjustSize()
    {
        if (mListModel != NULL)
        {
            setHeight(getRowHeight() * mListModel->getNumberOfElements());
        }
    }

    bool ListBox::isWrappingEnabled() const
    {
        return mWrappingEnabled;
    }

    void ListBox::setWrappingEnabled(bool wrappingEnabled)
    {
        mWrappingEnabled = wrappingEnabled;
    }
        
    void ListBox::addSelectionListener(SelectionListener* selectionListener)
    {
        mSelectionListeners.push_back(selectionListener);
    }
   
    void ListBox::removeSelectionListener(SelectionListener* selectionListener)
    {
        mSelectionListeners.remove(selectionListener);
    }

    void ListBox::distributeValueChangedEvent()
    {
        SelectionListenerIterator iter;

        for (iter = mSelectionListeners.begin(); iter != mSelectionListeners.end(); ++iter)
        {
            SelectionEvent event(this);
            (*iter)->valueChanged(event);
        }
    }

	unsigned int ListBox::getRowHeight() const
	{
		return getFont()->getHeight();
	}
}
