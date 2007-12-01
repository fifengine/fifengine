/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004, 2005, 2006, 2007 Olof Naessén and Per Larsson
 *
 *                                                         Js_./
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

/*
 * For comments regarding functions please see the header file.
 */

#include "guichan/gui.hpp"

#include "guichan/basiccontainer.hpp"
#include "guichan/exception.hpp"
#include "guichan/focushandler.hpp"
#include "guichan/graphics.hpp"
#include "guichan/input.hpp"
#include "guichan/keyinput.hpp"
#include "guichan/keylistener.hpp"
#include "guichan/mouseinput.hpp"
#include "guichan/mouselistener.hpp"
#include "guichan/widget.hpp"

namespace gcn
{
    Gui::Gui()
            :mTop(NULL),
             mGraphics(NULL),
             mInput(NULL),
             mTabbing(true),
             mShiftPressed(false),
             mMetaPressed(false),
             mControlPressed(false),
             mAltPressed(false),
             mLastMousePressButton(0),
             mLastMousePressTimeStamp(0),
             mLastMouseX(0),
             mLastMouseY(0),
             mClickCount(1),
             mLastMouseDragButton(0)
    {
        mFocusHandler = new FocusHandler();
    }

    Gui::~Gui()
    {
        if (Widget::widgetExists(mTop))
        {
            setTop(NULL);
        }

        delete mFocusHandler;
    }

    void Gui::setTop(Widget* top)
    {
        if (mTop != NULL)
        {
            mTop->_setFocusHandler(NULL);
        }
        if (top != NULL)
        {
            top->_setFocusHandler(mFocusHandler);
        }

        mTop = top;
    }

    Widget* Gui::getTop() const
    {
        return mTop;
    }

    void Gui::setGraphics(Graphics* graphics)
    {
        mGraphics = graphics;
    }

    Graphics* Gui::getGraphics() const
    {
        return mGraphics;
    }

    void Gui::setInput(Input* input)
    {
        mInput = input;
    }

    Input* Gui::getInput() const
    {
        return mInput;
    }

    void Gui::logic()
    {
        if (mTop == NULL)
        {
            throw GCN_EXCEPTION("No top widget set");
        }

        // Release of modal focus or modal mouse input focus might make it
        // necessary to distribute mouse events.
        handleModalFocusRelease();
        handleModalMouseInputFocusRelease();

        if (mInput != NULL)
        {
            mInput->_pollInput();

            handleKeyInput();
            handleMouseInput();
 
        } // end if

        mTop->logic();
    }

    void Gui::draw()
    {
        if (mTop == NULL)
        {
            throw GCN_EXCEPTION("No top widget set");
        }
        if (mGraphics == NULL)
        {
            throw GCN_EXCEPTION("No graphics set");
        }

        if (!mTop->isVisible())
        {
            return;
        }

        mGraphics->_beginDraw();

        // If top has a border,
        // draw it before drawing top
        if (mTop->getBorderSize() > 0)
        {
            Rectangle rec = mTop->getDimension();
            rec.x -= mTop->getBorderSize();
            rec.y -= mTop->getBorderSize();
            rec.width += 2 * mTop->getBorderSize();
            rec.height += 2 * mTop->getBorderSize();
            mGraphics->pushClipArea(rec);
            mTop->drawBorder(mGraphics);
            mGraphics->popClipArea();
        }

        mGraphics->pushClipArea(mTop->getDimension());
        mTop->draw(mGraphics);
        mGraphics->popClipArea();

        mGraphics->_endDraw();
    }

    void Gui::focusNone()
    {
        mFocusHandler->focusNone();
    }

    void Gui::setTabbingEnabled(bool tabbing)
    {
        mTabbing = tabbing;
    }

    bool Gui::isTabbingEnabled()
    {
        return mTabbing;
    }

    void Gui::addGlobalKeyListener(KeyListener* keyListener)
    {
        mKeyListeners.push_back(keyListener);
    }

    void Gui::removeGlobalKeyListener(KeyListener* keyListener)
    {
        mKeyListeners.remove(keyListener);
    }

