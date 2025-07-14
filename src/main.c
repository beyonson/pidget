#include "logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#define WIDTH 90
#define HEIGHT 90

// motif hints
typedef struct MotifHints
{
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
};

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

xcb_atom_t
get_atom (xcb_connection_t *conn, const char *name)
{
  xcb_generic_error_t *e;
  xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply (
      conn, xcb_intern_atom (conn, 0, strlen (name), name), &e);

  if (e != NULL)
    {
      log_message (3, "%s: failed to get atom", name);
      free (e);
      free (reply);

      return (xcb_atom_t){ 0 };
    }

  xcb_atom_t ret = reply->atom;
  log_message (0, "atom %s = 0x%08x", name, ret);

  if (ret == XCB_ATOM_NONE)
    log_message (3, "%s: no such atom", name);

  assert (ret != XCB_ATOM_NONE);

  free (reply);
  return ret;
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
  xcb_atom_t _NET_WM_WINDOW_TYPE;
  xcb_atom_t _NET_WM_WINDOW_TYPE_TOOLBAR;

  struct MotifHints motif_hints;

  motif_hints.flags = 2;
  motif_hints.functions = 0;
  motif_hints.decorations = 0;
  motif_hints.input_mode = 0;
  motif_hints.status = 0;

  /* Set up logging */
  set_log_level (0);

  /* Make connection to X server */
  c = xcb_connect (NULL, &screen_num);

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Request a window ID */
  win = xcb_generate_id (c);

  /* Set up atoms */
  xcb_intern_atom_cookie_t cookie3
      = xcb_intern_atom (c, 0, strlen ("_MOTIF_WM_HINTS"), "_MOTIF_WM_HINTS");
  xcb_intern_atom_reply_t *reply3 = xcb_intern_atom_reply (c, cookie3, NULL);

  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  /* Indices of win_events must align with mask according to xcb_cw_t */
  valwin[0] = screen->white_pixel;
  valwin[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_create_window (c, XCB_COPY_FROM_PARENT, win, screen->root, 0, 0, 150,
                     150, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     screen->root_visual, mask, valwin);

  /* Set the type of the window */
  xcb_change_property (
      c, XCB_PROP_MODE_REPLACE, win, reply3->atom,
      reply3->atom,  // THIS is essential
      32,            // format of property
      5,             // length of data (5x32 bit) , followed by pointer to data
      &motif_hints); // is this is a motif hints struct
  free (reply3);

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
