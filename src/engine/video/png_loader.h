#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned char* data;
    int width;
    int height;
    int channels;
} png_image_result;

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
