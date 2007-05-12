#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "gcnimageloader_v05.h"
#include "video/imagecache.h"

namespace FIFE {
  gcn::Image* GCNImageLoader::load(const std::string& filename, bool convertToDisplayFormat) {
    size_t id = ImageCache::instance()->addImageFromFile(filename);
    return new GCNImage(id);
  }
}

#endif
