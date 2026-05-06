// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/rendererbase.h"
%}

namespace FIFE {
	class Camera;
	class Layer;
	class Instance;
	class Map;
	
	class RendererBase {
	public:
		RendererBase();
		virtual ~RendererBase();
		virtual std::string getName() = 0;
		int32_t getPipelinePosition() { return m_position; }
		void setPipelinePosition(int32_t position);
		void setEnabled(bool enabled);
		bool isEnabled();
		void addActiveLayer(Layer* layer);
		void removeActiveLayer(Layer* layer);
		void clearActiveLayers();
		void activateAllLayers(Map* map);
		std::list<Layer*> getActiveLayers() const {return m_active_layers;}
	};
	
	class IRendererContainer {
	public:
		virtual ~IRendererContainer() {}
		virtual RendererBase* getRenderer(const std::string& renderername) = 0;
	};
	
}