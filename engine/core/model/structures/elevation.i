%module model
%{
#include "model/structures/elevation.h"
%}

%include "util/point.h"
%include "util/attributedclass.i"

namespace FIFE { namespace model {

	class Map;
	class Layer;
	class GeometryType;

	class Elevation : public AttributedClass {
		public:
			~Elevation();

			Map* getMap();

			Layer* addLayer(GeometryType* geometry);
			void removeLayer(Layer* index);

			template<typename T>
			std::list<Layer*> getLayers(const std::string& field, const T& value) const;

			size_t getNumLayers() const;
			void clearLayers();

			void setReferenceLayer(Layer* layer);
			Layer* getReferenceLayer();

			Point centerOfMass();

		private:
			Elevation();
	};
}}
