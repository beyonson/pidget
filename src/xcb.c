#include "xcb.h"
#include "logger.h"
#include <assert.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_errors.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_image.h>

xcb_gcontext_t gc;
xcb_pixmap_t backing_pixmap;

xcb_visualtype_t *
find_argb_visual (xcb_connection_t *conn, xcb_screen_t *screen)
{
  xcb_depth_iterator_t d_iter = xcb_screen_allowed_depths_iterator (screen);

  for (; d_iter.rem; xcb_depth_next (&d_iter))
    {
      if (d_iter.data->depth == 32)
        {
          xcb_visualtype_iterator_t v_iter
              = xcb_depth_visuals_iterator (d_iter.data);
          for (; v_iter.rem; xcb_visualtype_next (&v_iter))
            {
              if (v_iter.data->_class == XCB_VISUAL_CLASS_TRUE_COLOR)
                return v_iter.data;
            }
        }
    }
  return NULL;
}

xcb_window_t
xcb_init (xcb_connection_t *c, struct PixelBuffer png_buffer)
{
  xcb_window_t win;
  xcb_screen_t *screen;

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
  const uint32_t border_width[] = { 0 };
  const uint32_t border_color[] = { 0xFF09224A };

  xcb_visualtype_t *argb_visual = find_argb_visual (c, screen);

  if (!argb_visual)
    {
      fprintf (stderr, "No ARGB visual found\n");
      exit (1);
    }

  xcb_colormap_t colormap = xcb_generate_id (c);
  xcb_create_colormap (c, XCB_COLORMAP_ALLOC_NONE, colormap, screen->root,
                       argb_visual->visual_id);

  win = xcb_generate_id (c);
  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK
                  | XCB_CW_COLORMAP;
  uint32_t valwin[4];
  valwin[0] = 0x00000000;
  valwin[1] = 0x00000000;
  valwin[2] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_RELEASE
              | XCB_EVENT_MASK_KEY_PRESS;
  valwin[3] = colormap;

  /* Create the window */
  xcb_create_window (c, 32, win, screen->root, 0, 0, png_buffer.width,
                     png_buffer.height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                     argb_visual->visual_id, mask, valwin);

  xcb_change_window_attributes (c, win, XCB_CW_BORDER_PIXEL, border_color);
  xcb_configure_window (c, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);

  /* Set motif hints to remove window decorations */
  xcb_change_property (c, XCB_PROP_MODE_REPLACE, win, motif_reply->atom,
                       motif_reply->atom, 32, sizeof (motif_hints),
                       &motif_hints);
  free (motif_reply);

  /* Set forced window size */
  xcb_size_hints_t hints;

  xcb_icccm_size_hints_set_max_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_size_hints_set_min_size (&hints, WIDTH, HEIGHT);
  xcb_icccm_set_wm_size_hints (c, win, XCB_ATOM_WM_NORMAL_HINTS, &hints);

  /* Create backing pixmap */
  png_bytep *rows = (png_bytep *)png_buffer.pixels;
  uint8_t frog_bytes[png_buffer.width * png_buffer.height * 4];

  int counter = 0;
  for (int y = 0; y < png_buffer.height; y++)
    {
      png_bytep row = rows[y]; // row is png_byte *
      for (int x = 0; x < png_buffer.bytes_per_row; x++)
        {
          frog_bytes[counter] = row[x];
          counter += 1;
        }
    }

  xcb_image_t *image;

  image = xcb_image_create_native (
      c, png_buffer.width, png_buffer.height, XCB_IMAGE_FORMAT_Z_PIXMAP, 32,
      (uint8_t *)frog_bytes, png_buffer.width * png_buffer.height * 4,
      (uint8_t *)frog_bytes);

  if (!image)
    {
      log_message (3, "No image\n");
    }

  uint32_t values[] = { screen->white_pixel, screen->white_pixel, 0 };

  backing_pixmap = xcb_generate_id (c);

  xcb_create_pixmap (c, 32, backing_pixmap, win, png_buffer.width,
                     png_buffer.height);

  gc = xcb_generate_id (c);

  xcb_create_gc (c, gc, win,
                 XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
                     | XCB_GC_GRAPHICS_EXPOSURES,
                 values);

  xcb_image_put (c, backing_pixmap, gc, image, 0, 0, 0);

  /* Send image data to X server */
  xcb_copy_area (c, backing_pixmap, win, gc, 0, 0, 0, 0, png_buffer.width,
                 png_buffer.height);

  return win;
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

void
handle_event (xcb_connection_t *c, xcb_window_t win, xcb_generic_event_t *e)
{
  switch (e->response_type)
    {
    case 0:
      {
        xcb_generic_error_t *err = (xcb_generic_error_t *)e;
        xcb_errors_context_t *err_ctx;
        xcb_errors_context_new (c, &err_ctx);
        const char *major, *minor, *extension, *error;
        major = xcb_errors_get_name_for_major_code (err_ctx, err->major_code);
        minor = xcb_errors_get_name_for_minor_code (err_ctx, err->major_code,
                                                    err->minor_code);
        error = xcb_errors_get_name_for_error (err_ctx, err->error_code,
                                               &extension);
        printf ("XCB Error: %s:%s, %s:%s, resource %u sequence %u\n", error,
                extension ? extension : "no_extension", major,
                minor ? minor : "no_minor", (unsigned int)err->resource_id,
                (unsigned int)err->sequence);
        xcb_errors_context_free (err_ctx);
      }
    case XCB_KEY_PRESS:
      {
        /* Add handling code */
        break;
      }
    case XCB_KEY_RELEASE:
      {
        /* Add handling code */
        break;
      }
    case XCB_EXPOSE:
      {
        xcb_expose_event_t *ev = (xcb_expose_event_t *)e;
        xcb_copy_area (c, backing_pixmap, win, gc, 0, 0, 0, 0, ev->width,
                       ev->height);
        xcb_flush (c);
        break;
      }
    default:
      /* Unknown event type, ignore it */
      log_message (0, "Unknown event: %d\n", e->response_type);
      break;
    }
}