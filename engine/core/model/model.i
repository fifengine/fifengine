%module fife
%{
#include "model/model.h"
%}

%include "std_list.i"

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
		Model();
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
		const std::list<Object*>& getObjects(const std::string& name_space) const;

		size_t getNumMaps() const;
		void deleteMaps();

		AbstractPather* getPather(const std::string& pathername);
		CellGrid* getCellGrid(const std::string& gridtype);
		
		void setTimeMultiplier(float multip);
		double getTimeMultiplier() const;
		
	};
}
