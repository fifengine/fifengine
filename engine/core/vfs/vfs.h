/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_VFS_VFS_H
#define FIFE_VFS_VFS_H

// Standard C++ library includes
#include <string>
#include <vector>
#include <set>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"


namespace FIFE {

	class RawData;

	class VFSSourceProvider;
	class VFSSource;

	/** the main VFS (virtual file system) class
	 *
	 * The VFS is intended to provide transparent and portable access to files.
	 * @note The VFS searches for a provider in the order they are added to the
	 * VFS. Since the VFSHostSystem is added first, this implies, that host filesystem
	 * files will override whatever might be in other VFS Sources (e.g. the DAT files)
	 *
	 * @note All filenames have to be @b lowercase. The VFS will convert them to lowercase
	 * and emit a warning. This is done to avoid problems with filesystems which are not
	 * case sensitive.
	 */
	class VFS : public DynamicSingleton<VFS>{
		public:
			/** Constructor
			 * Called by the Engine on startup. Never create one yourself.
			 */
			VFS();

			/** Destructor
			 */
			virtual ~VFS();

			void cleanup();

			/** add new VFSSourceProvider
			 *
			 * @note VFS assumes ownership over the given provider - so don't do anything with it
			 * after you call this function, especialy don't delete it!
			 * @param provider the new provider
			 */
			void addProvider(VFSSourceProvider* provider);

			/** tries to create a new VFSSource for the given file
			 *
			 * all currently known VFSSourceProviders are tried until one succeeds - if no provider succeeds 0 is returned
			 * @param path the archive-file
			 * @return the new VFSSource or 0 if no provider was succesfull or the file was already used as source
			 */
			VFSSource* createSource(const std::string& path);

			/** create a new Source and add it to VFS
			 * @see VFSSource* createSource(const std::string& file)
			 */
			void addNewSource(const std::string& path);

			/** Add a new VFSSource */
			void addSource(VFSSource* source);

			/** remove a VFSSource */
			void removeSource(VFSSource* source);
			void removeSource(const std::string& path);

			/** Check if the given file exists
			 *
			 * @param file the filename
			 * @return true if it exists, false if not
			 */
			bool exists(const std::string& file) const;

			/** Check if the given path is a directory
			 *
			 * @param path to check
			 * @return true if it is a directory, false if not
			 */
			bool isDirectory(const std::string& path) const;

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
			std::set<std::string> listFiles(const std::string& path) const;

			/** List the files of a given directory matching a regex
			 *
			 * The whole string has to match the regex, this means if you want all files that end with .map don't search for
			 * "\.map" but ".*\.map" (and escape the \)
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			std::set<std::string> listFiles(const std::string& path, const std::string& filterregex) const;

			/** Get a directorylist of the given directory
			 *
			 * @param path the directory
			 * @return the directorylist
			 */
			std::set<std::string> listDirectories(const std::string& path) const;

			/** List the subdirectorys of a given directory matching a regex
			 *
			 * @param path the directory
			 * @param filterregex the regex the files have to match
			 * @return the filelist
			 */
			std::set<std::string> listDirectories(const std::string& path, const std::string& filterregex) const;

			/** Checks if a source is already present in a provider
			 *
			 * @param path the directory
			 * @return true if the source is present, false if not
			 */
			bool hasSource(const std::string& path) const;


		private:
			typedef std::vector<VFSSourceProvider*> type_providers;
			type_providers m_providers;

			typedef std::vector<VFSSource*> type_sources;
			type_sources m_sources;

			std::set<std::string> filterList(const std::set<std::string>& list, const std::string& fregex) const;
			VFSSource* getSourceForFile(const std::string& file) const;
	};

}

#endif
