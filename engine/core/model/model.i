%module model
%{
#include "model/model.h"
%}

namespace FIFE { namespace model {

  class Map;

  class MetaModel;

	class Model {
		public:

			Model();
			~Model();

			Map* addMap();
			void removeMap(Map*);

			template<typename T>
			std::list<Map*> getMaps(const std::string& field, const T& value) const;

			size_t getNumMaps() const;
			void clearMaps();

			MetaModel* getMetaModel();

		private:
	};
}}
