%module(directors="1") vfs
%{
#include "vfs/vfsutility.h"
%}

%include "std_string.i"
%include "std_vector.i"

namespace std {
   %template(vectors) vector<std::string>;
   %template(vectoru) vector<uint8_t>;
};

typedef unsigned char uint8_t;

namespace FIFE {
	class VFSUtility {
		public:
			VFSUtility();
			virtual ~VFSUtility();
			std::vector<uint8_t> readBytes(const std::string& fname);
			std::vector<std::string> readLines(const std::string& fname);
			std::vector<std::string> listFiles(const std::string& path);
			std::vector<std::string> listDirectories(const std::string& path);
			bool addSource(const std::string& sname);
	};
}
