/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

#pragma once

#include <string>
#include <vector>
#include <cassert>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

class Image
{
public:
	Image();
	~Image();

	void loadImage(std::string const& file, bool pot);
	void saveImage(std::string const& file);

	// bpp, mBpp = bits
	// pitch - in bytes

	void setImage(uint32_t width, uint32_t height, uint32_t bpp);
	void updateSubimage(uint32_t xoffset, uint32_t yoffset,
		Image const& img);

	uint8_t const* getData() const;
	uint8_t* getData();
	uint32_t getImageSize() const;

	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	uint32_t getBpp() const { return mBpp; }
	uint32_t getPitch() const { return mWidth * mBpp/8; }
	std::string const& getImageName() const { return mImageName; }

private:
	void releaseData();

	void loadImageFreeImage(std::string const& file, bool pot);
	void saveImageFreeImage(std::string const& file);		

	typedef std::vector<uint8_t> PixelsData;
	PixelsData mPixData;

	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mBpp;
	std::string mImageName;
};
