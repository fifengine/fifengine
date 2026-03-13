// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_DIRECTORYPROVIDER_H
#define FIFE_VFS_DIRECTORYPROVIDER_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfssourceprovider.h"

namespace FIFE
{

    class VFSSource;

    /** Provider for OS directories.
     */
    class DirectoryProvider : public VFSSourceProvider
    {
    public:
        DirectoryProvider() : VFSSourceProvider("OS Directory") { }

        /** Check if a given directory is readable.
         * The path must either be an absolute path or relative
         * to the current working directory where the client was launched
         *
         * @param path the filename to check
         * @return true if readable, false otherwise
         */
        virtual bool isReadable(const std::string& path) const override;

        /** Create a new instance of a VFSSource initialized with the given directory
         * This will only pass if the path is readable and recognized
         * as a directory as per the description of isReadable
         *
         * @param path the directory to open
         * @return the new VFSSource
         */
        virtual VFSSource* createSource(const std::string& path) override;

        /** Get the source instance of the path
         *
         * @param path The source path
         * @return A VFSSource or NULL of none is present
         */
        virtual VFSSource* getSource(const std::string& path) const override;

        /** Check whether the provider already has created a source with that path
         *
         * @param path The path to the source
         * @return true if the provider has already created a source with that path, false if not
         */
        virtual bool hasSource(const std::string& path) const override;

    private:
        std::string m_name;
        std::map<std::string, VFSSource*> m_sources;
    };
} // namespace FIFE

#endif