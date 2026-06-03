// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <SDL3/SDL_pixels.h>

#include <cstdint>
#include <functional>
#include <string>

namespace FIFE
{

    enum class FontType
    {
        TRUETYPE,
        SUBIMAGE
    };

    enum class FontWeight
    {
        Thin     = 100,
        Light    = 300,
        Regular  = 400,
        Medium   = 500,
        SemiBold = 600,
        Bold     = 700,
        Black    = 900
    };

    struct FontDefinition
    {
            std::string id;
            FontType type = FontType::TRUETYPE;
            std::string source;
            std::string glyphs;
            int size         = 12;
            bool antialias   = true;
            bool bold        = false;
            bool italic      = false;
            bool underline   = false;
            bool recoloring  = false;
            int rowSpacing   = 0;
            int glyphSpacing = 0;
            SDL_Color color  = {255, 255, 255, 255};
    };

    struct AssetHandle
    {
            uint64_t id                                = 0;
            bool operator==(AssetHandle const &) const = default;
    };

#if !defined(NDEBUG)
    struct AssetDebugInfo
    {
            std::string uri;
            std::string resolvedPath;
            std::string providerName;
    };
#endif

    using FontHandle                         = uint32_t;
    constexpr FontHandle FONT_HANDLE_INVALID = 0;

    using FontId = std::string;

    struct AssetRequest
    {
            enum class Type
            {
                Font,
                ImageFont,
                Texture,
                Audio,
                Generic
            };
            Type type = Type::Font;
            std::string id;
            std::string source;
            std::string variant;
            int size = 0;
    };

    struct FontFaceKey
    {
            AssetHandle asset;
            int ptsize                                 = 0;
            bool operator==(FontFaceKey const &) const = default;
    };

    struct FontInstanceKey
    {
            AssetHandle asset;
            int size                                       = 0;
            bool bold                                      = false;
            bool italic                                    = false;
            bool antialias                                 = true;
            int hinting                                    = 0;
            bool operator==(FontInstanceKey const &) const = default;
    };

} // namespace FIFE

template <>
struct std::hash<FIFE::AssetHandle>
{
        size_t operator()(FIFE::AssetHandle const & h) const noexcept
        {
            return std::hash<uint64_t>{}(h.id);
        }
};

template <>
struct std::hash<FIFE::FontFaceKey>
{
        size_t operator()(FIFE::FontFaceKey const & k) const noexcept
        {
            size_t h = std::hash<uint64_t>{}(k.asset.id);
            h ^= std::hash<int>{}(k.ptsize) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
};

template <>
struct std::hash<FIFE::FontInstanceKey>
{
        size_t operator()(FIFE::FontInstanceKey const & k) const noexcept
        {
            size_t h = std::hash<uint64_t>{}(k.asset.id);
            h ^= std::hash<int>{}(k.size) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<bool>{}(k.bold) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<bool>{}(k.italic) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<bool>{}(k.antialias) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(k.hinting) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
};
