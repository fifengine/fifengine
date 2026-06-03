// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "video/fonts/fontmanager.h"
#include "video/fonts/fontinstanceifontadapter.h"

#include <memory>
#include <vector>

namespace {

// Holds font adapters for the lifetime of the process.
// FontManager::getFont() returns a raw IFont* that renderers store by pointer.
// Since renderers do not take ownership, we keep the adapter alive here.
static std::vector<std::unique_ptr<FIFE::FontInstanceIFontAdapter>> s_fontCache;

} // anonymous namespace
%}

%nodefaultctor FIFE::FontManager;

namespace FIFE {
	class IFont;

	class FontManager {
	public:
		~FontManager();

		bool hasFamily(const std::string& id) const;
	};

	%extend FontManager {
		FIFE::IFont* getFont(const std::string& family, int size) {
			auto handle = $self->getFontHandle(family, size);
			auto instance = $self->getFontInstance(handle);
			auto adapter = std::make_unique<FIFE::FontInstanceIFontAdapter>(instance);
			auto ptr = adapter.get();
			s_fontCache.push_back(std::move(adapter));
			return ptr;
		}
	}
}
