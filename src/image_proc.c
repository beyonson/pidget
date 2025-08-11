#include "image_proc.h"
#include "common.h"
#include "config_parser.h"
#include <logger.h>
#include <math.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
load_images (struct PixelBuffer **png_buffer,
             struct PidgetConfigs *pidget_configs)
{
  int err;
  *png_buffer
      = malloc (pidget_configs->images_count * sizeof (struct PixelBuffer));

  for (unsigned i = 0; i < pidget_configs->images_count; i++)
    {
      err = read_png_file (pidget_configs->images[i], &(*png_buffer)[i],
                           pidget_configs);
      if (err)
        {
          log_message (3, "Error reading PNG file.\n");
          return 1;
        }
    }

  return 0;
}

int
change_hue (struct PixelBuffer *png_buffer, float color)
{
  png_bytep *rows = (png_bytep *)png_buffer->pixels;
  /* Change the hue */
  for (int y = 0; y < png_buffer->height; y++)
    {
      for (int x = 0; x < png_buffer->width; x++)
        {
          png_bytep row = rows[y];
          uint8_t rgb_arr[3];
          uint8_t min, max;
          float min_val, max_val, h, s, v;

          rgb_arr[RED] = row[x * 4 + 0];
          rgb_arr[GREEN] = row[x * 4 + 1];
          rgb_arr[BLUE] = row[x * 4 + 2];

          min = arr_min (rgb_arr, 3);
          max = arr_max (rgb_arr, 3);
          min_val = (float)rgb_arr[min];
          max_val = (float)rgb_arr[max];

          v = max_val;
          s = (max_val - min_val) / max_val;
          h = color;

          float c = v * s;
          float X = c * (1 - fabsf (fmodf (h / 60.0f, 2) - 1));
          float m = v - c;

          if (h >= 0 && h < 60)
            {
              rgb_arr[RED] = c;
              rgb_arr[GREEN] = X;
              rgb_arr[BLUE] = 0;
            }
          else if (h >= 60 && h < 120)
            {
              rgb_arr[RED] = X;
              rgb_arr[GREEN] = c;
              rgb_arr[BLUE] = 0;
            }
          else if (h >= 120 && h < 180)
            {
              rgb_arr[RED] = 0;
              rgb_arr[GREEN] = c;
              rgb_arr[BLUE] = X;
            }
          else if (h >= 180 && h < 240)
            {
              rgb_arr[RED] = 0;
              rgb_arr[GREEN] = X;
              rgb_arr[BLUE] = c;
            }
          else if (h >= 240 && h < 300)
            {
              rgb_arr[RED] = X;
              rgb_arr[GREEN] = 0;
              rgb_arr[BLUE] = c;
            }
          else
            { /* h >= 300 && h < 360 */
              rgb_arr[RED] = c;
              rgb_arr[GREEN] = 0;
              rgb_arr[BLUE] = X;
            }

          rgb_arr[RED] += m;
          rgb_arr[GREEN] += m;
          rgb_arr[BLUE] += m;

          row[x * 4 + 0] = rgb_arr[RED];
          row[x * 4 + 1] = rgb_arr[GREEN];
          row[x * 4 + 2] = rgb_arr[BLUE];
        }
    }

  return 0;
}

int
read_png_file (char *filename, struct PixelBuffer *png_buffer,
               struct PidgetConfigs *pidget_configs)
{
  png_bytep *row_pointers = NULL;

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

  if (png_buffer->bit_depth == 16)
    {
      log_message (0, "Stripping depth to 8\n");
      png_set_strip_16 (png_ptr);
    }

  png_read_update_info (png_ptr, info_ptr);

  if (row_pointers)
    abort ();

  row_pointers = (png_bytep *)malloc (sizeof (png_bytep) * png_buffer->height);
  for (int y = 0; y < png_buffer->height; y++)
    {
      (row_pointers)[y]
          = (png_byte *)malloc (png_get_rowbytes (png_ptr, info_ptr));
    }

  png_buffer->bytes_per_row = png_get_rowbytes (png_ptr, info_ptr);
  log_message (0, "Image bytes per row: %d\n", png_buffer->bytes_per_row);

  png_read_image (png_ptr, row_pointers);

  png_read_end (png_ptr, info_ptr);

  png_buffer->pixels = (void *)row_pointers;

  change_hue (png_buffer, pidget_configs->color);

  fclose (fp);

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  return 0;
}
