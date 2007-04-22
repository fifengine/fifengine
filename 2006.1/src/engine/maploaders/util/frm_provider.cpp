#include "frm_provider.h"

#include "../fallout/mffalloutfrm.h"
#include "animation.h"
#include "image.h"

#include "log.h"

namespace FIFE {

	FRMProvider::FRMProvider() : RenderableProvider(RenderableLocation()) {
	}

	FRMProvider::~FRMProvider() {
	}

	FRMProvider::FRMProvider(const RenderableLocation& location) :RenderableProvider(location) {
	}

	RenderAble* FRMProvider::createRenderable() {
		MFFalloutFRM frm(getLocation().toFilename());

		uint16_t dir = 0;
		uint16_t frame = 0; //getLocation().getFrame();

		if( getLocation().getType() == RenderableLocation::RL_FILE_FRAME_DIR ) {
			dir = getLocation().getDirection();
			frame = getLocation().getFrame();
		}

		if( getLocation().getType() == RenderableLocation::RL_FILE_FRAME ) {
			frame = getLocation().getFrame();
		}
		

		if( frm.getNumFrames() > 1 ) { // Animation!
			Animation* result = new Animation(frm.getNumFrames());

			for (uint32_t j=0; j < frm.getNumFrames() ; j++) {
				Image* image = dynamic_cast<Image*>(frm.getFrame(dir,j));

				if( image == 0 ) {
					Log("frm_provider") 
						<< getLocation().toString() << "  ... not loaded";
					delete result;
					return 0;
				}
				// Image has X/Y shifts set to per frame offset.
				result->setFrame(j, image );
			}
	
			// set the global -per- renderable shift
			result->setXShift(frm.getShiftX(dir));
			result->setYShift(frm.getShiftY(dir));
			
			result->setActionFrame(frm.getActionFrameIdx());
	
			result->setCurrentFrame(0);
			uint16_t frm_fps = frm.getFramesPerSecond();
			result->setFrameDuration(1000/frm_fps);

			return result;

		} else {
			RenderAble*  image = frm.getFrame( dir, frame );

			// set the global -per- renderable shift

			image->setXShift(frm.getShiftX(dir));
			image->setYShift(frm.getShiftY(dir));

			return image;
		}
	}

}
