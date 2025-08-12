#ifndef COMMON_H
#define COMMON_H

#include <xcb/xcb.h>

#define DEFAULT_COLOR "default"

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

#endif /* COMMON_H */
