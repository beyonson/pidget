#include "logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#define WIDTH 200
#define HEIGHT 200

typedef struct MotifHints
{
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
} MotifHints;

static void
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

xcb_window_t
xcb_init (xcb_connection_t *c)
{
  xcb_window_t win;
  xcb_size_hints_t hints;
  xcb_screen_t *screen;
  uint32_t mask = 0;
  uint32_t valwin[2];

  struct MotifHints motif_hints;

  motif_hints.flags = 2;
  motif_hints.functions = 0;
  motif_hints.decorations = 0;
  motif_hints.input_mode = 0;
  motif_hints.status = 0;

  /* Get the first screen */
  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

  /* Request a window ID */
  win = xcb_generate_id (c);

  /* Set up atoms */
  xcb_intern_atom_cookie_t motif_cookie
      = xcb_intern_atom (c, 0, strlen ("_MOTIF_WM_HINTS"), "_MOTIF_WM_HINTS");
  xcb_intern_atom_reply_t *motif_reply
      = xcb_intern_atom_reply (c, motif_cookie, NULL);

  /* Set window attributes */
  /* NOTE: Indices of win_events must align with mask according to xcb_cw_t */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  valwin[0] = 0x09224A;
  valwin[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS;
  const uint32_t border_width[] = { 5 };
  const uint32_t border_color[] = { 0xAAAAAA };

  /* Create the window */
  xcb_create_window (c, XCB_COPY_FROM_PARENT, win, screen->root, 0, 0, 150,
                     150, 100, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     screen->root_visual, mask, valwin);

  xcb_change_window_attributes (c, win, XCB_CW_BORDER_PIXEL, border_color);
  xcb_configure_window (c, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);

  /* Set motif hints to remove window decorations */
  xcb_change_property (c, XCB_PROP_MODE_REPLACE, win, motif_reply->atom,
                       motif_reply->atom, 32, sizeof (motif_hints),
                       &motif_hints);
  free (motif_reply);

  /* Set forced window size */
  xcb_icccm_size_hints_set_max_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_size_hints_set_min_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_set_wm_size_hints (c, win, XCB_ATOM_WM_NORMAL_HINTS, &hints);

  return win;
}
