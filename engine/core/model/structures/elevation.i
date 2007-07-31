%module model
%{
#include "model/structures/elevation.h"
%}

%include "util/point.h"

namespace FIFE { namespace model {

  class Map;

  class GeometryType;

	class Elevation {
		public:
			~Elevation();

			Map* getMap();

			Layer* addLayer(const Point& size, GeometryType* geometry);
			void removeLayer(Layer* index);

			template<typename T>
			Layer* getLayer(const std::string& field, const T& value) const;
			size_t getNumLayers() const;
			void clearLayers();

			template<typename T>
			void forEachLayer(T visitor); 

			void setReferenceLayer(Layer* layer);
			Layer* getReferenceLayer();

			Point centerOfMass();

		private:
			Elevation();
	};
}}
