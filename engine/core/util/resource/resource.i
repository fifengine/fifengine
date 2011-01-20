/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/
%module fife

%{
#include "util/base/sharedptr.h"
#include "util/resource/resource.h"
#include "util/resource/resourcemanager.h"
%}

namespace FIFE {

//	%warnfilter(473) ResourceLoader; // filter out "returning a pointer or reference in a director method is not recommended"
//	%feature("director") ResourceLoader;
//	class ResourceLoader {
//	public:
//		virtual ~ResourceLoader() { };
//		virtual IResource* loadResource(const ResourceLocation& location) = 0;
//	};

	typedef std::size_t ResourceHandle;

	class IResource;

	class IResourceLoader {
	public:
		virtual ~IResourceLoader() { }

		virtual void load(IResource* resource) = 0;
	};

	class IResource {
	public:
		enum ResourceState {
			RES_NOT_LOADED,
			RES_LOADED
		};

		virtual ~IResource();

		virtual const std::string& getName();

		ResourceHandle getHandle();

		virtual ResourceState getState();
		virtual void setState(ResourceState& state);

		virtual size_t getSize() = 0;

		virtual void load() = 0;
		virtual void free() = 0;
	};

    template <typename T>
    class SharedPtr {
    public:
        SharedPtr();
        
        template <typename U>
        explicit SharedPtr(U *ptr);
        
        SharedPtr(const SharedPtr& rhs);
        
        template <typename U>
        SharedPtr(const SharedPtr<U>& rhs);
        
        ~SharedPtr();
        
        inline T& operator*() const;
        inline T* operator->() const;
        inline T* get() const;
        inline void reset(T* ptr = 0);
        inline uint32_t useCount() const;
        inline uint32_t* useCountPtr() const;
        inline bool unique() const;
        operator bool();
	};

	typedef SharedPtr<IResource> ResourcePtr;
	
	class IResourceManager {
	public:
		virtual ~IResourceManager();

		virtual size_t getMemoryUsed() const;
		virtual uint32_t getTotalResourcesCreated() const;
		virtual uint32_t getTotalResourcesLoaded() const;
		virtual uint32_t getTotalResources() const;

		virtual ResourcePtr create(const std::string& name, IResourceLoader* loader = 0) = 0;
		virtual ResourcePtr load(const std::string& name, IResourceLoader* loader = 0);
		virtual ResourcePtr add(IResource* res);

		virtual bool exists(const std::string& name);
		virtual bool exists(ResourceHandle handle);

		virtual void reload(const std::string& name);
		virtual void reload(ResourceHandle handle);
		virtual void reloadAll();

		virtual void loadUnreferenced();

		virtual void free(const std::string& name);
		virtual void free(ResourceHandle handle);
		virtual void freeAll();
		virtual void freeUnreferenced();

		virtual void remove(const std::string& name);
		virtual void remove(ResourceHandle handle);
		virtual void removeAll();
		virtual void removeUnreferenced();

		virtual ResourceHandle getResourceHandle(const std::string& name);
	};
}
