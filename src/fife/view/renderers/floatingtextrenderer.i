// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/floatingtextrenderer.h"
#include "view/renderers/speechstyle.h"
%}

%include "view/renderers/speechstyle.h"

namespace FIFE {
	class RenderBackend;
	class IFont;

	class FloatingTextRenderer: public RendererBase {
	public:
		virtual ~FloatingTextRenderer();
		void setFont(IFont* font);

		void setSpeechStyle(Instance* instance, const SpeechStyle& style);
		void clearSpeechStyle(Instance* instance);
		void clearAllStyles();
		void setDefaultSpeechStyle(const SpeechStyle& style);
		const SpeechStyle& getDefaultSpeechStyle() const;
		const SpeechStyle& getEffectiveStyle(Instance* instance) const;

		static FloatingTextRenderer* getInstance(IRendererContainer* cnt);

	private:
		FloatingTextRenderer(RenderBackend* renderbackend, int32_t position, IFont* font);
	};

}
