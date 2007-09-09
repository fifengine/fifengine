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

#ifndef COMPLEXANIMATION_H
#define COMPLEXANIMATION_H
#include "renderable.h"
#include <stdint.h>
#include <map>

namespace FIFE {
	class Animation;
	/** ComplexAnimation
	 * 
	 * This class was/is thought to become a container for
	 * Animation that may have a direction/actions associated with it.
	 * Currently it is used nowhere, also with a proper implementation
	 * it is supposed to be used by MFFalloutFRM as frm files
	 * can contain this kind of data.
	 *
	 * @warning Untested Code!
	 */
	class ComplexAnimation : public RenderAble {
		public:
			ComplexAnimation();
			~ComplexAnimation();

			void registerAnimation(uint8_t action, uint8_t direction, Animation *anim);
			void setup(uint8_t startAction, uint8_t startDirection);

			Animation *activeAnimation;
			void setDirection(uint8_t directionId);
			void setAction(uint8_t actionId);

			virtual void setXShift(int xshift);
			virtual void setYShift(int yshift);
			virtual int getXShift() const;
			virtual int getYShift() const;
		protected:
			typedef std::map<uint8_t, Animation*> T_animByDirection;
			typedef std::map<uint8_t, T_animByDirection > T_animByAction;
			T_animByAction content;
			//std::map<uint8_t action, std::map<uint8_t direction, Animation *anim > > content;
			uint8_t nowAction;
			uint8_t nowDirection;
		private:
			void cleanup();
	};
};

#endif
