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

#ifndef FIFE_MAP_LOADERS_FALLOUT_FRM_H
#define FIFE_MAP_LOADERS_FALLOUT_FRM_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"

namespace FIFE { 
	class RawData;
	class RenderAble;

namespace map { namespace loaders { namespace fallout {

	/** Loads FRM format graphics.
	 *
	 * Uses \c RawData & \c VFS to access the FRM files (even inside DAT files) 
	 * and creates RenderAble (derived) instances.
	 *
	 * \see RawData
	 *
	 * \bug I want access to pixel data (after palette transformation);
	 * required for bitmap font support
	 *
	 *
	 * @note This class may return an Animation instead of an Image if it finds
	 * a Palette-Animation.
	 * @note Window transparency is created by setting the alpha value to 0x80
	 * for a color index of 108. This is a nice and easy solution.
	 * If this doesn't work out, (e.g. the color is used otherwise) the approach has
	 * to be rethought.
	 */
	class FRM {
		public:
			/** Constructor from a file
			 */
			FRM(const std::string& file);

			/** Destructor
			 */
			~FRM();

			/** Get a renderable for a single frame and orientation.
			 * @param direction One of fallouts 6 directions that can be encoded in FRM files
			 * @param frame One of the frames.
			 *
			 * @return A frame from an FRM. Due to PAL Animations this might as well be an Animation or an Image 
			 * The result is corrected by LightingLevel from the settings and palette transformed with "color.pal".
			 */ 
			RenderAble* getFrame(uint16_t direction = 0, uint16_t frame = 0);

			/** Get the number of frames to show per second
			 */
			uint16_t getFramesPerSecond() const;

			/** Get the frame in which an action shall take place.
			 * This means in some kick animation, this will denote the frame, where this animation
			 * will _hurt_ deal the damage, which is NOT the first frame.
			 */
			uint16_t getActionFrameIdx() const;

			/** Get the total number of frames @b per direction
			 */
			uint32_t getNumFrames() const;

			/** Get the @b global shift of one orientation.
			 * Images and Animations allways have a @b global
			 * shift associated, that is mostly used in positioning on the
			 * map. This can be requested here.
			 */
			int16_t getShiftX(size_t orientation) const;
			int16_t getShiftY(size_t orientation) const;

			/** Get the @b per-frame shift of of one frame of one orientation
			 * Frames of an animation have a shift with respect to the global
			 * shift. This can be requested here.
			 */
			int16_t getShiftX(size_t orientation, size_t frame) const;
			int16_t getShiftY(size_t orientation, size_t frame) const;

		protected:
			std::string m_file;
			RawDataPtr m_data;

			uint32_t m_version;
			uint16_t m_action_frame_idx;
			uint16_t m_frames_per_second;
			uint16_t m_frames_per_direction;
			uint16_t m_directions;

			uint8_t m_custom_gamma;

			std::vector<int16_t> m_shifts_x;
			std::vector<int16_t> m_shifts_y;
			std::vector<uint32_t> m_offsets;

			struct FrameInfo {
				RenderAble* image;
				uint16_t width;
				uint16_t height;
				int16_t  xoff;
				int16_t  yoff;
				unsigned int fpos;
			};
			// No reason to store pointers to new objects in here.
			std::vector<std::vector<FrameInfo> > m_frame_info;

			/** Initialize
			 * Reads in the custom gamma value from the settings.
			 */
			void init();

			/** Load FRM data
			 * This reads the header data for the FRM file and it is called before
			 * calling any the loadFrame function.
			 *
			 * @note The implementation is somewhat complicated as FRM files allways
			 * have 6 directions in their header. But if a minor number of directions
			 * is actually contained, the superfluous directions point to the same
			 * offset in the file.
			 */
			void load();

			/** Load one frame with the information given by a FrameInfo
			 * This sets the fi->image to a suiatable RenderAble
			 */
			void loadFrame(FrameInfo& fi);

			/** Transfer the palette indexed image data to a normal RGBA surface.
			 *
			 * This also uses the gamma value in @c m_custom_gamma
			 *
			 * Generates an Image* if the frm file doesn't contain any
			 * Palette-Animation or if the frm file contains more than 1
			 * frame per direction.
			 *
			 */
			RenderAble* transferImgToSurface(uint8_t* data, uint16_t w, uint16_t h);
	};

} } } }
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
