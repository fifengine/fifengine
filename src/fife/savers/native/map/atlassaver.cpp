// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "atlassaver.h"

// Standard C++ library includes
#include <cassert>
#include <string>

// FIFE includes
#include "util/xml/xmlhelper.h"
#include "video/image.h"

namespace FIFE
{
    AtlasSaver::AtlasSaver() : m_atlas(nullptr)
    {
    }

    AtlasSaver::~AtlasSaver() = default;

    void AtlasSaver::setAtlas(Atlas* atlas)
    {
        m_atlas = atlas;
    }

    void AtlasSaver::save(std::string const & filename)
    {
        assert("Atlas must be set before saving" && m_atlas != nullptr);

        XML::Document doc;
        auto* declaration = doc.NewDeclaration(R"(xml version="1.0" encoding="ascii")");
        doc.InsertEndChild(declaration);

        XML::Element* atlasElement = doc.NewElement("atlas");
        atlasElement->SetAttribute("id", m_atlas->getName().c_str());

        ImagePtr const packedImage = m_atlas->getPackedImage();
        if (packedImage) {
            atlasElement->SetAttribute("source", packedImage->getName().c_str());
        }

        size_t const imageCount = m_atlas->getImageCount();
        for (size_t i = 0; i < imageCount; ++i) {
            ImagePtr const subImage = m_atlas->getImage(static_cast<uint32_t>(i));
            if (!subImage) {
                continue;
            }

            XML::Element* imageElement = doc.NewElement("image");
            imageElement->SetAttribute("name", subImage->getName().c_str());

            Rect const & rect = subImage->getSubImageRect();
            imageElement->SetAttribute("x", rect.x);
            imageElement->SetAttribute("y", rect.y);
            imageElement->SetAttribute("w", static_cast<int>(rect.w));
            imageElement->SetAttribute("h", static_cast<int>(rect.h));

            atlasElement->InsertEndChild(imageElement);
        }

        doc.InsertEndChild(atlasElement);
        doc.SaveFile(filename.c_str());
    }
} // namespace FIFE
