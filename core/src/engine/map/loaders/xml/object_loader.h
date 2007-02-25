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

#ifndef FIFE_MAP_LOADERS_XML_OBJECT_H
#define FIFE_MAP_LOADERS_XML_OBJECT_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/point.h"
#include "map/objectinfo.h"
#include "tinyxml/tinyxml.h"
#include "attributedclass.h"

namespace FIFE { namespace map { namespace loaders { namespace xml {

	template<typename datatype>
	struct MaybeData {
		MaybeData() : haveData(false) {}

		MaybeData& operator=(const datatype& d) {
			data = d;
			haveData = true;
		}

		datatype data;
		bool haveData;
	};

	class ObjectLoader {
		public:
			ObjectLoader(TiXmlElement* element);

			ObjectInfo* create();
			void merge(ObjectInfo* object);

		private:
			MaybeData<int>  m_zvalue;
			MaybeData<bool> m_isstatic;
			MaybeData<Point> m_position;

			Table m_data;
	};

}}}}
#endif

