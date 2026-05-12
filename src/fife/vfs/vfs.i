// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/base/exception.h"
#include "vfs/vfs.h"
%}

%include "vfs/raw/rawdata.i"

%template(StringSet) std::set<std::string>;
%template(vectoru) std::vector<uint8_t>;


namespace FIFE {
	class VFS {
	public:

		VFS();
		virtual ~VFS();

		void cleanup();

		void addNewSource(const std::string& path);
		void removeSource(const std::string& path);

		bool exists(const std::string& file) const;
		RawData* open(const std::string& path);

		std::set<std::string> listFiles(const std::string& path) const;
		std::set<std::string> listDirectories(const std::string& path) const;
	};
}
