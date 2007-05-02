#include <iostream>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <png.h>
#include <getopt.h>

int write_png(const char *filename, SDL_Surface& surface) {
  FILE *file = fopen(filename, "wb");
  if (!file)
    return -1;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    return -1;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(file);
    return -1;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file);
    return -1;
  }

  png_init_io(png_ptr, file);

  png_set_IHDR(png_ptr, info_ptr, surface.w, surface.h, 8, PNG_COLOR_TYPE_RGB_ALPHA ,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  /* does this leak memory? */
  png_byte** row_pointers = (png_byte**)png_malloc(png_ptr, surface.h * sizeof(png_bytep));
  
  png_byte* p = (png_byte*)surface.pixels;
  for (int i = 0; i < surface.h; ++i) {
    row_pointers[i] = p;
    p += surface.pitch;
  }

  png_set_rows(png_ptr, info_ptr, row_pointers);

  png_write_png(png_ptr, info_ptr, 0, NULL);

  return 0;
}

int apply_mask(SDL_Surface *dst, SDL_Surface* src, SDL_Surface *mask, int dcol, int drow, int xoff, int yoff) {
  unsigned char* d = reinterpret_cast<unsigned char*>(dst->pixels);
  unsigned char* s = reinterpret_cast<unsigned char*>(src->pixels);
  unsigned char* m = reinterpret_cast<unsigned char*>(mask->pixels);
  int h = mask->h;
  int w = mask->w;
  int col = dcol;
  int row = drow;

  d += w*col*4;
  d += h*row*4*dst->w;

  s += src->w*3*yoff + xoff*3;
  
  for (int y=0; y < h; y++) {
    for (int x=0; x<w; x++) {
      *d = (*m) ? *s : 0; ++d; ++s;
      *d = (*m) ? *s : 0; ++d; ++s;
      *d = (*m) ? *s : 0; ++d; ++s;
      *d = (*m) ? 255 : 0; d++;
      ++m;
    }
    d += (dst->w-w)*4;
    s += (src->w-w)*3;
  }
  return 0;
}

int apply_mask_2(SDL_Surface *dst, SDL_Surface* src1, SDL_Surface* src2, SDL_Surface *mask,
    int dcol, int drow, int xoff, int yoff) {
  unsigned char* d = reinterpret_cast<unsigned char*>(dst->pixels);
  unsigned char* s1 = reinterpret_cast<unsigned char*>(src1->pixels);
  unsigned char* s2 = reinterpret_cast<unsigned char*>(src2->pixels);
  unsigned char* m = reinterpret_cast<unsigned char*>(mask->pixels);
  int h = mask->h;
  int w = mask->w;
  int col = dcol;
  int row = drow;

  d += w*col*4;
  d += h*row*4*dst->w;

  s1 += src1->w*3*yoff + xoff*3;
  s2 += src2->w*3*yoff + xoff*3;
  
  for (int y=0; y < h; y++) {
    for (int x=0; x<w; x++) {
      *d = (*m == 255) 
        ? *s1 
        : (*m == 128)
        ? *s2
        : 0; 
      ++d; ++s1;++s2;
      *d = (*m == 255)
        ? *s1
        : (*m == 128)
        ? *s2
        : 0;
      ++d; ++s1;++s2;
      *d = (*m == 255)
        ? *s1
        : (*m == 128)
        ? *s2
        : 0;
      ++d; ++s1;++s2;
      *d = (*m == 0) ? 0 : 255; d++;
      ++m;
    }
    d += (dst->w-w)*4;
    s1 += (src1->w-w)*3;
    s2 += (src2->w-w)*3;
  }
  return 0;
}

extern void usage(const char* name);

int main(int argc, char* argv[]) {
  std::string outfile;
  std::string infile;
  std::string secondin;
  std::string maskfile;
  int src_x = 0, src_y = 0;
  int w,h;
  w = 160;
  h = 72;
  int drow = 0, dcol = 0;
  bool _doCreate = false;
  SDL_Surface* result = NULL; // IMG_Load(outfile.c_str());
  SDL_Surface* mask = NULL;
  SDL_Surface*  src = NULL;
  SDL_Surface* src2 = NULL; 
  
  int option_index = 0;
  static struct option long_options[] = {
    {"create", 1, 0, 'c'},
    {"width",  1, 0, 'w'},
    {"height", 1, 0, 'h'},
    
    {"edit",   1, 0, 'e'},
    
    {"row",    1, 0, 'x'},
    {"col",    1, 0, 'y'},
    
    {"mask",   1, 0, 'm'},
    {"first",  1, 0, 'f'},
    {"second", 1, 0, 's'},
    
    {"xoff",   1, 0, 'X'},
    {"yoff",   1, 0, 'Y'},
    {"help",   0, 0, '?'},
    {0, 0, 0, 0}
  };
  while(1) {
    int c = getopt_long (argc, argv, "", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
      case 'c':
        _doCreate = true;
        outfile = std::string(optarg);
        break;
      case 'e':
        _doCreate = false;
        outfile = std::string(optarg);
        break;
      case 'w':
        w = atoi(optarg);
        break;
      case 'h':
        h = atoi(optarg);
        break;
      case 'y':
        drow = atoi(optarg);
        break;
      case 'x':
        dcol = atoi(optarg);
        break;
      case 'm':
        maskfile = std::string(optarg);
        break;
      case 'f':
        infile = std::string(optarg);
        break;
      case 's':
        secondin = std::string(optarg);
        break;
      case 'X':
        src_x = atoi(optarg);
        break;
      case 'Y':
        src_y = atoi(optarg);
        break;
      case '?':
        usage(argv[0]);
        return 0;
    }
  }
  
  if (outfile.size() == 0 || maskfile.size() == 0 || infile.size() == 0) {
    return 1;
  }
  if (_doCreate) {
  #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    result = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    result = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
  }
  else {
    result = IMG_Load(outfile.c_str());
  }

  SDL_LockSurface(result);
  mask = IMG_Load(maskfile.c_str());
  if (mask == NULL) {
    std::cerr << "Error: could not load mask: " << maskfile << std::endl;
    return 1;
  }
  src = IMG_Load(infile.c_str());
  if (src == NULL) {
    std::cerr << "Error: could not load input: " << infile << std::endl;
    return 1;
  }

  if (secondin.size() > 0) {
    src2 = IMG_Load(secondin.c_str());
    if (src2 == NULL) {
      std::cerr << "Error: could not load input: " << secondin << std::endl;
      return 1;
    }
  }

  if (secondin.size() == 0)
    apply_mask(result, src, mask, drow, dcol, src_x, src_y);
  else
    apply_mask_2(result, src, src2, mask, drow, dcol, src_x, src_y);

  SDL_UnlockSurface(result);
  write_png(const_cast<char*>(outfile.c_str()), *result);
  
  SDL_FreeSurface(result);
  SDL_FreeSurface(src);
  if (src2 != NULL)
    SDL_FreeSurface(src2);
  SDL_FreeSurface(mask);
  
  return 0;
}
