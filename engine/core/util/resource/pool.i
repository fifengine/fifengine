/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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
#include "util/resource/pooled_resource.h"
#include "util/resource/pool.h"
%}

namespace FIFE {

	class IPooledResource {
	public:
		virtual ~IPooledResource() {};
		virtual void addRef() = 0;
		virtual void decRef() = 0;
		virtual unsigned int getRefCount() = 0;
	};

	enum { RES_LOADED = 0x01, RES_NON_LOADED  = 0x02};

	class Pool: public FifeClass {
	public:
		static const int INVALID_ID = -1;
		virtual ~Pool();
		virtual int addResourceFromFile(const std::string& filename);
		virtual int getResourceCount(int status);
	private:
		Pool();
	};

}