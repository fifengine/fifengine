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

	class Model {
		public:

			Model();
			~Model();

			Map* addMap();
			void removeMap(Map*);

			std::list<Map*> getMaps() const;

			template<typename T>
			std::list<Map*> getMaps(const std::string& field, const T& value) const;
			%template(getMapsByBool) getMaps<bool>;
			%template(getMapsByInt) getMaps<long>;
			%template(getMapsByPoint) getMaps<Point>;
			%template(getMapsByRect) getMaps<Rect>;
			%template(getMapsByString) getMaps<std::string>;

			size_t getNumMaps() const;
			void clearMaps();

			MetaModel* getMetaModel();
			void update();
	};
}
