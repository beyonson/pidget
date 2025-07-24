#include "image_proc.c"
#include "logger.h"
#include "xcb.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

int
main ()
{
  xcb_connection_t *c;
  xcb_window_t win;
  int screen_num;
  struct pixel_buffer png_buffer;

  set_log_level (0);

  /* Load frog PNG file */
  png_bytep *row_pointers = NULL;
  png_buffer = read_png_file ("square.png", &row_pointers);

  /* Make connection to X server and initialize our window */
  c = xcb_connect (NULL, &screen_num);
  win = xcb_init (c, png_buffer);

  /* Map the window to our screen */
  xcb_map_window (c, win);

  /* Flush commands so our screen is drawn before pause */
  xcb_flush (c);

  /* Event loop */
  /* TODO: Use libev for event loop */
  xcb_generic_event_t *e;
  while ((e = xcb_wait_for_event (c)))
    {
      handle_event (c, win, e);
      free (e);
    }

  xcb_disconnect (c);

  free (row_pointers);

  return 0;
}
