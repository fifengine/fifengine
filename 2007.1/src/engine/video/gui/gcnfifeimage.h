#ifndef FIFE_GCN_IMAGE_H
#define FIFE_GCN_IMAGE_H

#if GUICHAN_VERSION == 4
#error "This code is for Guichan 0.5 and above - if this is, please -DGUICHAN_VERSION=X"
#endif

#include "guichan/color.hpp"
#include "guichan/image.hpp"

namespace FIFE {
  /** Derived gcn::Image to adapt guichan 0.5 to our engine.
   *
   * This class basically exists to restore the _getData() function,
   * which was removed from guichan.
   */
  class GCNImage : public gcn::Image {
    public:
      GCNImage();
      GCNImage(const size_t & id);
      //GCNImage(const std::string & filename);
      void free();
      int getWidth() const;
      int getHeight() const;
      gcn::Color getPixel(int x, int y);
      void putPixel(int x, int y, const gcn::Color& color);
      void convertToDisplayFormat();
      inline size_t _getData() const { return cacheId; }
    protected:
      size_t cacheId;
  };
}

#endif
