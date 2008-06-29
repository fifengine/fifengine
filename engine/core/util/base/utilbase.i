%module fife
%{
#include "util/base/resourceclass.h"
#include "util/base/fifeclass.h"
%}

%include "util/base/exception.h"
%include "util/resource/resource.i"

namespace FIFE {

	typedef unsigned int fifeid_t;
	
	class FifeClass: public IReferenceCounted {
	public:
		virtual ~FifeClass();
		fifeid_t getFifeId();
		void addRef();
		void decRef();
		unsigned int getRefCount();
	};

	class ResourceClass: public FifeClass, public IResource {
	public:
		virtual ~ResourceClass();
		const ResourceLocation& getResourceLocation();
		const std::string& getResourceFile();

		virtual void setResourceLocation(const ResourceLocation& location);
		virtual void setResourceFile(const std::string& filename);
	
		int getPoolId();
		void setPoolId(int poolid);
	
	private:
		ResourceClass();
	};	
	
}
