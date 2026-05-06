// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/floatingtextrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class IFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		virtual ~FloatingTextRenderer();
		void setFont(IFont* font);
		void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		void setBackground(uint8_t br, uint8_t bg, uint8_t bb, uint8_t ba = 255);
		void setBorder(uint8_t bbr, uint8_t bbg, uint8_t bbb, uint8_t bba = 255);
		void resetBackground();
		void resetBorder();

		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

	private:
		FloatingTextRenderer(RenderBackend* renderbackend, int32_t position, IFont* font);
	};


}