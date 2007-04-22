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
#include "graphix_primitives.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <algorithm>

#ifdef HAVE_OPENGL
 #include <GL/gl.h>
 #include <GL/glu.h>
#endif

#include "mathstuff.h"
#include "log.h"

namespace FIFE {
	static bool USEGL = false;
	static bool USEDOUBLEBUF = true;

	void prepareDisplay( bool useGL, bool useDB ) {
		static bool first_run = true;
		if ( first_run ) {
			first_run = false;
			USEGL = useGL;
#ifndef HAVE_OPENGL

			if ( USEGL )
				printf( "Open GL rendering not available!\n" );
#endif

		}
		USEDOUBLEBUF = useDB;
	}

	Uint32 getFlags() {
		const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo();
		Uint32 flags = SDL_HWPALETTE;
#ifdef HAVE_OPENGL

		if ( USEGL ) {
			flags |= SDL_OPENGL;
			if ( USEDOUBLEBUF )
				flags |= SDL_GL_DOUBLEBUFFER;
		} else {
#endif
			if ( USEDOUBLEBUF )
				flags |= SDL_DOUBLEBUF; //is only useful for non gl mode afaict
#ifdef HAVE_OPENGL

		}
#endif

		/* This checks if hardware blits can be done */
		if ( videoInfo->blit_hw )
			flags |= SDL_HWACCEL;

		return flags;
	}

	void initGL( int width, int height ) {
#ifdef HAVE_OPENGL
		if ( USEGL ) {
			int value = 0;
			printf( "\ninitGL: RGB bpp " );
			SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value );
			printf( "%d/", value );
			SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &value );
			printf( "%d/", value );
			SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &value );
			printf( "%d  ", value );
			SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value );
			printf( "Bit Depth is %d\n", value );

			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glViewport( 0, 0, width, height );
			glMatrixMode( GL_PROJECTION );
			gluOrtho2D( 0, width, height, 0 );
			glMatrixMode( GL_MODELVIEW );
		}
#endif

	}

	void clearScreen( SDL_Surface *screen ) {
#ifdef HAVE_OPENGL
		if ( USEGL ) {
			glClear( GL_COLOR_BUFFER_BIT );
		} else {
#endif
			SDL_Rect rect = {0, 0, screen->w, screen->h};
			SDL_FillRect( screen, &rect, 0x00 );
#ifdef HAVE_OPENGL

		}
#endif

	}

	void swapBuffers( SDL_Surface *screen ) {
#ifdef HAVE_OPENGL
		if ( USEGL )
			SDL_GL_SwapBuffers();
		else
#endif

			SDL_Flip( screen );
	}

	Image::Image() : m_surface( 0 ), m_dirty( true ) {
#ifdef HAVE_OPENGL
		mtexture_chunks = NULL;
		mrows = 0;
		mcols = 0;
#endif

	}


	Image::~Image() {
		if ( m_surface )
			SDL_FreeSurface( m_surface );
#ifdef HAVE_OPENGL

		if ( USEGL ) {
			if ( mtexture_chunks ) {
				for ( int i = 0; i < mrows*mcols; ++i ) {
					glDeleteTextures( 1, &( mtexture_chunks[ i ].texture ) );
				}
				free( mtexture_chunks );
			}
		}
#endif

	}

	void Image::setSurface( SDL_Surface* surface ) {
		if( m_surface == surface )
			return;
		if ( m_surface )
			SDL_FreeSurface( m_surface );
#ifdef HAVE_OPENGL

		if ( USEGL ) {
			if ( mtexture_chunks ) {
				for ( int i = 0; i < mrows*mcols; ++i ) {
					glDeleteTextures( 1, &( mtexture_chunks[ i ].texture ) );
				}
				free( mtexture_chunks );
				mtexture_chunks = NULL;
			}
			mrows = 0;
			mcols = 0;
		}
#endif
		m_surface = surface;
		m_dirty = true;
	}

	SDL_Surface* Image::getSurface() {
		return m_surface;
	}

	void Image::render( SDL_Rect *target, SDL_Surface *screen, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha ) {

		if ( !screen ) {
			Log( "Image" ) << "*screen NULL in render() call!";
			return ;
		}
		if ( !target ) {
			Log( "Image" ) << "*target NULL in render() call!";
			return ;
		}
		if ( !m_surface ) {
			Log( "Image" ) << "*m_surface NULL when render() was called!";
			return ;
		}

		int xmin = target->x;
		int xmax = target->x + m_surface->w;
		int ymin = target->y;
		int ymax = target->y + m_surface->h;

		if ( ( xmax < 0 ) || ( xmin > screen->w ) || ( ymax < 0 ) || ( ymin > screen->h ) )
			return ;

#ifdef HAVE_OPENGL

		if ( USEGL ) {
			int x_start, x_end, y_start, y_end;

			if ( m_dirty )
				generateGLTexture();

			glColor4ub( red, green, blue, alpha );

			for ( int j = 0; j < mrows; ++j ) {
				for ( int i = 0; i < mcols; ++i ) {
					glBindTexture( GL_TEXTURE_2D, mtexture_chunks[ j * mcols + i ].texture );

					glEnable( GL_TEXTURE_2D );
					glEnable( GL_BLEND );
					glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

					x_start = target->x + i * 256;
					if ( i == mcols - 1 ) {
						x_end = x_start + m_surface->w - i * 256;
					} else {
						x_end = x_start + 256;
					}

					y_start = target->y + j * 256;
					if ( j == mrows - 1 ) {
						y_end = y_start + m_surface->h - j * 256;
					} else {
						y_end = y_start + 256;
					}

					glBegin( GL_QUADS );
					glTexCoord2f( 0.0	, 0.0 );
					glVertex2i( x_start, y_start );

					glTexCoord2f( 0.0 , mtexture_chunks[ j * mcols + i ].fh );
					glVertex2i( x_start, y_end );

					glTexCoord2f( mtexture_chunks[ j * mcols + i ].fw , mtexture_chunks[ j * mcols + i ].fh );
					glVertex2i( x_end , y_end );

					glTexCoord2f( mtexture_chunks[ j * mcols + i ].fw , 0.0 );
					glVertex2i( x_end , y_start );
					glEnd();

					glDisable( GL_BLEND );
					glDisable( GL_TEXTURE_2D );
				}
			}
		} else
#endif

			SDL_BlitSurface( m_surface, 0, screen, target );
	}