    void Gui::handleMouseInput()
    {
        while (!mInput->isMouseQueueEmpty())
         {
             MouseInput mouseInput = mInput->dequeueMouseInput();

             // Save the current mouse state. It will be needed if modal focus
             // changes or modal mouse input focus changes.
             mLastMouseX = mouseInput.getX();
             mLastMouseY = mouseInput.getY();

             switch (mouseInput.getType())
             {
               case MouseInput::PRESSED:
                   handleMousePressed(mouseInput);
                   break;
               case MouseInput::RELEASED:
                   handleMouseReleased(mouseInput);
                   break;
               case MouseInput::MOVED:
                   handleMouseMoved(mouseInput);
                   break;
               case MouseInput::WHEEL_MOVED_DOWN:
                   handleMouseWheelMovedDown(mouseInput);
                   break;
               case MouseInput::WHEEL_MOVED_UP:
                   handleMouseWheelMovedUp(mouseInput);
                   break;
               default:
                   throw GCN_EXCEPTION("Unknown mouse input type.");
                   break;
             }
         }
    }

    void Gui::handleKeyInput()
    {
        while (!mInput->isKeyQueueEmpty())
        {
            KeyInput keyInput = mInput->dequeueKeyInput();

            // Save modifiers state
            mShiftPressed = keyInput.isShiftPressed();
            mMetaPressed = keyInput.isMetaPressed();
            mControlPressed = keyInput.isControlPressed();
            mAltPressed = keyInput.isAltPressed();

            KeyEvent keyEventToGlobalKeyListeners(NULL,
                                                  mShiftPressed,
                                                  mControlPressed,
                                                  mAltPressed,
                                                  mMetaPressed,
                                                  keyInput.getType(),
                                                  keyInput.isNumericPad(),
                                                  keyInput.getKey());

            distributeKeyEventToGlobalKeyListeners(keyEventToGlobalKeyListeners);

            // If a global key listener consumes the event it will not be
            // sent further to the source of the event.
            if (keyEventToGlobalKeyListeners.isConsumed())
            {
                continue;
            }

            bool keyEventConsumed = false;
            
            // Send key inputs to the focused widgets
            if (mFocusHandler->getFocused() != NULL)
            {
                KeyEvent keyEvent(getKeyEventSource(),
                                  mShiftPressed,
                                  mControlPressed,
                                  mAltPressed,
                                  mMetaPressed,
                                  keyInput.getType(),
                                  keyInput.isNumericPad(),
                                  keyInput.getKey());
                

                if (!mFocusHandler->getFocused()->isFocusable())
                {
                    mFocusHandler->focusNone();
                }
                else
                {                    
                    distributeKeyEvent(keyEvent);                    
                }

                keyEventConsumed = keyEvent.isConsumed();
            }

            // If the key event hasn't been consumed and
            // tabbing is enable check for tab press and
            // change focus.
            if (!keyEventConsumed
                && mTabbing
                && keyInput.getKey().getValue() == Key::TAB
                && keyInput.getType() == KeyInput::PRESSED)
            {
                if (keyInput.isShiftPressed())
                {
                    mFocusHandler->tabPrevious();
                }
                else
                {
                    mFocusHandler->tabNext();
                }
            }                           
                
        } // end while
    }

