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
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend) = 0;
		virtual ~GenericRendererElementInfo() {};
	};

	class GenericRendererLineInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n1;
		GenericRendererNode n2;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererLineInfo(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererLineInfo() {};
	};
	class GenericRendererPointInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererPointInfo(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererPointInfo() {};
	};
	class GenericRendererQuadInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n1;
		GenericRendererNode n2;
		GenericRendererNode n3;
		GenericRendererNode n4;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererQuadInfo(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b);
		virtual ~GenericRendererQuadInfo() {};
	};
	class GenericRendererImageInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n;
		int image;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererImageInfo(GenericRendererNode n, int image);
		virtual ~GenericRendererImageInfo() {};
	};
	class GenericRendererAnimationInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n;
		int animation;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererAnimationInfo(GenericRendererNode n, int animation);
		virtual ~GenericRendererAnimationInfo() {};
	};
	class GenericRendererTextInfo : public GenericRendererElementInfo {
	public:
		GenericRendererNode n;
		AbstractFont* font;
		std::string text;
		virtual void render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, RenderBackend* renderbackend);
		GenericRendererTextInfo(GenericRendererNode n, AbstractFont* font, std::string text);
		virtual ~GenericRendererTextInfo() {};
	};
	class GenericRenderer: public RendererBase {
	public:
		GenericRenderer(RenderBackend* renderbackend, int position);
		~GeometricRenderer();
		std::string getName();
		static GenericRenderer* getInstance(IRendererContainer* cnt);
		void addLine(GenericRendererNode n1, GenericRendererNode n2, uint8_t r, uint8_t g, uint8_t b);
		void addPoint(GenericRendererNode n, uint8_t r, uint8_t g, uint8_t b);
		void addQuad(GenericRendererNode n1, GenericRendererNode n2, GenericRendererNode n3, GenericRendererNode n4, uint8_t r, uint8_t g, uint8_t b);
		void addText(GenericRendererNode n, AbstractFont* font, std::string text);
		void addImage(GenericRendererNode n, int image);
		void addAnimation(GenericRendererNode n, int animation);
		void removeAll();
	};
}
