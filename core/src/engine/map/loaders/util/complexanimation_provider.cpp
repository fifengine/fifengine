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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "tinyxml/tinyxml.h"
#include "video/partialaction.h"
#include "video/complexanimation.h"
#include "debugutils.h"
#include "util/exception.h"
#include "imagecache.h"

#include "complexanimation_provider.h"

namespace FIFE { namespace map { namespace loaders { namespace util {

	namespace CAP {
		const std::string LogScope = "cmplx_anim_provider";

		const std::string RootTag   = "complex-animation";
		const std::string ActionTag = "action";
		const std::string PartialActionTag     = "partial-action";
		const std::string NextPartialActionTag = "next-partial-actions";
		const std::string AnimationTag = "animation";
		const std::string FramesTag = "frames";
		const std::string StepNumberTag = "step";

		const uint8_t AnyDirection = PartialAction::ANY_DIRECTION;
	}
	
	RenderAble* ComplexAnimationProvider::createRenderable() {
		TiXmlDocument doc(getLocation().getFilename());

		DEBUG_PRINT("CAP:" << getLocation().getFilename());

		if( !doc.LoadFile() ) {
			return 0;
		}
		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el;

		if (node) {
			el = node->ToElement();
		} else {
			return 0;
		}
		
		if( CAP::RootTag != el->Value() ) {
			return 0;
		}
		Debug(CAP::LogScope)
			 << getLocation().getFilename() << " XML seems okay, proceeding";

		m_complexAnimation = new ComplexAnimation( el->Attribute("name") );
		TiXmlElement* actionEl = el->FirstChildElement(CAP::ActionTag.c_str());
		readActions( actionEl );
		readNextPartialActions( actionEl );
		m_partialActions.clear();
		return m_complexAnimation;

	}

	void ComplexAnimationProvider::readActions(TiXmlElement* el) {
		int action;
		int direction, max_dir, i_dir;
		std::string animSrc;
		size_t      animId;
		RenderableLocation location;

		while (el) {
			el->QueryIntAttribute("id",&action);
			el->QueryIntAttribute("direction",&direction);

			TiXmlElement* animEl = el->FirstChildElement(CAP::AnimationTag.c_str());
			if( !animEl ) {
				el = el->NextSiblingElement(CAP::ActionTag.c_str());
				continue;
			}

			animSrc = animEl->Attribute("src");

			if (direction == PartialAction::ANY_DIRECTION) {
				max_dir = 6;
				i_dir   = 0;
			} else {
				max_dir = direction+1;
				i_dir   = direction;
			}

			for (; i_dir != max_dir; ++i_dir) {
				location = RenderableLocation( animSrc );
				location.setType( RenderAble::RT_ANIMATION );
				location.setDirection( i_dir );

				animId = ImageCache::instance()->addImageFromLocation(location);
				Debug(CAP::LogScope)
					<< getLocation().getFilename() 
					<< " registering action " << action 
					<< "(" << i_dir << ")"
					<< " src=" << animSrc;
				m_complexAnimation->registerAnimation(action, i_dir, animId);
			}

			// Read partial-actions in this one.
			// Will store partial actions in m_partialActions
			// We need to parse the next-partial-actions
			// Directive in a second pass
			//
			// This will also register the partial actions in the complex
			// animation.
			TiXmlElement* partialActionEl 
				= el->FirstChildElement(CAP::PartialActionTag.c_str());
			readPartialActions(action, direction, partialActionEl );

			el = el->NextSiblingElement(CAP::ActionTag.c_str());
		}
	}

