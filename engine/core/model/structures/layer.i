%module model
%{
#include "model/structures/layer.h"
%}

%include "util/point.h"
%include "util/attributedclass.i"

namespace FIFE { namespace model {

	class Elevation;
	class Selection;
	class Instance;
	class Object;

	class Layer : public AttributedClass {
		public:
			~Layer();

			Elevation* getElevation();

			void setShift(const Point& shift);
			const Point& getShift() const;

			bool hasInstances() const;

			Instance* addInstance(Object* object, const Point& p);
			void removeInstance(Instance* object);

			const std::vector<Instance*>& getInstances();

			template<typename T>
			std::vector<Instance*> getInstances(const std::string& field, const T& value);

			void setInstancesVisible(bool vis);
			void toggleInstancesVisible();
			bool areInstancesVisible() const;

		private:
			Layer();
	};
}}
