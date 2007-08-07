%module model
%{
#include "model/metamodel/dataset.h"
%}

%include "util/attributedclass.i"

namespace FIFE { namespace model {

  class GeometryType;
  class Object;

	class Dataset : public AttributedClass {
		public:

			~Dataset();

			Dataset* addDataset();

			Object* addObject(Object* inherited = 0);

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
			%template(getObjectsByRect) getObjects<Rect>;
			%template(getObjectsByPoint) getObjects<Point>;
			%template(getObjectsByString) getObjects<std::string>;

			template<typename T>
			std::list<GeometryType*> getGeometryTypes(const std::string& field, const T& value);
			%template(getGeometryTypesByBool) get<bool>;
			%template(getGeometryTypesByInt) get<long>;
			%template(getGeometryTypesByRect) get<Rect>;
			%template(getGeometryTypesByPoint) get<Point>;
			%template(getGeometryTypesByString) get<std::string>;

		private:

			Dataset();
	};
}}
