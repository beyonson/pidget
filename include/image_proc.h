#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include "common.h"
#include <png.h>

#define PNG_SIG_CMP_BYTES 4
#define RED 0
#define GREEN 1
#define BLUE 2

int 
load_images (struct PixelBuffer **png_buffer, struct PidgetConfigs *configs);

int
change_hue (struct PixelBuffer *png_buffer, float color);

int
read_png_file (char *filename, struct PixelBuffer *png_buffer, struct PidgetConfigs *configs);

#endif // IMAGE_PROC_H
