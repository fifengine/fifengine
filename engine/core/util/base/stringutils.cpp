/***************************************************************************
*   Copyright (C) 2005-2017 by the FIFE team                              *
*   http://www.fifengine.net                                              *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or                 *
*   modify it under the terms of the GNU Lesser General Public            *
*   License as published by the Free Software Foundation; either          *
*   version 2.1 of the License, or (at your option) any later version.    *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
*   Lesser General Public License for more details.                       *
*                                                                         *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
***************************************************************************/

// Standard C++ library includes
#include <cstdio>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "stringutils.h"

namespace FIFE {
	int32_t makeInt32(const std::string& str) {
		int32_t ret;
		sscanf(str.c_str(), "%d", &ret);
		return ret;
	}

	IntVector tokenize(const std::string& str, char delim, char group) {
		IntVector tokens;
		if(str.empty()) {
			return tokens;
		}

		int curr = 0;
		int start = 0;

		start = curr = static_cast<int>(str.find_first_not_of(delim));

		while(str[curr]) {
			if(str[curr] == group) {
				curr = static_cast<int>(str.find_first_of(group, curr+1));
				if((size_t)curr == std::string::npos) {
					return IntVector();
				}

				std::string token = str.substr(start+1, curr-start-1);
				tokens.push_back(makeInt32(token));
				start = curr + 1;
			} else if(str[curr] == delim) {
				if(str[curr-1] != delim && str[curr-1] != group) {
					std::string token = str.substr(start, curr-start);
					tokens.push_back(makeInt32(token));
				}
				start = curr + 1;
			}
			++curr;
		}

		if(tokens.size() == 0) {
			tokens.push_back(makeInt32(str));
			return tokens;
		}

		if(str[curr-1] != delim && str[curr-1] != group) {
			std::string token = str.substr(start, curr - 1);
			tokens.push_back(makeInt32(token));
		}

		return tokens;
	}
}
