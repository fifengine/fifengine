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

#ifndef FIFE_VFS_VFS_H
#define FIFE_VFS_VFS_H

// Standard C++ library includes
#include <set>
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "singleton.h"

#include "vfsdatastruct.h"

namespace FIFE {

	class RawData;
	typedef ::boost::shared_ptr<RawData> RawDataPtr;

	class VFSSource;
	class VFSWriteFormat;
	/** the main VFS (virtual file system) class
	 *
	 * The VFS is intended to provide transparent and portable access to files.
	 * @note The VFS searches for a provider in the order they are added to the
	 * VFS. Since the VFSHostSystem is added first, this implies, that host filesystem
	 * files will override whatever might be in other VFS Sources (e.g. the DAT files)
	 */
	class VFS : public DynamicSingleton<VFS> {
		public:
			typedef std::set<std::string> type_stringlist;

			/** Constructor
			 * Called by the Engine on startup. Never create one yourself.
			 */
			VFS();

			/** Destructor
			 */
			virtual ~VFS();

			void cleanup();

			/** Add a new VFSSource */
			void addSource(VFSSource* source);

			/** remove a VFSSource */
			void removeSource(VFSSource* source);
			
			/** Add a new VFSWriteFormat */
			void addFormat(VFSWriteFormat* format);
			
			/** remove a VFSWriteFormat */
			void removeFormat(VFSWriteFormat* format);

			/** Check if the given file exists
			 *
			 * @param file the filename
			 * @return true if it exists, false if not
			 */
			bool exists(const std::string& file) const;

			/** Open a file
			 *
			 * @param path the file to open
			 * @return the opened file (note: dont delete this - it's referenced count and will be deleted automatically)
			 * @throws NotFound if the file cannot be found
			 */
			RawDataPtr open(const std::string& path);

			/** Get a filelist of the given directory
			 *
			 * @param path the directory
			 * @return the filelist
			 */
			type_stringlist listFiles(const std::string& path) const;

			/** List the files of a given directory matching a regex
			 *
			 * The whole string has to match the regex, this means if you want all files that end with .map don't search for
			 * "\.map" but ".*\.map" (and escape the \)
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			type_stringlist listFiles(const std::string& path, const std::string& filterregex) const;


			/** Get a directorylist of the given directory
			 *
			 * @param path the directory
			 * @return the directorylist
			 */
			type_stringlist listDirectories(const std::string& path) const;

			/** List the subdirectorys of a given directory matching a regex
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			type_stringlist listDirectories(const std::string& path, const std::string& filterregex) const;

			/** Write to a file.
			 *  Writes the specified data to a file using the appropriate format.
			 * 
			 * @param data_structure the data structure to be written to disk.
			 * @param file_name the path and file name to write the data to.
			 * @param format the file format to use to write the data.
			 * @throws InvalidDataFormat if the data structure and file format do not match.
			 */
			bool fileWrite(void* data_structure, std::string& file_name, file_format* format);
			/** Finds a file format that matches the search string.
			 *  Searches the names and extensions of supported formats for one that 
			 *  matches the supplied string and returns the file format struct.
			 * @param search_string the string to search for
			 * @return the file_format instance that matches
			 */
			file_format* findFileFormat(std::string search_string);
		private:
			typedef std::vector<VFSSource*> type_sources;
			typedef std::vector<VFSWriteFormat*> type_formats;
			type_sources m_sources;
			type_formats m_write_formats;
			format_list m_supported_formats;

			void filterList(type_stringlist& list, const std::string& regex) const;
			/** Add a list of supported formats for a VFSWriteFormat to the VFS list */
			void addSupportedFormat(VFSWriteFormat* format);
			/** Remove a list of supported formats for a VFSWriteFormat from the VFS list */
			void removeSupportedFormat(VFSWriteFormat* format);
			std::string lower(const std::string&) const;
			VFSSource* getSourceForFile(const std::string& file) const;
			VFSWriteFormat* getWriteFormat(file_format* format) const;
	};

}

#endif
