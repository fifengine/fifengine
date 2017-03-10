/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

namespace FIFE {

	typedef std::size_t ResourceHandle;

	class IResource;

	class IResourceLoader {
	public:
		IResourceLoader() { }
		virtual ~IResourceLoader() { }

		virtual void load(IResource* resource) = 0;
	};

	class IResource {
	public:
		enum ResourceState {
			RES_INVALID = 0,
			RES_NOT_LOADED,
			RES_LOADED
		};

		IResource(const std::string& name, IResourceLoader* loader = 0)
		: m_name(name),
		  m_loader(loader),
		  m_state(RES_NOT_LOADED),
		  m_handle(m_curhandle++) { }

		virtual ~IResource() { }

		virtual const std::string& getName() { return m_name; }

		ResourceHandle getHandle() { return m_handle; }

		virtual ResourceState getState() { return m_state; }
		virtual void setState(const ResourceState& state) { m_state = state; }

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

} //FIFE

#endif