#ifdef HAVE_OPENGL
	void Image::generateGLTexture() {
		int w, h;
		SDL_Surface *image;
		SDL_Rect area, target_area;
		Uint32 saved_flags;
		Uint8 saved_alpha;

		mcols = ( m_surface->w / 256 ) + 1;
		mrows = ( m_surface->h / 256 ) + 1;

		if ( mtexture_chunks ) {
			for ( int i = 0; i < mrows*mcols; ++i ) {
				glDeleteTextures( 1, &( mtexture_chunks[ i ].texture ) );
			}
			free( mtexture_chunks );
			mtexture_chunks = NULL;
		}
		mtexture_chunks = static_cast<TextureChunk*>( malloc( sizeof( TextureChunk ) * mcols * mrows ) );

		for ( int j = 0; j < mrows; ++j ) {
			for ( int i = 0; i < mcols; ++i ) {
				if ( i == mcols - 1 ) {
					w = next_p2( m_surface->w - 256 * i );
				} else
					w = 256;
				if ( i == mrows - 1 ) {
					h = next_p2( m_surface->h - 256 * j );
				} else
					h = 256;

				image = SDL_CreateRGBSurface(
				            SDL_SWSURFACE,
				            w, h,
				            32,
				            RMASK,
				            GMASK,
				            BMASK,
				            AMASK
				        );
				if ( image == NULL ) {
					return ;
				}

				target_area.x = 0;
				target_area.y = 0;
				target_area.w = w;
				target_area.h = h;

				/* Save the alpha blending attributes */
				saved_flags = m_surface->flags & ( SDL_SRCALPHA | SDL_RLEACCELOK );
				saved_alpha = m_surface->format->alpha;
				if ( ( saved_flags & SDL_SRCALPHA ) == SDL_SRCALPHA ) {
					SDL_SetAlpha( m_surface, 0, 0 );
				}

				area.x = i * 256;
				area.y = j * 256;
				area.w = w;
				area.h = h;

				SDL_BlitSurface( m_surface, &area, image, &target_area );
				if ( i == mcols - 1 )
					mtexture_chunks[ j * mcols + i ].fw = static_cast<GLfloat>( m_surface->w - i * 256 ) / w;
				else
					mtexture_chunks[ j * mcols + i ].fw = 1.0;
				if ( j == mrows - 1 )
					mtexture_chunks[ j * mcols + i ].fh = static_cast<GLfloat>( m_surface->h - j * 256 ) / h;
				else
					mtexture_chunks[ j * mcols + i ].fh = 1.0;

				/* Restore the alpha blending attributes */
				if ( ( saved_flags & SDL_SRCALPHA ) == SDL_SRCALPHA ) {
					SDL_SetAlpha( m_surface, saved_flags, saved_alpha );
				}

				/* Create an OpenGL texture for the image */
				glGenTextures( 1, &( mtexture_chunks[ j * mcols + i ].texture ) );
				glBindTexture( GL_TEXTURE_2D, mtexture_chunks[ j * mcols + i ].texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

				SDL_FreeSurface( image ); /* No longer needed */
			}
		}

		m_dirty = false;
	}
#endif

} //FIFE
