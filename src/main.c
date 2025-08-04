#include "image_proc.h"
#include "logger.h"
#include "xcb.h"
#include <assert.h>
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>

ev_timer timeout_watcher;
ev_io xcb_watcher;

struct PixelBuffer png_buffer;
struct XcbObject xcb_object;

static void
xcb_event_cb (EV_P_ ev_io *w, int revents)
{
  xcb_generic_event_t *e;
  while ((e = xcb_poll_for_event (xcb_object.conn)))
    {
      if (e != NULL)
        {
          handle_event (&xcb_object, e);
          free (e);
        }
      else
        {
          log_message (0, "No event received\n");
        }
    }
}

static void
timeout_cb (EV_P_ ev_timer *w, int revents)
{
  pidget_hop_random (&xcb_object);
}

int
main ()
{
  int screen_num;

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

  pidget_set_origin (&xcb_object);

  xcb_flush (xcb_object.conn);

  /* Event loop */
  struct ev_loop *loop = EV_DEFAULT;
  ev_timer_init (&timeout_watcher, timeout_cb, 1.0, 2.0);
  ev_timer_start (loop, &timeout_watcher);

  ev_io_init (&xcb_watcher, xcb_event_cb,
              xcb_get_file_descriptor (xcb_object.conn), EV_READ);
  ev_io_start (loop, &xcb_watcher);

  ev_run (loop, 0);

  xcb_disconnect (xcb_object.conn);

  return 0;
}
