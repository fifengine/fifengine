// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdio>
#include <string>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "stringutils.h"

namespace FIFE
{
    int32_t makeInt32(const std::string& str)
    {
        int32_t ret;
        sscanf(str.c_str(), "%d", &ret);
        return ret;
    }

    IntVector tokenize(const std::string& str, char delim, char group)
    {
        IntVector tokens;
        if (str.empty()) {
            return tokens;
        }

        int curr  = 0;
        int start = 0;

        start = curr = static_cast<int>(str.find_first_not_of(delim));

        while (str[curr]) {
            if (str[curr] == group) {
                curr = static_cast<int>(str.find_first_of(group, curr + 1));
                if ((size_t)curr == std::string::npos) {
                    return IntVector();
                }

                std::string token = str.substr(start + 1, curr - start - 1);
                tokens.push_back(makeInt32(token));
                start = curr + 1;
            } else if (str[curr] == delim) {
                if (str[curr - 1] != delim && str[curr - 1] != group) {
                    std::string token = str.substr(start, curr - start);
                    tokens.push_back(makeInt32(token));
                }
                start = curr + 1;
            }
            ++curr;
        }

        if (tokens.size() == 0) {
            tokens.push_back(makeInt32(str));
            return tokens;
        }

        if (str[curr - 1] != delim && str[curr - 1] != group) {
            std::string token = str.substr(start, curr - 1);
            tokens.push_back(makeInt32(token));
        }

        return tokens;
    }
} // namespace FIFE
