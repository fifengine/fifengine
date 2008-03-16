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

		std::list<Map*> getMaps() const;
		std::list<Map*> getMaps(const std::string& field, const std::string& value) const;

		Object* createObject(const std::string& identifier, const std::string& name_space, Object* parent=0);
		Object* getObject(const std::string& id, const std::string& name_space);
		std::list<Object*>& getObjects(const std::string& name_space);

		size_t getNumMaps() const;
		void deleteMaps();

		AbstractPather* getPather(const std::string& pathername);
		
		void setTimeMultiplier(float multip);
		double getTimeMultiplier() const;
		
	};
}
