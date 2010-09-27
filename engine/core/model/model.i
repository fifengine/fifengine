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
#include "model/model.h"
%}

namespace FIFE {
  class Map;
  class Object;
}

namespace std {
  %template(StringList) list<std::string>;
  %template(ObjectList) list<FIFE::Object*>;
  %template(MapList) list<FIFE::Map*>;
}

namespace FIFE {
	class AbstractPather;

	class Model: public FifeClass {
	public:
		Model(RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers, ImagePool* imagepool, AnimationPool* animpool);
		~Model();

		Map* createMap(const std::string& identifier);
		void deleteMap(Map*);

		const std::list<Map*>& getMaps() const;
		Map* getMap(const std::string& id) const;

		std::list<std::string> getNamespaces() const;

		Object* createObject(const std::string& identifier, const std::string& name_space, Object* parent=0);
		bool deleteObject(Object*);
		bool deleteObjects();
		Object* getObject(const std::string& id, const std::string& name_space);
		std::list<Object*> getObjects(const std::string& name_space) const;

		uint32_t getNumMaps() const;
		void deleteMaps();

		AbstractPather* getPather(const std::string& pathername);
		CellGrid* getCellGrid(const std::string& gridtype);
		
		void setTimeMultiplier(float multip);
		double getTimeMultiplier() const;
		
	};
}
