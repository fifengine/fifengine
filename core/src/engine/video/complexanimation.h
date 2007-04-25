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

#ifndef FIFE_VIDEO_COMPLEXANIMATION_H
#define FIFE_VIDEO_COMPLEXANIMATION_H

// Standard C++ library includes
#include <list>
#include <string>
#include <vector>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/action.h"
#include "timeevent.h"

#include "util/point.h"
#include "renderable.h"

namespace FIFE {
	namespace map { class Geometry; }
	class Animation;

	class PartialAction;
	typedef boost::shared_ptr<PartialAction> PartialActionPtr;

	/** ComplexAnimation.
	 * 
	 * This class is a container for the more complex animations of
	 * Critters and such. It is basically used for anything that
	 * has more than one state. The visual representation of these
	 * states has to be smooth. A "walking" animation for example
	 * might be made for walking over two hex grid cells. But
	 * one usually would set one "walking" animation for one
	 * move and another for the second. This class is designed
	 * to handle exactly this (and maybe other) situations.
	 * The information what animation phase shall be continued
	 * with which animation phase is saved in the partial actions.
	 * Each action (like walking) has a set of partial actions
	 * associated. One is by convention the starting animation
	 * phase. Each partial action in turn has "next partial actions".
	 * So if after a walking action a running action is requested,
	 * the complex animation will look up the correct next partial action,
	 * which will point to an animation phase.
	 *
	 * All this information is loaded from an XML file.
	 *
	 * This class itself is a "Renderable" but since it has a state
	 * of it's own (which an Image doesn't have) users shall make
	 * a copy of the instance returned by the ImageCache and use that.
	 *
	 * To use this class write a XML description file, load it via
	 * it's filename into the ImageCache and optain a copy.
	 * Then you can start enqueueing actions. This cause the object
	 * to update itself and go through the animations/actions on the
	 * queue.
	 * You can also empty the queue, and enqueue new actions.
	 *
	 *
	 * @see PartialAction
	 * @warning Untested Code!
	 */
	class ComplexAnimation : public RenderAble {
		private:
			class UpdateEvent : public TimeEvent {
				public:
					UpdateEvent(ComplexAnimation* complexAnimation);
					virtual ~UpdateEvent();

					virtual void updateEvent(unsigned long time);
				private:
					unsigned long m_lastTime;
					ComplexAnimation* m_complexAnimation;

			};

		public:
			/** Private-Constructor.
			 */
			// XXX XXX XXX
			// This should only be used by
			// FIFE::map::loaders::fallout::ComplexAnimationProvider. It was
			// actually private before and exposed via a friend declaration, but
			// I couldn't figure out how to befriend the class after moving it to
			// that deeply nested namespace. This needs to be reviewed -- zahlman
			explicit ComplexAnimation(const std::string& name);
		public:

			/** Destructor.
			 */
			~ComplexAnimation();

			/** Copy-Constructor.
			 */
			ComplexAnimation(const ComplexAnimation& master);

			/** Registers a animation.
			 *
			 * @param action The action of the animation.
			 * @param direction The direction of the animation.
			 * @param anim Pointer to the animation.
			 */
			void registerAnimation(uint8_t action, uint8_t direction, size_t animation);

			/** Register Partial Action
			 *
			 */
			void registerPartialAction(const PartialActionPtr& partialAction);

			/** Sets the action of the animation.
			 *
			 * @param action    The action ID of the animation.
			 * @param direction The direction of the animation.
			 */
			void setActiveAction(const map::Action& action, bool now = true);

			void enqueueAction(const map::Action& action);

			void emptyQueue();

			void resetGeometry(map::Geometry *geometry);

			void render(const Rect& rect, Screen* screen, uint8_t alpha = 255);

			/** Sets the X shift of the Image.
			 *
			 * @param xshift The X shift of the Image.
			 */
			virtual void setXShift(int xshift);
			/** Sets the Y shift of the Image.
			 *
			 * @param yshift The Y shift of the Image.
			 */
			virtual void setYShift(int yshift);
			/** Gets the X shift of the Image.
			 *
			 * @return The X shift of the Image.
			 */
			virtual int getXShift() const;
			/** Gets the Y shift of the Image.
			 *
			 * @return The Y shift of the Image.
			 */
			virtual int getYShift() const;

			/// @copydoc Renderable::getWidth
			virtual unsigned getWidth() const;

			/// @copydoc Renderable::getHeight
			virtual unsigned getHeight() const;
		protected:
			typedef std::vector<Point>  type_warpOffsets;
			typedef std::vector<size_t> type_animByDirection;
			typedef std::vector<PartialActionPtr> type_partialActions;

			typedef struct {
				type_animByDirection animations;
				type_partialActions  partialActions;
			} s_action;
			typedef std::vector<s_action> type_actions;
			typedef boost::shared_ptr<type_actions> type_actionsPtr;

			type_warpOffsets m_warpOffsets;
			type_actionsPtr m_actionsPtr;

			typedef std::list<map::Action> type_actionQueue;

			type_actionQueue m_actionQueue;
			std::string m_name;

			typedef struct {
				uint8_t action;
				uint8_t direction;
				map::Action  curAction; // FIXME:Ugly, duplicates info.

				PartialActionPtr partialAction;

				int16_t frameCounter;
				int16_t numFrames;
				int8_t animDir;

				size_t  animation;
				int16_t frame;

				int16_t frameTime;
				int16_t frameDuration;

				Point shift,size,warpShift;
			} s_state;

			s_state m_state;
			UpdateEvent* m_updateEvent;
		
			void update(int32_t timeDelta);
			bool chooseNextPartialAction(uint8_t action, uint8_t direction);
			bool activateNextAction();
			const PartialActionPtr& findFirstPartialAction(uint8_t action, uint8_t direction) const;

			void initState();
			void calculateScreenBox(Animation* anim);

	};
}

#endif
