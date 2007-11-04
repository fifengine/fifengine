%module fife
%{
#include "model/structures/layer.h"
%}

%include "std_vector.i"

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/grids/cellgrids.i"
%include "util/point.i"
%include "util/attributedclass.i"

namespace FIFE {
  class Instance;
}

namespace std {
  %template(InstanceVector) vector<FIFE::Instance*>;
}

namespace FIFE {

	class Elevation;
	class Selection;
	class Instance;
	class Object;
	class CellGrid;

	class Layer : public AttributedClass {
		public:
			Layer(const std::string& identifier, Elevation* elevation, CellGrid* geometry);
			~Layer();

			CellGrid* getCellGrid() const { return m_grid; }

			Elevation* getElevation();
			bool hasInstances() const;
			Instance* addInstance(Object* object, const ModelCoordinate& p, const std::string& id="");
			Instance* addInstance(Object* object, const ExactModelCoordinate& p, const std::string& id="");
			void removeInstance(Instance* object);

			const std::vector<Instance*>& getInstances();
			std::vector<Instance*> getInstances(const std::string& field, const std::string& value);

			void setInstancesVisible(bool vis);
			void toggleInstancesVisible();
			bool areInstancesVisible() const;

			void update();
	};
}
