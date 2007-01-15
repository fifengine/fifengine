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

#ifndef FIFE_GLSCREEN_H
#define FIFE_GLSCREEN_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
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
			/** Creates a new GL screen.
			 */
			GLScreen();
			
			/** Destroys this GL screen.
			 */
			virtual ~GLScreen();

			/** Returns the screen width.
			 *
			 * @return Width of the screen.
			 */
			virtual unsigned int getWidth() const;
			
			/** Returns the screen height.
			 *
			 * @return Height of the screen.
			 */
			virtual unsigned int getHeight() const;

			/** Draws a @c gcn::Image onto this target.
			 *
			 * @param image The @c gcn::Image to draw.
			 * @param srcX The source X position from which to draw.
			 * @param srcY The source Y position from which to draw.
			 * @param dstX The target X position to which to draw.
			 * @param dstY The target Y position to which to draw.
			 * @param width The width of the source rectangle to draw.
			 * @param heiht The height of the source rectangle to draw.
			 * @note Casts the received @c image->_getData() to an ImageCache id
			 * and calls the RenderAbles @c render function. Also applies current
			 * clipping rectangle.
			 */
			virtual void drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height);
			
			/** Draws a point with the currently set color.
			 *
			 * @param x X coordinate of the point.
			 * @param y Y coordinate of the point.
			 */
			virtual void drawPoint(int x, int y);
			
			/** Draws a line with the currently set color.
			 *
			 * @param x1 X coordinate of the initial point.
			 * @param y1 Y coordinate of the initial point.
			 * @param x2 X coordinate of the final point.
			 * @param y2 Y coordinate of the final point.
			 */
			virtual void drawLine(int x1, int y1, int x2, int y2);
			
			/** Draws a rectangle with the currently set color.
			 *
			 * @param rectangle Rectangle to draw.
			 */
			virtual void drawRectangle(const gcn::Rectangle& rectangle);

			/** Draws a filled rectangle with the currently set color.
			 *
			 * @param rectangle Rectangle to draw.
			 */
			virtual void fillRectangle(const gcn::Rectangle& rectangle);

#if GUICHAN_VERSION != 4
			/** Draws a text string.
			 *
			 * @note: This is needed for guichan > 0.4; the only purpose
			 * is to activate GL_TEXTURE_2D before drawing and cleanup
			 * the flag afterwards.
			 *
			 * @note: Ugly, yet the best fix I can see.
			 */
			virtual void drawText(const std::string& text, int x, int y, unsigned int alignment = LEFT);
#endif

			/** Sets the current @c gcn::Graphics color.
			 *
			 * @param color New current color.
			 */
			virtual void setColor(const gcn::Color& color);

			/** Gets the current @c gcn::Graphics color.
			 *
			 * @return The current color.
			 */
			virtual const gcn::Color& getColor();
			
			/* Initializes drawing.
			 */
			virtual void _beginDraw ();
			/* Deinitializes drawing.
			 */
			virtual void _endDraw ();

			/** Pushes a new clip area onto the stack.
			 *
			 * @param area Rectangle to be used as clipping area.
			 * @return False if the the new area lays totally outside the current clip area. Note that an empty clip area will be 
			 * pushed in this case.
			 */
			virtual bool pushClipArea(gcn::Rectangle area);
			
			/** Removes the topmost clip area from the stack.
			 */
			virtual void popClipArea();
			
			/** Gets the current ClipArea.
			 *
			 * @return The current clip area.
			 */
			virtual const gcn::ClipRectangle& getCurrentClipArea();

		private:
			// Pointer to gcn::OpenGLGraphics, which contains the vital functions to draw in OpenGL.
			gcn::OpenGLGraphics* m_gcn_graph;

	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
