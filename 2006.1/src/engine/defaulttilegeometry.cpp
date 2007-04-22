/***************************************************************************
 *   Copyright (C) 2006 by the FIFE Team                                   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "defaulttilegeometry.h"
#include "debugutils.h"

#include <math.h>

namespace FIFE {

	static const unsigned int TILE_WIDTH = 80;
	static const unsigned int TILE_HEIGHT = 36;
	
	// if we move 1 to the right we need to add this to the last xpos
	static const int X_DELTA_X = 48; 
	// if we move 1 to the right we need to add this to the last ypos
	static const int X_DELTA_Y =-12; 
	// if we move 1 tile down we need to add this to the last xpos
	static const int Y_DELTA_X = 32; 
	// if we move 1 tile down we need to add this to the last ypos
	static const int Y_DELTA_Y = 24; 

/*

		Tile grid calculations
		
		XX  YX  1  0
		YX  YY  0  1
		
		--- toScreen()
		
		48   32  1 0
		-12  24  0 1
		
		--- Calculate the inverse:
		
		48  32  1 0
		0  128  1 4
		
		192 128 4 0
		0   128 1 4
		
		--- fromScreen()
		
		192 0  3 -4
		0  128 1 4
*/

	DefaultTileGeometry::DefaultTileGeometry() {};
	DefaultTileGeometry::~DefaultTileGeometry() {};
	
	Point DefaultTileGeometry::toScreen(const Point& pos) {
		Point retval;
		retval.x =  pos.x * X_DELTA_X + pos.y * Y_DELTA_X;
		retval.y =  pos.x * X_DELTA_Y + pos.y * Y_DELTA_Y;
		return retval;
	}
	
	Point DefaultTileGeometry::fromScreen(const Point& pos) {
		// This tries to calculate the inverse of the matrix multiplication
		// from the toScreen() function.

		//Log("tile_geometry") << "<<< Postion: " << pos.x << "," << pos.y;
		Point retval;
		retval.x = (3*pos.x - 4*pos.y)/192;
		retval.y = (1*pos.x + 4*pos.y)/128;
		//Log("tile_geometry") << ">>> Postion: " << retval.x << "," << retval.y;
		Point test = toScreen(retval);
		//Log("tile_geometry") << "=== Postion: " << test.x << "," << test.y;


		//PANIC_PRINT("not implemented!");
		return retval;
	}
	
	Rect  DefaultTileGeometry::screenBoundingRect( const Rect& rect ) {
		Point ul( rect.x,rect.y );
		Point ur( rect.x + rect.w,rect.y);
		Point ll( rect.x,rect.y + rect.h);
		Point lr( rect.x + rect.w, rect.y + rect.h );
		Rect retval;
		
		retval.x = toScreen(ul).x;
		retval.y = toScreen(ur).y;
		retval.w = toScreen(lr).x - toScreen(ul).x;
		retval.h = toScreen(ll).y - toScreen(ur).y;
		
		return retval;
	}
	
	Rect  DefaultTileGeometry::gridBoundingRect( const Rect& rect ) {
/*
		double phi_a, phi_b, phi_c, phi_d, phi_e, phi_f;
		double X1, X2, Y1, Y2;

		phi_a = atan(X_DELTA_Y/double(X_DELTA_X));
		phi_b = M_PI / 2 - phi_a;
		phi_c = atan(Y_DELTA_Y/double(Y_DELTA_X));
		phi_d = M_PI / 2 - phi_c;
		phi_e = M_PI - phi_b - phi_d;
		phi_f = M_PI - phi_a - phi_c;

		X1 = rect.h / sin(phi_e) * sin(phi_d);
		X2 = rect.w / sin(phi_f) * sin(phi_c);
		int x_range = int(ceil((X1 + X2)/double(sqrt(X_DELTA_X*X_DELTA_X + X_DELTA_Y*X_DELTA_Y)))) + 1;

		Y1 = rect.h / sin(phi_e)*sin(phi_b);
		Y2 = rect.w / sin(phi_f)*sin(phi_c);
		int y_range = int(ceil((Y1 + Y2)/double(sqrt(Y_DELTA_X*Y_DELTA_X + Y_DELTA_Y*Y_DELTA_Y)))) + 1;

		return Rect(0,0,x_range,y_range);
*/
		Point ul( rect.x,rect.y );
		Point ur( rect.x + rect.w,rect.y);
		Point ll( rect.x,rect.y + rect.h);
		Point lr( rect.x + rect.w, rect.y + rect.h );
		Rect retval;
		
		retval.x = fromScreen(ll).x;
		retval.y = fromScreen(ul).y;
		retval.w = fromScreen(ur).x - fromScreen(ll).x;
		retval.h = fromScreen(lr).y - fromScreen(ul).y;
		
		return retval;
	}
	
	Point DefaultTileGeometry::baseSize() {
		return Point(TILE_WIDTH, TILE_HEIGHT);
	}

};


