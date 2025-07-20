#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define PNG_SIG_CMP_BYTES 4

int
read_png_file (char *filename, png_bytepp *row_pointers)
{
  int width, height, bit_depth, color_type;
  int png_transforms = PNG_TRANSFORM_STRIP_16;

  FILE *fp = fopen (filename, "rb");
  if (!fp)
    {
      return 1;
    }

  /* Check that loaded file is PNG */
  char header[PNG_SIG_CMP_BYTES];
  fread (header, 1, PNG_SIG_CMP_BYTES, fp);

  int is_png = 0;
  is_png = !png_sig_cmp (header, 0, PNG_SIG_CMP_BYTES);

  if (!is_png)
    {
      fclose (fp);
      return 1;
    }

  /* Allocate and initialize png_struct and png_info */
  png_structp png_ptr
      = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    return 1;

  png_infop info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return 1;
    }

  png_infop end_info = png_create_info_struct (png_ptr);
  if (!end_info)
    {
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp)NULL);
      return 1;
    }

  /* TODO: Either setjmp here or add compiler flag PNG_SETJMP_NOT_SUPPORTED */
  if (setjmp (png_jmpbuf (png_ptr)))
    abort ();

  /* Add back signature bytes that we checked */
  png_set_sig_bytes (png_ptr, PNG_SIG_CMP_BYTES);

  png_init_io (png_ptr, fp);

  png_read_info (png_ptr, info_ptr);

  width = png_get_image_width (png_ptr, info_ptr);
  height = png_get_image_height (png_ptr, info_ptr);
  color_type = png_get_color_type (png_ptr, info_ptr);
  bit_depth = png_get_bit_depth (png_ptr, info_ptr);

  if (bit_depth == 16)
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

  png_read_image (png_ptr, *row_pointers);

  png_read_end (png_ptr, info_ptr);

  fclose (fp);

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  return 0;
}
