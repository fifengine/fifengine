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

#ifndef FIFESDLSCREEN_H
#define FIFESDLSCREEN_H

#include "video/screen.h"
#include <SDL_video.h>

namespace gcn {
	class SDLGraphics;
}

namespace FIFE {

	/** A SDL Screen Surface
	 *
     * This is the main render target for the SDL renderbackend.
	 *
	 * This class implements the @c gcn::Graphics interface by containing a @c gcn::SDLGraphics instance
	 * and proxying the calls to that instance.
	 */
	class SDLScreen : public Screen {
		public:
			/** Create a new SDLScreen instance from a display surface
			 *  @param surface The surface that is the real rendering target.
			 */
			SDLScreen(SDL_Surface* surface);

			/** Destructor
			 * @note This doesn't delete the surface given in the constructor
			 */
			virtual ~SDLScreen();

			virtual unsigned int getWidth() const;
			virtual unsigned int getHeight() const;

			/** Return a pointer to the contained SDL surface
			 * @return A pointer to the SDL surface as set i the constructor
			 */
			SDL_Surface* getSurface() const;

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

			/** Draw a pixel with the currently set color
			 */
			virtual void drawPoint(int x, int y);

			/** Draw a line with the currently set color
			 */
			virtual void drawLine(int x1, int y1, int x2, int y2);

			/** Draw a rectangle with the currently set color
			 */
			virtual void drawRectangle(const gcn::Rectangle& rectangle);

			/** Draw a filled rectangle with the currently set color
			 */
			virtual void fillRectangle(const gcn::Rectangle& rectangle);

			/** Set the current @c gcn::Graphics color
			 */
			virtual void setColor(const gcn::Color& color);

			/** Get the current @c gcn::Graphics color
			 */
			virtual const gcn::Color& getColor();

			/** Begin drawing operations
			 */
			virtual void _beginDraw();

			/** End drawing operations
			 */
			virtual void _endDraw();

			/** Push clip rectangle
			 */
			virtual bool pushClipArea(gcn::Rectangle area);
			/** Pop clip rectangle
			 */
			virtual void popClipArea();

			/** Get current top clip rectangle
			 */
			virtual const gcn::ClipRectangle& getCurrentClipArea();


		private:
			SDL_Surface* m_screen;
			gcn::SDLGraphics* m_gcn_graph;

	};

}

#endif
