#ifndef IANFALLOUTLST_H
#define IANFALLOUTLST_H

#include <string>
#include <vector>

namespace FIFE {

	class MFFalloutLST {
		public:
			MFFalloutLST(const std::string& path);
			~MFFalloutLST();

			const std::string& getProFile(size_t index) const;

		private:
			std::string m_filename;

			typedef std::vector<std::string> type_list;
			type_list m_list;

			void load();
			std::string cleanupString(std::string str, const std::string& c) const;

	};

}//FIFE

#endif

