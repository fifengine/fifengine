// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "vfs.h"

// Standard C++ library includes
#include <algorithm>
#include <format>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/filesystem.h"
#include "vfs/raw/rawdata.h"
#include "vfssource.h"
#include "vfssourceprovider.h"

namespace FIFE
{
    namespace
    {
        /** Logger to use for this source file.
         *  @relates Logger
         */
        Logger& _log()
        {
            static Logger log(LM_VFS);
            return log;
        }
    } // namespace

    VFS::VFS() = default;

    VFS::~VFS()
    {
        cleanup();
    }

    void VFS::cleanup()
    {
        // Swap with empty vector to avoid re-entrancy: clearing m_sources directly
        // would trigger VFSSource::~VFSSource() which calls removeSource() back
        // on the same vector during clear(). Swapping first ensures removeSource()
        // sees an empty m_sources and does nothing.
        type_sources sources;
        m_sources.swap(sources);
        m_providers.clear();
    }

    void VFS::addProvider(std::unique_ptr<VFSSourceProvider> provider)
    {
        provider->setVFS(this);
        // FL_LOG(_log(), std::format("new provider: {}", provider->getName()));
        m_providers.push_back(std::move(provider));
    }

    VFSSource* VFS::createSource(std::string const & path)
    {

        if (hasSource(path)) {
            // FL_WARN(_log(), std::format("{} is already used as VFS source", path));
            return nullptr;
        }

        auto end = m_providers.end();
        for (auto i = m_providers.begin(); i != end; ++i) {
            VFSSourceProvider* provider = i->get();
            if (!provider->isReadable(path)) {
                continue;
            }

            try {
                VFSSource* source = provider->createSource(path);
                return source;
            } catch (Exception const & ex) {
                // FL_WARN(
                // _log(),
                // std::format(
                // "{} thought it could load {} but didn't succeed ({})", provider->getName(), path, ex.what()));
                continue;
            } catch (...) {
                // FL_WARN(
                // _log(),
                // std::format(
                // "{} thought it could load {} but didn't succeed (unknown exception)",
                // provider->getName(),
                // path));
                continue;
            }
        }

        // FL_WARN(_log(), std::format("no provider for {} found", path));
        return nullptr;
    }

    void VFS::addNewSource(std::string const & path)
    {
        std::unique_ptr<VFSSource> source(createSource(path));
        if (source) {
            addSource(std::move(source));
        } else {
            // FL_WARN(_log(), std::format("Failed to add new VFS source: {}", path));
        }
    }

    void VFS::addSource(std::unique_ptr<VFSSource> source)
    {
        m_sources.push_back(std::move(source));
    }

    void VFS::removeSource(VFSSource* source)
    {
        auto i = std::ranges::find_if(m_sources, [source](auto const & ptr) {
            return ptr.get() == source;
        });
        if (i != m_sources.end()) {
            m_sources.erase(i);
        }
    }

    void VFS::removeSource(std::string const & path)
    {
        for (auto const & provider : m_providers) {
            if (!provider->hasSource(path)) {
                continue;
            }

            VFSSource const * source = provider->getSource(path);

            auto it = std::ranges::find_if(m_sources, [source](auto const & ptr) {
                return ptr.get() == source;
            });

            if (it != m_sources.end()) {
                removeSource(it->get());
                return;
            }
        }
    }

    VFSSource* VFS::getSourceForFile(std::string const & file) const
    {
        return findSourceForFile(file);
    }

    VFSSource* VFS::findSourceForFile(std::string const & file) const
    {
        auto it = std::ranges::find_if(m_sources, [&file](auto const & s) {
            return s->fileExists(file);
        });

        if (it != m_sources.end()) {
            return it->get();
        }

        // FL_WARN(_log(), std::format("no source for {} found", file));
        return nullptr;
    }

    bool VFS::exists(std::string const & file) const
    {
        return getSourceForFile(file) != nullptr;
    }

