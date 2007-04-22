#include "advimage.h"

#include "video/renderable.h"
#include "video/animation.h"
#include "imagecache.h"

namespace gcn {
	AdvImage::AdvImage(const std::string& filename) : Image(filename) {
	}

	AdvImage::~AdvImage() {
	}

	FIFE::Animation* AdvImage::getImageAsAnimation() {
		FIFE::RenderAble* ra =  FIFE::ImageCache::instance()->getImage(
				 reinterpret_cast<size_t>(_getData()));
		if (ra->getType() == FIFE::RenderAble::RT_ANIMATION) {
			FIFE::Animation* a = dynamic_cast<FIFE::Animation*>(ra);
			return a;
		}
		return NULL;
	}


	void AdvImage::setAnimActive(bool active) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a)
			return;
		a->setIsActive(active);
	}

	void AdvImage::setAnimDirection(bool forward) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a)
			return;
		a->setDirection(forward);
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
