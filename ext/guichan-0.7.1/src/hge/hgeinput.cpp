/*     _______   __   __   __   ______   __   __   _______   __   __
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

#include "guichan/hge/hgeinput.hpp"
#include "guichan/exception.hpp"

namespace gcn
{
    HGE *HGEInput::mHGE = NULL;

    HGEInput::HGEInput()
    {
        mHGE = hgeCreate(HGE_VERSION);

        mMouseX = 0;
        mMouseY = 0;

        mLeftMouseButtonDown = false;
        mRightMouseButtonDown = false;
        mMiddleMouseButtonDown = false;
    }

    HGEInput::~HGEInput()
    {
        mHGE->Release();
    }

    bool  HGEInput::isKeyQueueEmpty()
    {
        return mKeyInputQueue.empty();
    }

    bool  HGEInput::isMouseQueueEmpty()
    {
        return mMouseInputQueue.empty();
    }

    KeyInput  HGEInput::dequeueKeyInput()
    {
        if (isKeyQueueEmpty())
        {
            throw GCN_EXCEPTION("Key queue is empty.");
        }

        KeyInput keyInput;

        keyInput = mKeyInputQueue.front();
        mKeyInputQueue.pop();

        return keyInput;

    }

    MouseInput HGEInput::dequeueMouseInput()
    {
        if (isMouseQueueEmpty())
        {
            throw GCN_EXCEPTION("Mouse queue is empty.");
        }

        MouseInput mouseInput;

        mouseInput = mMouseInputQueue.front();
        mMouseInputQueue.pop();

        return mouseInput;
    }

    void  HGEInput::_pollInput()
    {
        hgeInputEvent ie;
    
        mHGE->Input_GetEvent(&ie);
        
        pollMouseInput();
        pollKeyInput(ie);
    }

    void  HGEInput::pollMouseInput()
    {
        float curMouseX, curMouseY;
        int mouseWheel;

        bool leftBtn, rightBtn, centerBtn;

        mHGE->Input_GetMousePos(&curMouseX, &curMouseY);
        mouseWheel = mHGE->Input_GetMouseWheel();

        leftBtn = mHGE->Input_GetKeyState(HGEK_LBUTTON);
        rightBtn = mHGE->Input_GetKeyState(HGEK_RBUTTON);
        centerBtn = mHGE->Input_GetKeyState(HGEK_MBUTTON);

        // Check mouse movement
        if (mMouseX != curMouseX 
            || mMouseY != curMouseY )
        {
            mMouseInputQueue.push(MouseInput(MouseInput::EMPTY,
                                             MouseInput::MOVED,
                                             curMouseX,
                                             curMouseY,
                                             0));
     
            mMouseX = curMouseX;
            mMouseY = curMouseY;
        }

        // Check mouse wheel
        if (mouseWheel > 0)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::EMPTY,
                                             MouseInput::WHEEL_MOVED_UP,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (mouseWheel < 0)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::EMPTY,
                                             MouseInput::WHEEL_MOVED_DOWN,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }

        //check mouse buttons
        if (!mLeftMouseButtonDown && leftBtn)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::LEFT,
                                             MouseInput::PRESSED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (mLeftMouseButtonDown && !leftBtn )
        {
            mMouseInputQueue.push(MouseInput(MouseInput::LEFT,
                                             MouseInput::RELEASED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (!mRightMouseButtonDown && rightBtn)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::RIGHT,
                                             MouseInput::PRESSED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (mRightMouseButtonDown &&  !rightBtn)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::RIGHT,
                                             MouseInput::RELEASED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (!mMiddleMouseButtonDown && centerBtn)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::MIDDLE,
                                             MouseInput::PRESSED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }
        else if (mMiddleMouseButtonDown && !centerBtn)
        {
            mMouseInputQueue.push(MouseInput(MouseInput::MIDDLE,
                                             MouseInput::RELEASED,
                                             curMouseX,
                                             curMouseY,
                                             0));
        }

        mLeftMouseButtonDown  = leftBtn;
        mRightMouseButtonDown = rightBtn;
        mMiddleMouseButtonDown  = centerBtn;
    }

    void  HGEInput::pollKeyInput(hgeInputEvent &ki)
    {
        if (ki.type != INPUT_KEYDOWN 
            && ki.type != INPUT_KEYUP )
        {
             return ;
        }
        
        if (ki.type == INPUT_KEYDOWN)
        {
            Key keyObj = convertToKey(ki.key, ki.chr);
  
            KeyInput keyInput(keyObj, KeyInput::PRESSED);
  
            keyInput.setNumericPad(isNumericPad(ki.key));
  
            keyInput.setShiftPressed(ki.flags & HGEINP_SHIFT);
            keyInput.setAltPressed(ki.flags & HGEINP_ALT);
            keyInput.setControlPressed(ki.flags & HGEINP_CTRL);
        
            mKeyInputQueue.push(keyInput);
        }
  
        if (ki.type == INPUT_KEYUP)
        {
            Key keyObj = convertToKey(ki.key, ki.chr);
  
            KeyInput keyInput(keyObj, KeyInput::RELEASED);
  
            keyInput.setNumericPad(isNumericPad(ki.key));
  
            keyInput.setShiftPressed(ki.flags & HGEINP_SHIFT);
            keyInput.setAltPressed(ki.flags & HGEINP_ALT);
            keyInput.setControlPressed(ki.flags & HGEINP_CTRL);
        
            mKeyInputQueue.push(keyInput);
        }
    }

    Key HGEInput::convertToKey(int key, int chr)
    {
        int keysym;

        switch(key)
        {
            case HGEK_TAB:
              keysym = Key::TAB;  
              break;
            case HGEK_ESCAPE:
              keysym = Key::ESCAPE;
              break;
            case HGEK_ALT:
              keysym = Key::LEFT_ALT;
              break;
            /*
            Unsupported by HGE.
            case HGEK_ALT: 
            keysym = Key::RIGHT_ALT;
            break;
            */
            case HGEK_SHIFT:
              keysym = Key::LEFT_SHIFT;
              break;
            /*
            Unsuppored by HGE.
            case HGEK_SHIFT:
            keysym = Key::RIGHT_SHIFT;
            break;
            */
            case HGEK_CTRL:
              keysym = Key::LEFT_CONTROL;
              break;
            /*
            Unsupported by HGE.
            case HGEK_CTRL:
            keysym = Key::RIGHT_CONTROL;
            break;
            */
            case HGEK_LWIN:
              keysym = Key::LEFT_META;
              break;
            case HGEK_RWIN:
              keysym = Key::RIGHT_META;
              break;   
            case HGEK_INSERT:
              keysym = Key::INSERT;
              break;  
            case HGEK_HOME:
              keysym = Key::HOME;
              break;  
            case HGEK_PGUP:
              keysym = Key::PAGE_UP;
              break;
            case HGEK_PGDN:
              keysym = Key::PAGE_DOWN;
              break;    
            case HGEK_DELETE:
              keysym = Key::DELETE;
              break;
            /*
            Unsupported by HGE.
            case HGEK_BACKSPACE:
            keysym = Key::DELETE;
            pad = true;
            break;
            */
            case HGEK_END:
              keysym = Key::END;
              break;
            case HGEK_CAPSLOCK:
              keysym = Key::CAPS_LOCK;
              break;
            case HGEK_BACKSPACE:
              keysym = Key::BACKSPACE;
              break;
            case HGEK_F1:
              keysym = Key::F1;
              break;
            case HGEK_F2:
              keysym = Key::F2;
              break;
            case HGEK_F3:
              keysym = Key::F3;
              break;
            case HGEK_F4:
              keysym = Key::F4;
              break;
            case HGEK_F5:
              keysym = Key::F5;
              break;
            case HGEK_F6:
              keysym = Key::F6;
              break;
            case HGEK_F7:
              keysym = Key::F7;
              break;
            case HGEK_F8:
              keysym = Key::F8;
              break;
            case HGEK_F9:
              keysym = Key::F9;
              break;
            case HGEK_F10:
              keysym = Key::F10;
              break;
            case HGEK_F11:
              keysym = Key::F11;
              break;
            case HGEK_F12:
              keysym = Key::F12;
              break;
            /* 
            Unsupported by HGE
            case HGEK_PRTSCR:
            keysym = Key::PRINT_SCREEN;
            break;
            */
            case HGEK_PAUSE:
              keysym = Key::PAUSE;
              break;
            case HGEK_SCROLLLOCK:
              keysym = Key::SCROLL_LOCK;
              break;
            case HGEK_NUMLOCK:
              keysym = Key::NUM_LOCK;
              break;
            case HGEK_LEFT:
              keysym = Key::LEFT;
              break;
            case HGEK_RIGHT:
              keysym = Key::RIGHT;
              break;
            case HGEK_UP:
              keysym = Key::UP;
              break;
            case HGEK_DOWN:
              keysym = Key::DOWN;
              break;
            case HGEK_ENTER:
              keysym = Key::ENTER;
              break;
            default:
              keysym = chr;
        }

        Key k = Key(keysym);

        return k;
    }

    bool  HGEInput::isNumericPad(int keyCode)
    {
        switch(keyCode)
        {
          case HGEK_NUMPAD0:
          case HGEK_NUMPAD1:
          case HGEK_NUMPAD2:
          case HGEK_NUMPAD3:
          case HGEK_NUMPAD4:
          case HGEK_NUMPAD5:
          case HGEK_NUMPAD6:
          case HGEK_NUMPAD7:
          case HGEK_NUMPAD8:
          case HGEK_NUMPAD9:
              return true;
          default: 
              return false;
        }
    }
}