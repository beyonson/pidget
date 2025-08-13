#include "pidget.h"
#include "config_parser.h"
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

static void
xcb_event_cb (EV_P_ ev_io *w, int revents)
{
  struct PidgetCallbackData *data = (PidgetCallbackData *)w->data;
  xcb_generic_event_t *e;
  while ((e = xcb_poll_for_event (data->xcb_object->conn)))
    {
      if (e != NULL)
        {
          handle_event (data->xcb_object, e, data->png_buffer,
                        data->pidget_configs);
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
  struct PidgetCallbackData *data = (PidgetCallbackData *)w->data;
  pidget_hop_random (data->xcb_object, data->png_buffer, data->pidget_configs);
}

static void
sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
  puts ("Caught SIGINT (Ctrl-C). Exiting gracefully.");
  ev_break (EV_A_ EVBREAK_ALL); // Break out of the event loop
}

int
launch_pidget (struct PidgetConfigs *pidget_configs, float start_timeout,
               float movement_timeout)
{
  ev_timer timeout_watcher;
  ev_io xcb_watcher;
  ev_signal signal_watcher;
  int err;
  int screen_num;

  /* One PixelBuffer will be used for each drawing */
  /* These will then be used to load into the backing_pixmaps
   * of our XcbObject */
  struct PixelBuffer *png_buffer;
  struct XcbObject xcb_object;
  struct PidgetCallbackData *pidget_data
      = malloc (sizeof (struct PidgetCallbackData));

  /* Load frog images from config file */
  err = load_images (&png_buffer, pidget_configs);
  if (err)
    {
      log_message (3, "Error loading images.\n");
      return 1;
    }

  pidget_data->png_buffer = png_buffer;
  pidget_data->xcb_object = &xcb_object;
  pidget_data->pidget_configs = pidget_configs;

  /* Make connection to X server and initialize our window */
  xcb_object.conn = xcb_connect (NULL, &screen_num);
  /* TODO: Make de-init, which frees all memory */
  err = pidget_xcb_init (&xcb_object);
  if (err)
    {
      log_message (3, "Error initializing XCB.\n");
      return 1;
    }

  err = pidget_xcb_load_image (&xcb_object, png_buffer[0], false);
  if (err)
    {
      log_message (3, "Error loading image with XCB.\n");
      return 1;
    }

  xcb_map_window (xcb_object.conn, xcb_object.win);

  pidget_set_origin (&xcb_object);

  xcb_flush (xcb_object.conn);

  /* Event loop crud */
  struct ev_loop *loop = ev_loop_new (EVFLAG_AUTO);

  ev_timer_init (&timeout_watcher, timeout_cb, start_timeout,
                 movement_timeout);
  ev_timer_start (loop, &timeout_watcher);
  timeout_watcher.data = pidget_data;

  ev_io_init (&xcb_watcher, xcb_event_cb,
              xcb_get_file_descriptor (xcb_object.conn), EV_READ);
  ev_io_start (loop, &xcb_watcher);
  xcb_watcher.data = pidget_data;

  //  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  //  ev_signal_start (loop, &signal_watcher);
  //  signal_watcher.data = pidget_data;

  ev_run (loop, 0);

  /* Clean-up */
  xcb_disconnect (xcb_object.conn);
  ev_loop_destroy (loop);
  free (png_buffer);

  return 0;
}
