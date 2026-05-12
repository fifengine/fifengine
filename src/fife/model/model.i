// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/model.h"
%}

namespace FIFE {
  class Map;
  class Object;
}

%template(StringList) std::list<std::string>;
%template(ObjectList) std::list<FIFE::Object*>;
%template(MapList) std::list<FIFE::Map*>;

namespace FIFE {
	class IPather;

	class Model: public FifeClass {
	public:
		Model(RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers);
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

		uint32_t getMapCount() const;
		void deleteMaps();

		void adoptPather(IPather* pather);
		IPather* getPather(const std::string& pathername);
		CellGrid* getCellGrid(const std::string& gridtype);

		void setTimeMultiplier(float multip);
		double getTimeMultiplier() const;

	};
}