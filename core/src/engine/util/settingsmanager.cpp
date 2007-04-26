/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <fstream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"
#include "util/settingsmanager.h"

namespace FIFE {

	SettingsManager::SettingsManager() : 
		m_settings(),
		m_settings_file_name("") {
	}

	SettingsManager::~SettingsManager() {
	}

	void SettingsManager::loadSettings(const std::string& settings_file_name) {
		std::ifstream settings(settings_file_name.c_str());
		if (!settings) {
			Log() << "no settingsfile (" << settings_file_name << ") found";
			return;
		}

		std::string buffer;
		while (getline(settings, buffer)) {
			size_t pos = buffer.find_first_of("=");
			if (!buffer.size() || buffer[0] == '#' || pos == std::string::npos) {
				continue;
			}

			std::string key = buffer.substr(0, pos);
			std::string value = buffer.substr(pos + 1, buffer.size());
			m_settings.insert(std::make_pair(key, value));
		}
	}

	void SettingsManager::saveSettings(const std::string& settings_file_name, bool create_on_failure) const {
		std::fstream testfile(settings_file_name.c_str(), std::ios_base::in);
		if (!testfile && !create_on_failure) {
			throw CannotOpenFile(settings_file_name);
		}
		testfile.close();

		std::ofstream settings(settings_file_name.c_str());
		type_settings::const_iterator end = m_settings.end();
		for (type_settings::const_iterator i = m_settings.begin(); i != end; ++i) {
			settings << i->first << "=" << i->second << std::endl;
		}
	}

}//FIFE