	void ComplexAnimationProvider::readPartialActions(
		uint8_t action, uint8_t direction, TiXmlElement* el) {
		PartialActionPtr partialAction;
		int number;
		int firstFrame, numFrames;
		int warpFactor;
		while( el ) {
			el->QueryIntAttribute("number",&number);

			// Query <frame-range>
			TiXmlElement* frameRangeEl 
				= el->FirstChildElement(CAP::FramesTag.c_str());
			if (!frameRangeEl) {
				el = el->NextSiblingElement(CAP::PartialActionTag.c_str());
				continue;
			}
			frameRangeEl->QueryIntAttribute("start",&firstFrame); 
			frameRangeEl->QueryIntAttribute("number",&numFrames); 

			// Query <warp-factor>
			warpFactor = 0;
			TiXmlElement* warpFactorEl 
				= el->FirstChildElement(CAP::StepNumberTag.c_str());
			if (warpFactorEl) {
				warpFactorEl->QueryIntAttribute("number",&warpFactor);
			}

			Debug(CAP::LogScope)
				<< "Partial action: "
				<< int(action) << "("<<int(direction)<<")#" << number << " "
				<< firstFrame << ":" << numFrames 
				<< "  wf=" << warpFactor;

			partialAction = PartialActionPtr(
				new PartialAction(action, number, direction));
			partialAction->setFirstFrame( firstFrame );
			partialAction->setNumFrames( numFrames );
			partialAction->setWarpFactor( warpFactor );

			m_partialActions.push_back( partialAction );
			m_complexAnimation->registerPartialAction( partialAction );

			el = el->NextSiblingElement(CAP::PartialActionTag.c_str());
		}
	}

	void ComplexAnimationProvider::readNextPartialActions(TiXmlElement* el) {
		PartialActionPtr partialAction;
		int action;
		int direction;
		int number;
		while (el) { // Iterate through <actions>
			el->QueryIntAttribute("id",&action);
			el->QueryIntAttribute("direction",&direction);
			Debug(CAP::LogScope) 
				<< "Reading next-partial-actions for " 
				<< action << "("<< direction << ")";

			// Loop through the <partial-action> elements
			TiXmlElement* partialActionEl =
				el->FirstChildElement(CAP::PartialActionTag.c_str());
			while (partialActionEl) {
				partialActionEl->QueryIntAttribute("number",&number);
				Debug(CAP::LogScope)
					<< "Reading npa's for #" << number;

				partialAction =
					findPartialAction(action,direction,number);
				readNextPartialActionsElement(partialAction, partialActionEl );

				partialActionEl =
					partialActionEl->NextSiblingElement(CAP::PartialActionTag.c_str());
			}
			el = el->NextSiblingElement(CAP::ActionTag.c_str());
		}
	}

	void ComplexAnimationProvider::readNextPartialActionsElement(
			PartialActionPtr& partialAction, TiXmlElement* el) {
		int action, direction, number;
		if (!el || !partialAction) {
			return;
		}
		TiXmlElement* partialActionEl =
			el->FirstChildElement(CAP::PartialActionTag.c_str());

		while (partialActionEl) {
			partialActionEl->QueryIntAttribute("action", &action);
			if( !partialActionEl->QueryIntAttribute("direction", &direction) ) {
				direction = 255;
			}
			partialActionEl->QueryIntAttribute("number",&number);
			Debug(CAP::LogScope)
				<< " npa: "
				<< action << "(" << direction << ")#" << number;
			PartialActionPtr nextPartialAction =
				findPartialAction(action,direction,number);
			if( nextPartialAction ) {
				partialAction->addNextPartialAction( nextPartialAction );
			}
			Debug(CAP::LogScope) 
				<< " added next-partial-action: "
				<< action << "(" << direction << ")#" << number;

			partialActionEl =
				partialActionEl->NextSiblingElement(CAP::PartialActionTag.c_str());
		}
	}

	PartialActionPtr ComplexAnimationProvider::findPartialAction(
		uint8_t action, uint8_t direction, uint8_t number) {
		for (size_t i = 0; i != m_partialActions.size(); ++i) {
			PartialActionPtr& pa = m_partialActions[i];
			if (pa->getAction() == action &&
			     (pa->getDirection() == direction ||
					  pa->getDirection() == CAP::AnyDirection ||
					  direction == CAP::AnyDirection) &&
				  pa->getNumber() == number) {
				return pa;
			}
		}
		return PartialActionPtr();
	}

} } } }

