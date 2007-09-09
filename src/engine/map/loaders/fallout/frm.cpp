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
#include "exception.h"
#include "debugutils.h"
#include "imagecache.h"
#include "settingsmanager.h"

#include "frm.h"
#include "pal.h"
#include "palutil.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {
	// FIXME: If this is an intrinsic part of the file format, then we should
	// just use arrays of that size. Otherwise, there's a bug anyway :)
	const int FRM_MAX_FRAME_CT = 6;

	FRM::FRM(const std::string& path) : 
		m_file(path),
		m_version(0),
		m_frames_per_direction(0),
		m_directions(0),
		m_shifts_x(FRM_MAX_FRAME_CT),
		m_shifts_y(FRM_MAX_FRAME_CT),
		m_offsets(FRM_MAX_FRAME_CT),
		m_frame_info(FRM_MAX_FRAME_CT) {

		init();
		load();
	}

	void FRM::init() {
		m_custom_gamma = SettingsManager::instance()->read<int>("LightingLevel", 4);
		if (m_custom_gamma < 1) {
			m_custom_gamma = 1;
		}
		if (m_custom_gamma > 4) {
			m_custom_gamma = 4;
		}
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

	RenderAble* FRM::transferImgToSurface(uint8_t* data, uint16_t width, uint16_t height) {
		PAL palette("color.pal");
		int id = 0;
		PALUtil* palutil = PALUtil::instance();
		uint8_t* dataCopy = data;

		uint32_t size = width * height * 4;
		boost::scoped_array<uint8_t> imgdata(new uint8_t[size]);
		uint32_t* pixeldata = reinterpret_cast<uint32_t*>(imgdata.get());
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				uint8_t index = *(data++);
				if (index == 0) {
					*pixeldata = 0x00000000;
				} else {
					id |= palutil->checkPixel(index);

					uint8_t alpha = 0xff;
					if( index == 108 ) { // 108 is the transparent window pixel index
						alpha = 0x80;
					}
/** @todo: find solution: 13 should be yellow/red and transparent only for specific objects.
					else
					if( 13 == index ) {	///< 13 is transparent force-field
						alpha = 0x80;
					}
*/
					// FIXME Unify this with palutil gamma calculations. And fix them ;)
					// --zahlman
					uint8_t red = std::min(palette.getRed(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t green = std::min(palette.getGreen(index) * m_custom_gamma, 0xff) & 0xff;
					uint8_t blue = std::min(palette.getBlue(index) * m_custom_gamma, 0xff) & 0xff;

					*pixeldata = (red << 24) | (green << 16) | (blue << 8) | alpha;
				}
				++pixeldata;
			}
		}

		if (id && m_frames_per_direction == 1) {
			PALAnimation palanim(id, dataCopy, width, height, m_custom_gamma );
			if( palanim.isValid() ) {
				return palanim.generateAnimation();
			}
		}

		return CRenderBackend()->createStaticImageFromRGBA(imgdata.get(), width, height);
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

} } } }
