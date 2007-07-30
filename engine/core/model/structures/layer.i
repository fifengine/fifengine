%module layer
%{
#include "model/structures/layer.h"
%}

%include "util/point.h"

namespace FIFE { namespace model {

  class Elevation;
  class Selection;

  class Object;

	class Layer {
		public:
			~Layer();

			Elevation* getElevation();

			const Point& getSize() const;
			void setShift(const Point& shift);
			const Point& getShift() const;

			bool hasInstances() const;

			Instance* addInstance(Object* object, const Point& p);
			void removeInstance(Instance* object);

			const std::vector<Instance*>& getInstances();

			template<typename T>
			std::vector<Instance*> getInstances(const std::string& field, const T& value);

			template<typename T>
			void forEachInstance(T visitor);

			void setInstancesVisible(bool vis);
			void toggleInstancesVisible();
			bool areInstancesVisible() const;

			bool isGridOverlayEnabled() const;
			void setGridOverlayEnabled(bool);
			void toggleGridOverlayEnabled();

			const Selection& getSelection();

			bool isValidPosition(const Point& pos) const;
			bool isValidPosition(int32_t x,int32_t y) const;

		private:
			Layer();
	};
}}
