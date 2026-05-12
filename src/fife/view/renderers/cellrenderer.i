// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/cellrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	class IFont;

	class CellRenderer: public RendererBase {
	public:
		virtual ~CellRenderer();
		std::string getName();
		void setBlockerColor(uint8_t r, uint8_t g, uint8_t b);
		void setPathColor(uint8_t r, uint8_t g, uint8_t b);
		void setEnabledBlocking(bool enabled);
		bool isEnabledBlocking();
		void setEnabledPathVisual(bool enabled);
		bool isEnabledPathVisual();
		void addPathVisual(Instance* instance);
		void removePathVisual(Instance* instance);
		void setEnabledCost(const std::string& costId, bool enabled);
		bool isEnabledCost(const std::string& costId);
		void setFont(IFont* font);
		IFont* getFont();
		
		static CellRenderer* getInstance(IRendererContainer* cnt);
		
	private:
		CellRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
