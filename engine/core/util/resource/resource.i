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
#include "util/resource/resource.h"
#include "util/resource/resource_ptr.h"
#include "util/resource/resource_location.h"
#include "util/resource/pool.h"
%}

namespace FIFE {

	enum ResourceLocationType {
		RES_TYPE_FILE = 0,
		RES_TYPE_IMAGE = 1
	};
	
	
	class ResourceLocation {
	public:
		ResourceLocation(const std::string& filename);
		virtual ~ResourceLocation() {};
		const std::string& getFilename() const;
		virtual bool operator ==(const ResourceLocation& loc) const;
		virtual bool operator <(const ResourceLocation& loc) const;
		virtual ResourceLocation* clone() const;
		
		ResourceLocationType getType() const;
	};


	class IReferenceCounted {
		virtual ~IReferenceCounted();
		virtual void addRef() = 0;
		virtual void decRef() = 0;
		virtual unsigned int getRefCount() = 0;
	};

	class IResource: public IReferenceCounted {
	public:
		virtual ~IResource() {};
		virtual const ResourceLocation& getResourceLocation() = 0;
		virtual const std::string& getResourceFile() = 0;
		virtual void setResourceLocation(const ResourceLocation& location) = 0;
		virtual void setResourceFile(const std::string& filename) = 0;
		virtual int getPoolId() = 0;
		virtual void setPoolId(int poolid) = 0;
		virtual unsigned int getRefCount() = 0;
	};

	%warnfilter(473) ResourceLoader; // filter out "returning a pointer or reference in a director method is not recommended"
	%feature("director") ResourceLoader;
	class ResourceLoader {
	public:
		virtual ~ResourceLoader() { };
		virtual IResource* loadResource(const ResourceLocation& location) = 0;
	};

	class ResourceSaver {
	public:
		virtual ~ResourceSaver() { };
		virtual void save(const ResourceLocation& location, IResource* resource) = 0;
		virtual void save(const std::string& filename, IResource* resource) { save(ResourceLocation(filename), resource); }
	};

	class Pool {
	public:
		static const int INVALID_ID = -1;
		virtual ~Pool();
		virtual int addResourceFromFile(const std::string& filename);
		virtual int addResourceFromLocation(ResourceLocation* loc);
		virtual int getResourceCount(int status);
		virtual int purgeLoadedResources();
		virtual void addResourceLoader(ResourceLoader* loader);
		virtual void clearResourceLoaders();
		virtual void release(unsigned int index, bool dec = false);
		virtual IResource& get(unsigned int index, bool inc = false);
		virtual void printStatistics();

	private:
		Pool();
	};

	class ResourcePtr {
	public:
		ResourcePtr(IResource* ptr = 0);
		ResourcePtr(Pool* pool,int index);
		ResourcePtr(const ResourcePtr& r);
		void release();
		void load();
		void unload();
	};
}
