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
#include "defaultobjectgeometry.h"

#include "debugutils.h"

namespace FIFE {

	DefaultObjectGeometry::DefaultObjectGeometry() {};
	DefaultObjectGeometry::~DefaultObjectGeometry() {};
	
	Point DefaultObjectGeometry::toScreen(const Point& pos) {
		Point p;
		p.x = 4816 - (((pos.x+1)/2)*32 + (pos.x/2)*16 - pos.y*16);
		p.y = (pos.y*12 + (pos.x/2)*12) - 1080 - 96;
		return p;
	}
	
	Point DefaultObjectGeometry::fromScreen(const Point& pos) {
		// Place into signed ints from unsigned shorts.
		int _x = pos.x;
		int _y = pos.y + 96;

		_y += 1092;

		// The world space is then broken up into rectagles of width 32 and height 12
		// and see which one the mouse is over. 32 is the width of the hex tile and 12 is 2/3
		int col = _x / 32;
		int row = _y / 12;
	
		// Adjust the x and y to be relative to the corner of the rect is has been found in.
		_x -= col * 32;
		_y -= row * 12;
	
		float TanTheta = 16.0f / 4.0f;    // Two constants required for calculations.
		float TanBeta = 4.0f / 16.0f;    // Compiler wil resolve down to constants.

		if (_y < 4) {
			// if in the top 4 pixels, where the tile goes like either /\ or \/ depending on the row.
			if (row % 2 == 1) {
				// if odd row, where the rect grid will split each hex tile vertically, by default
				// selecting the area in the bottom right hand corner of the rect. The following corrects
				if (_x < 16 && _x * TanTheta > _y) {
					//top left sector
					--row;
				}
				else if (_x >= 16 && ((32 - _x) * TanTheta) > _y) {
					//top right sector;
					--row;
				}
				else if (_x < 16) {
					//bottom left hand sector
					--col;
				}
			} else {
				// if even row, where the default value is both the botom left and bottom right sectors.
				// the following corrects.
				if (_x < 16 && (16-_x) * TanBeta > _y) {
					// top left sector.
					--row;
					--col;
				}
				else if (_x >= 16 && (_x - 16) * TanBeta > _y){
					// top right sector.
					--row;
				}
			}
		} else if (row % 2 == 1 && _x < 16) {
			// if odd row, but not in the top section with the sloped divisions,
			// and in the left hand side.
			--col;
		}

		//Convert the hex grid coordinates from teh conceptual square grid to actual coords.
		int InitialRowNumber = row * 200;
		int RowStartCol = 150 + (row / 2);
		int DistanceFromStartColToCol = RowStartCol - col;
		int Offset = DistanceFromStartColToCol % 200;
	
		int grid_pos = (InitialRowNumber - ((DistanceFromStartColToCol / 2)*200)) + Offset;
		return Point(grid_pos % 200, grid_pos / 200 );
	}
	
	Rect  DefaultObjectGeometry::screenBoundingRect( const Rect& rect ) {
		return Rect();
	}

	Rect  DefaultObjectGeometry::gridBoundingRect( const Rect& rect ) {
		Point ul( rect.x,rect.y );
		Point ur( rect.x + rect.w,rect.y);
		Point ll( rect.x,rect.y + rect.h);
		Point lr( rect.x + rect.w, rect.y + rect.h );
		//DEBUG_PRINT(fromScreen(ul) << fromScreen(ur) << fromScreen(ll));
		
		Rect retval;
		
		retval.x = fromScreen(ur).x;
		retval.y = fromScreen(ul).y;
		retval.w = fromScreen(ll).x - fromScreen(ur).x;
		retval.h = fromScreen(lr).y - fromScreen(ul).y;
		
		return retval;
	}
	
	
	Point DefaultObjectGeometry::baseSize() { 
		//FIXME: Plain Wrong 
		return Point(11,11);
	}

};


