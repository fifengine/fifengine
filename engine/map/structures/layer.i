%module mapstructure
%{
#include "map/structures/layer.h"
%}

namespace FIFE { namespace map {

	class Layer {
		public:
			~Layer();

			bool hasObjects() const;

		private:
			Layer();
	};
}}
