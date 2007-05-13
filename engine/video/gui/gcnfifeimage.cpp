#include "video/gui/gcnfifeimage.h"
#include "video/imagecache.h"
#include "util/debugutils.h"

namespace FIFE {
  GCNImage::GCNImage() : gcn::Image(), 
  cacheId(ImageCache::UNDEFINED_ID) {}

  GCNImage::GCNImage(const size_t & id) : gcn::Image(), cacheId(id) {} 

// disabled this constructor because it is extremly stupid to do it that way
/*
  GCNImage::GCNImage(const std::string & filename) : gcn::Image(), 
    cacheId(ImageCache::UNDEFINED_ID) {
    GCNImage * img = static_cast<GCNImage*>(gcn::Image::load(filename, false));
    cacheId = img->cacheId;
    delete img;
  }
*/

  void GCNImage::free() {
    // the imagecache should do this; should we tell it?
  }

  int GCNImage::getWidth() const {
    RenderAble* img = ImageCache::instance()->getImage(cacheId);
    assert(img);
    return img->getWidth();
  }

  int GCNImage::getHeight() const {
    RenderAble* img = ImageCache::instance()->getImage(cacheId);
    assert(img);
    return img->getHeight();
  }

  gcn::Color GCNImage::getPixel(int x, int y) {
    PANIC_PRINT("not implemented");
    return gcn::Color();
  }

  void GCNImage::putPixel(int x, int y, const gcn::Color& color) {
    PANIC_PRINT("not implemented");
  }

  void GCNImage::convertToDisplayFormat() {
    // empty on purpose
    // good idea?
  }

}