    std::vector<std::string> VFS::split(std::string const & str, char delimiter) const
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);

        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool VFS::isDirectory(std::string const & path) const
    {
        // Normalize separators (Windows may provide backslashes) and add
        // a trailing slash so splitting is consistent across platforms.
        std::string normalized = path;
        std::ranges::replace(normalized, '\\', '/');
        std::string const newpath       = normalized + "/";
        std::vector<std::string> tokens = split(newpath, '/');

        std::string currentpath = "/";
        auto token              = tokens.begin();
        while (token != tokens.end()) {
            if (!(*token).empty()) {
                if (*token != "." && *token != ".." && listDirectories(currentpath, *token).empty()) {
                    return false;
                }
                currentpath += *token + "/";
            }
            ++token;
        }

        return true;
    }

    std::unique_ptr<RawData> VFS::open(std::string const & path)
    {
        FL_DBG(_log(), std::format("Opening: {}", path));

        VFSSource const * source = getSourceForFile(path);
        if (source == nullptr) {
            throw NotFound(path);
        }

        return source->open(path);
    }

    std::set<std::string> VFS::listFiles(std::string const & pathstr) const
    {
        std::set<std::string> list;
        auto end = m_sources.end();
        for (auto i = m_sources.begin(); i != end; ++i) {
            std::set<std::string> const sourcelist = (*i)->listFiles(pathstr);
            list.insert(sourcelist.begin(), sourcelist.end());
        }

        return list;
    }

    std::set<std::string> VFS::listFiles(std::string const & path, std::string const & filterregex) const
    {
        std::set<std::string> const list = listFiles(path);
        return filterList(list, filterregex);
    }

    std::set<std::string> VFS::listDirectories(std::string const & pathstr) const
    {
        std::set<std::string> list;
        auto end = m_sources.end();
        for (auto i = m_sources.begin(); i != end; ++i) {
            std::set<std::string> const sourcelist = (*i)->listDirectories(pathstr);
            list.insert(sourcelist.begin(), sourcelist.end());
        }

        return list;
    }

    std::set<std::string> VFS::listDirectories(std::string const & path, std::string const & filterregex) const
    {
        std::set<std::string> const list = listDirectories(path);
        return filterList(list, filterregex);
    }

    std::set<std::string> VFS::filterList(std::set<std::string> const & list, std::string const & fregex) const
    {
        std::set<std::string> results;
        std::regex const regex(fregex);
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

    std::unique_ptr<RawData> VFS::readFile(std::string const & path)
    {
        VFSSource const * source = getSourceForFile(path);
        if (source == nullptr) {
            FL_DBG(_log(), std::format("readFile: {} not found, returning nullptr", path));
            return nullptr;
        }
        return source->open(path);
    }

    bool VFS::hasSource(std::string const & path) const
    {
        auto checkPath = [this](std::string const & candidate) -> bool {
            auto const end = m_providers.end();

            for (auto it = m_providers.begin(); it != end; ++it) {
                auto const * provider = it->get();

                if (!provider->hasSource(candidate)) {
                    continue;
                }

                auto const * source = provider->getSource(candidate);

                // O(n) lookup in the worst case, but only few sources expected
                if (std::ranges::find_if(m_sources, [source](auto const & ptr) {
                        return ptr.get() == source;
                    }) != m_sources.end()) {
                    return true;
                }
            }
            return false;
        };

        std::string const & original = path;

        fs::path const raw(path);

        std::string const normalized = raw.lexically_normal().string();
        std::string const absolute   = GetAbsolutePath(raw).lexically_normal().string();

        if (checkPath(original)) {
            return true;
        }

        if (normalized != original && checkPath(normalized)) {
            return true;
        }

        if (absolute != original && absolute != normalized && checkPath(absolute)) {
            return true;
        }

        return false;
    }
} // namespace FIFE
