/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

%module fife
%{
#include "view/renderers/lightrenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	class LightRendererElementInfo {
	public:
		virtual std::string getName() { return 0; };
		virtual RendererNode* getNode() { return NULL; };
		virtual int32_t getId() { return -1; };
		virtual int32_t getSrcBlend() { return -1; };
		virtual int32_t getDstBlend() { return -1; };
		virtual void setStencil(uint8_t stencil_ref, float alpha_ref) {};
		virtual int32_t getStencil() { return 0; };
		virtual float getAlpha() { return 0; };
		virtual void removeStencil() {};
		virtual std::vector<uint8_t> getColor() {};
		virtual float getRadius() { return 0; };
		virtual int32_t getSubdivisions() { return 0; };
		virtual float getXStretch() { return 0; };
		virtual float getYStretch() { return 0; };
		virtual ~LightRendererElementInfo() {};
	};

	class LightRendererImageInfo : public LightRendererElementInfo {
	public:
		LightRendererImageInfo(RendererNode n, ImagePtr image, int32_t src, int32_t dst);
		virtual ~LightRendererImageInfo() {};
		
		ImagePtr getImage();
	};
	class LightRendererAnimationInfo : public LightRendererElementInfo {
	public:
		LightRendererAnimationInfo(RendererNode n, AnimationPtr animation, int32_t src, int32_t dst);
		virtual ~LightRendererAnimationInfo() {};
		
		AnimationPtr getAnimation();
	};
	class LightRendererSimpleLightInfo : public LightRendererElementInfo {
	public:
		LightRendererSimpleLightInfo(RendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src, int32_t dst);
		virtual ~LightRendererSimpleLightInfo() {};
	};
	class LightRendererResizeInfo : public LightRendererElementInfo {
	public:
		LightRendererResizeInfo(RendererNode n, ImagePtr image, int32_t width, int32_t height, int32_t src, int32_t dst);
		virtual ~LightRendererResizeInfo() {};
		
		ImagePtr getImage();
	};
	class LightRenderer: public RendererBase {
	public:
		LightRenderer(RenderBackend* renderbackend, int32_t position);
		~LightRenderer();
		std::string getName();
		static LightRenderer* getInstance(IRendererContainer* cnt);
		void addImage(const std::string &group, RendererNode n, ImagePtr image, int32_t src=-1, int32_t dst=-1);
		void addAnimation(const std::string &group, RendererNode n, AnimationPtr animation, int32_t src=-1, int32_t dst=-1);
		void addSimpleLight(const std::string &group, RendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src=-1, int32_t dst=-1);
		void resizeImage(const std::string &group, RendererNode n, ImagePtr image, int32_t width, int32_t height, int32_t src=-1, int32_t dst=-1);
		void addStencilTest(const std::string &group, uint8_t stencil_ref=0, float alpha_ref=0.0);
		void removeStencilTest(const std::string &group);
		std::list<std::string> getGroups();
		std::vector<LightRendererElementInfo*> getLightInfo(const std::string &group);
		void removeAll(const std::string &group);
		void removeAll();
	};
}

namespace std {
	%template(LightRendererElementInfoVector) vector<FIFE::LightRendererElementInfo*>;	
}
