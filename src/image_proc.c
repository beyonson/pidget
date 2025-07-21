#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define PNG_SIG_CMP_BYTES 4

struct pixel_buffer {
  int width;
  int height;
  int bit_depth;
  int bytes_per_row;
  void *pixels;
} pixel_buffer;

struct pixel_buffer
read_png_file (char *filename, png_bytepp *row_pointers)
{
  struct pixel_buffer png_buffer;
  int width, height, bit_depth;
  int png_transforms = PNG_TRANSFORM_STRIP_16;

  FILE *fp = fopen (filename, "rb");
  if (!fp)
    {
      return png_buffer;
    }

  /* Check that loaded file is PNG */
  char header[PNG_SIG_CMP_BYTES];
  fread (header, 1, PNG_SIG_CMP_BYTES, fp);

  int is_png = 0;
  is_png = !png_sig_cmp (header, 0, PNG_SIG_CMP_BYTES);

  if (!is_png)
    {
      fclose (fp);
      printf("File loaded is not a PNG\n");
      return png_buffer;
    }

  /* Allocate and initialize png_struct and png_info */
  png_structp png_ptr
      = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    return png_buffer;

  png_infop info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return png_buffer;
    }

  png_infop end_info = png_create_info_struct (png_ptr);
  if (!end_info)
    {
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
      return png_buffer;
    }

  /* TODO: Either setjmp here or add compiler flag PNG_SETJMP_NOT_SUPPORTED */
  if (setjmp (png_jmpbuf (png_ptr)))
    abort ();

  /* Add back signature bytes that we checked */
  png_set_sig_bytes (png_ptr, PNG_SIG_CMP_BYTES);

  png_init_io (png_ptr, fp);

  png_read_info (png_ptr, info_ptr);

  png_buffer.width = png_get_image_width (png_ptr, info_ptr);
  png_buffer.height = png_get_image_height (png_ptr, info_ptr);
  png_buffer.bit_depth = png_get_bit_depth (png_ptr, info_ptr);

  if (png_buffer.bit_depth == 16)
    png_set_strip_16 (png_ptr);

  png_read_update_info (png_ptr, info_ptr);

  if (*row_pointers)
    abort ();

  *row_pointers = (png_bytep *)malloc (sizeof (png_bytep) * height);
  for (int y = 0; y < height; y++)
    {
      (*row_pointers)[y]
          = (png_byte *)malloc (png_get_rowbytes (png_ptr, info_ptr));
    }
  png_buffer.bytes_per_row = png_get_rowbytes (png_ptr, info_ptr);

  png_read_image (png_ptr, *row_pointers);

  png_read_end (png_ptr, info_ptr);

  png_buffer.pixels = *row_pointers;

  fclose (fp);

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  printf("Height: %d\n", png_buffer.height);
  printf("Width: %d\n", png_buffer.width);

  return png_buffer;
}
