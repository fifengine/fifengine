// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "png_writer.h"
#include <png.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

int save_png_from_memory(const char* path, const void* pixels, int width, int height, int pitch, int channels)
{
    if (!path || !pixels) return 0;
    if (width <= 0 || height <= 0) return 0;
    if (channels != 3 && channels != 4) return 0;

    FILE* fp = fopen(path, "wb");
    if (!fp) return 0;

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) { fclose(fp); return 0; }

    png_infop info = png_create_info_struct(png);
    if (!info) { png_destroy_write_struct(&png, NULL); fclose(fp); return 0; }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return 0;
    }

    png_init_io(png, fp);

    int color_type = (channels == 4) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
    png_set_IHDR(png, info, (png_uint_32)width, (png_uint_32)height, 8, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    // Build row pointers
    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (!row_pointers) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return 0;
    }

    const unsigned char* src = (const unsigned char*)pixels;
    for (int y = 0; y < height; ++y)
        row_pointers[y] = (png_bytep)(src + (size_t)y * pitch);

    png_write_image(png, row_pointers);
    png_write_end(png, info);

    free(row_pointers);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    return 1;
}
