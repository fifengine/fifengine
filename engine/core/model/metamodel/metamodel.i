%module model
%{
#include "model/metamodel/metamodel.h"
%}

%include "std_list.i"

namespace FIFE { namespace model {
  class Object;
} }

namespace std {
   %template(ObjectList) list<FIFE::model::Object*>;
}

namespace FIFE { namespace model {

  class Dataset;

	class MetaModel {
		public:

			MetaModel();
			~MetaModel();

			Dataset* addDataset();

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;
//			%template(getObjectsByBool) getObjects<bool>;
//			%template(getObjectsByInt) getObjects<long>;
//			%template(getObjectsByPoint) getObjects<Point>;
//			%template(getObjectsByRect) getObjects<Rect>;
//			%template(getObjectsByString) getObjects<std::string>;

		private:
	};
}}
