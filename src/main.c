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

ev_timer timeout_watcher;
ev_io xcb_watcher;
ev_signal signal_watcher;

/* One PixelBuffer will be used for each drawing */
/* These will then be used to load into the backing_pixmaps
 * of our XcbObject */
struct PixelBuffer *png_buffer;
struct PidgetConfigs pidget_configs;
struct XcbObject xcb_object;

static void
xcb_event_cb (EV_P_ ev_io *w, int revents)
{
  xcb_generic_event_t *e;
  while ((e = xcb_poll_for_event (xcb_object.conn)))
    {
      if (e != NULL)
        {
          handle_event (&xcb_object, e, png_buffer, &pidget_configs);
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
  pidget_hop_random (&xcb_object, png_buffer, &pidget_configs);
}

static void
sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
  puts ("Caught SIGINT (Ctrl-C). Exiting gracefully.");
  ev_break (EV_A_ EVBREAK_ALL); // Break out of the event loop
}

int
main (int argc, char *argv[])
{
  /* Parse CLI options */
  int err, opt;
  char *config_file = NULL;

  while ((opt = getopt (argc, argv, "hc:")) != -1)
    {
      switch (opt)
        {
        case 'c':
          config_file = optarg;
          break;
        case 'h':
          log_message (3, "Usage: %s [-c <config file>]\n", argv[0]);
          exit (EXIT_SUCCESS);
        case '?':
          log_message (3, "Usage: %s [-c <config file>]\n", argv[0]);
          exit (EXIT_FAILURE);
        }
    }

  set_log_level (0);

  /* Config file parsing and checking */
  if (config_file != NULL)
    {
      log_message (0, "Config file: %s\n", config_file);
      pidget_configs.file_name = config_file;
      err = parse_config_file (&pidget_configs);
    }
  else
    {
      log_message (1, "Using default configuration.\n", config_file);
      pidget_configs.file_name = "config.yml";
      err = parse_config_file (&pidget_configs);
    }

  if (err)
    {
      log_message (3, "Error in configuration file.\n");
      return 1;
    }

  int screen_num;

  /* Load frog images from config file */
  err = load_images (&png_buffer, &pidget_configs);
  if (err)
    {
      log_message (3, "Error loading images.\n");
      return 1;
    }

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
  struct ev_loop *loop = EV_DEFAULT;

  ev_timer_init (&timeout_watcher, timeout_cb, 2.0, 2.0);
  ev_timer_start (loop, &timeout_watcher);

  ev_io_init (&xcb_watcher, xcb_event_cb,
              xcb_get_file_descriptor (xcb_object.conn), EV_READ);
  ev_io_start (loop, &xcb_watcher);

  ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start (loop, &signal_watcher);

  ev_run (loop, 0);

  /* Clean-up */
  xcb_disconnect (xcb_object.conn);
  free (png_buffer);

  return 0;
}
