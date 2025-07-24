#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include <png.h>

#define PNG_SIG_CMP_BYTES 4

struct pixel_buffer
{
  int width;
  int height;
  int bit_depth;
  int bytes_per_row;
  void *pixels;
};

struct pixel_buffer
read_png_file (char *filename, png_bytepp *row_pointers);

#endif // IMAGE_PROC_H
