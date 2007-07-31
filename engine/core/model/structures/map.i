%module model
%{
#include "model/structures/map.h"
%}

namespace FIFE { namespace model {

  class Elevation;

  class Dataset;

	class Map {
		public:

			Map();
			~Map();

			void useDataset(Dataset* dataset);

			Elevation* addElevation();
			void removeElevation(Elevation*);

			template<typename T>
			Elevation* getElevation(const std::string& field, const T& value) const;
			size_t getNumElevations() const;
			void clearElevations();

			template<typename T>
			void forEachElevation(T visitor); 

		private:
	};
}}
