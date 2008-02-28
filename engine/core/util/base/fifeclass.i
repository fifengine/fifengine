%module fife
%{
#include "util/base/fifeclass.h"
%}

namespace FIFE {
	typedef unsigned int fifeid_t;
	
	class FifeClass {
	public:
		virtual ~FifeClass();
		fifeid_t getFifeId();
	};
}
