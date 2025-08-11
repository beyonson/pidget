#ifndef COMMON_H
#define COMMON_H

#include <xcb/xcb.h>

typedef struct XcbObject
{
  xcb_connection_t *conn;
  xcb_window_t win;
  xcb_gcontext_t gc;
  xcb_pixmap_t backing_pixmap;
  xcb_screen_t *screen;
} XcbObject;

struct PixelBuffer 
{
  int width;
  int height;
  int bit_depth;
  int bytes_per_row;
  void *pixels;
};

typedef struct PidgetConfigs 
{
  char *file_name;
  char *color;
  float gravity;
  uint8_t images_count;
  char **images;
  struct PixelBuffer *png_buffer;
} PidgetConfigs;

/* Returns index of max value */
uint8_t arr_max (uint8_t num_array[], uint8_t length);

/* Returns index of min value */
uint8_t arr_min (uint8_t num_array[], uint8_t length);

#endif /* COMMON_H */
