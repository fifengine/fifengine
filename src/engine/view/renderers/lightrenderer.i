// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "view/renderers/lightrenderer.h"
%}

namespace FIFE {
	class RenderBackend;

	class LightRendererElementInfo {
	public:
		LightRendererElementInfo(RendererNode n, int32_t src, int32_t dst);
		virtual ~LightRendererElementInfo();
		virtual std::string getName() = 0;
		RendererNode* getNode();
		int32_t getSrcBlend();
		int32_t getDstBlend();
		void setStencil(uint8_t stencil_ref);
		int32_t getStencil();
		void removeStencil();
		virtual std::vector<uint8_t> getColor();
		virtual float getRadius();
		virtual int32_t getSubdivisions();
		virtual float getXStretch();
		virtual float getYStretch();
	};

	class LightRendererImageInfo : public LightRendererElementInfo {
	public:
		LightRendererImageInfo(RendererNode n, ImagePtr image, int32_t src, int32_t dst);
		virtual ~LightRendererImageInfo();

		ImagePtr getImage();
	};

	class LightRendererAnimationInfo : public LightRendererElementInfo {
	public:
		LightRendererAnimationInfo(RendererNode n, AnimationPtr animation, int32_t src, int32_t dst);
		virtual ~LightRendererAnimationInfo();

		AnimationPtr getAnimation();
	};

	class LightRendererSimpleLightInfo : public LightRendererElementInfo {
	public:
		LightRendererSimpleLightInfo(RendererNode n, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t r, uint8_t g, uint8_t b, int32_t src, int32_t dst);
		virtual ~LightRendererSimpleLightInfo();
		std::vector<uint8_t> getColor();
		float getRadius();
		int32_t getSubdivisions();
		float getXStretch();
		float getYStretch();
	};

	class LightRendererResizeInfo : public LightRendererElementInfo {
	public:
		LightRendererResizeInfo(RendererNode n, ImagePtr image, int32_t width, int32_t height, int32_t src, int32_t dst);
		virtual ~LightRendererResizeInfo();

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
		void addStencilTest(const std::string &group, uint8_t stencil_ref=0);
		void removeStencilTest(const std::string &group);
		std::list<std::string> getGroups();
		std::vector<LightRendererElementInfo*> getLightInfo(const std::string &group);
		void removeAll(const std::string &group);
		void removeAll();
	};
}

%template(LightRendererElementInfoVector) std::vector<FIFE::LightRendererElementInfo*>;