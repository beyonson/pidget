#include "image_proc.h"
#include <logger.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>

int
read_png_file (char *filename, png_bytepp *row_pointers,
               struct PixelBuffer *png_buffer)
{
  FILE *fp = fopen (filename, "rb");
  if (!fp)
    {
      return 1;
    }

  /* Check that loaded file is PNG */
  png_byte header[PNG_SIG_CMP_BYTES];
  fread (header, 1, PNG_SIG_CMP_BYTES, fp);

  int is_png = 0;
  is_png = !png_sig_cmp (header, 0, PNG_SIG_CMP_BYTES);

  if (!is_png)
    {
      fclose (fp);
      log_message (3, "File loaded is not a PNG\n");
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

  png_buffer->width = png_get_image_width (png_ptr, info_ptr);
  png_buffer->height = png_get_image_height (png_ptr, info_ptr);
  png_buffer->bit_depth = png_get_bit_depth (png_ptr, info_ptr);

  log_message (0, "Image width: %d\n", png_buffer->width);
  log_message (0, "Image height: %d\n", png_buffer->height);
  log_message (0, "Image depth: %d\n", png_buffer->bit_depth);

  if (png_get_color_type (png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB_ALPHA)
    {
      log_message (0, "Swapping alpha\n");
      // png_set_swap_alpha(png_ptr);
    }

  png_read_update_info (png_ptr, info_ptr);

  if (*row_pointers)
    abort ();

  *row_pointers
      = (png_bytep *)malloc (sizeof (png_bytep) * png_buffer->height);
  for (int y = 0; y < png_buffer->height; y++)
    {
      (*row_pointers)[y]
          = (png_byte *)malloc (png_get_rowbytes (png_ptr, info_ptr));
    }

  png_buffer->bytes_per_row = png_get_rowbytes (png_ptr, info_ptr);
  log_message (0, "Image bytes per row: %d\n", png_buffer->bytes_per_row);

  png_read_image (png_ptr, *row_pointers);

  png_read_end (png_ptr, info_ptr);

  png_buffer->pixels = (void *)*row_pointers;

  fclose (fp);

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  return 0;
}
