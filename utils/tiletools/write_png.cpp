#include <png.h>
#include <stdio.h>
#include <SDL.h>

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
