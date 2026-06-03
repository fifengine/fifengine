// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/model.h"
%}

%ignore FIFE::Model::Model;
%ignore FIFE::Model::adoptPather;
%ignore FIFE::Model::adoptCellGrid;

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
		~Model();

		Map* createMap(const std::string& identifier);
		void deleteMap(Map*);

		std::list<Map*> getMaps() const;
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
		void adoptCellGrid(CellGrid* grid);
		CellGrid* getCellGrid(const std::string& gridtype);

		void setTimeMultiplier(float multip);
		double getTimeMultiplier() const;

	};
}

%template(RendererBaseVector) std::vector<FIFE::RendererBase*>;

%extend FIFE::Model {
  Model(FIFE::RenderBackend* rb, const std::vector<FIFE::RendererBase*>& renderers) {
    std::vector<std::unique_ptr<FIFE::RendererBase>> unique_ptrs;
    unique_ptrs.reserve(renderers.size());
    for (auto* r : renderers) {
      unique_ptrs.emplace_back(r);
    }
    return new FIFE::Model(rb, std::move(unique_ptrs));
  }

  void adoptPather(FIFE::IPather* pather) {
    $self->adoptPather(std::unique_ptr<FIFE::IPather>(pather));
  }

  void adoptCellGrid(FIFE::CellGrid* grid) {
    $self->adoptCellGrid(std::unique_ptr<FIFE::CellGrid>(grid));
  }
}