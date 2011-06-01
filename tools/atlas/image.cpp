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

#include "image.h"

#include <memory.h>
#include <FreeImage.h>

// -------------------------------------------------------------------------
Image::Image()
: mWidth(0), mHeight(0), mBpp(0)
{
	mPixData.clear();
}
// -------------------------------------------------------------------------
Image::~Image()
{
	//mPixData.clear();
}
// -------------------------------------------------------------------------
void Image::releaseData()
{
	mPixData.clear();
}
// -------------------------------------------------------------------------
void Image::loadImage(std::string const& file, bool pot)
{
	releaseData();
	loadImageFreeImage(file, pot);
	mImageName = file;
}
// -------------------------------------------------------------------------
void Image::saveImage(std::string const& file)
{
	assert(!mPixData.empty());
	saveImageFreeImage(file);
}
// -------------------------------------------------------------------------
void Image::setImage(uint32_t width, uint32_t height, uint32_t bpp)
{
	assert(width > 0);
	assert(height > 0);
	assert(!(bpp % 8));

	// new black-transparent image
	mPixData.clear();
	mPixData.resize(width * height * bpp/8);
	memset(&mPixData[0], 0, width*height*bpp/8);

	// set new parameters
	mWidth = width;
	mHeight = height;
	mBpp = bpp;
}
// -------------------------------------------------------------------------
void Image::updateSubimage(uint32_t xoffset, uint32_t yoffset,
	Image const& img)
{
	assert(xoffset + img.getWidth() <= mWidth);
	assert(yoffset + img.getHeight() <= mHeight);
	assert(img.getData());

	// change yoffset so the 0,0 is in the top,left not bottom,left
	yoffset = mHeight - yoffset - img.getHeight();

	uint32_t srcPitch = img.getPitch();
	uint32_t dstPitch = getPitch();
	uint8_t* pDst = getData() + yoffset * dstPitch;

	for(uint32_t y = 0; y < img.getHeight(); ++y)
	{
		uint8_t const* pSrc = img.getData() + (y * srcPitch);
		memcpy(pDst + xoffset * mBpp/8, pSrc, srcPitch);
		//memset(pDst + xoffset * mBpp/8, 127, srcPitch);
		pDst += dstPitch;
	}
}
// -------------------------------------------------------------------------
uint8_t const* Image::getData() const
{
	return &mPixData[0];
}
// -------------------------------------------------------------------------
uint8_t* Image::getData()
{
	return &mPixData[0];
}
// -------------------------------------------------------------------------
uint32_t Image::getImageSize() const
{
	return getHeight() * getPitch();
}
// -------------------------------------------------------------------------
void Image::loadImageFreeImage(std::string const& file, bool pot)
{
	// check file format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file.c_str(), 0);
	if(fif == FIF_UNKNOWN)
	{
		fif = FreeImage_GetFIFFromFilename(file.c_str());
		if(fif == FIF_UNKNOWN)
		{
			throw std::string("Unknown image format of %s") + file;
		}
	}


	// if the file format is supported by FreeImage open it
	FIBITMAP* dib;
	if(FreeImage_FIFSupportsReading(fif))
	{
		dib = FreeImage_Load(fif, file.c_str());

		// something went wrong
		if(!dib)
		{
			throw std::string("Error occurred during opening ") + file;
		}
	}
	else
	{
		throw std::string("Image format of ") + file + std::string("isn't supported.");
	}

	// get basic parameters
	mWidth = FreeImage_GetWidth(dib);
	mHeight = FreeImage_GetHeight(dib);

	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(dib);
	mBpp = FreeImage_GetBPP(dib);

	if(imageType != FIT_BITMAP)
		throw std::string("Unsupport file format.");
		
	// Note: FreeImage loads rgba as bgra.

	// All images in atlas have to be in the same format
	if(mBpp != 32)
	{
		FIBITMAP* newDib = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(dib);
		dib = newDib;

		mBpp = FreeImage_GetBPP(dib);
		colourType = FreeImage_GetColorType(dib);
	}

	if(pot)
	{
		bool perform = false;

		// check if width is not power of 2
		if((mWidth & (mWidth - 1)) != 0)
		{
			uint32_t powof2 = 1;
			while(powof2 < mWidth) powof2 <<= 1;
			mWidth = powof2;
			perform = true;
		}

		// check if height is not power of 2
		if((mHeight & (mHeight - 1)) != 0)
		{
			uint32_t powof2 = 1;
			while(powof2 < mHeight) powof2 <<= 1;
			mHeight = powof2;
			perform = true;
		}

		if(perform)
		{
			FIBITMAP* newDib = FreeImage_Rescale(dib, mWidth, mHeight, FILTER_BSPLINE);
			FreeImage_Unload(dib);
			dib = newDib;
		}
	}

	uint8_t* srcData = FreeImage_GetBits(dib);
	int srcPitch = FreeImage_GetPitch(dib); // no. of rows * bytes/pixel
	uint32_t dstPitch = mWidth * mBpp/8;

	mPixData.resize(dstPitch * mHeight);

	// copy data (dont need to be vertically flipped)
	uint8_t* pSrc;
	uint8_t* pDst = &mPixData[0];
	memset(pDst, 0, dstPitch * mHeight);

	for(uint32_t y = 0; y < mHeight; ++y)
	{
		pSrc = srcData + (y * srcPitch);
		memcpy(pDst, pSrc, dstPitch);
		pDst += dstPitch;
	}

	FreeImage_Unload(dib);
}
// -------------------------------------------------------------------------
void Image::saveImageFreeImage(std::string const& file)
{
	FREE_IMAGE_TYPE imageType = FIT_BITMAP;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(file.c_str());
	if(fif == FIF_UNKNOWN)
		throw std::string("Unknown image format of %s") + file;

	// Check support for this image type & bit depth
	if(!FreeImage_FIFSupportsExportType(fif, imageType) ||
		!FreeImage_FIFSupportsExportBPP(fif, mBpp))
	{
		throw std::string("Image format isn't supported.");
	}

	FIBITMAP* dib = FreeImage_AllocateT(
		imageType,
		static_cast<int>(mWidth),
		static_cast<int>(mHeight),
		static_cast<int>(mBpp));

	if(!dib)
		throw std::string("Out of memory");

	uint8_t* srcData = (uint8_t*)getData();
	uint32_t dstPitch = FreeImage_GetPitch(dib);
	uint32_t srcPitch = getPitch();

	// Copy data, invert scanlines and respect FreeImage pitch
	uint8_t* pSrc;
	uint8_t* pDst = FreeImage_GetBits(dib);

	for(uint32_t y = 0; y < mHeight; ++y)
	{
		pSrc = srcData + (y * srcPitch);
		memcpy(pDst, pSrc, srcPitch);
		pDst += dstPitch;
	}
	//pDst = FreeImage_GetBits(dib);

	FreeImage_Save(fif, dib, file.c_str());
	FreeImage_Unload(dib);
}
//---------------------------------------------------------------------
