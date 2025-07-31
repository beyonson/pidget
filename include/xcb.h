#include <xcb/xcb.h>
#include "image_proc.h"

#ifndef XCB_H
#define XCB_H

typedef struct XcbObject
{
  xcb_connection_t *conn;
  xcb_window_t win;
  xcb_gcontext_t gc;
  xcb_pixmap_t backing_pixmap;
  xcb_screen_t *screen;
} XcbObject;

typedef struct MotifHints
{
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
} MotifHints;

xcb_visualtype_t *
find_argb_visual (xcb_connection_t *conn, xcb_screen_t *screen);

int
pidget_xcb_init (XcbObject *xcb_object, struct PixelBuffer *png_buffer);

int
pidget_xcb_load_image (XcbObject *xcb_object, struct PixelBuffer png_buffer);

void
handle_event (XcbObject *xcb_object, xcb_generic_event_t *e);

#endif // XCB_H
