%module fife
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

			void addDataset(Dataset* dataset);
			std::list<Dataset*> getDatasets();
			std::list<Dataset*> getDatasetsRec();

			Elevation* createElevation(const std::string& identifier);
			void deleteElevation(Elevation*);

			std::list<Elevation*> getElevations() const;
			std::list<Elevation*> getElevations(const std::string& field, const std::string& value) const;

			size_t getNumElevations() const;
			void deleteElevations();

			void update();
	};
}
