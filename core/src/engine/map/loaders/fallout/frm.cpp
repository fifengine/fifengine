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
#include <cstring>
#include <algorithm>

// 3rd party library includes
#include <boost/scoped_array.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "video/animation.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "util/exception.h"
#include "debugutils.h"
#include "imagecache.h"
#include "settingsmanager.h"

#include "frm.h"
#include "animatedpal.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	// FIXME: If this is an intrinsic part of the file format, then we should
	// just use arrays of that size. Otherwise, there's a bug anyway :)
	const int FRM_MAX_FRAME_CT = 6;

	FRM::FRM(const std::string& path, AnimatedPalette* palette) : 
		m_file(path),
		m_palette(palette),
		m_version(0),
		m_frames_per_direction(0),
		m_directions(0),
		m_shifts_x(FRM_MAX_FRAME_CT),
		m_shifts_y(FRM_MAX_FRAME_CT),
		m_offsets(FRM_MAX_FRAME_CT),
		m_frame_info(FRM_MAX_FRAME_CT),
		m_override(&ColorOverride::EMPTY_OVERRIDE) {

		init();
		load();
	}

	void FRM::init() {
		// This isn't very nice, but there's no single static place to load
		// the overrides. Load it here, buy only once.
		if (!loaded_overrides) {
			loaded_overrides = true;
			ColorOverride::load("content/gfx/fallout_overrides.xml", overrides);
		}
		
		ColorOverride::OverrideMap::iterator i = overrides.find(m_file);
		if (i != overrides.end()) {
			m_override = &i->second;
		}
				
		m_light_level = SettingsManager::instance()->read<int>("LightingLevel", 4);
		if (m_light_level < 1) {
			m_light_level = 1;
		}
		if (m_light_level > 4) {
			m_light_level = 4;
		}
		
		m_palette->setLightLevel(m_light_level);
	}

	FRM::~FRM() {
		m_data.reset();
	}

	RenderAble* FRM::getFrame(uint16_t dir, uint16_t frame) {
		FrameInfo fi = m_frame_info[dir][frame];
		loadFrame(fi);
		return fi.image;
	}

	void FRM::load() {
		m_data = VFS::instance()->open(m_file);

		m_data->setIndex(0);
		m_version = m_data->read32Big();

		m_frames_per_second = m_data->read16Big();
		m_action_frame_idx = m_data->read16Big();

		m_frames_per_direction = m_data->read16Big();

		// safeguard against div-by-zero; maybe it should generally be +1?
		if (m_frames_per_second == 0) {
			m_frames_per_second = 50; //m_frames_per_direction / 2;
		}

		for (size_t i = 0; i < m_shifts_x.size(); ++i) {
			m_shifts_x[i] = m_data->read16Big();
		}

		for (size_t i = 0; i < m_shifts_y.size(); ++i) {
			m_shifts_y[i] = m_data->read16Big();
		}

		for (size_t i = 0; i < m_offsets.size(); ++i) {
			m_offsets[i] = m_data->read32Big();
		}

		// not sure if mess up with the indices
		for (size_t i = 0; i < m_offsets.size(); ++i) {
			unsigned int pos = 0x3e + m_offsets[i];
			m_data->setIndex(pos);
			int16_t xoff_total = 0;
			int16_t yoff_total = 0;
			for (uint16_t k = 0; k < m_frames_per_direction; ++k) {

				FrameInfo fi;
				fi.width  = m_data->read16Big();
				fi.height = m_data->read16Big();
				uint32_t s = m_data->read32Big();
				if (s != static_cast<uint32_t>(fi.width * fi.height)) {
					// FIXME cleanup?
					throw InvalidFormat("Frame size mismatch in file " + m_file);
				}
				xoff_total += m_data->read16Big();
				fi.xoff = xoff_total;
				yoff_total += m_data->read16Big();
				fi.yoff = yoff_total;
				fi.fpos = m_data->getCurrentIndex();
				fi.image = 0;

				m_data->moveIndex(s);
				m_frame_info[i].push_back(fi);
			}
		}

		// adds the offset ref fix (formerly in loadFrames()):
		m_directions = 0;
		for (size_t i = 0; i < m_offsets.size(); ++i) {
			uint32_t offset =  m_offsets[i];
			size_t ref_i = i;
			for(size_t j =0; j<i; ++j) {
				if(offset == m_offsets[j]) {
					ref_i = j;
				}
			}
			for (size_t k = 0; k < m_frames_per_direction; ++k) {
				if (ref_i < i) {
					m_frame_info[i][k].fpos = m_frame_info[ref_i][k].fpos;
					m_frame_info[i][k].xoff = m_frame_info[ref_i][k].xoff;
					m_frame_info[i][k].yoff = m_frame_info[ref_i][k].yoff;
					m_frame_info[i][k].width = m_frame_info[ref_i][k].width;
					m_frame_info[i][k].height = m_frame_info[ref_i][k].height;
				} else {
					++m_directions;
				}
			}
		}
		m_directions /= m_frames_per_direction;
	}

	int16_t FRM::getShiftX(size_t idx) const {
		if (idx >= m_shifts_x.size())
			throw IndexOverflow("no such shift");

		return *(m_shifts_x.begin() + idx);
	}

	int16_t FRM::getShiftY(size_t idx) const {
		if (idx >= m_shifts_y.size())
			throw IndexOverflow("no such shift");

		return *(m_shifts_y.begin() + idx);
	}

	int16_t FRM::getShiftX(size_t idx, size_t frame) const {
		return m_frame_info[idx][frame].xoff;
	}

	int16_t FRM::getShiftY(size_t idx, size_t frame) const {
		return m_frame_info[idx][frame].yoff;
	}

	void FRM::loadFrame(FrameInfo& fi) {
		if (fi.image) {
			return;
		}
		uint32_t size = fi.width * fi.height;
		m_data->setIndex(fi.fpos);
		boost::scoped_array<uint8_t> imgdata(new uint8_t[size]);
		m_data->readInto(imgdata.get(), size);
		RenderAble* img = transferImgToSurface(imgdata.get(), fi.width, fi.height);
		img->setXShift(fi.xoff);
		img->setYShift(fi.yoff);
		fi.image = img;
	}
	
	
	/**
	  Creates an SDL_Surface with pixel data taken from the given 8-bit pixel data and palette.
	  The returned surface's format will be RGBA if withAlphaChannel is true,
	  or 8-bit indexed surface if withAlphaChannel is false.
	*/
	SDL_Surface* createSDLSurface(uint8_t* data, uint16_t width, uint16_t height, const AnimatedPalette* palette, const ColorOverride* override) 
	{
		SDL_Surface* image = 0;
		
		if (!override->areAllOverridesOpaque()) {			
			image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
			if (image == 0) {
				return 0;
			}
			// load pixels into image
			SDL_LockSurface(image);

			int wastedspace = image->pitch - image->format->BytesPerPixel * width;
			uint8_t* from = data;
			uint32_t* pixeldata = static_cast<uint32_t*>(image->pixels);
			
			RGBA rgba;
						
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					uint8_t index = *(from++);

					rgba.r = palette->getRed(index);
					rgba.g = palette->getGreen(index);
					rgba.b = palette->getBlue(index);
					rgba.a = (index == 0) ? 0 : 255;

					override->getOverride(index, rgba);					
					*pixeldata = (rgba.r << 24) | (rgba.g << 16) | (rgba.b << 8) | rgba.a;
					
					++pixeldata;
				}
				pixeldata += wastedspace;
			}
			SDL_UnlockSurface(image);
		
		} else {
		
			// Create an SDL palette palette
			SDL_Color colors[256];
			
			for (int i = 0; i < 256; i++) {
				RGBA rgba;
				rgba.r = palette->getRed(i);
				rgba.g = palette->getGreen(i);
				rgba.b = palette->getBlue(i);
				rgba.a = 255;

				override->getOverride(i, rgba);
				
				colors[i].r = rgba.r;
				colors[i].g = rgba.g;
				colors[i].b = rgba.b;
			}
		
			// create 8-bit palette surface
			image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);
			if (image == 0) {
				return 0;
			}
			SDL_SetColors(image, colors, 0, 256);
		
			// set color key. 0 is transparent
			SDL_SetColorKey(image, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
			
			// load pixels into image
			SDL_LockSurface(image);
			if (image->pitch != width) {
				// have to copy a line at a time
			
				uint8_t* from = data;
				uint8_t* to = static_cast<uint8_t*>(image->pixels);
				for (int line = 0; line < height; line++) {
					std::memcpy(to, from, width);
				
					from += width;
					to += image->pitch;
				}
			
			} else {
				std::memcpy(image->pixels, data, width * height);
			}
			SDL_UnlockSurface(image);
		}
		
		return image;
	}
	
	

	RenderAble* FRM::transferImgToSurface(uint8_t* data, uint16_t width, uint16_t height) {
		int size = width * height;
		
		// Check for animated pixels. Can only animate one set of pixels right now. Do not animate
		// if more sets are found.
		const AnimatedBlock* block = 0;
		for (int i = 0; i < size; i++) {
			const AnimatedBlock* b = m_palette->getBlock(data[i]);
			if (b != 0 && b != block) {
				if (block == 0) {
					// first pixel of any animation block found, remember it
					block = b;
				} else {
					// found a pixel from a second animation block. Can't animate the palette.
					block = 0;
					break;
				}
			} 		
		}
		m_palette->setCurrentAnimation(block);
				
		if (block == 0) {
			// No animation. Create one image.
			SDL_Surface* image = createSDLSurface(data, width, height, m_palette, m_override);
			if (image != 0) {
				return CRenderBackend()->createStaticImageFromSDL(image);
			}
		} else {
			// Create an animation.
			Log("palanim") 
				<< "Generating Animation " << block->getName() 
				<< " with w,h= " << width << ", " << height 
				<< " frames: " << block->getNumFrames()
				<< " delay:  " << block->getFrameDuration();

			Animation *anim = new Animation(block->getNumFrames());
			anim->setFrameDuration(block->getFrameDuration());
	
			for(int i = 0; i < block->getNumFrames(); ++i) {			
				m_palette->setCurrentFrame(i);

				SDL_Surface* image = createSDLSurface(data, width, height, m_palette, m_override);
				Image* createdImage = 0;
				if (image != 0) {
					createdImage = CRenderBackend()->createStaticImageFromSDL(image);
				}
			
				if (!createdImage) {
					PANIC_PRINT("image == NULL");
				}
				anim->setFrame(i, createdImage);
				
			}
			anim->setCurrentFrame(0);
			return anim;
		}
		
		return 0;
	}

	uint16_t FRM::getFramesPerSecond() const {
		return m_frames_per_second;
	}

	uint16_t FRM::getActionFrameIdx() const {
		return m_action_frame_idx;
	}

	uint32_t FRM::getNumFrames() const {
		return m_frames_per_direction;
	}
	
	bool FRM::loaded_overrides = false;
	std::map<std::string, ColorOverride> FRM::overrides;

} } } }
