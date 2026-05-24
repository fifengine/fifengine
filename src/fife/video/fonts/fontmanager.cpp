// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontmanager.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <algorithm>

#include "imagefontface.h"
#include "truetypefontface.h"

namespace FIFE
{

    FontManager::FontManager(std::unique_ptr<AssetResolver> resolver) :
        m_resolver(std::move(resolver)), m_definitionLoader(std::make_unique<FontDefinitionLoader>())
    {
        if (!m_resolver) {
            throw std::invalid_argument("AssetResolver must not be null");
        }
    }

    void FontManager::loadManifest(std::string const & path, bool /*isVfs*/)
    {
        assert("Path must not be empty" && !path.empty());
        auto definitions = m_definitionLoader->loadFromFile(path);
        processDefinitions(definitions);
    }

    void FontManager::loadManifestFromString(std::string const & xml, std::string const & /*source*/)
    {
        assert("XML must not be empty" && !xml.empty());
        auto definitions = m_definitionLoader->loadFromString(xml);
        processDefinitions(definitions);
    }

    void FontManager::processDefinitions(std::vector<FontDefinition> const & definitions)
    {
        assert("At least one definition required" && !definitions.empty());
        std::unordered_map<std::string, std::vector<FontDefinition const *>> groups;
        for (auto const & def : definitions) {
            auto pos        = def.id.find('/');
            std::string const fid = (pos != std::string::npos) ? def.id.substr(0, pos) : def.id;
            groups[fid].push_back(&def);
        }

        for (auto const& [fid, faceDefs] : groups) {
            auto family = std::make_unique<FontFamily>(fid);

            for (auto const * def : faceDefs) {
                AssetRequest const request(AssetRequest::Type::Font, def->id, def->source);
                AssetHandle const handle = m_resolver->resolve(request);

                std::shared_ptr<FontFace> face;
                if (def->type == FontType::TRUETYPE) {
                    face = std::make_shared<TrueTypeFontFace>(handle, def->source, def->size);
                } else if (def->type == FontType::SUBIMAGE) {
                    face = std::make_shared<ImageFontFace>(handle, def->source, def->glyphs);
                } else {
                    continue;
                }

                m_faceCache.put(FontFaceKey{.asset = handle, .ptsize = face->getPointSize()}, face);
                m_faceDefinitions[handle] = *def;

                FontWeight const weight = def->bold ? FontWeight::Bold : FontWeight::Regular;
                family->addFace(face, weight, def->italic);
            }

            m_families[fid] = std::move(family);
        }
    }

    bool FontManager::hasFamily(std::string const & id) const
    {
        return m_families.contains(id);
    }

    FontFamily& FontManager::getFamily(std::string const & id)
    {
        auto it = m_families.find(id);
        if (it == m_families.end()) {
            throw std::out_of_range("Family not found: " + id);
        }
        return *it->second;
    }

    FontHandle FontManager::getFontHandle(FontInstanceKey const & key)
    {
        assert("Resolver must be valid" && m_resolver != nullptr);
        auto face = m_faceCache.get(FontFaceKey{.asset = key.asset, .ptsize = key.size});
        return getFontHandle(key, face);
    }

    FontHandle FontManager::getFontHandle(FontInstanceKey const & key, std::shared_ptr<FontFace> const & face)
    {
        assert("Resolver must be valid" && m_resolver != nullptr);
        auto it = m_instanceKeyToHandle.find(key);
        if (it != m_instanceKeyToHandle.end()) {
            return it->second;
        }

        std::shared_ptr<FontInstance> instance;
        if (auto ttfFace = std::dynamic_pointer_cast<TrueTypeFontFace>(face)) {
            instance = std::make_shared<TrueTypeFontInstance>(
                ttfFace, key.size, key.antialias, key.bold, key.italic, key.hinting);
        } else if (auto imgFace = std::dynamic_pointer_cast<ImageFontFace>(face)) {
            instance = std::make_shared<ImageFontInstance>(
                imgFace, key.size, key.antialias, key.bold, key.italic, key.hinting);
        } else {
            throw std::runtime_error("Unknown font face type");
        }

        FontHandle const handle    = nextHandle();
        m_instanceKeyToHandle[key] = handle;
        m_instanceCache[handle]    = instance;
        m_allocatedHandles.insert(handle);

        return handle;
    }

