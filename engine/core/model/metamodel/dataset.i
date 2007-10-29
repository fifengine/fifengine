%module fife
%{
#include "model/metamodel/dataset.h"
%}

%include "util/attributedclass.i"

namespace FIFE {

	class Object;

	class Dataset : public AttributedClass {
		public:

			Dataset(const std::string& identifier);
			~Dataset();

			void setSource(const std::string& src);
			const std::string& getSource();

			Dataset* addDataset(const std::string& identifier);

			Object* addObject(const std::string& identifier, Object* inherited = 0);

			std::list<Dataset*> getDatasets();
			std::list<Dataset*> getDatasetsRec();
			std::list<Dataset*> getDatasets(const std::string& field, const std::string& value);

			std::list<Object*> getObjects();
			std::list<Object*> getObjects(const std::string& field, const std::string& value);

		private:

			Dataset();
	};
}
