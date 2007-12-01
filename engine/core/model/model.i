%module fife
%{
#include "model/model.h"
%}

%include "std_list.i"

namespace FIFE {
  class Map;
}

namespace std {
  %template(MapList) list<FIFE::Map*>;
}

namespace FIFE {
	class MetaModel;
	class AbstractPather;

	class Model: public FifeClass {
	public:

		Model();
		~Model();

		Map* createMap(const std::string& identifier);
		void deleteMap(Map*);

		std::list<Map*> getMaps() const;
		std::list<Map*> getMaps(const std::string& field, const std::string& value) const;

		size_t getNumMaps() const;
		void deleteMaps();

		MetaModel* getMetaModel();
		void update();
		AbstractPather* getPather(const std::string& pathername);
	};
}
