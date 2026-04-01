// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RESOURE_H
#define FIFE_RESOURE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

namespace FIFE
{

    typedef std::size_t ResourceHandle;

    class IResource;

    class IResourceLoader
    {
    public:
        IResourceLoader() { }
        virtual ~IResourceLoader() { }

        virtual void load(IResource* resource) = 0;
    };

    class IResource
    {
    public:
        enum ResourceState
        {
            RES_INVALID = 0,
            RES_NOT_LOADED,
            RES_LOADED
        };

        explicit IResource(const std::string& name, IResourceLoader* loader = nullptr) :
            m_name(name), m_loader(loader), m_state(RES_NOT_LOADED), m_handle(m_curhandle++)
        {
        }

        virtual ~IResource() { }

        virtual const std::string& getName()
        {
            return m_name;
        }

        ResourceHandle getHandle()
        {
            return m_handle;
        }

        virtual ResourceState getState()
        {
            return m_state;
        }
        virtual void setState(const ResourceState& state)
        {
            m_state = state;
        }

        virtual size_t getSize() = 0;

        virtual void load() = 0;
        virtual void free() = 0;

    protected:
        std::string m_name;
        IResourceLoader* m_loader;
        ResourceState m_state;

    private:
        ResourceHandle m_handle;
        static ResourceHandle m_curhandle;
    };

    typedef SharedPtr<IResource> ResourcePtr;

} // namespace FIFE

#endif
