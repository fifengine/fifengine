// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/coordinaterenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class IFont;

	class CoordinateRenderer: public RendererBase {
	public:
		virtual ~CoordinateRenderer();
		std::string getName();
		void setFont(IFont* font);
		void setColor(uint8_t r, uint8_t g, uint8_t b);
		void setZoom(bool enable);
		
		static CoordinateRenderer* getInstance(IRendererContainer* cnt);
				
	private:
		CoordinateRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