    void Gui::handleMouseMoved(const MouseInput& mouseInput)
    {
        // Check if the mouse leaves the application window.
        if (mFocusHandler->getLastWidgetWithMouse() != NULL
            && (mouseInput.getX() < 0
                || mouseInput.getY() < 0
                || !mTop->getDimension().isPointInRect(mouseInput.getX(), mouseInput.getY()))
            )
        {
            int lastWidgetWithMouseX, lastWidgetWithMouseY;
            mFocusHandler->getLastWidgetWithMouse()->getAbsolutePosition(lastWidgetWithMouseX, lastWidgetWithMouseY);

            distributeMouseEvent(mFocusHandler->getLastWidgetWithMouse(),
                                 MouseEvent::EXITED,
                                 mouseInput.getButton(),
                                 mouseInput.getX(),
                                 mouseInput.getX(),
                                 true,
                                 true);

            mFocusHandler->setLastWidgetWithMouse(NULL);

            return;
        }

        Widget* sourceWidget = getMouseEventSource(mouseInput.getX(), mouseInput.getY());

        if (sourceWidget != mFocusHandler->getLastWidgetWithMouse())
        {
            if (mFocusHandler->getLastWidgetWithMouse() != NULL)
            {
                int lastWidgetWithMouseX, lastWidgetWithMouseY;
                mFocusHandler->getLastWidgetWithMouse()->getAbsolutePosition(lastWidgetWithMouseX, lastWidgetWithMouseY);

                distributeMouseEvent(mFocusHandler->getLastWidgetWithMouse(),
                                     MouseEvent::EXITED,
                                     mouseInput.getButton(),
                                     mouseInput.getX(),
                                     mouseInput.getY(),
                                     true,
                                     true);
                mClickCount = 1;
                mLastMousePressTimeStamp = 0;
            }

            int sourceWidgetX, sourceWidgetY;
            sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

            distributeMouseEvent(sourceWidget,
                                 MouseEvent::ENTERED,
                                 mouseInput.getButton(),
                                 mouseInput.getX(),
                                 mouseInput.getY(),
                                 true,
                                 true);

            mFocusHandler->setLastWidgetWithMouse(sourceWidget);
        }

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            int draggedWidgetX, draggedWidgetY;
            mFocusHandler->getDraggedWidget()->getAbsolutePosition(draggedWidgetX, draggedWidgetY);

            distributeMouseEvent(mFocusHandler->getDraggedWidget(),
                                 MouseEvent::DRAGGED,
                                 mLastMouseDragButton,
                                 mouseInput.getX(),
                                 mouseInput.getY());
        }
        else
        {
            int sourceWidgetX, sourceWidgetY;
            sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

            distributeMouseEvent(sourceWidget,
                                 MouseEvent::MOVED,
                                 mouseInput.getButton(),
                                 mouseInput.getX(),
                                 mouseInput.getY());
        }
    }

    void Gui::handleMousePressed(const MouseInput& mouseInput)
    {
        Widget* sourceWidget = getMouseEventSource(mouseInput.getX(), mouseInput.getY());

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            sourceWidget = mFocusHandler->getDraggedWidget();
        }

        int sourceWidgetX, sourceWidgetY;
        sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);
        
        distributeMouseEvent(sourceWidget,
                             MouseEvent::PRESSED,
                             mouseInput.getButton(),
                             mouseInput.getX(),
                             mouseInput.getY());

        mFocusHandler->setLastWidgetPressed(sourceWidget);
        
        if (mFocusHandler->getModalFocused() != NULL
            && sourceWidget->hasModalFocus()
            || mFocusHandler->getModalFocused() == NULL)
        {
            sourceWidget->requestFocus();
        }

        mFocusHandler->setDraggedWidget(sourceWidget);
        mLastMouseDragButton = mouseInput.getButton();

        if (mLastMousePressTimeStamp < 300
            && mLastMousePressButton == mouseInput.getButton())
        {
            mClickCount++;
        }
        else
        {
            mClickCount = 1;
        }

        mLastMousePressButton = mouseInput.getButton();
        mLastMousePressTimeStamp = mouseInput.getTimeStamp();
    }

    void Gui::handleMouseWheelMovedDown(const MouseInput& mouseInput)
    {
        Widget* sourceWidget = getMouseEventSource(mouseInput.getX(), mouseInput.getY());

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            sourceWidget = mFocusHandler->getDraggedWidget();
        }

        int sourceWidgetX, sourceWidgetY;
        sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

        distributeMouseEvent(sourceWidget,
                             MouseEvent::WHEEL_MOVED_DOWN,
                             mouseInput.getButton(),
                             mouseInput.getX(),
                             mouseInput.getY());
    }

    void Gui::handleMouseWheelMovedUp(const MouseInput& mouseInput)
    {
        Widget* sourceWidget = getMouseEventSource(mouseInput.getX(), mouseInput.getY());

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            sourceWidget = mFocusHandler->getDraggedWidget();
        }

        int sourceWidgetX, sourceWidgetY;
        sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

        distributeMouseEvent(sourceWidget,
                             MouseEvent::WHEEL_MOVED_UP,
                             mouseInput.getButton(),
                             mouseInput.getX(),
                             mouseInput.getY());
    }

    void Gui::handleMouseReleased(const MouseInput& mouseInput)
    {
        Widget* sourceWidget = getMouseEventSource(mouseInput.getX(), mouseInput.getY());

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            if (sourceWidget != mFocusHandler->getLastWidgetPressed())
            {
                mFocusHandler->setLastWidgetPressed(NULL);
            }
            
            sourceWidget = mFocusHandler->getDraggedWidget();
        }

        int sourceWidgetX, sourceWidgetY;
        sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);
        
        distributeMouseEvent(sourceWidget,
                             MouseEvent::RELEASED,
                             mouseInput.getButton(),
                             mouseInput.getX(),
                             mouseInput.getY());

        if (mouseInput.getButton() == mLastMousePressButton            
            && mFocusHandler->getLastWidgetPressed() == sourceWidget)
        {
            distributeMouseEvent(sourceWidget,
                                 MouseEvent::CLICKED,
                                 mouseInput.getButton(),
                                 mouseInput.getX(),
                                 mouseInput.getY());
            
            mFocusHandler->setLastWidgetPressed(NULL);
        }
        else
        {
            mLastMousePressButton = 0;
            mClickCount = 0;
        }

        if (mFocusHandler->getDraggedWidget() != NULL)
        {
            mFocusHandler->setDraggedWidget(NULL);
        }
    }

    void Gui::handleModalFocusRelease()
    {
        if (mFocusHandler->getLastWidgetWithModalFocus() 
            != mFocusHandler->getModalFocused())
        {
            Widget* sourceWidget = getWidgetAt(mLastMouseX, mLastMouseY);

            if (sourceWidget != mFocusHandler->getLastWidgetWithModalFocus()
                && mFocusHandler->getLastWidgetWithModalFocus() != NULL)
            {
                int sourceWidgetX, sourceWidgetY;
                sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

                distributeMouseEvent(sourceWidget,
                                      MouseEvent::ENTERED,
                                      mLastMousePressButton,
                                      mLastMouseX,
                                      mLastMouseY);
            }

            mFocusHandler->setLastWidgetWithModalFocus(mFocusHandler->getModalFocused());
        }
    }

    void Gui::handleModalMouseInputFocusRelease()
    {
        if (mFocusHandler->getLastWidgetWithModalMouseInputFocus() 
            != mFocusHandler->getModalMouseInputFocused())
        {
            Widget* sourceWidget = getWidgetAt(mLastMouseX, mLastMouseY);

            if (sourceWidget != mFocusHandler->getLastWidgetWithModalMouseInputFocus() 
                && mFocusHandler->getLastWidgetWithModalMouseInputFocus()  != NULL)
            {
                int sourceWidgetX, sourceWidgetY;
                sourceWidget->getAbsolutePosition(sourceWidgetX, sourceWidgetY);

                distributeMouseEvent(sourceWidget,
                                     MouseEvent::ENTERED,
                                     mLastMousePressButton,
                                     mLastMouseX,
                                     mLastMouseY);
            }

            mFocusHandler->setLastWidgetWithModalMouseInputFocus(mFocusHandler->getModalMouseInputFocused());
        }
    }

    Widget* Gui::getWidgetAt(int x, int y)
    {
        // If the widget's parent has no child then we have found the widget..
        Widget* parent = mTop;
        Widget* child = mTop;

        while (child != NULL)
        {
            Widget* swap = child;
            int parentX, parentY;
            parent->getAbsolutePosition(parentX, parentY);
            child = parent->getWidgetAt(x - parentX, y - parentY);
            parent = swap;
        }

        return parent;
    }

    Widget* Gui::getMouseEventSource(int x, int y)
    {
        Widget* widget = getWidgetAt(x, y);

        if (mFocusHandler->getModalMouseInputFocused() != NULL
            && !widget->hasModalMouseInputFocus())
        {
            return mFocusHandler->getModalMouseInputFocused();
        }

        return widget;
    }

    Widget* Gui::getKeyEventSource()
    {
        Widget* widget = mFocusHandler->getFocused();

        while (widget->_getInternalFocusHandler() != NULL
               && widget->_getInternalFocusHandler()->getFocused() != NULL)
        {
            widget = widget->_getInternalFocusHandler()->getFocused();
        }

        return widget;
    }

    void Gui::distributeMouseEvent(Widget* source,
                                   int type,
                                   int button,
                                   int x,
                                   int y,
                                   bool force,
                                   bool toSourceOnly)
    {
        Widget* parent = source;
        Widget* widget = source;

        if (mFocusHandler->getModalFocused() != NULL
            && !widget->hasModalFocus())
        {
            return;
        }

        if (mFocusHandler->getModalMouseInputFocused() != NULL
            && !widget->hasModalMouseInputFocus())
        {
            return;
        }

        while (parent != NULL)
        {
            // If the widget has been removed due to input
            // cancel the distribution.
            if (!Widget::widgetExists(widget))
            {
                break;
            }

            parent = (Widget*)widget->getParent();

            if (widget->isEnabled() || force)
            {
                int widgetX, widgetY;
                widget->getAbsolutePosition(widgetX, widgetY);

                MouseEvent mouseEvent(source,
                                      mShiftPressed,
                                      mControlPressed,
                                      mAltPressed,
                                      mMetaPressed,
                                      type,
                                      button,
                                      x - widgetX,
                                      y - widgetY,
                                      mClickCount);
                                      
                std::list<MouseListener*> mouseListeners = widget->_getMouseListeners();

                // Send the event to all mouse listeners of the widget.
                for (std::list<MouseListener*>::iterator it = mouseListeners.begin();
                     it != mouseListeners.end();
                     ++it)
                {
                    switch (mouseEvent.getType())
                    {
                      case MouseEvent::ENTERED:
                          (*it)->mouseEntered(mouseEvent);
                          break;
                      case MouseEvent::EXITED:
                          (*it)->mouseExited(mouseEvent);
                          break;
                      case MouseEvent::MOVED:
                          (*it)->mouseMoved(mouseEvent);
                          break;
                      case MouseEvent::PRESSED:
                          (*it)->mousePressed(mouseEvent);
                          break;
                      case MouseEvent::RELEASED:
                          (*it)->mouseReleased(mouseEvent);
                          break;
                      case MouseEvent::WHEEL_MOVED_UP:
                          (*it)->mouseWheelMovedUp(mouseEvent);
                          break;
                      case MouseEvent::WHEEL_MOVED_DOWN:
                          (*it)->mouseWheelMovedDown(mouseEvent);
                          break;
                      case MouseEvent::DRAGGED:
                          (*it)->mouseDragged(mouseEvent);
                          break;
                      case MouseEvent::CLICKED:
                          (*it)->mouseClicked(mouseEvent);
                          break;
                      default:
                          throw GCN_EXCEPTION("Unknown mouse event type.");
                    }                    
                }
                
                if (toSourceOnly)
                {
                    break;
                }

            }

            Widget* swap = widget;
            widget = parent;
            parent = (Widget*)swap->getParent();

            // If a non modal focused widget has been reach
            // and we have modal focus cancel the distribution.
            if (mFocusHandler->getModalFocused() != NULL
                && !widget->hasModalFocus())
            {
                break;
            }

            // If a non modal mouse input focused widget has been reach
            // and we have modal mouse input focus cancel the distribution.
            if (mFocusHandler->getModalMouseInputFocused() != NULL
                && !widget->hasModalMouseInputFocus())
            {
                break;
            }
        }
    }

    void Gui::distributeKeyEvent(KeyEvent& keyEvent)
    {
        Widget* parent = keyEvent.getSource();
        Widget* widget = keyEvent.getSource();

        if (mFocusHandler->getModalFocused() != NULL
            && !widget->hasModalFocus())
        {
            return;
        }

        if (mFocusHandler->getModalMouseInputFocused() != NULL
            && !widget->hasModalMouseInputFocus())
        {
            return;
        }

        while (parent != NULL)
        {
            // If the widget has been removed due to input
            // cancel the distribution.
            if (!Widget::widgetExists(widget))
            {
                break;
            }

            parent = (Widget*)widget->getParent();

            if (widget->isEnabled())
            {
                std::list<KeyListener*> keyListeners = widget->_getKeyListeners();
            
                // Send the event to all key listeners of the source widget.
                for (std::list<KeyListener*>::iterator it = keyListeners.begin();
                     it != keyListeners.end();
                     ++it)
                {
                    switch (keyEvent.getType())
                    {
                      case KeyEvent::PRESSED:
                          (*it)->keyPressed(keyEvent);
                          break;
                      case KeyEvent::RELEASED:
                          (*it)->keyReleased(keyEvent);
                          break;
                      default:
                          throw GCN_EXCEPTION("Unknown key event type.");
                    }                
                }
            }

            Widget* swap = widget;
            widget = parent;
            parent = (Widget*)swap->getParent();

            // If a non modal focused widget has been reach
            // and we have modal focus cancel the distribution.
            if (mFocusHandler->getModalFocused() != NULL
                && !widget->hasModalFocus())
            {
                break;
            }
        }
    }

    void Gui::distributeKeyEventToGlobalKeyListeners(KeyEvent& keyEvent)
    {
        KeyListenerListIterator it;

        for (it = mKeyListeners.begin(); it != mKeyListeners.end(); it++)
        {
            switch (keyEvent.getType())
            {
              case KeyEvent::PRESSED:
                  (*it)->keyPressed(keyEvent);
                  break;
              case KeyEvent::RELEASED:
                  (*it)->keyReleased(keyEvent);
                  break;
              default:
                  throw GCN_EXCEPTION("Unknown key event type.");
            }

            if (keyEvent.isConsumed())
            {
                break;
            }
        }
    }
}
