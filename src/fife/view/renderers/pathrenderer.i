// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/pathrenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	enum PathLineStyle {
		PLS_SOLID = 0,
		PLS_DASHED,
		PLS_ARROWED,
		PLS_GRADIENT,
		PLS_HEATMAP
	};

	struct PathStyleConfig {
		SDL_Color color;
		float width;
		PathLineStyle style;
		float dashLength;
		float dashGap;
		bool drawArrows;
		float arrowSize;
	};

	class PathRenderer: public RendererBase {
	public:
		virtual ~PathRenderer();
		std::string getName();

		void addPath(Instance* inst, const PathStyleConfig& config);
		void removePath(Instance* inst);
		void clearPaths();
		void setPathStyle(Instance* inst, const PathStyleConfig& config);

		static PathRenderer* getInstance(IRendererContainer* cnt);

	private:
		PathRenderer(RenderBackend* renderbackend, int32_t position);
	};
}
