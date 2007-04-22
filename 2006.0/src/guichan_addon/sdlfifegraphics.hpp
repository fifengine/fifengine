/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GCN_SDLFIFEGRAPHICS_HPP
#define GCN_SDLFIFEGRAPHICS_HPP

#include <SDL.h>

#include "graphix_primitives.h"

#include "guichan/sdl/sdlgraphics.hpp"
#include "guichan/graphics.hpp"
#include "guichan/platform.hpp"

namespace gcn
{
	class DECLSPEC SDLFIFEGraphics: public SDLGraphics
	{
		public:

			SDLFIFEGraphics();

			virtual void setTarget(SDL_Surface* target);

			virtual SDL_Surface* getTarget() const;

			virtual void drawSDLSurface(SDL_Surface* surface, SDL_Rect source,
                                    SDL_Rect destination);
			virtual void _beginDraw();

			virtual void _endDraw();

			virtual bool pushClipArea(Rectangle area);

			virtual void popClipArea();

			virtual void drawImage(const Image* image, int srcX, int srcY,
                               int dstX, int dstY, int width,
                               int height);

			virtual void drawPoint(int x, int y);

			virtual void drawLine(int x1, int y1, int x2, int y2);

			virtual void drawRectangle(const Rectangle& rectangle);

			virtual void fillRectangle(const Rectangle& rectangle);

			virtual void setColor(const Color& color);

			virtual const Color& getColor();

			void drawFIFEImage(FIFE::Image *image, int x, int y, int width, int height, Color col = Color(255, 255, 255, 255));

    protected:
			virtual void drawHLine(int x1, int y, int x2);

			virtual void drawVLine(int x, int y1, int y2);

			SDL_Surface* mTarget;
			Color mColor;
			bool mAlpha;
	};
}

#endif // end GCN_SDLFIFEGRAPHICS_HPP
