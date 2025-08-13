#ifndef COMMON_H
#define COMMON_H

#include <xcb/xcb.h>

#define DEFAULT_COLOR "default"
#define DEFAULT_CONFIG "/usr/local/share/pidget/config.yml"
#define DEFAULT_IMAGE_PATH "/usr/local/share/pidget/"
#define MAX_FILEPATH_LENGTH 255
#define MAX_FILENAME_LENGTH 255

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
  char *color;
  char *file_name;
  float gravity;
  char *image_path;
  uint8_t images_count;
  char **images;
  int num_pidgets;
  struct PixelBuffer *png_buffer;
} PidgetConfigs;

#endif /* COMMON_H */
