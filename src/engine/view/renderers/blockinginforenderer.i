// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/blockinginforenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	class BlockingInfoRenderer: public RendererBase {
	public:
		virtual ~BlockingInfoRenderer();
		std::string getName();
		void setColor(uint8_t r, uint8_t g, uint8_t b);
		
		static BlockingInfoRenderer* getInstance(IRendererContainer* cnt);
		
	private:
		BlockingInfoRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
