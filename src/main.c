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

  set_log_level (0);

  /* Make connection to X server and initialize our window */
  c = xcb_connect (NULL, &screen_num);
  win = xcb_init (c);

  /* Map the window to our screen */
  xcb_map_window (c, win);

  /* Flush commands so our screen is drawn before pause */
  xcb_flush (c);

  /* Event loop */
  /* TODO: Use libev for event loop */
  xcb_generic_event_t *e;

  while ((e = xcb_wait_for_event (c)))
    {
      switch (e->response_type & ~0x80)
        {
        case XCB_KEY_PRESS:
          {
            xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
            print_modifiers (ev->state);
            log_message (0, "Key pressed in window %ld\n", ev->event);
            break;
          }
        case XCB_KEY_RELEASE:
          {
            xcb_key_release_event_t *ev = (xcb_key_release_event_t *)e;
            log_message (0, "Key released in window %ld\n", ev->event);
            break;
          }
        default:
          /* Unknown event type, ignore it */
          log_message (0, "Unknown event: %d\n", e->response_type);
          break;
        }
      free (e);
    }

  xcb_disconnect (c);

  return 0;
}
