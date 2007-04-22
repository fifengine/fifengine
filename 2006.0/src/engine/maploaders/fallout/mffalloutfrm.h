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
#ifndef FRMLOADER_H
#define FRMLOADER_H

#include <string>
#include <vector>
#include <stdint.h>
#include <SDL_video.h>

#include "vfs.h"

namespace FIFE {

	class RawData;
	class PALAnimation;

	class MFFalloutFRM {
		public:
			MFFalloutFRM(const std::string& file);
			~MFFalloutFRM();

			SDL_Surface* getFrame(unsigned int orientation = 0, unsigned int frame = 0);

			uint16_t getFramesPerSecond() const;
			uint16_t getActionFrameIdx() const;
			uint32_t getNumFrames() const;

			uint16_t getShiftX(size_t orientation) const;
			uint16_t getShiftY(size_t orientation) const;
			int16_t getShiftX(size_t orientation, size_t frame) const;
			int16_t getShiftY(size_t orientation, size_t frame) const;

			PALAnimation* getPalAnimation();
		private:
			std::string m_file;
			RawDataPtr m_data;

			uint32_t m_version;
			uint16_t m_action_frame_idx;
			uint16_t m_frames_per_second;
			uint16_t m_frames_per_direction;
			PALAnimation* m_found_pal_animation;

			uint8_t m_custom_gamma;

			// I am pretty sure the next two need to be signed (skyb)
			std::vector<int16_t> m_shifts_x;
			std::vector<int16_t> m_shifts_y;
			std::vector<uint32_t> m_offsets;

			struct FrameInfo {
				uint16_t width;
				uint16_t height;
				int16_t  xoff;
				int16_t  yoff;
				unsigned int fpos;
			};
			std::vector<std::vector<FrameInfo*> > m_frame_info;

			void load();
			void transferImgToSurface(uint8_t* data, uint16_t w, uint16_t h, SDL_Surface* surface, bool& chk_pal_anim);
	};

}//FIFE


#endif
