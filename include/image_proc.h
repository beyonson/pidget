#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include "common.h"
#include <png.h>

#define PNG_SIG_CMP_BYTES 4
#define RED 0
#define GREEN 1
#define BLUE 2

typedef struct ColorRGB
{
  float r;
  float g;
  float b;
} ColorRGB;

int load_images (struct PixelBuffer **png_buffer,
                 struct PidgetConfigs *configs);

int hex_to_rgb (const char *hexColor, struct ColorRGB *color_rgb);

void apply_tint (struct PixelBuffer *png_buffer, char *color,
                 float blend_factor);

int read_png_file (char *filename, struct PixelBuffer *png_buffer,
                   struct PidgetConfigs *configs);

#endif // IMAGE_PROC_H
