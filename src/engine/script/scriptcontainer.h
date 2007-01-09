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

#ifndef FIFE_SCRIPTCONTAINER_H
#define FIFE_SCRIPTCONTAINER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes

namespace FIFE {

	class ScriptContainer {
		public:
			typedef enum {
				Undefined = 0,
				ScriptString = 1,
				ScriptFile = 2
			} Type;

			ScriptContainer(Type ct = Undefined, const std::string& val = "");

			Type type;
			std::string value;

			static ScriptContainer fromFile(const std::string& filename);
			static ScriptContainer fromString(const std::string& string);

			bool isValid() const;

			bool operator<(const ScriptContainer& b) const;
	};
		
}

#endif // FIFE_SCRIPTCONTAINER_H
