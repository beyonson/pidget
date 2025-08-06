#include "image_proc.h"
#include "logger.h"
#include "xcb.h"
#include <assert.h>
#include <ev.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>

ev_timer timeout_watcher;
ev_io xcb_watcher;
ev_signal signal_watcher;

/* One PixelBuffer will be used for each drawing */
/* These will then be used to load into the backing_pixmaps
 * of our XcbObject */
struct PixelBuffer *png_buffer;
struct XcbObject xcb_object;

static void
xcb_event_cb (EV_P_ ev_io *w, int revents)
{
  xcb_generic_event_t *e;
  while ((e = xcb_poll_for_event (xcb_object.conn)))
    {
      if (e != NULL)
        {
          handle_event (&xcb_object, e, png_buffer);
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
  pidget_hop_random (&xcb_object, png_buffer);
}

static void
sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
  puts ("Caught SIGINT (Ctrl-C). Exiting gracefully.");
  ev_break (EV_A_ EVBREAK_ALL); // Break out of the event loop
}

int
main ()
{
  int screen_num;
  int num_images = 3;

  set_log_level (0);

  /* Load frog PNG file */
  png_buffer = malloc (num_images * sizeof (struct PixelBuffer));
  read_png_file ("images/frog-1.png", &png_buffer[0]);
  read_png_file ("images/frog-2.png", &png_buffer[1]);
  read_png_file ("images/frog-3.png", &png_buffer[2]);

  /* Make connection to X server and initialize our window */
  xcb_object.conn = xcb_connect (NULL, &screen_num);
  /* TODO: Make de-init, which frees all memory */
  pidget_xcb_init (&xcb_object);
  pidget_xcb_load_image (&xcb_object, png_buffer[0], false);

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

  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start (loop, &signal_watcher);

  ev_run (loop, 0);

  xcb_disconnect (xcb_object.conn);

  free (png_buffer);

  return 0;
}
