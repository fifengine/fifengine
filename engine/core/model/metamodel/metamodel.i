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

			Dataset* addDataset();

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

		private:
	};
}}
