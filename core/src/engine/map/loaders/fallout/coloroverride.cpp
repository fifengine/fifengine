/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes
#include <vector>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "xml/tinyxml/tinyxml.h"
#include "util/xmlutil.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "util/debugutils.h"
#include "util/exception.h"

#include "coloroverride.h"

namespace FIFE { namespace map { namespace loaders { namespace fallout {

	ColorOverride::ColorOverride()
	{
		m_alpha_set.reset();
		m_color_set.reset();
	}

	bool ColorOverride::areAllOverridesOpaque() const {
		return m_alpha_set.none();
	}
			
	bool ColorOverride::getOverride(uint8_t index, RGBA& override) const {
		bool result = false;
		if (m_alpha_set.test(index)) {
			override.a = m_colors[index].a;
			result = true;
		}
		if (m_color_set.test(index)) {
			const RGBA& rgba = m_colors[index];
			override.r = rgba.r;
			override.g = rgba.g;
			override.b = rgba.b;
			result = true;
		}
		return result;
	}
			
	void ColorOverride::addOverride(uint8_t index, const RGBA& override) {
		if (override.a == 255) {
			m_alpha_set.reset(index);
		} else {
			m_alpha_set.set(index);
		}
		m_color_set.set(index);
		m_colors[index] = override;
	}
	
	void ColorOverride::addOverride(uint8_t index, uint8_t alpha) {
		if (alpha == 255) {
			m_alpha_set.reset(index);
		} else {
			m_alpha_set.set(index);
		}
		m_colors[index].a = alpha;	
	}


	void collectNames(std::vector<std::string>& names, TiXmlElement* el) {
		// collect the names into the names vector
		
		// name attribute
		const char* name = el->Attribute("name");
		if (name) {
			names.push_back(name);	
		}
		
		// name child node
		TiXmlElement* name_element = el->FirstChildElement("name");
		while (name_element) {
			name = name_element->GetText();
			if (name) {
				names.push_back(name);
			}
			name_element = name_element->NextSiblingElement("name");
		}
		
	}
	
	// Gets an integer attribute that is 0-255
	bool boundAttribute(TiXmlElement* element, const char* name, uint8_t& result) {
		int value;
		if (element->QueryIntAttribute(name, &value) != TIXML_SUCCESS) {
			return false;	
		}
		
		if (value > 255) {
			value = 255;
		} else if (value  < 0) {
			value = 0;
		}
			
		result = static_cast<uint8_t>(value);
		return true;
	}

	void ColorOverride::load(const std::string& filename, OverrideMap& overrides) {
		TiXmlDocument doc(filename);
		doc.LoadFile(); // Throws CannotOpenFile

		TiXmlNode* node = doc.RootElement();
		TiXmlElement* el = node->ToElement();

		std::vector<std::string> names;

		TiXmlElement* overrideEl = el->FirstChildElement("override");
		while (overrideEl) {
			names.clear();
			collectNames(names, overrideEl);
			
			TiXmlElement* color_element = overrideEl->FirstChildElement("color");
			
			RGBA rgba;
			uint8_t index;
			while (color_element) {
				if (boundAttribute(color_element, "index", index)) {
					rgba.r = rgba.g = rgba.b = rgba.a = 255;
					
					bool found_rgb = false;
					found_rgb |= boundAttribute(color_element, "r", rgba.r);
					found_rgb |= boundAttribute(color_element, "g", rgba.g);
					found_rgb |= boundAttribute(color_element, "b", rgba.b);
					bool found_a = boundAttribute(color_element, "a", rgba.a);
					
					if (found_rgb | found_a) {
						// record overrided color for each of the names
						for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i) {
							ColorOverride& override = overrides[*i];
							if (found_rgb) {
								//std::cout << "Add color override " << (int)index << " to " << *i << ": r=" << (int)rgba.r << " g=" << (int)rgba.g << " b=" << (int)rgba.b << " a=" << (int)rgba.a << std::endl;
								override.addOverride(index, rgba);
							} else {
								// only override the alpha component
								//std::cout << "Add color override " << (int)index << " to " << *i << ": a=" << (int)rgba.a << std::endl;
								override.addOverride(index, rgba.a);
							}
						}
					}
				}
				color_element = color_element->NextSiblingElement("color");
			}
			
			overrideEl = overrideEl->NextSiblingElement("override");
		}
	}
	
	const ColorOverride ColorOverride::EMPTY_OVERRIDE;

} } } } //FIFE::map::loaders::fallout
