%module fife
%{
#include "util/fifeclass.h"
%}

namespace FIFE {
	typedef unsigned int fifeid_t;
	
	class FifeClass {
	public:
		virtual ~FifeClass();
		fifeid_t getFifeId();
	};
}
