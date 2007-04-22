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

#ifndef FIFE_RAWWRITETEXT_H
#define FIFE_RAWWRITETEXT_H

// Standard C++ library includes
#include <fstream>
#include <iostream>
#include <string>

// 3rd party library includes
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

// FIFE includes
#include "vfs/vfsdatastruct.h"

namespace FIFE {

	class RawWriteText {
	public:
		/** Constructor, opens and/or creates the file when the object is created.
		 *  There should be 1 RawDataWrite in existence for each file being written.
		 * @param path the path and filename to write the file to
		 * @param flags the specific flags to use to open the file in addition to the default
		 */
		RawWriteText(std::string& path);
		/** Destructor.  When called will ensure the file is closed and flushed to the disk
		 */
		~RawWriteText();
		/** Writes a single line to the file, terminated by '\n'
		 * @param buffer the buffer to write from.  Note that the buffer can contain multiple lines,
		 * however, only the first line will be written, including the terminating '\n'
		 */
		void writeLine(std::string& buffer);
		/** Writes a specified amount of data to the file
		 * @param buffer the data to write to the file
		 * @param start_pos the position to start writing from
		 * @param len the length of the data to write to the file from the buffer
		 */
		void write(std::string& buffer, size_t start_pos, size_t len);
		/** Write a new line to the file
		 */
		void newLine();
		/** Templated write function writes the entire passed object to the file.
		 * No error checking is performed, it is just passed directly across.
		 * \note the object must have operator<< overloaded in order to function properly.
		 * @param buffer The object to write.
		 */
		template <class T>
		void rawWrite(T buffer){
			m_file << buffer;
		}
		/** Closes a file and flushes it to disk 
		 * \warning This operation will delete the current object, making
		 * it unusable if this is explicitly called
		 */
		void close();
	private:
		//typedef boost::filesystem::ofstream output_file;
		//output_file m_file;
		std::ofstream m_file;
	};

}

#endif
