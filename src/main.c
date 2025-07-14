#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#include "logger.h"

#define WIDTH 90
#define HEIGHT 90

void
print_modifiers (uint32_t mask)
{
  const char **mod,
      *mods[] = { "Shift",   "Lock",    "Ctrl",   "Alt",     "Mod2",
                  "Mod3",    "Mod4",    "Mod5",   "Button1", "Button2",
                  "Button3", "Button4", "Button5" };
  log_message (0, "Modifier mask: ");
  for (mod = mods; mask; mask >>= 1, mod++)
    if (mask & 1)
      log_message (0, *mod);
  putchar ('\n');
}

int
main ()
{
  xcb_connection_t *c;
  xcb_screen_t *screen;
  xcb_window_t win;
  int screen_num;
  uint32_t mask = 0;
  uint32_t valwin[2];
  xcb_size_hints_t hints;

  /* Set up logging */
  set_log_level (0);

  /* Make connection to X server */
  c = xcb_connect (NULL, &screen_num);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Request a window ID */
  win = xcb_generate_id (c);

  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  /* Indices of win_events must align with mask according to xcb_cw_t */
  valwin[0] = screen->white_pixel;
  valwin[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_create_window (c, XCB_COPY_FROM_PARENT, win, screen->root, 0, 0, 150,
                     150, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     screen->root_visual, mask, valwin);

  /* Set forced window size */
  xcb_icccm_size_hints_set_max_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_size_hints_set_min_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_set_wm_size_hints (c, win, XCB_ATOM_WM_NORMAL_HINTS, &hints);

  /* Map the window to our screen */
  xcb_map_window (c, win);

  /* Flush commands so our screen is drawn before pause */
  xcb_flush (c);

  /* Event loop */
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
