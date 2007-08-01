%module model
%{
#include "model/structures/map.h"
%}

%include "util/attributedclass.i"

namespace FIFE { namespace model {

  class Elevation;

  class Dataset;

	class Map : public AttributedClass {
		public:

			Map();
			~Map();

			void useDataset(Dataset* dataset);

			Elevation* addElevation();
			void removeElevation(Elevation*);

			template<typename T>
			std::list<Elevation*> getElevations(const std::string& field, const T& value) const;

			size_t getNumElevations() const;
			void clearElevations();

		private:
	};
}}
