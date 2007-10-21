%module model
%{
#include "model/metamodel/metamodel.h"
%}

%include "std_list.i"

namespace FIFE {
	class Object;
	class Dataset;
}

namespace std {
	%template(ObjectList) list<FIFE::Object*>;
	%template(DatasetList) list<FIFE::Dataset*>;
}

namespace FIFE {

	class MetaModel {
		public:

			MetaModel();
			~MetaModel();

			Dataset* addDataset(const std::string& identifier);

			std::list<Dataset*> getDatasets();
			std::list<Dataset*> getDatasets(const std::string& field, const std::string& value);

			std::list<Object*> getObjects(const std::string& field, const std::string& value);

		private:
	};
}
