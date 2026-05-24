// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontfamily.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>
#include <utility>

namespace FIFE
{

    FontFamily::FontFamily(std::string id) : m_id(std::move(id))
    {
        if (m_id.empty()) {
            throw std::invalid_argument("Family ID must not be empty");
        }
    }

    void FontFamily::addFace(std::shared_ptr<FontFace> face, FontWeight weight, bool italic)
    {
        if (!face) {
            throw std::invalid_argument("Face must not be null");
        }
        m_faces.push_back({.face = std::move(face), .weight = weight, .italic = italic});
    }

    void FontFamily::addFallback(std::string familyId)
    {
        if (familyId.empty()) {
            throw std::invalid_argument("Fallback family ID must not be empty");
        }
        m_fallbackIds.push_back(std::move(familyId));
    }

    FontFace* FontFamily::selectFace(FontWeight weight, bool italic, uint32_t codepoint) const
    {
        assert("Family must have at least one face registered before selection" && !m_faces.empty());

        auto it = std::ranges::find_if(m_faces, [&](auto const & e) {
            return e.weight == weight && e.italic == italic && e.face->supports(codepoint);
        });
        if (it != m_faces.end()) {
            return it->face.get();
        }

        it = std::ranges::find_if(m_faces, [&](auto const & e) {
            return e.weight == weight && e.face->supports(codepoint);
        });
        if (it != m_faces.end()) {
            return it->face.get();
        }

        it = std::ranges::find_if(m_faces, [&](auto const & e) {
            return e.face->supports(codepoint);
        });
        if (it != m_faces.end()) {
            return it->face.get();
        }

        if (!m_faces.empty()) {
            return m_faces.front().face.get();
        }

        return nullptr;
    }

} // namespace FIFE
