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
/*
	This is the Visual representation of an object on the map.
	It clearly belongs to the rendering code.
	It can have an static image or an animation as real
	representation. Others may be added as needed.
		-phoku
*/
#ifndef VISUAL_H
#define VISUAL_H

#include <string>
#include <vector>
#include <SDL_video.h>
#include "graphix_primitives.h"
#include "mapgeometry.h"

namespace FIFE {

	class MapView;
	class Animation;
	class ComplexAnimation;
	struct VisualInternals;

	class Visual {
		public:
			enum visualType {
				VT_STATIC_IMAGE = 0,
				VT_STATIC_ANIM,
				VT_COMPLEX_ANIM
			};

			enum positionType {
				PT_HEX_GRID = 0,
				PT_TILE_GRID,
				PT_ABSOLUTE
			};

			enum layerType {
				LT_FLOOR = 0,
				LT_FLOOR_OVERLAY_1,
				LT_FLOOR_OVERLAY_2,
				LT_OBJECT,
				LT_OBJECT_OVERLAY_1,
				LT_OBJECT_OVERLAY_2
			};
			
			static const int POS_NOT_ON_MAP = -1;

			Visual();
			virtual ~Visual();

			// Positon
			int getPosition() const;
			void setPosition(int pos);

			void setLayerType(layerType);
			void setPositionType(positionType);
			void setVisualType(visualType);

			layerType getLayerType() const;
			positionType getPositionType() const;
			visualType getVisualType() const;

			// Setting the GFX
			void setImage(size_t imgid, int xshift, int yshift);
			void setAnimation( Animation* anim );
			void setComplexAnimation( ComplexAnimation* anim );

			void setShift( int xshift, int yshift);

			// Z Sorting
			int getZValue();

			// Rendering
			void render(RenderContext rc,SDL_Surface*);

		private:
			struct VisualInternals* _d;

			void init();
			void cleanup();

			void renderImage(SDL_Rect* target, SDL_Surface* surface);
			void renderAnimation(SDL_Rect* target, SDL_Surface* surface);

			Visual(const Visual&);
			Visual& operator=(const Visual&);

			friend class MapView;
	};

}//FIFE

#endif

