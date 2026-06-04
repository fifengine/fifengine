// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_FONTFAMILY_H
#define FIFE_VIDEO_FONTS_FONTFAMILY_H

#include <memory>
#include <string>
#include <vector>

#include "fontface.h"
#include "fonttypes.h"
#include "platform.h"

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

#endif
