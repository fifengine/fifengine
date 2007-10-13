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
			template<typename T>
			std::list<Dataset*> getDatasets(const std::string& field, const T& value);
			%template(getDatasetsByBool) getDatasets<bool>;
			%template(getDatasetsByInt) getDatasets<long>;
			%template(getDatasetsByRect) getDatasets<Rect>;
			%template(getDatasetsByPoint) getDatasets<Point>;
			%template(getDatasetsByString) getDatasets<std::string>;

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value);
			%template(getObjectsByBool) getObjects<bool>;
			%template(getObjectsByInt) getObjects<long>;
			%template(getObjectsByPoint) getObjects<Point>;
			%template(getObjectsByRect) getObjects<Rect>;
			%template(getObjectsByString) getObjects<std::string>;

		private:
	};
}
