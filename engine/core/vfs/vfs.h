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
#include "util/singleton.h"

namespace FIFE {

	class RawData;

	class VFSSource;
	/** the main VFS (virtual file system) class
	 *
	 * The VFS is intended to provide transparent and portable access to files.
	 * @note The VFS searches for a provider in the order they are added to the
	 * VFS. Since the VFSHostSystem is added first, this implies, that host filesystem
	 * files will override whatever might be in other VFS Sources (e.g. the DAT files)
	 */
	class VFS : public DynamicSingleton<VFS> {
		public:
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

			/** Set a root directory for this filesystem, not necessarily the root directory of the drive.
			 */
			void setRootDir(const std::string& path);
			const std::string& getRootDir();
			
			/** Check if the given file exists
			 *
			 * @param file the filename
			 * @return true if it exists, false if not
			 */
			bool exists(const std::string& file) const;

			/** Open a file
			 *
			 * @param path the file to open
			 * @return the opened file; delete this when done.
			 * @throws NotFound if the file cannot be found
			 */
			RawData* open(const std::string& path);

			/** Get a filelist of the given directory
			 *
			 * @param path the directory
			 * @return the filelist
			 */
			std::vector<std::string> listFiles(const std::string& path) const;

			/** List the files of a given directory matching a regex
			 *
			 * The whole string has to match the regex, this means if you want all files that end with .map don't search for
			 * "\.map" but ".*\.map" (and escape the \)
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			std::vector<std::string> listFiles(const std::string& path, const std::string& filterregex) const;

			/** Get a directorylist of the given directory
			 *
			 * @param path the directory
			 * @return the directorylist
			 */
			std::vector<std::string> listDirectories(const std::string& path) const;

			/** List the subdirectorys of a given directory matching a regex
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			std::vector<std::string> listDirectories(const std::string& path, const std::string& filterregex) const;

		private:
			typedef std::vector<VFSSource*> type_sources;
			type_sources m_sources;

			std::string m_root;

			void filterList(std::vector<std::string>& list, const std::string& regex) const;
			std::string lower(const std::string&) const;
			VFSSource* getSourceForFile(const std::string& file) const;
	};

}

#endif
