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

			Object* addObject(const Object* inherited = 0);

			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

			template<typename T>
			std::list<GeometryType*> getGeometryTypes(const std::string& field, const T& value) const;

		private:

			Dataset();
	};
}}
