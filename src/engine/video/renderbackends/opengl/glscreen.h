/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
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
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFEGLSCREEN_H
#define FIFEGLSCREEN_H

#include "video/screen.h"

namespace gcn {
	class OpenGLGraphics;
}

namespace FIFE {

	/** OpenGL Screen surface.
	 *
	 * Implements the main render target for the OpenGL backend.
	 *
	 * @see Screen
	 * @see RenderBackendOpenGL
	 */
	class GLScreen : public Screen {
		public:
			/** Create a new GL screen.
			 */
			GLScreen();
			
			/** Destroys this GL screen.
			 */
			virtual ~GLScreen();

			/** Returns the screen width.
			 * @return Width of the screen
			 */
			virtual unsigned int getWidth() const;
			
			/** Returns the screen height.
			 * @return Height of the screen
			 */
			virtual unsigned int getHeight() const;

			/** Draw a @c gcn::Image onto this target.
			 * @param image The @c gcn::Image to draw.
			 * @param srcX The source X position from which to draw
			 * @param srcY The source Y position from which to draw
			 * @param dstX The target X position to which to draw
			 * @param dstY The target Y position to which to draw 
			 * @param width The width of the source rectangle to draw
			 * @param heiht The height of the source rectangle to draw
			 * 
			 * @note Casts the received @c image->_getData() to an ImageCache id
			 * and calls the RenderAbles @c render function. Also applies current
			 * clipping rectangle.
			 */
			virtual void drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height);
			
			/** Draw a pixel with the currently set color.
			 */
			virtual void drawPoint(int x, int y);
			
			/** Draw a line with the currently set color.
			 */
			virtual void drawLine(int x1, int y1, int x2, int y2);
			
			/** Draw a rectangle with the currently set color.
			 */
			virtual void drawRectangle(const gcn::Rectangle& rectangle);

			/** Draw a filled rectangle with the currently set color.
			 */
			virtual void fillRectangle(const gcn::Rectangle& rectangle);

			/** Set the current @c gcn::Graphics color.
			 */
			virtual void setColor(const gcn::Color& color);

			/** Get the current @c gcn::Graphics color.
			 */
			virtual const gcn::Color& getColor();
			
			virtual void _beginDraw ();
			virtual void _endDraw ();

			/** Push clip rectangle.
			 */
			virtual bool pushClipArea(gcn::Rectangle area);
			
			/** Pop clip rectangle.
			 */
			virtual void popClipArea();
			
			/** Get current top clip rectangle.
			 */
			virtual const gcn::ClipRectangle& getCurrentClipArea();

		private:
			gcn::OpenGLGraphics* m_gcn_graph;

	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
