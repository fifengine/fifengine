%module model
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

	class Model {
	public:

		Model();
		~Model();

		Map* addMap(const std::string& identifier);
		void removeMap(Map*);

		std::list<Map*> getMaps() const;
		std::list<Map*> getMaps(const std::string& field, const std::string& value) const;

		size_t getNumMaps() const;
		void clearMaps();

		MetaModel* getMetaModel();
		void update();
		AbstractPather* getPather(const std::string& pathername);
	};
}
