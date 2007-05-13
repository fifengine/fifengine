#include "gcnimageloader.h"
#include "video/imagecache.h"

namespace FIFE {
  gcn::Image* GCNImageLoader::load(const std::string& filename, bool convertToDisplayFormat) {
    size_t id = ImageCache::instance()->addImageFromFile(filename);
    return new GCNImage(id);
  }
}
