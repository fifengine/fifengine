// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "soundclipmanager.h"

// Standard C++ library includes
#include <format>
#include <map>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

namespace FIFE
{
    namespace
    {
        Logger& _log = []() -> Logger& {
            static Logger log(LM_RESMGR);
            return log;
        }();
    } // namespace

    SoundClipManager::~SoundClipManager() = default;

    size_t SoundClipManager::getMemoryUsed() const
    {
        size_t totalSize = 0;

        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        for (; it != itend; ++it) {
            totalSize += it->second->getSize();
        }

        return totalSize;
    }

    size_t SoundClipManager::getTotalResourcesCreated() const
    {
        auto it      = m_sclipHandleMap.begin();
        auto itend   = m_sclipHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_NOT_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t SoundClipManager::getTotalResourcesLoaded() const
    {
        auto it      = m_sclipHandleMap.begin();
        auto itend   = m_sclipHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t SoundClipManager::getTotalResources() const
    {
        return m_sclipHandleMap.size();
    }

    SoundClipPtr SoundClipManager::create(IResourceLoader* loader)
    {
        auto* ptr = new SoundClip(loader);
        return add(ptr);
    }

    SoundClipPtr SoundClipManager::create(std::string const & name, IResourceLoader* loader)
    {
        if (exists(name)) {
            {
                FL_WARN(
                    _log,
                    std::format(
                        "SoundClipManager::create(std::string, IResourceLoader* loader) - Resource name {} was "
                        "previously created.  Returning original SoundClip...",
                        name));
            }
            return get(name);
        }

        auto* ptr = new SoundClip(name, loader);
        return add(ptr);
    }

    SoundClipPtr SoundClipManager::load(std::string const & name, IResourceLoader* loader)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() == IResource::RES_NOT_LOADED) {
                nit->second->load();
            }

            return nit->second;
        }

        // was not found so create and load resource
        SoundClipPtr const ptr = create(name, loader);
        ptr->load();

        if (ptr->getState() == IResource::RES_NOT_LOADED) {
            {
                FL_WARN(
                    _log,
                    std::format(
                        "SoundClipManager::load(std::string) - Resource name {} was not found and could not be loaded.",
                        name));
            }
            remove(name);
        }

