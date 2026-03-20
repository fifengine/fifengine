// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <regex>
#include <set>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "vfs.h"
#include "vfssource.h"
#include "vfssourceprovider.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VFS);

    VFS::VFS() = default;

    VFS::~VFS()
    {
        cleanup();
    }

    void VFS::cleanup()
    {
        type_sources sources = m_sources;
        auto end             = sources.end();
        for (auto i = sources.begin(); i != end; ++i) {
            delete *i;
        }

        auto end2 = m_providers.end();
        for (auto j = m_providers.begin(); j != end2; ++j) {
            delete *j;
        }

        m_providers.clear();
    }

    void VFS::addProvider(VFSSourceProvider* provider)
    {
        provider->setVFS(this);
        m_providers.push_back(provider);
        FL_LOG(_log, LMsg("new provider: ") << provider->getName());
    }

    VFSSource* VFS::createSource(const std::string& path)
    {

        if (hasSource(path)) {
            FL_WARN(_log, LMsg(path) << " is already used as VFS source");
            return nullptr;
        }

        auto end = m_providers.end();
        for (auto i = m_providers.begin(); i != end; ++i) {
            VFSSourceProvider* provider = *i;
            if (!provider->isReadable(path)) {
                continue;
            }

            try {
                VFSSource* source = provider->createSource(path);
                return source;
            } catch (const Exception& ex) {
                FL_WARN(
                    _log,
                    LMsg(provider->getName())
                        << " thought it could load " << path << " but didn't succeed (" << ex.what() << ")");
                continue;
            } catch (...) {
                FL_WARN(
                    _log,
                    LMsg(provider->getName())
                        << " thought it could load " << path << " but didn't succeed (unknown exception)");
                continue;
            }
        }

        FL_WARN(_log, LMsg("no provider for ") << path << " found");
        return nullptr;
    }

    void VFS::addNewSource(const std::string& path)
    {
        VFSSource* source = createSource(path);
        if (source != nullptr) {
            addSource(source);
        } else {
            FL_WARN(_log, LMsg("Failed to add new VFS source: ") << path);
        }
    }

    void VFS::addSource(VFSSource* source)
    {
        m_sources.push_back(source);
    }

    void VFS::removeSource(VFSSource* source)
    {
        auto i = std::ranges::find(m_sources, source);
        if (i != m_sources.end()) {
            m_sources.erase(i);
        }
    }

    void VFS::removeSource(const std::string& path)
    {
        for (VFSSourceProvider* provider : m_providers) {
            if (!provider->hasSource(path)) {
                continue;
            }

            VFSSource* source = provider->getSource(path);

            auto it = std::ranges::find(m_sources, source);

            if (it != m_sources.end()) {
                removeSource(*it);
                return;
            }
        }
    }

    VFSSource* VFS::getSourceForFile(const std::string& file) const
    {
        auto it = std::ranges::find_if(m_sources, [&file](const VFSSource* s) {
            return s->fileExists(file);
        });

        if (it != m_sources.end()) {
            return *it;
        }

        FL_WARN(_log, LMsg("no source for ") << file << " found");
        return nullptr;
    }

    bool VFS::exists(const std::string& file) const
    {
        return getSourceForFile(file) != nullptr;
    }

    std::vector<std::string> VFS::split(const std::string& str, char delimiter) const
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);

        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool VFS::isDirectory(const std::string& path) const
    {
        // Add a slash in case there isn't one in the string
        const std::string newpath       = path + "/";
        std::vector<std::string> tokens = split(newpath, '/');

        std::string currentpath = "/";
        auto token              = tokens.begin();
        while (token != tokens.end()) {
            if (!(*token).empty()) {
                if (*token != "." && *token != ".." && listDirectories(currentpath, *token).empty()) {
                    return false;
                } else {
                    currentpath += *token + "/";
                }
            }
            ++token;
        }

        return true;
    }

    RawData* VFS::open(const std::string& path)
    {
        FL_DBG(_log, LMsg("Opening: ") << path);

        const VFSSource* source = getSourceForFile(path);
        if (source == nullptr) {
            throw NotFound(path);
        }

        return source->open(path);
    }

    std::set<std::string> VFS::listFiles(const std::string& pathstr) const
    {
        std::set<std::string> list;
        auto end = m_sources.end();
        for (auto i = m_sources.begin(); i != end; ++i) {
            std::set<std::string> sourcelist = (*i)->listFiles(pathstr);
            list.insert(sourcelist.begin(), sourcelist.end());
        }

        return list;
    }

    std::set<std::string> VFS::listFiles(const std::string& path, const std::string& filterregex) const
    {
        std::set<std::string> list = listFiles(path);
        return filterList(list, filterregex);
    }

    std::set<std::string> VFS::listDirectories(const std::string& pathstr) const
    {
        std::set<std::string> list;
        auto end = m_sources.end();
        for (auto i = m_sources.begin(); i != end; ++i) {
            std::set<std::string> sourcelist = (*i)->listDirectories(pathstr);
            list.insert(sourcelist.begin(), sourcelist.end());
        }

        return list;
    }

    std::set<std::string> VFS::listDirectories(const std::string& path, const std::string& filterregex) const
    {
        std::set<std::string> list = listDirectories(path);
        return filterList(list, filterregex);
    }

    std::set<std::string> VFS::filterList(const std::set<std::string>& list, const std::string& fregex) const
    {
        std::set<std::string> results;
        std::regex regex(fregex);
        auto end = list.end();
        for (auto i = list.begin(); i != end;) {
            std::cmatch match;
            if (std::regex_match((*i).c_str(), match, regex)) {
                results.insert(*i);
            }
            ++i;
        }
        return results;
    }

    bool VFS::hasSource(const std::string& path) const
    {
        auto end = m_providers.end();
        for (auto i = m_providers.begin(); i != end; ++i) {
            const VFSSourceProvider* provider = *i;
            if (provider->hasSource(path)) {
                const VFSSource* source = provider->getSource(path);

                auto srcIt = std::ranges::find(m_sources, source);
                return srcIt != m_sources.end();
            }
        }
        return false;
    }
} // namespace FIFE
