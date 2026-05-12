// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "stringutils.h"

// Standard C++ library includes
#include <cstdlib>
#include <string>
#include <utility>

// FIFE includes
namespace FIFE
{
    int32_t makeInt32(std::string const & str)
    {
        return static_cast<int32_t>(std::strtol(str.c_str(), nullptr, 10));
    }

    IntVector tokenize(std::string const & str, char delim, char group)
    {
        IntVector tokens;
        if (str.empty()) {
            return tokens;
        }

        std::string::size_type curr = str.find_first_not_of(delim);
        if (curr == std::string::npos) {
            return tokens;
        }

        std::string::size_type start = curr;

        while (curr < str.size()) {
            if (str[curr] == group) {
                curr = str.find_first_of(group, curr + 1);
                if (curr == std::string::npos) {
                    return {};
                }

                std::string const token = str.substr(start + 1, curr - start - 1);
                tokens.push_back(makeInt32(token));
                start = curr + 1;
            } else if (str[curr] == delim) {
                if (str[curr - 1] != delim && str[curr - 1] != group) {
                    std::string const token = str.substr(start, curr - start);
                    tokens.push_back(makeInt32(token));
                }
                start = curr + 1;
            }
            ++curr;
        }

        if (tokens.empty()) {
            tokens.push_back(makeInt32(str));
            return tokens;
        }

        if (str[curr - 1] != delim && str[curr - 1] != group) {
            std::string const token = str.substr(start, curr - start);
            tokens.push_back(makeInt32(token));
        }

        return tokens;
    }
} // namespace FIFE
