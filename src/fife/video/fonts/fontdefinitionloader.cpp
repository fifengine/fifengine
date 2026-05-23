// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "fontdefinitionloader.h"

// Standard C++ library includes
#include <cassert>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/xml/xmlhelper.h"

namespace
{
    FIFE::Logger& _log = []() -> FIFE::Logger& {
        static FIFE::Logger log(LM_VIDEO);
        return log;
    }();
} // namespace

namespace FIFE
{

    std::vector<FontDefinition> FontDefinitionLoader::loadFromXml(std::string const & xmlContent)
    {
        if (xmlContent.empty()) {
            throw InvalidFormat("XML content must not be empty");
        }

        XML::Document doc;
        if (!XML::Parse(doc, xmlContent)) {
            throw InvalidFormat("Failed to parse font definition XML: malformed XML content");
        }

        XML::Element const * root = doc.RootElement();
        if (root == nullptr || !XML::HasName(root, "fonts")) {
            throw InvalidFormat("Root element must be <fonts>");
        }

        char const * version = XML::Attribute(root, "version");
        if (version == nullptr) {
            FL_WARN(_log, "<fonts> element is missing 'version' attribute");
        }

        std::vector<FontDefinition> result;
        std::set<std::string> seenIds;

        for (XML::Element const * familyElem = root->FirstChildElement("family"); familyElem != nullptr;
             familyElem                      = familyElem->NextSiblingElement("family")) {

            char const * familyId = XML::Attribute(familyElem, "id");
            if (familyId == nullptr) {
                continue;
            }
            std::string const familyIdStr(familyId);

            for (XML::Element const * faceElem = familyElem->FirstChildElement("face"); faceElem != nullptr;
                 faceElem                      = faceElem->NextSiblingElement("face")) {

                FontDefinition def;

                char const * source = faceElem->GetText();
                if (source == nullptr || *source == '\0') {
                    throw InvalidFormat("Font source must not be empty");
                }
                def.source = source;

                int weight = 400;
                XML::QueryAttribute(faceElem, "weight", &weight);

                char const * typeAttr = XML::Attribute(faceElem, "type");
                if (typeAttr != nullptr && std::string(typeAttr) == "bitmap") {
                    def.type = FontType::SUBIMAGE;
                } else {
                    def.type = FontType::TRUETYPE;
                }

                int size = 12;
                XML::QueryAttribute(faceElem, "size", &size);
                def.size = size;

                char const * antialiasAttr = XML::Attribute(faceElem, "antialias");
                if (antialiasAttr != nullptr) {
                    def.antialias = (std::string(antialiasAttr) == "1" || std::string(antialiasAttr) == "true");
                }

                int rowSpacing = 0;
                XML::QueryAttribute(faceElem, "row_spacing", &rowSpacing);
                def.rowSpacing = rowSpacing;

                int glyphSpacing = 0;
                XML::QueryAttribute(faceElem, "glyph_spacing", &glyphSpacing);
                def.glyphSpacing = glyphSpacing;

                char const * colorAttr = XML::Attribute(faceElem, "color");
                if (colorAttr != nullptr) {
                    std::string colorStr(colorAttr);
                    int r = 255, g = 255, b = 255, a = 255;
                    std::stringstream ss(colorStr);
                    char comma;
                    ss >> r >> comma >> g >> comma >> b;
                    if (ss >> comma >> a) { /* four values */
                    }
                    def.color = {
                        static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
                }

                char const * recoloringAttr = XML::Attribute(faceElem, "recoloring");
                if (recoloringAttr != nullptr) {
                    def.recoloring = (std::string(recoloringAttr) == "1" || std::string(recoloringAttr) == "true");
                }

                char const * glyphs = XML::Attribute(faceElem, "glyphs");
                if (glyphs != nullptr) {
                    def.glyphs = glyphs;
                }

                def.id = familyIdStr + "/" + std::to_string(weight);

                if (!seenIds.insert(def.id).second) {
                    throw Duplicate("Duplicate font id: " + def.id);
                }

                result.push_back(std::move(def));
            }
        }

        return result;
    }

    std::vector<FontDefinition> FontDefinitionLoader::loadFromFile(std::string const & path)
    {
        if (path.empty()) {
            throw InvalidFormat("Path must not be empty");
        }
        std::ifstream file(path);
        if (!file.is_open()) {
            throw InvalidFormat("Failed to open font definition file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromXml(buffer.str());
    }

    std::vector<FontDefinition> FontDefinitionLoader::loadFromString(std::string const & xml)
    {
        return loadFromXml(xml);
    }

} // namespace FIFE
