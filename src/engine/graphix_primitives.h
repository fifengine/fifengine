/***************************************************************************
*   Copyright (C) 2005-2006 by the FIFE Team                              *
*   fife-public@lists.sourceforge.net                                     *
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
*   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
***************************************************************************/
#ifndef GRAPHIX_PRIMITIVES_H
#define GRAPHIX_PRIMITIVES_H

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef HAVE_OPENGL
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif

namespace FIFE {

	void prepareDisplay( bool useGL, bool useDB );
	Uint32 getFlags();
	void initGL( int width, int height );
	void clearScreen( SDL_Surface *screen );
	void swapBuffers( SDL_Surface *screen );

	class Image {
		public:
			Image();
			~Image();

			void setSurface( SDL_Surface* surface );
			SDL_Surface* getSurface();

			/** Renders the Image with top left corner at {x,y}*/
			inline void render( SDL_Rect *target, SDL_Surface *screen ) {
				render( target, screen, 255, 255, 255, 255 );
			}
			/** Renders the Image with top left corner at {x,y}*/
			inline void render( SDL_Rect *target, SDL_Surface *screen, Uint8 alpha ) {
				render( target, screen, 255, 255, 255, alpha );
			}
			void render( SDL_Rect *target, SDL_Surface *screen, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha );

#ifdef HAVE_OPENGL

			void generateGLTexture();
#endif

		private:
			SDL_Surface* m_surface;
			bool m_dirty;

#ifdef HAVE_OPENGL

			typedef struct TextureChunk {
				GLuint texture;
				GLfloat fw;
				GLfloat fh;
			};

			TextureChunk *mtexture_chunks;
			int mrows;
			int mcols;
#endif

	};

} //FIFE

#endif
