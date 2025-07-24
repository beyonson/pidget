#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include <png.h>

#define PNG_SIG_CMP_BYTES 4

struct PixelBuffer 
{
  int width;
  int height;
  int bit_depth;
  int bytes_per_row;
  void *pixels;
};

int
read_png_file (char *filename, png_bytepp *row_pointers, struct PixelBuffer *png_buffer);

#endif // IMAGE_PROC_H
