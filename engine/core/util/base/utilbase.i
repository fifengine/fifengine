/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/base/resourceclass.h"
#include "util/base/fifeclass.h"
%}

%include "util/base/exception.h"
%include "util/resource/resource.i"

namespace FIFE {

	typedef unsigned int fifeid_t;
	
	class FifeClass: public IReferenceCounted {
	public:
		virtual ~FifeClass();
		fifeid_t getFifeId();
		void addRef();
		void decRef();
		unsigned int getRefCount();
	};

	class ResourceClass: public FifeClass, public IResource {
	public:
		virtual ~ResourceClass();
		const ResourceLocation& getResourceLocation();
		const std::string& getResourceFile();

		virtual void setResourceLocation(const ResourceLocation& location);
		virtual void setResourceFile(const std::string& filename);
	
		int getPoolId();
		void setPoolId(int poolid);
	
	private:
		ResourceClass();
	};	
	
}
