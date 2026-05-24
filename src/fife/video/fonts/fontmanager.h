// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "assetresolver.h"
#include "fontdefinitionloader.h"
#include "fontface.h"
#include "fontfacecache.h"
#include "fontfamily.h"
#include "fontinstance.h"
#include "fonttypes.h"

namespace FIFE
{

    class FontManager
    {
        public:
            explicit FontManager(std::unique_ptr<AssetResolver> resolver);
            ~FontManager() = default;

            FontManager(FontManager const &)            = delete;
            FontManager& operator=(FontManager const &) = delete;

            void loadManifest(std::string const & path, bool isVfs = true);
            void loadManifestFromString(std::string const & xml, std::string const & source = "string");

            bool hasFamily(std::string const & id) const;
            FontFamily& getFamily(std::string const & id);

            FontHandle getFontHandle(FontInstanceKey const & key);
            std::shared_ptr<FontInstance> getFontInstance(FontHandle handle);
            std::optional<FontDefinition> getFontDefinition(FontHandle handle) const;

            FontHandle getFontHandle(
                std::string const & familyId,
                int size,
                FontWeight weight = FontWeight::Regular,
                bool italic       = false,
                bool antialias    = true,
                int hinting       = 0,
                int glyphSpacing  = 0,
                int rowSpacing    = 0);

            void setFallbackFont(std::string const & id);

            void clearInstanceCache();
            size_t getInstanceCacheSize() const;
            size_t getFaceCacheSize() const;

        private:
            FontHandle nextHandle();
            void processDefinitions(std::vector<FontDefinition> const & definitions);
            FontHandle getFontHandle(FontInstanceKey const & key, std::shared_ptr<FontFace> const & face);

            std::unique_ptr<AssetResolver> m_resolver;
            std::unique_ptr<FontDefinitionLoader> m_definitionLoader;

            FontFaceCache m_faceCache;
            std::unordered_map<AssetHandle, FontDefinition> m_faceDefinitions;
            std::unordered_map<FontInstanceKey, FontHandle> m_instanceKeyToHandle;
            std::unordered_map<FontHandle, std::shared_ptr<FontInstance>> m_instanceCache;
            std::unordered_map<std::string, std::unique_ptr<FontFamily>> m_families;
            std::unordered_set<FontHandle> m_allocatedHandles;

            FontHandle m_nextHandle{1};
            std::optional<std::string> m_fallbackId;

            friend class FontManagerTestHelper;
    };

} // namespace FIFE
