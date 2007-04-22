/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "settingsmanager.h"
#include "debugutils.h"
#include "exception.h"
#include <fstream>

static const std::string SETTINGS_FILE = "fife.config";

namespace FIFE {

	SettingsManager::SettingsManager() : m_settings() {
		loadSettings();
	}

	SettingsManager::~SettingsManager() {
		saveSettings();
	}

	void SettingsManager::loadSettings() {
		std::ifstream settings(SETTINGS_FILE.c_str());
		if (!settings) {
			Log() << "no settingsfile (" << SETTINGS_FILE << ") found";
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

	void SettingsManager::saveSettings() const {
		std::ofstream settings(SETTINGS_FILE.c_str());
		if (!settings) throw CannotOpenFile(SETTINGS_FILE);

		type_settings::const_iterator end = m_settings.end();
		for (type_settings::const_iterator i = m_settings.begin(); i != end; ++i) {
			settings << i->first << "=" << i->second << std::endl;
		}
	}

}//FIFE

