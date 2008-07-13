/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FRM_H_
#define FRM_H_

// Some constant offset in the frm file for easier readability of the later code
// The documentation use both "direction" and "orientation". I stick to "orientation"
#define FRM_OFFSET_VERSION_NR           0x00
#define FRM_OFFSET_FRAMESPERORIENT      0x08
#define FRM_OFFSET_SHIFTX_ORIENT0       0x0A
#define FRM_OFFSET_SHIFTX_ORIENT1       0x0C
#define FRM_OFFSET_SHIFTX_ORIENT2       0x0E
#define FRM_OFFSET_SHIFTX_ORIENT3       0x10
#define FRM_OFFSET_SHIFTX_ORIENT4       0x12
#define FRM_OFFSET_SHIFTX_ORIENT5       0x14
#define FRM_OFFSET_SHIFTY_ORIENT0       0x16
#define FRM_OFFSET_SHIFTY_ORIENT1       0x18
#define FRM_OFFSET_SHIFTY_ORIENT2       0x1A
#define FRM_OFFSET_SHIFTY_ORIENT3       0x1C
#define FRM_OFFSET_SHIFTY_ORIENT4       0x1E
#define FRM_OFFSET_SHIFTY_ORIENT5       0x20
#define FRM_OFFSET_FIRSTFRAME_ORIENT0   0x22
#define FRM_OFFSET_FIRSTFRAME_ORIENT1   0x26
#define FRM_OFFSET_FIRSTFRAME_ORIENT2   0x2A
#define FRM_OFFSET_FIRSTFRAME_ORIENT3   0x2E
#define FRM_OFFSET_FIRSTFRAME_ORIENT4   0x32
#define FRM_OFFSET_FIRSTFRAME_ORIENT5   0x36
#define FRM_OFFSET_FRAMEAREASIZE        0x3A
#define FRM_OFFSET_FRAMEDATA            0x3E

// These are relative offset
// Example: The absolute offset of the height of frame 0 is therefore:
//          0x40 == FRM_OFFSET_FRAMEDATA + FRMFRAME_OFFSET_HEIGHT
#define FRMFRAME_OFFSET_WIDTH           0x00
#define FRMFRAME_OFFSET_HEIGHT          0x02
#define FRMFRAME_OFFSET_SIZE            0x04
#define FRMFRAME_OFFSET_XOFF            0x08
#define FRMFRAME_OFFSET_YOFF            0x0A
#define FRMFRAME_OFFSET_DATA            0x0C

typedef struct {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} frm_color;


typedef struct _frm_frame {
	uint16_t width;       // self-explanatory
	uint16_t height;      // 
	uint32_t framesize;   // Should be equal to width*height
	uint8_t  orient;      // The orientation of the frame.
	unsigned char **rowptr; // the actual image data. The byte value is in index in a palette (pal.h)
	
	struct _frm_frame *next_frame; // Pointer to the next frame. NULL ends the list :)
} frm_frame;

typedef struct {
	uint16_t frameperorient; // FramesPerOrientation at offset 0x8 (size: two bytes) in the frm file
	uint32_t frameareasize; // Frameareasize at offset 0x3A (size: four bytes) in the frm file 
	                     // I haven't checked, if this value is always correct. If it is, we should
	                     // use the information for the memory allocation.

	uint32_t nrofframes;  // The number of frames in this animation 
	
	frm_color *palette; // A pointer to the palette. Normally the one from pal.h
	
	frm_frame *frames;  // A single linked list of frm_frame. Each frm_frame describes one frame
	                    // There should be nrofframes many.
} frm_anim;



frm_anim *loadFRMAnim(FILE *frmfile);
void freefrmanim(frm_anim *anim);

#endif /*FRM_H_*/
