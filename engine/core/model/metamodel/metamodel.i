%module model
%{
#include "model/metamodel/metamodel.h"
%}

%include "std_list.i"

namespace FIFE { namespace model {
  class Object;
  class Dataset;
} }

namespace std {
   %template(ObjectList) list<FIFE::model::Object*>;
   %template(DatasetList) list<FIFE::model::Dataset*>;
}

namespace FIFE { namespace model {

	class MetaModel {
		public:

			MetaModel();
			~MetaModel();

			Dataset* addDataset();

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
}}
