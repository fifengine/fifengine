// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/cellselectionrenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	class CellSelectionRenderer: public RendererBase {
	public:
		virtual ~CellSelectionRenderer();
		std::string getName();
		static CellSelectionRenderer* getInstance(IRendererContainer* cnt);
		void reset();
		void selectLocation(const Location* loc);
		void deselectLocation(const Location* loc);
		const std::vector<Location> getLocations() const;
		void setColor(uint8_t r, uint8_t g, uint8_t b);
		
	private:
		CellSelectionRenderer(RenderBackend* renderbackend, int32_t position);
	};

}
