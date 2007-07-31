%module model
%{
#include "model/metamodel/dataset.h"
%}

namespace FIFE { namespace model {

	class Dataset {
		public:

			~Dataset();

			Dataset* addDataset();

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

			template<typename T>
			std::list<GeometryType*> getGeometryTypes(const std::string& field, const T& value) const;

		private:

			Dataset();
	};
}}
