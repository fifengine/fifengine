%module fife
%{
#include "model/metamodel/timeprovider.h"
%}

namespace FIFE {
	class TimeProvider {
	public:
		TimeProvider(TimeProvider* master);
		~TimeProvider();
		
		void setMultiplier(float multiplier);
		float getMultiplier() const;
		float getTotalMultiplier() const;
		unsigned int getGameTicks() const;
	};
}