        return ptr;
    }

    SoundClipPtr SoundClipManager::add(SoundClip* res)
    {
        assert(res);
        assert(!(exists(res->getHandle()) || exists(res->getName())));

        SoundClipPtr const resptr(res);

        std::pair<SoundClipHandleMapIterator, bool> returnValue;
        returnValue = m_sclipHandleMap.insert(SoundClipHandleMapPair(res->getHandle(), resptr));

        if (returnValue.second) {
            m_sclipNameMap.insert(
                SoundClipNameMapPair(returnValue.first->second->getName(), returnValue.first->second));
        } else {
            {
                FL_WARN(
                    _log,
                    std::format(
                        "SoundClipManager::add(IResource*) - Resource {} already exists.... ignoring.",
                        res->getName()));
            }
        }

        return returnValue.first->second;
    }

    bool SoundClipManager::exists(std::string const & name)
    {
        auto it = m_sclipNameMap.find(name);
        return it != m_sclipNameMap.end();
    }

    bool SoundClipManager::exists(ResourceHandle handle)
    {
        auto it = m_sclipHandleMap.find(handle);
        return it != m_sclipHandleMap.end();
    }

    void SoundClipManager::reload(std::string const & name)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            nit->second->load();
            return;
        }

        {
            FL_WARN(_log, std::format("SoundClipManager::reload(std::string) - Resource name {} not found.", name));
        }
    }

    void SoundClipManager::reload(ResourceHandle handle)
    {
        auto it = m_sclipHandleMap.find(handle);

        if (it != m_sclipHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
            return;
        }

        {
            FL_WARN(
                _log, std::format("SoundClipManager::reload(ResourceHandle) - Resource handle {} not found.", handle));
        }
    }

    void SoundClipManager::reloadAll()
    {
        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
        }
    }

    void SoundClipManager::loadUnreferenced()
    {
        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED) {
                it->second->load();
                count++;
            }
        }
        {
            FL_DBG(
                _log, std::format("SoundClipManager::loadUnreferenced() - Loaded {} unreferenced resources.", count));
        }
    }

    void SoundClipManager::free(std::string const & name)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            return;
        }

        {
            FL_WARN(_log, std::format("SoundClipManager::free(std::string) - Resource name {} not found.", name));
        }
    }

    void SoundClipManager::free(ResourceHandle handle)
    {
        auto it = m_sclipHandleMap.find(handle);
        if (it != m_sclipHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            return;
        }

        {
            FL_WARN(
                _log, std::format("SoundClipManager::free(ResourceHandle) - Resource handle {} not found.", handle));
        }
    }

    void SoundClipManager::freeAll()
    {
        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        int32_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        {
            FL_DBG(_log, std::format("SoundClipManager::freeAll() - Freed all {} resources.", count));
        }
    }

    void SoundClipManager::freeUnreferenced()
    {
        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        {
            FL_DBG(_log, std::format("SoundClipManager::freeUnreferenced() - Freed {} unreferenced resources.", count));
        }
    }

    void SoundClipManager::remove(SoundClipPtr& resource)
    {
        auto it  = m_sclipHandleMap.find(resource->getHandle());
        auto nit = m_sclipNameMap.find(resource->getName());

        if (it != m_sclipHandleMap.end()) {
            m_sclipHandleMap.erase(it);

            if (nit != m_sclipNameMap.end()) {
                m_sclipNameMap.erase(nit);
                return;
            }
            assert(false); // should never get here
        }

        {
            FL_WARN(
                _log,
                std::format(
                    "SoundClipManager::remove(ResourcePtr&) - Resource {} was not found.", resource->getName()));
        }
    }

    void SoundClipManager::remove(std::string const & name)
    {
        std::size_t handle = 0;

        auto nit = m_sclipNameMap.find(name);
        if (nit != m_sclipNameMap.end()) {
            handle = nit->second->getHandle();
            m_sclipNameMap.erase(nit);
        } else {
            {
                FL_WARN(_log, std::format("SoundClipManager::remove(std::string) - Resource {} was not found.", name));
            }
            return;
        }

        auto it = m_sclipHandleMap.find(handle);
        if (it != m_sclipHandleMap.end()) {
            m_sclipHandleMap.erase(it);
            return;
        }

        assert(false); // should never get here
    }

    void SoundClipManager::remove(ResourceHandle handle)
    {
        std::string name;

        auto it = m_sclipHandleMap.find(handle);

        if (it != m_sclipHandleMap.end()) {
            name = it->second->getName();
            m_sclipHandleMap.erase(it);
        } else {
            {
                FL_WARN(
                    _log,
                    std::format(
                        "SoundClipManager::remove(ResourceHandle) - Resource handle {} was not found.", handle));
            }
            return;
        }

        auto nit = m_sclipNameMap.find(name);
        if (nit != m_sclipNameMap.end()) {
            m_sclipNameMap.erase(nit);
            return;
        }

        assert(false); // should never get here
    }

    void SoundClipManager::removeAll()
    {
        // should always be equal
        assert(m_sclipHandleMap.size() == m_sclipNameMap.size());

        size_t const count = m_sclipHandleMap.size();

        m_sclipHandleMap.clear();
        m_sclipNameMap.clear();

        {
            FL_DBG(_log, std::format("SoundClipManager::removeAll() - Removed all {} resources.", count));
        }
    }

    void SoundClipManager::removeUnreferenced()
    {
        auto it    = m_sclipHandleMap.begin();
        auto itend = m_sclipHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2) {
                remove(it->second->getHandle());
                count++;
            }
        }

        {
            FL_DBG(
                _log,
                std::format("SoundClipManager::removeUnreferenced() - Removed {} unreferenced resources.", count));
        }
    }

    SoundClipPtr SoundClipManager::get(std::string const & name)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                nit->second->load();
            }
            return nit->second;
        }

        // not found so attempt to create and load the resource
        SoundClipPtr const ptr = load(name);
        return ptr;
    }

    SoundClipPtr SoundClipManager::get(ResourceHandle handle)
    {
        auto it = m_sclipHandleMap.find(handle);
        if (it != m_sclipHandleMap.end()) {
            if (it->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                it->second->load();
            }
            return it->second;
        }

        {
            FL_WARN(
                _log, std::format("SoundClipManager::get(ResourceHandle) - Resource handle {} is undefined.", handle));
        }

        return {};
    }

    ResourceHandle SoundClipManager::getResourceHandle(std::string const & name)
    {
        auto nit = m_sclipNameMap.find(name);
        if (nit != m_sclipNameMap.end()) {
            return nit->second->getHandle();
        }

        {
            FL_WARN(
                _log,
                std::format("SoundClipManager::getResourceHandle(std::string) - Resource {} is undefined.", name));
        }

        return 0;
    }

} // namespace FIFE
