#include "mffalloutlst.h"
#include "exception.h"
#include "vfs.h"
#include "rawdata.h"
#include "log.h"

namespace FIFE {

	MFFalloutLST::MFFalloutLST(const std::string& path) : m_filename(path), m_list() {
		load();
	}

	MFFalloutLST::~MFFalloutLST() {}

	void MFFalloutLST::load() {
		RawDataPtr input = VFS::instance()->open(m_filename);

		std::string buffer;
		while (input->getLine(buffer)) {
			buffer = cleanupString(buffer, " ");
			buffer = cleanupString(buffer, "\r");

			m_list.push_back(buffer);
		}
	}

	const std::string& MFFalloutLST::getProFile(size_t index) const {
		if (m_list.size() <= index) {
			Log("MFFalloutLST") << "size: " << m_list.size() << " requested: " << index;
			throw IndexOverflow("Index overflow in " + m_filename);
		}

		return *(m_list.begin() + index);
	}

	std::string MFFalloutLST::cleanupString(std::string str, const std::string& c) const {
		size_t pos = str.find(c);
		while (pos != std::string::npos) {
			str.erase(pos);
			pos = str.find(c);
		}

		return str;
	}

}//FIFE

