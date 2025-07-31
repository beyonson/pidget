#include "image_proc.h"
#include "logger.h"
#include "xcb.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

int
main ()
{
  int screen_num;
  struct PixelBuffer png_buffer;
  struct XcbObject xcb_object;

  set_log_level (0);

  /* Load frog PNG file */
  png_bytep *row_pointers = NULL;
  read_png_file ("frog.png", &row_pointers, &png_buffer);

  /* Make connection to X server and initialize our window */
  xcb_object.conn = xcb_connect (NULL, &screen_num);
  pidget_xcb_init (&xcb_object, &png_buffer);
  pidget_xcb_load_image (&xcb_object, png_buffer);

  /* Map the window to our screen */
  xcb_map_window (xcb_object.conn, xcb_object.win);
  xcb_flush (xcb_object.conn);

  /* Event loop */
  /* TODO: Use libev for event loop */
  xcb_generic_event_t *e;
  while ((e = xcb_wait_for_event (xcb_object.conn)))
    {
      handle_event (&xcb_object, e);
      free (e);
    }

  xcb_disconnect (xcb_object.conn);

  return 0;
}
