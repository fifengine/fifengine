%module model
%{
#include "model/metamodel/dataset.h"
%}

namespace FIFE { namespace model {

	class Dataset {
		public:

			~Dataset();

			const std::string& getTypeName() const;
			const std::string& getFilename() const;

			Dataset* addDataset(const std::string& type, const std::string& filename);

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

			template<typename T>
			std::list<GeometryType*> getGeometryTypes(const std::string& field, const T& value) const;

		private:

			Dataset(const std::string& type, const std::string& filename);
	};
}}
