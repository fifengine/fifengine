/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifdef GUICHAN05
#include "gcnimageloader_v05.h"
#else

#ifndef FIFE_GCNIMAGELOADER_H
#define FIFE_GCNIMAGELOADER_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/imageloader.hpp>

// FIFE includes

namespace FIFE {

	/* Image Loader.
	 */
	class GCNImageLoader : public gcn::ImageLoader {
		public:
			/* Constructor.
			 */
			GCNImageLoader();
			/* Destructor.
			 */
			virtual ~GCNImageLoader();

			/* Prepares an image for reading.
			 *
			 * After you have called this function you can retrieve information about it and edit it.
			 *
			 * @param filename The image file to prepare.
			 */
			virtual void prepare(const std::string& filename);
			/* This function frees an image.
			 *
			 * @note There is generally no reason to call this function as it is called upon by the Image object when 	
			 * destroying an Image.
			 * @param image The file to be freed and removed.
			 */
			virtual void free(gcn::Image* image);
			/* Returns a pointer of raw data of an image.
			 *
			 * The raw data is in 32 bit RGBA format. The function will not free a prepared image, 
			 * so finalize() or discard() should be used afterwards.
			 *
			 * @return A pointer to the raw image data.
			 */
			virtual void* getRawData();
			/* Finalizes an image meaning it will return the image data.
			 *
			 * If the image contains pixels with "magic pink" (0xff00ff) they will be treated as transparent pixels.
			 *
			 * @return A pointer to the image data.
			 */
			virtual void* finalize();
			/* Discards a prepared image.
			 */
			virtual void discard();
			/* Gets the height of the image.
			 *
			 * @return Height of the image.
			 */
			virtual int getHeight() const;
			/* Gets the width of the image.
			 *
			 * @return Width of the image.
			 */
			virtual int getWidth() const;
			/* Gets the color of a pixel at coordinate x and y.
			 *
			 * @param x The X coordinate.
			 * @param y The Y coordinate.
			 */
			virtual gcn::Color getPixel(int x, int y);
			/* Puts a pixel with a certain color at coordinate x and y.
			 *
			 * @param x The X coordinate.
			 * @param y The Y coordinate.
			 * @param color The color of the pixel to put.
			 */
			virtual void putPixel(int x, int y, const gcn::Color& color);

		private:
			// The image filename.
			std::string m_filename;
			// The image ID.
			size_t m_rid;
	};

}

#endif
#endif
