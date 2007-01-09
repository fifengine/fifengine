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

#ifndef FIFE_VFSWRITEFORMAT_H
#define FIFE_VFSWRITEFORMAT_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "vfs.h"
#include "vfsdatastruct.h"

namespace FIFE {

	/** VFSWriteFormat abstract baseclass
	 *
	 * VFSWriteFormats provide access to transparently write different file formats to disk.
	 * @see VFS
	 */
	class VFSWriteFormat {
		public:
			VFSWriteFormat();
			virtual ~VFSWriteFormat();

			/** searches the list of supported formats and returns true if the format is supported.
			 * 
			 * @param format the format to test for support.
			 * @return true for supporting the format, false for not supporting it.
			 */
			bool supportsFormat(file_format* format);
			
			/** Writes a file to disk
			 * 
			 * @param data_structure the data to write to disk
			 * @param file_name the path and file to write the data to
			 * @param format the file format to write the data
			 * @return true if the file was successfully written, false if any portion failed
			 */
			 virtual bool writeFile(void* data_structure, std::string& file_name, file_format* format) = 0;
			
			/** Returns a list of the supported formats for this VFSWriteFormat
			 * 
			 * @return the list of supported formats
			 */
			format_list getSupportedFormats();
		protected:
			format_list m_supported_formats;
	};

}

#endif
