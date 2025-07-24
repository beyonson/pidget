#include <xcb/xcb.h>
#include "image_proc.h"

#ifndef XCB_H
#define XCB_H

#define WIDTH 409
#define HEIGHT 450

typedef struct XcbObject
{
  xcb_window_t win;
  xcb_gcontext_t gc;
  xcb_pixmap_t backing_pixmap;
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

xcb_window_t
xcb_init (xcb_connection_t *c, struct PixelBuffer png_buffer);

xcb_atom_t
get_atom (xcb_connection_t *conn, const char *name);

void
handle_event (xcb_connection_t *c, xcb_window_t win, xcb_generic_event_t *e);

#endif // XCB_H
