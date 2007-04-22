#include <string>
#include <boost/lexical_cast.hpp>
#include "animation.h"
#include "exception.h"
//#include "imagecache.h"
#include "graphix_primitives.h"

namespace FIFE {

	Animation::Animation(size_t numFrames) : frames(numFrames), 
																					 m_shifts_x(numFrames),
																					 m_shifts_y(numFrames) { 
																						 currentFrame = 0;
																						 frameChangedAt = 0;
																						 frameDuration = 0;
																						 animateForward = true;
																					 }
	Animation::~Animation() {}

	void Animation::setFrame(size_t num, size_t cacheId, int16_t x_shift, int16_t y_shift) {
		if (num >= frames.size()) {
			throw IndexOverflow(boost::lexical_cast<std::string>(num) + std::string(" >= animation size ") +
					boost::lexical_cast<std::string>(frames.size()));
		}
		frames[num] = cacheId;
		m_shifts_x[num] = x_shift;
		m_shifts_y[num] = y_shift;
	}
	void Animation::setFrameDuration(Uint32 length) {
		frameDuration = length;
	}
	void Animation::setCurrentFrame(size_t num) {
		if (num >= frames.size()) {
			throw IndexOverflow(boost::lexical_cast<std::string>(num) + std::string(" >= animation size ") + 
					boost::lexical_cast<std::string>(frames.size()));
		}
		currentFrame = num;
	}
	void Animation::update(Uint32 nowTicks) {
		if (nowTicks - frameChangedAt >= frameDuration) {
			flipFrame();
			frameChangedAt = nowTicks;
		}
	}
	/*
		 void Animation::render(SDL_Rect *target, SDL_Surface *screen) {
		 Image* img = ImageCache::instance()->getImage(currentFrame);
		 img->render(target, screen);
		 }
		 void Animation::render(SDL_Rect *target, SDL_Surface *screen, Uint8 alpha) {
		 Image* img = ImageCache::instance()->getImage(currentFrame);
		 img->render(target, screen, alpha);
		 }*/
	size_t Animation::getCurrentFrame() {
		return frames[currentFrame];
	}
	int16_t Animation::getCurrentShiftX() {
		return m_shifts_x[currentFrame];
	}
	int16_t Animation::getCurrentShiftY() {
		return m_shifts_y[currentFrame];
	}
	void Animation::flipFrame() {
		switch(animateForward) {
			case true:
				if (currentFrame < frames.size() - 1)
					currentFrame++;
				else {
					animateForward = false;
				}
				break;
			case false:
				if (currentFrame > 0)
					currentFrame--;
				else {
					animateForward = true;
				}
				break;
		}

		// just forward ?
		/*
			 if (currentFrame < frames.size() - 1)
			 currentFrame++;
			 else
			 currentFrame = 0;
			 */
	}

	ComplexAnimation::ComplexAnimation() {
		nowAction = nowDirection = 0;
	}
	ComplexAnimation::~ComplexAnimation() {
		cleanup();
	}
	void ComplexAnimation::cleanup() {
		T_animByAction::iterator i = content.begin();
		while (i != content.end()) {
			T_animByDirection::iterator j = i->second.begin();
			while (j != i->second.end()) {
				delete j->second;
				j++;
			}
			i->second.clear();
			i++;
		}
		content.clear();
	}
	void ComplexAnimation::registerAnimation(uint8_t action, uint8_t direction, Animation *anim) {
		T_animByAction::iterator i = content.find(action);
		// thow if i == content.end()
		T_animByDirection::iterator j = i->second.find(direction);
		// throw if j == i.second.end()
		assert(anim != NULL);
		j->second = anim;
	}
	void ComplexAnimation::setup(uint8_t startAction, uint8_t startDirection) {
		nowAction = startAction;
		nowDirection = startDirection;
		// actually find it?
		activeAnimation = content.find(nowAction)->second.find(nowDirection)->second;
	}
	void ComplexAnimation::setDirection(uint8_t directionId) {
		// check if it exists
		T_animByAction::iterator i = content.find(nowAction);
		T_animByDirection::iterator j = i->second.find(directionId);
	}
	void ComplexAnimation::setAction(uint8_t actionId) {
		T_animByAction::iterator i = content.find(actionId);
		activeAnimation = i->second.find(nowDirection)->second;
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
