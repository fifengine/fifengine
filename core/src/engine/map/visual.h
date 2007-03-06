/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_MAP_VISUAL_H
#define FIFE_MAP_VISUAL_H

// Standard C++ library includes
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/rect.h"
#include "video/renderable.h"

#include "location.h"

namespace FIFE {

	class Screen;
	class ComplexAnimation;

}

namespace FIFE { namespace map { namespace effect {
	class Effect;
}}}

namespace FIFE { namespace map {

	class ObjectInfo;
	class Layer;

	class Visual {
		public:
			explicit Visual(ObjectInfo* moi = 0);
			~Visual();

			void addEffect(effect::Effect* ve);
			void removeEffect(effect::Effect* ve);

			void reset(Layer* grid);
			void render(Screen* screen, const Point& viewport, uint8_t alpha = 255);

			void setRenderable( size_t renderable,
				RenderAble::RenderableTypes type = RenderAble::RT_IMAGE );

			void setLocation(const Location& location);
			const Location& getLocation() const;
			size_t getLayer() const { return m_location.layer; };
			size_t getLinearPosition() const { return m_linearposition; };

			size_t getZValue() const { return m_zvalue; };
			void setZValue(size_t z) { m_zvalue = z; };

			ComplexAnimation* getComplexAnimation();

			const Rect& getScreenBox() const { return m_screenbox; };

			ObjectInfo* getObject() { return m_moi; };

			uint8_t getAlpha() const { return m_alpha; };
			void setAlpha(uint8_t alpha) { m_alpha=alpha; };

		private:
			Location m_location;
			size_t m_renderable;
			uint8_t m_alpha;

			std::set<effect::Effect*> m_effects;

			RenderAble::RenderableTypes m_renderableType;
			ComplexAnimation* m_renderableCopy;

			Rect m_screenbox;
			Point m_position;
			size_t m_linearposition;
			size_t m_zvalue;

			ObjectInfo* m_moi;

			void calculateScreenbox(RenderAble*);
	};

	inline
	void Visual::setLocation(const Location& location) {
		m_location = location;
	}

	inline
	const Location& Visual::getLocation() const {
		return m_location;
	}

	inline
	ComplexAnimation* Visual::getComplexAnimation() {
		return m_renderableCopy;
	}


} } // FIFE::map

#endif // FIFE_MAPVISUAL_H
