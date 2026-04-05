// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <map>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"

#include "soundclipmanager.h"

namespace FIFE
{
    static Logger _log(LM_RESMGR);

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

    SoundClipPtr SoundClipManager::create(const std::string& name, IResourceLoader* loader)
    {
        if (exists(name)) {
            {
                LMsg msg;
                msg << "SoundClipManager::create(std::string, IResourceLoader* loader) - "
                    << "Resource name " << name << " was previously created.  Returning original SoundClip...";
                FL_WARN(_log, msg);
            }
            return get(name);
        }

        auto* ptr = new SoundClip(name, loader);
        return add(ptr);
    }

    SoundClipPtr SoundClipManager::load(const std::string& name, IResourceLoader* loader)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() == IResource::RES_NOT_LOADED) {
                nit->second->load();
            }

            return nit->second;
        }

        // was not found so create and load resource
        const SoundClipPtr ptr = create(name, loader);
        ptr->load();

        if (ptr->getState() == IResource::RES_NOT_LOADED) {
            {
                LMsg msg;
                msg << "SoundClipManager::load(std::string) - " << "Resource name " << name
                    << " was not found and could not be loaded.";
                FL_WARN(_log, msg);
            }
            remove(name);
        }

        return ptr;
    }

    SoundClipPtr SoundClipManager::add(SoundClip* res)
    {
        assert(res);
        assert(!(exists(res->getHandle()) || exists(res->getName())));

        const SoundClipPtr resptr(res);

        std::pair<SoundClipHandleMapIterator, bool> returnValue;
        returnValue = m_sclipHandleMap.insert(SoundClipHandleMapPair(res->getHandle(), resptr));

        if (returnValue.second) {
            m_sclipNameMap.insert(
                SoundClipNameMapPair(returnValue.first->second->getName(), returnValue.first->second));
        } else {
            {
                LMsg msg;
                msg << "SoundClipManager::add(IResource*) - " << "Resource " << res->getName()
                    << " already exists.... ignoring.";
                FL_WARN(_log, msg);
            }
        }

        return returnValue.first->second;
    }

    bool SoundClipManager::exists(const std::string& name)
    {
        auto it = m_sclipNameMap.find(name);
        return it != m_sclipNameMap.end();
    }

    bool SoundClipManager::exists(ResourceHandle handle)
    {
        auto it = m_sclipHandleMap.find(handle);
        return it != m_sclipHandleMap.end();
    }

    void SoundClipManager::reload(const std::string& name)
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
            LMsg msg;
            msg << "SoundClipManager::reload(std::string) - " << "Resource name " << name << " not found.";
            FL_WARN(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::reload(ResourceHandle) - " << "Resource handle " << handle << " not found.";
            FL_WARN(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::loadUnreferenced() - " << "Loaded " << count << " unreferenced resources.";
            FL_DBG(_log, msg);
        }
    }

    void SoundClipManager::free(const std::string& name)
    {
        auto nit = m_sclipNameMap.find(name);

        if (nit != m_sclipNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            return;
        }

        {
            LMsg msg;
            msg << "SoundClipManager::free(std::string) - " << "Resource name " << name << " not found.";
            FL_WARN(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::free(ResourceHandle) - " << "Resource handle " << handle << " not found.";
            FL_WARN(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::freeAll() - " << "Freed all " << count << " resources.";
            FL_DBG(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::freeUnreferenced() - " << "Freed " << count << " unreferenced resources.";
            FL_DBG(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::remove(ResourcePtr&) - " << "Resource " << resource->getName()
                << " was not found.";
            FL_WARN(_log, msg);
        }
    }

    void SoundClipManager::remove(const std::string& name)
    {
        std::size_t handle = 0;

        auto nit = m_sclipNameMap.find(name);
        if (nit != m_sclipNameMap.end()) {
            handle = nit->second->getHandle();
            m_sclipNameMap.erase(nit);
        } else {
            {
                LMsg msg;
                msg << "SoundClipManager::remove(std::string) - " << "Resource " << name << " was not found.";
                FL_WARN(_log, msg);
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
                LMsg msg;
                msg << "SoundClipManager::remove(ResourceHandle) - " << "Resource handle " << handle
                    << " was not found.";
                FL_WARN(_log, msg);
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

        const size_t count = m_sclipHandleMap.size();

        m_sclipHandleMap.clear();
        m_sclipNameMap.clear();

        {
            LMsg msg;
            msg << "SoundClipManager::removeAll() - " << "Removed all " << count << " resources.";
            FL_DBG(_log, msg);
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
            LMsg msg;
            msg << "SoundClipManager::removeUnreferenced() - " << "Removed " << count << " unreferenced resources.";
            FL_DBG(_log, msg);
        }
    }

    SoundClipPtr SoundClipManager::get(const std::string& name)
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
        const SoundClipPtr ptr = load(name);
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
            LMsg msg;
            msg << "SoundClipManager::get(ResourceHandle) - " << "Resource handle " << handle << " is undefined.";
            FL_WARN(_log, msg);
        }

        return {};
    }

    ResourceHandle SoundClipManager::getResourceHandle(const std::string& name)
    {
        auto nit = m_sclipNameMap.find(name);
        if (nit != m_sclipNameMap.end()) {
            return nit->second->getHandle();
        }

        {
            LMsg msg;
            msg << "SoundClipManager::getResourceHandle(std::string) - " << "Resource " << name << " is undefined.";
            FL_WARN(_log, msg);
        }

        return 0;
    }

} // namespace FIFE
