%module model
%{
#include "model/metamodel/metamodel.h"
%}

namespace FIFE { namespace model {

  class Dataset;

	class MetaModel {
		public:

			MetaModel();
			~MetaModel();

			Dataset* addDataset(const std::string& type, const std::string& filename);

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

		private:
	};
}}
