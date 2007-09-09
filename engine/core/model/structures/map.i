%module model
%{
#include "model/structures/map.h"
%}

%include "std_list.i"

%include "util/attributedclass.i"

namespace FIFE {
  class Elevation;
}

namespace std {
  %template(ElevationList) list<FIFE::Elevation*>;
}

namespace FIFE {

  class Elevation;

  class Dataset;

	class Map : public AttributedClass {
		public:

			Map(const std::string& identifier);
			~Map();

			void useDataset(Dataset* dataset);

			Elevation* addElevation(const std::string& identifier);
			void removeElevation(Elevation*);

			template<typename T>
			std::list<Elevation*> getElevations(const std::string& field, const T& value) const;
			%template(getElevationsByBool) getElevations<bool>;
			%template(getElevationsByInt) getElevations<long>;
			%template(getElevationsByRect) getElevations<Rect>;
			%template(getElevationsByPoint) getElevations<Point>;
			%template(getElevationsByString) getElevations<std::string>;

			size_t getNumElevations() const;
			void clearElevations();

			void update();
	};
}
