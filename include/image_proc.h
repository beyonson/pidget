#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include "common.h"
#include <png.h>

#define PNG_SIG_CMP_BYTES 4

int 
load_images (struct PixelBuffer **png_buffer, struct PidgetConfigs *configs);

int
read_png_file (char *filename, struct PixelBuffer *png_buffer);

#endif // IMAGE_PROC_H
