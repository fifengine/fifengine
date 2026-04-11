// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_XML_HELPER_H
#define FIFE_XML_HELPER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstring>
#include <string>

// 3rd party library includes
#include <tinyxml2.h>

namespace FIFE::XML
{
    using Document = tinyxml2::XMLDocument;
    using Element  = tinyxml2::XMLElement;
    using Error    = tinyxml2::XMLError;

    constexpr Error SUCCESS = tinyxml2::XML_SUCCESS;

    inline bool Parse(Document& document, const std::string& xml)
    {
        return document.Parse(xml.c_str(), xml.size()) == SUCCESS;
    }

    inline const char* Attribute(const Element* element, const char* name)
    {
        return element != nullptr ? element->Attribute(name) : nullptr;
    }

    inline bool HasName(const Element* element, const char* expectedName)
    {
        return element != nullptr && std::strcmp(element->Name(), expectedName) == 0;
    }

    inline Error QueryAttribute(const Element* element, const char* name, int* value)
    {
        return element != nullptr ? element->QueryIntAttribute(name, value) : tinyxml2::XML_NO_ATTRIBUTE;
    }

    inline Error QueryAttribute(const Element* element, const char* name, double* value)
    {
        return element != nullptr ? element->QueryDoubleAttribute(name, value) : tinyxml2::XML_NO_ATTRIBUTE;
    }

    inline Error QueryAttribute(const Element* element, const char* name, float* value)
    {
        return element != nullptr ? element->QueryFloatAttribute(name, value) : tinyxml2::XML_NO_ATTRIBUTE;
    }
} // namespace FIFE::XML

#endif
