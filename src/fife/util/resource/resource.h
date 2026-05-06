// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_RESOURE_H
#define FIFE_RESOURE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/base/sharedptr.h"

namespace FIFE
{

    using ResourceHandle = std::size_t;
    class IResource;

    class /*FIFE_API*/ IResourceLoader
    {
        public:
            IResourceLoader()          = default;
            virtual ~IResourceLoader() = default;

            virtual void load(IResource* resource) = 0;
    };

    class /*FIFE_API*/ IResource
    {
        public:
            enum ResourceState : uint8_t
            {
                RES_INVALID = 0,
                RES_NOT_LOADED,
                RES_LOADED
            };

            // TODO m_handle(m_curhandle++)
            explicit IResource(std::string name, IResourceLoader* loader = nullptr) :
                m_name(std::move(name)), m_loader(loader), m_state(RES_NOT_LOADED), m_handle(m_curhandle++)
            {
            }

            virtual ~IResource() = default;

            IResource(IResource const &)            = default;
            IResource& operator=(IResource const &) = default;

            virtual std::string const & getName()
            {
                return m_name;
            }

            ResourceHandle getHandle() const
            {
                return m_handle;
            }

            virtual ResourceState getState()
            {
                return m_state;
            }
            virtual void setState(ResourceState const & state)
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

    using ResourcePtr = SharedPtr<IResource>;
} // namespace FIFE

#endif
