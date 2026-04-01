#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Save raw RGB/RGBA image data to a PNG file.
 * `pixels` must point to `height` rows each `pitch` bytes long.
 * `channels` must be 3 (RGB) or 4 (RGBA).
 * Returns 1 on success, 0 on failure.
 */
int save_png_from_memory(const char* path, const void* pixels, int width, int height, int pitch, int channels);

#ifdef __cplusplus
}
#endif
