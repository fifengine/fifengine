/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

#ifndef FIFE_GUI_WIDGETS_DOCKAREA_H
#define FIFE_GUI_WIDGETS_DOCKAREA_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resizablewindow.h"


namespace fcn {
	class DockArea : public ResizableWindow {
	public:
		DockArea();
		DockArea(bool active);
		virtual ~DockArea();

		void setActiveDockArea(bool active);
		bool isActiveDockArea() const;

		void setTopSide(bool side);
		bool isTopSide() const;
		void setRightSide(bool side);
		bool isRightSide() const;
		void setBottomSide(bool side);
		bool isBottomSide() const;
		void setLeftSide(bool side);
		bool isLeftSide() const;

		void dockWidget(Widget* widget);
		void undockWidget(Widget* widget);
		void setHighlighted(bool highlighted);
		bool isHighlighted() const;

		/**
         * Sets the highlight color of the widget.
         *
         * @param color The highlight color.
         * @see getHighlightColor
         */
        void setHighlightColor(const Color& color);

        /**
         * Gets the highlight color.
         *
         * @return The highlight color.
         * @see setHighlightColor
         */
        const Color& getHighlightColor() const;
		
		void repositionWidget(Widget* widget);


		// Inherited from ResizableWindow

		virtual void add(Widget* widget);

		virtual void remove(Widget* widget);

		virtual void resizeToContent(bool recursiv=true);

		virtual void expandContent(bool recursiv=true);
	
	
		// Inherited from ResizableWindow / MouseListener

		virtual void mouseEntered(MouseEvent& mouseEvent);

		virtual void mouseExited(MouseEvent& mouseEvent);

		virtual void mousePressed(MouseEvent& mouseEvent);

		virtual void mouseReleased(MouseEvent& mouseEvent);

		virtual void mouseMoved(MouseEvent& mouseEvent);
	
		virtual void mouseDragged(MouseEvent& mouseEvent);

	protected:
		void repositionDockAreas();
		void keepInBounds();

		bool m_activeDockArea;

		bool m_topSide;
		bool m_rightSide;
		bool m_bottomSide;
		bool m_leftSide;

		bool m_highlighted;

		Color m_highlightColor;
		Color m_savedColor;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
