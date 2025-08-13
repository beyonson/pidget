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

  log_message (0, "Loading %d images.\n", pidget_configs->images_count);

  for (unsigned i = 0; i < pidget_configs->images_count; i++)
    {
      char image_file[MAX_FILEPATH_LENGTH + MAX_FILENAME_LENGTH];
      snprintf (image_file, sizeof (image_file), "%s%s",
                pidget_configs->image_path, pidget_configs->images[i]);
      err = read_png_file (image_file, &(*png_buffer)[i], pidget_configs);
      if (err)
        {
          log_message (3, "Error reading PNG file.\n");
          return 1;
        }
    }

  return 0;
}

int
hex_to_rgb (const char *hexColor, struct ColorRGB *color_rgb)
{
  /* Skip the '#' if present */
  const char *hex = (hexColor[0] == '#') ? hexColor + 1 : hexColor;
  char component[3] = { 0 };

  strncpy (component, hex, 2);
  color_rgb->r = (int)strtol (component, NULL, 16);

  strncpy (component, hex + 2, 2);
  color_rgb->g = (int)strtol (component, NULL, 16);

  strncpy (component, hex + 4, 2);
  color_rgb->b = (int)strtol (component, NULL, 16);

  return 0;
}

void
apply_tint (struct PixelBuffer *png_buffer, char *color, float blend_factor)
{
  png_bytep *rows = (png_bytep *)png_buffer->pixels;
  struct ColorRGB color_rgb;
  hex_to_rgb (color, &color_rgb);

  for (int y = 0; y < png_buffer->height; y++)
    {
      for (int x = 0; x < png_buffer->width; x++)
        {
          png_bytep row = rows[y];
          if (row[x * 4 + 3] != 0)
            {
              row[x * 4 + 0] = (uint8_t)((1.0f - blend_factor) * row[x * 4 + 0]
                                         + blend_factor * color_rgb.r);
              row[x * 4 + 1] = (uint8_t)((1.0f - blend_factor) * row[x * 4 + 1]
                                         + blend_factor * color_rgb.g);
              row[x * 4 + 2] = (uint8_t)((1.0f - blend_factor) * row[x * 4 + 2]
                                         + blend_factor * color_rgb.b);
            }
        }
    }
}

int
read_png_file (char *filename, struct PixelBuffer *png_buffer,
               struct PidgetConfigs *pidget_configs)
{
  int err;
  png_bytep *row_pointers = NULL;

  FILE *fp = fopen (filename, "rb");
  if (!fp)
    {
      return 1;
    }

  /* Check that loaded file is PNG */
  png_byte header[PNG_SIG_CMP_BYTES];
  err = fread (header, 1, PNG_SIG_CMP_BYTES, fp);
  if (err != PNG_SIG_CMP_BYTES)
    {
      log_message (ERROR, "PNG signature read failed\n");
      return 1;
    }

  int is_png = 0;
  is_png = !png_sig_cmp (header, 0, PNG_SIG_CMP_BYTES);

  if (!is_png)
    {
      fclose (fp);
      log_message (ERROR, "File loaded is not a PNG\n");
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

  if (png_buffer->bit_depth == 16)
    {
      log_message (1, "Stripping image depth to 8\n");
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

  png_read_image (png_ptr, row_pointers);

  png_read_end (png_ptr, info_ptr);

  png_buffer->pixels = (void *)row_pointers;

  if (strcmp (pidget_configs->color, DEFAULT_COLOR) != 0)
    {
      apply_tint (png_buffer, pidget_configs->color, 0.5);
    }

  fclose (fp);

  png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

  return 0;
}
