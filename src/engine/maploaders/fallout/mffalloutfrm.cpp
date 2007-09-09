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
#include "mffalloutfrm.h"
#include "exception.h"
#include "mffalloutpal.h"
#include "debugutils.h"
#include "rawdata.h"
#include "settingsmanager.h"
#include "palutil.h"

#include <SDL.h>

namespace FIFE {

	MFFalloutFRM::MFFalloutFRM(const std::string& path) : m_file(path), m_data(VFS::instance()->open(m_file)), m_version(0),
	m_frames_per_direction(0), m_shifts_x(6), m_shifts_y(6), m_offsets(6), m_frame_info(6) {

		m_found_pal_animation = 0;
		m_custom_gamma = SettingsManager::instance()->read<int>("LightingLevel",4);
		if( m_custom_gamma < 1 )
			m_custom_gamma = 1;
		if( m_custom_gamma > 4 )
			m_custom_gamma = 4;

		load();
	}

	MFFalloutFRM::~MFFalloutFRM() {
		for (size_t i = 0; i < m_offsets.size(); ++i) {
			for (size_t j = 0; j < m_frame_info[i].size(); ++j) {
				delete m_frame_info[i][j];
			}
			m_frame_info[i].clear();
		}
		m_frame_info.clear();
	}

	void MFFalloutFRM::load() {
		m_data->setIndex(0);
		m_version = m_data->read32Big();

		m_frames_per_second = m_data->read16Big();
		m_action_frame_idx = m_data->read16Big();
		//	m_data->moveIndex(4);
		
		m_frames_per_direction = m_data->read16Big();
		
		// safeguard against div-by-zero; maybe it should generally be +1?
		if (m_frames_per_second == 0)
			m_frames_per_second = m_frames_per_direction / 2;

		for (size_t i = 0; i < m_shifts_x.size(); ++i) {
			m_shifts_x[i] = m_data->read16Big();
		}

		for (size_t i = 0; i < m_shifts_y.size(); ++i) {
			m_shifts_y[i] = m_data->read16Big();
		}

		for (size_t i = 0; i < m_offsets.size(); ++i)
			m_offsets[i] = m_data->read32Big();

		// not sure if mess up with the indices
		for (size_t i = 0; i < m_offsets.size(); ++i) {
			uint16_t k = m_frames_per_direction;
			unsigned int pos = 0x3e + m_offsets[i];
			m_data->setIndex(pos);
			int16_t xoff_total = m_shifts_x[i];
			int16_t yoff_total = m_shifts_y[i];
			while (k >= 1) {
				FrameInfo *fi = new FrameInfo();
				fi->width  = m_data->read16Big();
				fi->height = m_data->read16Big();
				uint32_t s = m_data->read32Big();
				if (s != static_cast<uint32_t>(fi->width * fi->
				                               height)) {
					throw InvalidFormat("Frame size mismatch in file " + m_file);
				}
				fi->xoff = m_data->read16Big();
				fi->yoff = m_data->read16Big();
				xoff_total += fi->xoff;
				fi->xoff = xoff_total;
				yoff_total += fi->yoff;
				fi->yoff += yoff_total;
				fi->fpos = m_data->getCurrentIndex();
				m_frame_info[i].push_back(fi);
				m_data->moveIndex(s);
				k--;
			}
		}
	}

	uint16_t MFFalloutFRM::getShiftX(size_t idx) const {
		if (idx >= m_shifts_x.size())
			throw IndexOverflow("no such shift");

		return *(m_shifts_x.begin() + idx);
	}

	uint16_t MFFalloutFRM::getShiftY(size_t idx) const {
		if (idx >= m_shifts_y.size())
			throw IndexOverflow("no such shift");

		return *(m_shifts_y.begin() + idx);
	}

	int16_t MFFalloutFRM::getShiftX(size_t idx, size_t frame) const {
		FrameInfo* fi = m_frame_info[idx][frame];
		return fi->xoff;
	}

	int16_t MFFalloutFRM::getShiftY(size_t idx, size_t frame) const {
		FrameInfo* fi = m_frame_info[idx][frame];
		return fi->yoff;
	}


	SDL_Surface* MFFalloutFRM::getFrame(unsigned int orientation, unsigned int frame) {
		MFFalloutPAL palette("color.pal");
		if (orientation >= m_offsets.size()) { // is this off by one?
			throw IndexOverflow("Orientation greater than 6");
		}
		if (frame > m_frames_per_direction) {
			throw IndexOverflow("Frame index greater than m_frames_per_direction");
		}

		FrameInfo* fi = m_frame_info[orientation][frame];

		m_data->setIndex(fi->fpos);
		uint32_t size = fi->width * fi->height;
		uint8_t *imgdata = new uint8_t[size];
		bool check_pal_anim = true;
		m_data->readInto(imgdata, size);
		
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, fi->width, 
			fi->height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

		SDL_LockSurface(surface);
		transferImgToSurface(imgdata,fi->width,fi->height,surface,check_pal_anim); 
		SDL_UnlockSurface(surface);

		if( !check_pal_anim )
			delete[] imgdata;

		return surface;
	}

	void MFFalloutFRM::transferImgToSurface(uint8_t* data, uint16_t width, uint16_t height, SDL_Surface* surface, bool& chk_pal_anim) {
		MFFalloutPAL palette("color.pal");
		PALUtil* palUtil = PALUtil::instance();
		uint8_t palAniID = 0;
		uint8_t* data_copy = data;

		uint32_t* pixeldata = static_cast<uint32_t*>(surface->pixels);
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				uint8_t index = *(data++);
				if (index == 0) {
					*pixeldata = 0x00000000;
				} else {
					if( chk_pal_anim )
						palAniID |= palUtil->checkPixel(index);

					uint8_t red = std::min(palette.getRed(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t green = std::min(palette.getGreen(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t blue = std::min(palette.getBlue(index) * m_custom_gamma, 0xff) & 0xff;
				
					*pixeldata = (red << 24) | (green << 16) | (blue << 8) | 0xff;
				}
			
				++pixeldata;
			}
		};
		if( palAniID != 0 ) {
			m_found_pal_animation = new PALAnimation( palAniID, data_copy, width,height,m_custom_gamma );
			chk_pal_anim = true;
		} else {
			chk_pal_anim = false;
		};
	};

	PALAnimation* MFFalloutFRM::getPalAnimation()  {
		if( m_found_pal_animation )
			if( !m_found_pal_animation->isValid() ) {
				delete m_found_pal_animation;
				m_found_pal_animation = 0;
			};
			
		return m_found_pal_animation;
	};

	uint16_t MFFalloutFRM::getFramesPerSecond() const {
		return m_frames_per_second;
	}

	uint16_t MFFalloutFRM::getActionFrameIdx() const {
		return m_action_frame_idx;
	}

	uint32_t MFFalloutFRM::getNumFrames() const {
		return m_frames_per_direction;
	}

}

//FIFE
