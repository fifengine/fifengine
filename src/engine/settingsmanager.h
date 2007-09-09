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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "singleton.h"
#include "exception.h"
#include <map>
#include <string>

#include <boost/lexical_cast.hpp>

namespace FIFE {

	class SettingsManager : public DynamicSingleton<SettingsManager> {
		public:
			SettingsManager();
			virtual ~SettingsManager();

			void loadSettings();
			void saveSettings() const;

			template <typename T> T read(const std::string& key, const T& def) {
				type_settings::const_iterator i = m_settings.find(key);
				if (i == m_settings.end()) {
					write(key, def);
					return def;
				}

				return boost::lexical_cast<T>(i->second);
			}

			template <typename T> void write(const std::string& key, const T& value) {
				m_settings[key] = boost::lexical_cast<std::string>(value);
			}

		private:

			typedef std::map<std::string, std::string> type_settings;
			type_settings m_settings;

	};

}//FIFE

#endif

