// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PNG_LOADER_H
#define FIFE_PNG_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
struct png_image_result
{
    unsigned char* data;
    int width;
    int height;
    int channels;
};
#else
typedef struct png_image_result
{
    unsigned char* data;
    int width;
    int height;
    int channels;
} png_image_result;
#endif

/**
 * Load a PNG file from disk. Returns 1 on success, 0 on failure.
 * The caller must call `free_png_image` when finished.
 */
int load_png_from_file(const char* path, png_image_result* out);

/**
 * Free data allocated by `load_png_from_file`.
 */
void free_png_image(png_image_result* img);

#ifdef __cplusplus
}
#endif

#endif // FIFE_PNG_LOADER_H
