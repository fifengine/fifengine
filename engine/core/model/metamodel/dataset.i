%module fife
%{
#include "model/metamodel/dataset.h"
#include "model/metamodel/dataset_loader.h"
%}

%include "util/base/utilbase.i"

namespace FIFE {

	class Object;

	class Dataset : public AttributedClass {
		public:

			Dataset(const std::string& identifier);
			~Dataset();

			void setSource(const std::string& src);
			const std::string& getSource();

			void addDataset(Dataset&);

			Object* createObject(const std::string& identifier, Object* inherited = 0);

			std::list<Dataset*> getDatasets();
			std::list<Dataset*> getDatasetsRec();
			std::list<Dataset*> getDatasets(const std::string& field, const std::string& value);

			std::list<Object*> getObjects();
			std::list<Object*> getObjects(const std::string& field, const std::string& value);

		private:

			Dataset();
	};

	class DatasetLoader : public ResourceLoader {
	public:
		Dataset* load(const ResourceLocation& location); 
		Dataset* load(const std::string& filename); 
	};
}
