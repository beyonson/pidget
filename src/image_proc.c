#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#define PNG_SIG_CMP_BYTES 4

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

int
read_png_file (char *filename)
{
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

  /* Add back signature bytes that we checked */
  int height, width, depth;
  png_set_sig_bytes (png_ptr, PNG_SIG_CMP_BYTES);

  png_init_io (png_ptr, fp);

  int png_transforms = PNG_TRANSFORM_STRIP_16;
  png_read_png (png_ptr, info_ptr, png_transforms, NULL);

  height = png_get_image_height (png_ptr, info_ptr);
  width = png_get_image_width (png_ptr, info_ptr);
  depth = png_get_bit_depth (png_ptr, info_ptr);

  printf ("Image stats:\n");
  printf ("Height: %d\n", height);
  printf ("Width: %d\n", width);
  printf ("Depth: %d\n", depth);

  fclose (fp);

  return 0;
}
