%module fife
%{
#include "view/renderers/genericrenderer.h"
%}

namespace FIFE {
	class RenderBackend;
	
	class GenericRendererNode {
	public:
		GenericRendererNode(Instance* attached_instance, Location* relative_location, const Point &relative_point = Point(0,0));
		GenericRendererNode(Instance* attached_instance, const Point &relative_point = Point(0,0));
		GenericRendererNode(Location* attached_location, const Point &relative_point = Point(0,0));
		GenericRendererNode(const Point &attached_point);
		~GenericRendererNode();
		
		void setAttached(Instance* attached_instance, Location* relative_location, const Point &relative_point);
		void setAttached(Instance* attached_instance, Location* relative_location);
		void setAttached(Instance* attached_instance, const Point &relative_point);
		void setAttached(Instance* attached_instance);
		void setAttached(Location* attached_location, const Point &relative_point);
		void setAttached(Location* attached_location);
		void setAttached(const Point &attached_point);
		
		void setRelative(Location* relative_location);
		void setRelative(Location* relative_location, Point relative_point);
		void setRelative(const Point &relative_point);
		
		Instance* getAttachedInstance();
		Location* getAttachedLocation();
		Point getAttachedPoint();
		
		Location* getOffsetLocation();
		Point getOffsetPoint();
		
		Instance* getInstance();
		Location* getLocation();
		Point getPoint();

		Point getCalculatedPoint(Camera* cam, Layer* layer, std::vector<Instance*>& instances);
	private:
		Instance* m_instance;
		Location* m_location;
		Point m_point;
	};
	class GenericRendererElementInfo {
	public:
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool) {};
		virtual ~GenericRendererElementInfo() {};
	};

	class GenericRendererLineInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererLineInfo(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererLineInfo() {};
	private:
		GenericRendererNode m_edge1;
		GenericRendererNode m_edge2;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
	};
	class GenericRendererPointInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererPointInfo(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererPointInfo() {};
	private:
		GenericRendererNode m_anchor;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
	};
	class GenericRendererQuadInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererQuadInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererQuadInfo() {};
	private:
		GenericRendererNode m_edge1;
		GenericRendererNode m_edge2;
		GenericRendererNode m_edge3;
		GenericRendererNode m_edge4;
		uint8_t m_red;
		uint8_t m_green;
		uint8_t m_blue;
	};
	class GenericRendererImageInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererImageInfo(GenericRendererNode n, int image);
		virtual ~GenericRendererImageInfo() {};
	private:
		GenericRendererNode m_anchor;
		int m_image;
	};
	class GenericRendererAnimationInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererAnimationInfo(GenericRendererNode n, int animation);
		virtual ~GenericRendererAnimationInfo() {};
	private:
		GenericRendererNode m_anchor;
		int m_animation;
	};
	class GenericRendererTextInfo : public GenericRendererElementInfo {
	public:
		void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);
		GenericRendererTextInfo(GenericRendererNode n, AbstractFont* font, std::string text);
		virtual ~GenericRendererTextInfo() {};
	private:
		GenericRendererNode m_anchor;
		AbstractFont* m_font;
		std::string m_text;
	};
	class GenericRenderer: public RendererBase {
	public:
		GenericRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool);
		~GeometricRenderer();
		std::string getName();
		static GenericRenderer* getInstance(IRendererContainer* cnt);
		void addLine(int group, GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b);
		void addPoint(int group, GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b);
		void addQuad(int group, GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b);
		void addText(int group, GenericRendererNode n, AbstractFont* font, std::string text);
		void addImage(int group, GenericRendererNode n, int image);
		void addAnimation(int group, GenericRendererNode n, int animation);
		void removeAll(int group);
	};
}
