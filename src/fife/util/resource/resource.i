// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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
		virtual void setState(const ResourceState& state);

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
	%template(SharedResourcePointer) SharedPtr<IResource>;
	
	class IResourceManager {
	public:
		virtual size_t getMemoryUsed() const = 0;
		virtual size_t getTotalResourcesCreated() const = 0;
		virtual size_t getTotalResourcesLoaded() const = 0;
		virtual size_t getTotalResources() const = 0;
	};
}