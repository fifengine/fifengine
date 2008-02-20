/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/
%module fife

%{
#include "util/resource/resource.h"
#include "util/resource/resource_location.h"
%}

namespace FIFE {

	class IResource {
	public:
		virtual ~IResource() {};
		virtual const std::string& getResourceFile() = 0;
		virtual void setResourceFile(const std::string& location) = 0;
	};

	class IResourceLoader {
	public:
		virtual ~IResourceLoader() {};
		virtual IResource* loadResource(const std::string& location) = 0;
	};

	class IResourceSaver {
	public:
		virtual ~IResorceSaver() {};
		virtual void save(const std::string& location, IResource* resource) = 0;
	};
}
