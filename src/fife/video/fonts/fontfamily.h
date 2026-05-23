// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "fontface.h"
#include "platform.h"
#include "fonttypes.h"

namespace FIFE
{

    class FIFE_API FontFamily
    {
        public:
            explicit FontFamily(std::string id);

            std::string const & getId() const
            {
                return m_id;
            }

            void addFace(std::shared_ptr<FontFace> face, FontWeight weight, bool italic = false);
            void addFallback(std::string familyId);

            FontFace* selectFace(FontWeight weight, bool italic, uint32_t codepoint) const;
            std::vector<std::string> const & getFallbacks() const
            {
                return m_fallbackIds;
            }

            bool isEmpty() const
            {
                return m_faces.empty();
            }

        private:
            struct FaceEntry
            {
                    std::shared_ptr<FontFace> face;
                    FontWeight weight;
                    bool italic;
            };

            std::string m_id;
            std::vector<FaceEntry> m_faces;
            std::vector<std::string> m_fallbackIds;
    };

} // namespace FIFE
