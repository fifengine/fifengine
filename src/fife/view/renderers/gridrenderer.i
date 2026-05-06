// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/gridrenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	class GridRenderer: public RendererBase {
	public:
		virtual ~GridRenderer();
		std::string getName();
		void setColor(Uint8 r, Uint8 g, Uint8 b);

		static GridRenderer* getInstance(IRendererContainer* cnt);

	private:
		GridRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
