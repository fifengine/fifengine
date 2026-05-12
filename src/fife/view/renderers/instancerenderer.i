// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/instancerenderer.h"
%}

namespace FIFE {
	class Location;
	class RenderBackend;

	class InstanceRenderer: public RendererBase {
	public:
		virtual ~InstanceRenderer();
		std::string getName();
		void addOutlined(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t width, int32_t threshold = 1);
		void removeOutlined(Instance* instance);
		void removeAllOutlines();
		void addColored(Instance* instance, int32_t r, int32_t g, int32_t b, int32_t a = 128);
		void removeColored(Instance* instance);
		void removeAllColored();
		void addTransparentArea(Instance* instance, const std::list<std::string> &groups, uint32_t w, uint32_t h, uint8_t trans, bool front = true);
		void removeTransparentArea(Instance* instance);
		void removeAllTransparentAreas();
		void addIgnoreLight(const std::list<std::string> &groups);
		void removeIgnoreLight(const std::list<std::string> &groups);
		void removeAllIgnoreLight();
		static InstanceRenderer* getInstance(IRendererContainer* cnt);
		void setRemoveInterval(uint32_t interval);
		uint32_t getRemoveInterval() const;
	private:
		InstanceRenderer(RenderBackend* renderbackend, int32_t position);
	};
}