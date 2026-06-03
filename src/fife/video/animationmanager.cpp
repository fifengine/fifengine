// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "animationmanager.h"

// Standard C++ library includes
#include <format>
#include <map>
#include <string>
#include <utility>
#include <vector>

// FIFE includes
#include "util/log/logger.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
#include "video/renderbackend.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_RESMGR);
            return log;
        }
    } // namespace

    AnimationManager::~AnimationManager() = default;

    size_t AnimationManager::getMemoryUsed() const
    {
        size_t totalSize = 0;

        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        for (; it != itend; ++it) {
            totalSize += it->second->getSize();
        }

        return totalSize;
    }

    size_t AnimationManager::getTotalResourcesCreated() const
    {
        auto it      = m_animHandleMap.begin();
        auto itend   = m_animHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_NOT_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t AnimationManager::getTotalResourcesLoaded() const
    {
        auto it      = m_animHandleMap.begin();
        auto itend   = m_animHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t AnimationManager::getTotalResources() const
    {
        return m_animHandleMap.size();
    }

    AnimationPtr AnimationManager::create(IResourceLoader* loader)
    {
        auto* ptr = new Animation(loader);
        return add(ptr);
    }

    AnimationPtr AnimationManager::create(std::string const & name, IResourceLoader* loader)
    {
        if (exists(name)) {
            FL_WARN(
                _log(),
                std::format(
                    "AnimationManager::create(std::string, IResourceLoader* loader) - Resource name {} was previously "
                    "created.  Returning original Animation...",
                    name));
            return getPtr(name);
        }

        auto* ptr = new Animation(name, loader);
        return add(ptr);
    }

    AnimationPtr AnimationManager::load(std::string const & name, IResourceLoader* loader)
    {
        auto nit = m_animNameMap.find(name);

        if (nit != m_animNameMap.end()) {
            if (nit->second->getState() == IResource::RES_NOT_LOADED) {
                nit->second->load();
            }

            return nit->second;
        }

        // was not found so create and load resource
        AnimationPtr const ptr = create(name, loader);
        ptr->load();

        if (ptr->getState() == IResource::RES_NOT_LOADED) {
            FL_WARN(
                _log(),
                std::format(
                    "AnimationManager::load(std::string) - Resource name {} was not found and could not be loaded.",
                    name));
            remove(name);
        }

        return ptr;
    }

    AnimationPtr AnimationManager::add(Animation* res)
    {
        assert(res);
        assert(!(exists(res->getHandle()) || exists(res->getName())));

        AnimationPtr const resptr(res);

        std::pair<AnimationHandleMapIterator, bool> returnValue;
        returnValue = m_animHandleMap.insert(AnimationHandleMapPair(res->getHandle(), resptr));

        if (returnValue.second) {
            m_animNameMap.insert(AnimationNameMapPair(returnValue.first->second->getName(), returnValue.first->second));
        } else {
            FL_WARN(
                _log(),
                std::format(
                    "AnimationManager::add(IResource*) - Resource {} already exists.... ignoring.", res->getName()));
        }

        return returnValue.first->second;
    }

    bool AnimationManager::exists(std::string const & name)
    {
        auto it = m_animNameMap.find(name);
        return it != m_animNameMap.end();
    }

    bool AnimationManager::exists(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);
        return it != m_animHandleMap.end();
    }

    void AnimationManager::reload(std::string const & name)
    {
        auto nit = m_animNameMap.find(name);

        if (nit != m_animNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            nit->second->load();
            return;
        }

        FL_WARN(_log(), std::format("AnimationManager::reload(std::string) - Resource name {} not found.", name));
    }

    void AnimationManager::reload(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);

        if (it != m_animHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
            return;
        }

        FL_WARN(
            _log(), std::format("AnimationManager::reload(ResourceHandle) - Resource handle {} not found.", handle));
    }

    void AnimationManager::reloadAll()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
        }
    }

    void AnimationManager::loadUnreferenced()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED) {
                it->second->load();
                count++;
            }
        }
        FL_DBG(_log(), std::format("AnimationManager::loadUnreferenced() - Loaded {} unreferenced resources.", count));
    }

    void AnimationManager::free(std::string const & name)
    {
        auto nit = m_animNameMap.find(name);

        if (nit != m_animNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            return;
        }

        FL_WARN(_log(), std::format("AnimationManager::free(std::string) - Resource name {} not found.", name));
    }

    void AnimationManager::free(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);
        if (it != m_animHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            return;
        }

        FL_WARN(_log(), std::format("AnimationManager::free(ResourceHandle) - Resource handle {} not found.", handle));
    }

    void AnimationManager::freeAll()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        int32_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        FL_DBG(_log(), std::format("AnimationManager::freeAll() - Freed all {} resources.", count));
    }

    void AnimationManager::freeUnreferenced()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        FL_DBG(_log(), std::format("AnimationManager::freeUnreferenced() - Freed {} unreferenced resources.", count));
    }

    void AnimationManager::remove(AnimationPtr& resource)
    {
        auto it  = m_animHandleMap.find(resource->getHandle());
        auto nit = m_animNameMap.find(resource->getName());

        if (it != m_animHandleMap.end()) {
            m_animHandleMap.erase(it);

            if (nit != m_animNameMap.end()) {
                m_animNameMap.erase(nit);
                return;
            }
            assert(false); // should never get here
        }

        FL_WARN(
            _log(),
            std::format("AnimationManager::remove(ResourcePtr&) - Resource {} was not found.", resource->getName()));
    }

    void AnimationManager::remove(std::string const & name)
    {
        std::size_t handle = 0;

        auto nit = m_animNameMap.find(name);
        if (nit != m_animNameMap.end()) {
            handle = nit->second->getHandle();
            m_animNameMap.erase(nit);
        } else {
            FL_WARN(_log(), std::format("AnimationManager::remove(std::string) - Resource {} was not found.", name));
            return;
        }

        auto it = m_animHandleMap.find(handle);
        if (it != m_animHandleMap.end()) {
            m_animHandleMap.erase(it);
            return;
        }

        assert(false); // should never get here
    }

    void AnimationManager::remove(ResourceHandle handle)
    {
        std::string name;

        auto it = m_animHandleMap.find(handle);

        if (it != m_animHandleMap.end()) {
            name = it->second->getName();
            m_animHandleMap.erase(it);
        } else {
            FL_WARN(
                _log(),
                std::format("AnimationManager::remove(ResourceHandle) - Resource handle {} was not found.", handle));
            return;
        }

        auto nit = m_animNameMap.find(name);
        if (nit != m_animNameMap.end()) {
            m_animNameMap.erase(nit);
            return;
        }

        assert(false); // should never get here
    }

    void AnimationManager::removeAll()
    {
        // should always be equal
        assert(m_animHandleMap.size() == m_animNameMap.size());

        size_t const count = m_animHandleMap.size();

        m_animHandleMap.clear();
        m_animNameMap.clear();

        FL_DBG(_log(), std::format("AnimationManager::removeAll() - Removed all {} resources.", count));
    }

    void AnimationManager::removeUnreferenced()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        std::vector<ResourceHandle> imgHandles;

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2) {
                imgHandles.push_back(it->second->getHandle());
                count++;
            }
        }

        for (ResourceHandle const imgHandle : imgHandles) {
            remove(imgHandle);
        }

        FL_DBG(
            _log(), std::format("AnimationManager::removeUnreferenced() - Removed {} unreferenced resources.", count));
    }

    AnimationPtr AnimationManager::get(std::string const & name)
    {
        auto nit = m_animNameMap.find(name);

        if (nit != m_animNameMap.end()) {
            if (nit->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                nit->second->load();
            }
            return nit->second;
        }

        // not found so attempt to create and load the resource
        AnimationPtr const ptr = load(name);
        return ptr;
    }

    AnimationPtr AnimationManager::get(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);
        if (it != m_animHandleMap.end()) {
            if (it->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                it->second->load();
            }
            return it->second;
        }

        FL_WARN(
            _log(), std::format("AnimationManager::get(ResourceHandle) - Resource handle {} is undefined.", handle));

        return {};
    }

    AnimationPtr AnimationManager::getPtr(std::string const & name)
    {
        auto nit = m_animNameMap.find(name);

        if (nit != m_animNameMap.end()) {
            return nit->second;
        }

        FL_WARN(_log(), std::format("AnimationManager::getPtr(std::string) - Resource {} is undefined.", name));

        return {};
    }

    AnimationPtr AnimationManager::getPtr(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);
        if (it != m_animHandleMap.end()) {
            return it->second;
        }

        FL_WARN(
            _log(), std::format("AnimationManager::getPtr(ResourceHandle) - Resource handle {} is undefined.", handle));

        return {};
    }

    ResourceHandle AnimationManager::getResourceHandle(std::string const & name)
    {
        auto nit = m_animNameMap.find(name);
        if (nit != m_animNameMap.end()) {
            return nit->second->getHandle();
        }

        FL_WARN(
            _log(), std::format("AnimationManager::getResourceHandle(std::string) - Resource {} is undefined.", name));

        return 0;
    }

    void AnimationManager::invalidate(std::string const & name)
    {
        auto it = m_animNameMap.find(name);
        if (it != m_animNameMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

    void AnimationManager::invalidate(ResourceHandle handle)
    {
        auto it = m_animHandleMap.find(handle);
        if (it != m_animHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

    void AnimationManager::invalidateAll()
    {
        auto it    = m_animHandleMap.begin();
        auto itend = m_animHandleMap.end();

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

} // namespace FIFE
