#ifdef _WIN32
#define ssize_t int
#include <io.h>
#include <sys/types.h>
#include <Winsock2.h>
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <stdio.h>

#include <png.h>

#include "frm.h"
#include "out_png.h"



void png_my_error(png_structp a, png_const_charp b) {
	printf("png_my_error: %s\n", b);
}
void png_my_warning(png_structp a, png_const_charp b) {
	printf("png_my_warn: %s\n", b);
}

void writePNGFile(frm_frame *frame, frm_color *pal, FILE *file) {
	if (!file) {
		printf("ERR: write open failed\n");
		return;
	}
		
	png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, png_my_error, png_my_warning);
    if (!png_ptr)
       return;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return;
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		printf("Error: return setjmp\n");
		return;
	}
	
	png_init_io(png_ptr, file);
	png_set_IHDR(png_ptr, info_ptr, frame->width, frame->height, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_set_PLTE(png_ptr, info_ptr, (png_color*)pal, 256);
	png_set_rows(png_ptr, info_ptr, frame->rowptr);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
}
