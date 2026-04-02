// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "png_loader.h"
#include <png.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

int load_png_from_file(const char* path, png_image_result* out)
{
    if (!path || !out) return 0;

    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) { fclose(fp); return 0; }

    png_infop info = png_create_info_struct(png);
    if (!info) { png_destroy_read_struct(&png, NULL, NULL); fclose(fp); return 0; }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 0;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type, filter_method;
    png_get_IHDR(png, info, &width, &height, &bit_depth, &color_type, &interlace_type, &compression_type, &filter_method);

    // Convert to 8-bit RGB/RGBA
    if (bit_depth == 16) png_set_strip_16(png);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);

    // Ensure we have either RGB or RGBA
    png_read_update_info(png, info);

    png_uint_32 rowbytes = png_get_rowbytes(png, info);
    png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if (!row_pointers) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 0;
    }

    unsigned char* image_data = (unsigned char*)malloc(rowbytes * height);
    if (!image_data) {
        free(row_pointers);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return 0;
    }

    for (png_uint_32 i = 0; i < height; ++i)
        row_pointers[i] = image_data + i * rowbytes;

    png_read_image(png, row_pointers);

    // fill out result
    out->data = image_data;
    out->width = (int)width;
    out->height = (int)height;
    // channels = rowbytes / width
    out->channels = (int)(rowbytes / width);

    free(row_pointers);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return 1;
}

void free_png_image(png_image_result* img)
{
    if (!img) return;
    if (img->data) free(img->data);
    img->data = NULL;
    img->width = img->height = img->channels = 0;
}
