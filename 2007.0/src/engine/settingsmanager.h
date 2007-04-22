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

#ifndef FIFE_SETTINGSMANAGER_H
#define FIFE_SETTINGSMANAGER_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"
#include "singleton.h"

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
				try {
					return boost::lexical_cast<T>(i->second);
				} catch( boost::bad_lexical_cast& ) {
					return def;
				}
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