    std::shared_ptr<FontInstance> FontManager::getFontInstance(FontHandle handle)
    {
        auto it = m_instanceCache.find(handle);
        if (it == m_instanceCache.end()) {
            throw std::out_of_range("FontInstance handle not found: " + std::to_string(handle));
        }
        return it->second;
    }

    std::optional<FontDefinition> FontManager::getFontDefinition(FontHandle handle) const
    {
        auto instanceIt = m_instanceCache.find(handle);
        if (instanceIt == m_instanceCache.end() || instanceIt->second == nullptr) {
            return std::nullopt;
        }

        auto face = instanceIt->second->getFace();
        if (face == nullptr) {
            return std::nullopt;
        }

        auto definitionIt = m_faceDefinitions.find(face->getAssetHandle());
        if (definitionIt == m_faceDefinitions.end()) {
            return std::nullopt;
        }

        return definitionIt->second;
    }

    FontHandle FontManager::getFontHandle(
        std::string const & familyId,
        int size,
        FontWeight weight,
        bool italic,
        bool antialias,
        int hinting,
        int /*glyphSpacing*/,
        int /*rowSpacing*/)
    {
        if (!hasFamily(familyId)) {
            if (m_fallbackId && hasFamily(*m_fallbackId)) {
                return getFontHandle(*m_fallbackId, size, weight, italic, antialias, hinting, 0, 0);
            }
            throw std::runtime_error("Font family not found: " + familyId);
        }

        auto const & family   = getFamily(familyId);
        FontFace const * face = family.selectFace(weight, italic, 0x0041);
        if (face == nullptr) {
            throw std::runtime_error("No suitable face found in family: " + familyId);
        }

        int effectiveSize = (size > 0) ? size : face->getPointSize();
        effectiveSize     = std::max(effectiveSize, 1);

        AssetHandle const handle = face->getAssetHandle();
        FontFaceKey const cacheKey{.asset = handle, .ptsize = effectiveSize};

        std::shared_ptr<FontFace> renderFace;
        if (effectiveSize == face->getPointSize()) {
            renderFace = m_faceCache.get(FontFaceKey{.asset = handle, .ptsize = face->getPointSize()});
        } else if (m_faceCache.has(cacheKey)) {
            renderFace = m_faceCache.get(cacheKey);
        } else {
            auto defIt = m_faceDefinitions.find(handle);
            if (defIt == m_faceDefinitions.end()) {
                throw std::runtime_error("Font definition not found for handle in family: " + familyId);
            }
            if (defIt->second.type != FontType::TRUETYPE) {
                throw std::runtime_error("Cannot render non-TrueType font at different size: " + familyId);
            }
            renderFace = std::make_shared<TrueTypeFontFace>(handle, defIt->second.source, effectiveSize);
            m_faceCache.put(cacheKey, renderFace);
        }

        FontInstanceKey const key{.asset = handle, .size = effectiveSize, .bold = weight == FontWeight::Bold, .italic = italic, .antialias = antialias, .hinting = hinting};

        return getFontHandle(key, renderFace);
    }

    void FontManager::setFallbackFont(std::string const & id)
    {
        m_fallbackId = id;
    }

    void FontManager::clearInstanceCache()
    {
        m_instanceCache.clear();
        m_instanceKeyToHandle.clear();
        m_allocatedHandles.clear();
        m_nextHandle = 1;
    }

    size_t FontManager::getInstanceCacheSize() const
    {
        return m_instanceCache.size();
    }

    size_t FontManager::getFaceCacheSize() const
    {
        return m_faceCache.size();
    }

    FontHandle FontManager::nextHandle()
    {
        while (m_allocatedHandles.contains(m_nextHandle)) {
            ++m_nextHandle;
        }
        FontHandle const handle = m_nextHandle++;
        m_allocatedHandles.insert(handle);
        return handle;
    }

} // namespace FIFE
