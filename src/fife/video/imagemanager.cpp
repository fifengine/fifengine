// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "imagemanager.h"

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
#include "video/image.h"
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

    ImageManager::~ImageManager() = default;

    size_t ImageManager::getMemoryUsed() const
    {
        size_t totalSize = 0;

        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        for (; it != itend; ++it) {
            totalSize += it->second->getSize();
        }

        return totalSize;
    }

    size_t ImageManager::getTotalResourcesCreated() const
    {
        auto it      = m_imgHandleMap.begin();
        auto itend   = m_imgHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_NOT_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t ImageManager::getTotalResourcesLoaded() const
    {
        auto it      = m_imgHandleMap.begin();
        auto itend   = m_imgHandleMap.end();
        size_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                count++;
            }
        }

        return count;
    }

    size_t ImageManager::getTotalResources() const
    {
        return m_imgHandleMap.size();
    }

    ImagePtr ImageManager::create(IResourceLoader* loader)
    {
        return add(RenderBackend::instance()->createImage(loader));
    }

    ImagePtr ImageManager::create(std::string const & name, IResourceLoader* loader)
    {
        if (exists(name)) {
            // FL_WARN(
            // _log(),
            // std::format(
            // "ImageManager::create(std::string, IResourceLoader* loader) - Resource name {} was previously "
            // "created.  Returning original Image...",
            // name));
            return getPtr(name);
        }

        return add(RenderBackend::instance()->createImage(name, loader));
    }

    ImagePtr ImageManager::load(std::string const & name, IResourceLoader* loader)
    {
        auto nit = m_imgNameMap.find(name);

        if (nit != m_imgNameMap.end()) {
            if (nit->second->getState() == IResource::RES_NOT_LOADED) {
                nit->second->load();
            }

            return nit->second;
        }

        // was not found so create and load resource
        ImagePtr const ptr = create(name, loader);
        ptr->load();

        if (ptr->getState() == IResource::RES_NOT_LOADED) {
            // FL_WARN(
            // _log(),
            // std::format(
            // "ImageManager::load(std::string) - Resource name {} was not found and could not be loaded.", name));
            remove(name);
        }

        return ptr;
    }

    ImagePtr ImageManager::loadBlank(uint32_t width, uint32_t height)
    {
        size_t const pixelBufferSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4U;

        std::vector<uint8_t> pixdata(pixelBufferSize, 0);
        auto ptr = RenderBackend::instance()->createImage(pixdata.data(), width, height);
        ptr->setState(IResource::RES_LOADED);
        return add(std::move(ptr));
    }

    ImagePtr ImageManager::loadBlank(std::string const & name, uint32_t width, uint32_t height)
    {
        auto nit = m_imgNameMap.find(name);
        if (nit != m_imgNameMap.end()) {
            remove(nit->second);
        }
        size_t const pixelBufferSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4U;
        std::vector<uint8_t> pixdata(pixelBufferSize, 0);
        auto ptr = RenderBackend::instance()->createImage(name, pixdata.data(), width, height);
        ptr->setState(IResource::RES_LOADED);
        return add(std::move(ptr));
    }

    ImagePtr ImageManager::add(std::unique_ptr<Image> res)
    {
        assert(res);
        ResourceHandle const handle = res->getHandle();
        std::string const & name    = res->getName();
        assert(!(exists(handle) || exists(name)));

        ImagePtr const resptr(res.release());

        std::pair<ImageHandleMapIterator, bool> returnValue;
        returnValue = m_imgHandleMap.insert(ImageHandleMapPair(handle, resptr));

        if (returnValue.second) {
            m_imgNameMap.insert(ImageNameMapPair(returnValue.first->second->getName(), returnValue.first->second));
        } else {
            // FL_WARN(
            // _log(), std::format("ImageManager::add(IResource*) - Resource {} already exists.... ignoring.", name));
        }

        return returnValue.first->second;
    }

    bool ImageManager::exists(std::string const & name)
    {
        auto it = m_imgNameMap.find(name);
        return it != m_imgNameMap.end();
    }

    bool ImageManager::exists(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);
        return it != m_imgHandleMap.end();
    }

    void ImageManager::reload(std::string const & name)
    {
        auto nit = m_imgNameMap.find(name);

        if (nit != m_imgNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            nit->second->load();
            return;
        }

        // FL_WARN(_log(), std::format("ImageManager::reload(std::string) - Resource name {} not found.", name));
    }

    void ImageManager::reload(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);

        if (it != m_imgHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
            return;
        }

        // FL_WARN(_log(), std::format("ImageManager::reload(ResourceHandle) - Resource handle {} not found.", handle));
    }

    void ImageManager::reloadAll()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            it->second->load();
        }
    }

    void ImageManager::loadUnreferenced()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED) {
                it->second->load();
                count++;
            }
        }
        FL_DBG(_log(), std::format("ImageManager::loadUnreferenced() - Loaded {} unreferenced resources.", count));
    }

    void ImageManager::free(std::string const & name)
    {
        auto nit = m_imgNameMap.find(name);

        if (nit != m_imgNameMap.end()) {
            if (nit->second->getState() == IResource::RES_LOADED) {
                nit->second->free();
            }
            return;
        }

        // FL_WARN(_log(), std::format("ImageManager::free(std::string) - Resource name {} not found.", name));
    }

    void ImageManager::free(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);
        if (it != m_imgHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
            }
            return;
        }

        // FL_WARN(_log(), std::format("ImageManager::free(ResourceHandle) - Resource handle {} not found.", handle));
    }

    void ImageManager::freeAll()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        int32_t count = 0;

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        FL_DBG(_log(), std::format("ImageManager::freeAll() - Freed all {} resources.", count));
    }

    void ImageManager::freeUnreferenced()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        int32_t count = 0;
        for (; it != itend; ++it) {
            if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED) {
                it->second->free();
                count++;
            }
        }

        FL_DBG(_log(), std::format("ImageManager::freeUnreferenced() - Freed {} unreferenced resources.", count));
    }

    void ImageManager::remove(ImagePtr& resource)
    {
        auto it  = m_imgHandleMap.find(resource->getHandle());
        auto nit = m_imgNameMap.find(resource->getName());

        if (it != m_imgHandleMap.end()) {
            m_imgHandleMap.erase(it);

            if (nit != m_imgNameMap.end()) {
                m_imgNameMap.erase(nit);
                return;
            }
            assert(false); // should never get here
        }

        // FL_WARN(
        // _log(),
        // std::format("ImageManager::remove(ResourcePtr&) - Resource {} was not found.", resource->getName()));
    }

    void ImageManager::remove(std::string const & name)
    {
        std::size_t handle = 0;

        auto nit = m_imgNameMap.find(name);
        if (nit != m_imgNameMap.end()) {
            handle = nit->second->getHandle();
            m_imgNameMap.erase(nit);
        } else {
            // FL_WARN(_log(), std::format("ImageManager::remove(std::string) - Resource {} was not found.", name));
            return;
        }

        auto it = m_imgHandleMap.find(handle);
        if (it != m_imgHandleMap.end()) {
            m_imgHandleMap.erase(it);
            return;
        }

        assert(false); // should never get here
    }

    void ImageManager::remove(ResourceHandle handle)
    {
        std::string name;

        auto it = m_imgHandleMap.find(handle);

        if (it != m_imgHandleMap.end()) {
            name = it->second->getName();
            m_imgHandleMap.erase(it);
        } else {
            // FL_WARN(
            // _log(),
            // std::format("ImageManager::remove(ResourceHandle) - Resource handle {} was not found.", handle));
            return;
        }

        auto nit = m_imgNameMap.find(name);
        if (nit != m_imgNameMap.end()) {
            m_imgNameMap.erase(nit);
            return;
        }

        assert(false); // should never get here
    }

    void ImageManager::removeAll()
    {
        // should always be equal
        assert(m_imgHandleMap.size() == m_imgNameMap.size());

        size_t const count = m_imgHandleMap.size();

        m_imgHandleMap.clear();
        m_imgNameMap.clear();

        FL_DBG(_log(), std::format("ImageManager::removeAll() - Removed all {} resources.", count));
    }

    void ImageManager::removeUnreferenced()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

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

        FL_DBG(_log(), std::format("ImageManager::removeUnreferenced() - Removed {} unreferenced resources.", count));
    }

    ImagePtr ImageManager::get(std::string const & name)
    {
        auto nit = m_imgNameMap.find(name);

        if (nit != m_imgNameMap.end()) {
            if (nit->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                nit->second->load();
            }
            return nit->second;
        }

        // not found so attempt to create and load the resource
        ImagePtr const ptr = load(name);
        return ptr;
    }

    ImagePtr ImageManager::get(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);
        if (it != m_imgHandleMap.end()) {
            if (it->second->getState() != IResource::RES_LOADED) {
                // resource is not loaded so load it
                it->second->load();
            }
            return it->second;
        }

        // FL_WARN(_log(), std::format("ImageManager::get(ResourceHandle) - Resource handle {} is undefined.", handle));

        return {};
    }

    ImagePtr ImageManager::getPtr(std::string const & name)
    {
        auto nit = m_imgNameMap.find(name);

        if (nit != m_imgNameMap.end()) {
            return nit->second;
        }

        // FL_WARN(_log(), std::format("ImageManager::getPtr(std::string) - Resource {} is undefined.", name));

        return {};
    }

    ImagePtr ImageManager::getPtr(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);
        if (it != m_imgHandleMap.end()) {
            return it->second;
        }

        // FL_WARN(_log(), std::format("ImageManager::getPtr(ResourceHandle) - Resource handle {} is undefined.",
        // handle));

        return {};
    }

    ResourceHandle ImageManager::getResourceHandle(std::string const & name)
    {
        auto nit = m_imgNameMap.find(name);
        if (nit != m_imgNameMap.end()) {
            return nit->second->getHandle();
        }

        // FL_WARN(_log(), std::format("ImageManager::getResourceHandle(std::string) - Resource {} is undefined.",
        // name));

        return 0;
    }

    void ImageManager::invalidate(std::string const & name)
    {
        auto it = m_imgNameMap.find(name);
        if (it != m_imgNameMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

    void ImageManager::invalidate(ResourceHandle handle)
    {
        auto it = m_imgHandleMap.find(handle);
        if (it != m_imgHandleMap.end()) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

    void ImageManager::invalidateAll()
    {
        auto it    = m_imgHandleMap.begin();
        auto itend = m_imgHandleMap.end();

        for (; it != itend; ++it) {
            if (it->second->getState() == IResource::RES_LOADED) {
                it->second->invalidate();
            }
        }
    }

} // namespace FIFE
