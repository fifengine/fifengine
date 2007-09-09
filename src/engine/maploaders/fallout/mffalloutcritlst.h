#ifndef FIFEFALLOUTCRITLST_H
#define FIFEFALLOUTCRITLST_H

#include <string>
#include <vector>

namespace FIFE {

	class MFFalloutCritLST {
		public:
			MFFalloutCritLST(const std::string& path);
			~MFFalloutCritLST();

			const std::string& getProFile(size_t index) const;
			const size_t getReIndex(size_t index);

		private:
			std::string m_filename;

			typedef std::vector<std::string> type_list;
			type_list m_filelist;

			typedef std::vector<size_t> index_list;
			index_list m_indexlist;

			typedef std::vector<bool> special_list;
			special_list m_speciallist;

			void load();
			std::string cleanupString(std::string str, const std::string& c) const;

	};

}//FIFE

#endif

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
