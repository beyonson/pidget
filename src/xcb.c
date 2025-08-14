#include "xcb.h"
#include "logger.h"
#include <assert.h>
#include <endian.h>
#include <math.h>
#include <png.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_errors.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_image.h>

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

int
lockscreen_xcb_init (XcbObject *xcb_object)
{
  /* Get the first screen */
  xcb_object->screen
      = xcb_setup_roots_iterator (xcb_get_setup (xcb_object->conn)).data;

  /* Request a window ID */
  xcb_object->win = xcb_generate_id (xcb_object->conn);

  /* Set up atoms */
  xcb_intern_atom_cookie_t motif_cookie = xcb_intern_atom (
      xcb_object->conn, 0, strlen ("_MOTIF_WM_HINTS"), "_MOTIF_WM_HINTS");
  xcb_intern_atom_reply_t *motif_reply
      = xcb_intern_atom_reply (xcb_object->conn, motif_cookie, NULL);

  /* Set window attributes */
  /* NOTE: Indices of win_events must align with mask according to xcb_cw_t */
  const uint32_t border_width[] = { 0 };
  const uint32_t border_color[] = { 0xFF09224A };

  /* TODO: allow non-ARGB visual */
  xcb_visualtype_t *argb_visual
      = find_argb_visual (xcb_object->conn, xcb_object->screen);

  if (!argb_visual)
    {
      log_message (3, "No ARGB visual found\n");
      return 1;
    }

  xcb_colormap_t colormap = xcb_generate_id (xcb_object->conn);
  xcb_create_colormap (xcb_object->conn, XCB_COLORMAP_ALLOC_NONE, colormap,
                       xcb_object->screen->root, argb_visual->visual_id);

  xcb_object->win = xcb_generate_id (xcb_object->conn);
  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK
                  | XCB_CW_COLORMAP;
  uint32_t valwin[4];
  valwin[0] = 0xFFFFFFFF;
  valwin[1] = 0xFFFFFFFF;
  valwin[2] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_RELEASE
              | XCB_EVENT_MASK_KEY_PRESS;
  valwin[3] = colormap;

  /* Create the window */
  xcb_create_window (
      xcb_object->conn, 32, xcb_object->win, xcb_object->screen->root, 0, 0,
      xcb_object->screen->width_in_pixels,
      xcb_object->screen->height_in_pixels, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      argb_visual->visual_id, mask, valwin);

  // Set fullscreen hint using EWMH
  xcb_ewmh_connection_t ewmh;
  xcb_intern_atom_cookie_t *ewmh_cookie
      = xcb_ewmh_init_atoms (xcb_object->conn, &ewmh);
  if (!xcb_ewmh_init_atoms_replies (&ewmh, ewmh_cookie, NULL))
    {
      // Handle error
    }

  xcb_atom_t state_fullscreen = ewmh._NET_WM_STATE_FULLSCREEN;
  xcb_change_property (xcb_object->conn, XCB_PROP_MODE_REPLACE,
                       xcb_object->win, ewmh._NET_WM_STATE, XCB_ATOM_ATOM,
                       32, // format
                       1,  // number of elements
                       &state_fullscreen);

  xcb_flush (xcb_object->conn);

  return 0;
}

int
pidget_xcb_init (XcbObject *xcb_object)
{
  struct MotifHints motif_hints;

  motif_hints.flags = 2;
  motif_hints.functions = 0;
  motif_hints.decorations = 0;
  motif_hints.input_mode = 0;
  motif_hints.status = 0;

  /* Get the first screen */
  xcb_object->screen
      = xcb_setup_roots_iterator (xcb_get_setup (xcb_object->conn)).data;

  /* Request a window ID */
  xcb_object->win = xcb_generate_id (xcb_object->conn);

  /* Set up atoms */
  xcb_intern_atom_cookie_t motif_cookie = xcb_intern_atom (
      xcb_object->conn, 0, strlen ("_MOTIF_WM_HINTS"), "_MOTIF_WM_HINTS");
  xcb_intern_atom_reply_t *motif_reply
      = xcb_intern_atom_reply (xcb_object->conn, motif_cookie, NULL);

  /* Set window attributes */
  /* NOTE: Indices of win_events must align with mask according to xcb_cw_t */
  const uint32_t border_width[] = { 0 };
  const uint32_t border_color[] = { 0xFF09224A };

  /* TODO: allow non-ARGB visual */
  xcb_visualtype_t *argb_visual
      = find_argb_visual (xcb_object->conn, xcb_object->screen);

  if (!argb_visual)
    {
      log_message (3, "No ARGB visual found\n");
      return 1;
    }

  xcb_colormap_t colormap = xcb_generate_id (xcb_object->conn);
  xcb_create_colormap (xcb_object->conn, XCB_COLORMAP_ALLOC_NONE, colormap,
                       xcb_object->screen->root, argb_visual->visual_id);

  xcb_object->win = xcb_generate_id (xcb_object->conn);
  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK
                  | XCB_CW_COLORMAP;
  uint32_t valwin[4];
  valwin[0] = 0x00000000;
  valwin[1] = 0x00000000;
  valwin[2] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_RELEASE
              | XCB_EVENT_MASK_KEY_PRESS;
  valwin[3] = colormap;

  /* Create the window */
  xcb_create_window (xcb_object->conn, 32, xcb_object->win,
                     xcb_object->screen->root, 0, 0, 96, 192, 0,
                     XCB_WINDOW_CLASS_INPUT_OUTPUT, argb_visual->visual_id,
                     mask, valwin);

  xcb_change_window_attributes (xcb_object->conn, xcb_object->win,
                                XCB_CW_BORDER_PIXEL, border_color);
  xcb_configure_window (xcb_object->conn, xcb_object->win,
                        XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);

  const static uint32_t values[] = { XCB_STACK_MODE_ABOVE };
  xcb_configure_window (xcb_object->conn, xcb_object->win,
                        XCB_CONFIG_WINDOW_STACK_MODE, values);
  xcb_flush (xcb_object->conn); // Ensure the request is sent to the X server

  /* Set motif hints to remove window decorations */
  xcb_change_property (xcb_object->conn, XCB_PROP_MODE_REPLACE,
                       xcb_object->win, motif_reply->atom, motif_reply->atom,
                       32, sizeof (motif_hints), &motif_hints);
  free (motif_reply);

  /* Set forced window size, for tiling WMs */
  xcb_size_hints_t hints;

  xcb_icccm_size_hints_set_max_size (&hints, 96, 192);
  xcb_icccm_size_hints_set_min_size (&hints, 96, 192);
  xcb_icccm_set_wm_size_hints (xcb_object->conn, xcb_object->win,
                               XCB_ATOM_WM_NORMAL_HINTS, &hints);

  xcb_flush (xcb_object->conn);

  return 0;
}

int
pidget_xcb_load_image (XcbObject *xcb_object, struct PixelBuffer png_buffer,
                       int mirrored)
{
  /* Create backing pixmap */
  png_bytep *rows = (png_bytep *)png_buffer.pixels;
  uint8_t frog_bytes[png_buffer.width * png_buffer.height * 4];

  if (!mirrored)
    {
      for (int y = 0; y < png_buffer.height; y++)
        {
          png_bytep row = rows[y]; // row is png_byte *
          for (int x = 0; x < png_buffer.width; x++)
            {
              int dst_i = (y * png_buffer.width + x) * 4;

              uint8_t r = row[x * 4 + 0];
              uint8_t g = row[x * 4 + 1];
              uint8_t b = row[x * 4 + 2];
              uint8_t a = row[x * 4 + 3];

#if __BYTE_ORDER == __LITTLE_ENDIAN
              frog_bytes[dst_i + 0] = b;
              frog_bytes[dst_i + 1] = g;
              frog_bytes[dst_i + 2] = r;
              frog_bytes[dst_i + 3] = a;
#else
              frog_bytes[dst_i + 0] = r;
              frog_bytes[dst_i + 1] = g;
              frog_bytes[dst_i + 2] = b;
              frog_bytes[dst_i + 3] = a;
#endif
            }
        }
    }
  else
    {
      for (int y = 0; y < png_buffer.height; y++)
        {
          png_bytep row = rows[y];
          for (int x = 0; x < png_buffer.width; x++)
            {
              int src_x = png_buffer.width - x - 1;

              uint8_t r = row[src_x * 4 + 0];
              uint8_t g = row[src_x * 4 + 1];
              uint8_t b = row[src_x * 4 + 2];
              uint8_t a = row[src_x * 4 + 3];

              int dst_i = (y * png_buffer.width + x) * 4;

#if __BYTE_ORDER == __LITTLE_ENDIAN
              frog_bytes[dst_i + 0] = b;
              frog_bytes[dst_i + 1] = g;
              frog_bytes[dst_i + 2] = r;
              frog_bytes[dst_i + 3] = a;
#else
              frog_bytes[dst_i + 0] = r;
              frog_bytes[dst_i + 1] = g;
              frog_bytes[dst_i + 2] = b;
              frog_bytes[dst_i + 3] = a;
#endif
            }
        }
    }

  xcb_image_t *image;

  image = xcb_image_create_native (
      xcb_object->conn, png_buffer.width, png_buffer.height,
      XCB_IMAGE_FORMAT_Z_PIXMAP, 32, (uint8_t *)frog_bytes,
      png_buffer.width * png_buffer.height * 4, (uint8_t *)frog_bytes);

  if (!image)
    {
      log_message (3, "No image\n");
      return 1;
    }

  uint32_t values[] = { xcb_object->screen->white_pixel,
                        xcb_object->screen->white_pixel, 0 };

  xcb_object->backing_pixmap = xcb_generate_id (xcb_object->conn);

  xcb_create_pixmap (xcb_object->conn, 32, xcb_object->backing_pixmap,
                     xcb_object->win, png_buffer.width, png_buffer.height);

  xcb_object->gc = xcb_generate_id (xcb_object->conn);

  xcb_create_gc (xcb_object->conn, xcb_object->gc, xcb_object->win,
                 XCB_GC_FOREGROUND | XCB_GC_BACKGROUND
                     | XCB_GC_GRAPHICS_EXPOSURES,
                 values);

  xcb_image_put (xcb_object->conn, xcb_object->backing_pixmap, xcb_object->gc,
                 image, 0, 0, 0);

  uint32_t size_values[]
      = { png_buffer.width, png_buffer.height, XCB_STACK_MODE_ABOVE };
  xcb_configure_window (xcb_object->conn, xcb_object->win,
                        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT
                            | XCB_CONFIG_WINDOW_STACK_MODE,
                        size_values);
  /* Send image data to X server */
  xcb_copy_area (xcb_object->conn, xcb_object->backing_pixmap, xcb_object->win,
                 xcb_object->gc, 0, 0, 0, 0, png_buffer.width,
                 png_buffer.height);
  xcb_flush (xcb_object->conn);

  return 0;
}

void
pidget_set_origin (XcbObject *xcb_object)
{
  int ybelow;
  xcb_get_geometry_reply_t *geom;
  xcb_translate_coordinates_reply_t *trans_coords;

  xcb_get_geometry_cookie_t gg_cookie
      = xcb_get_geometry (xcb_object->conn, xcb_object->win);

  geom = xcb_get_geometry_reply (xcb_object->conn, gg_cookie, NULL);

  xcb_translate_coordinates_cookie_t trans_coords_cookie
      = xcb_translate_coordinates (xcb_object->conn, xcb_object->win,
                                   geom->root, -(geom->border_width),
                                   (geom->border_width));

  trans_coords = xcb_translate_coordinates_reply (xcb_object->conn,
                                                  trans_coords_cookie, NULL);
  if (!trans_coords)
    {
      log_message (3, "Can't get translated coordinates.");
      goto error_trans;
    }

  ybelow = (xcb_object->screen->height_in_pixels - geom->border_width * 2
            - geom->height);

  uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
  uint32_t values[] = { 0, ybelow };

  xcb_configure_window (xcb_object->conn, xcb_object->win, mask, values);
  xcb_flush (xcb_object->conn);

  free (trans_coords);
error_trans:
  free (geom);
  return;
}

void
pidget_hop_random (struct XcbObject *xcb_object,
                   struct PixelBuffer *png_buffer,
                   struct PidgetConfigs *pidget_configs)
{
  int rx, ry, xright;
  int mirrored = false;
  xcb_get_geometry_reply_t *geom;
  xcb_translate_coordinates_reply_t *trans_coords;

  xcb_get_geometry_cookie_t gg_cookie
      = xcb_get_geometry (xcb_object->conn, xcb_object->win);

  geom = xcb_get_geometry_reply (xcb_object->conn, gg_cookie, NULL);

  xcb_translate_coordinates_cookie_t trans_coords_cookie
      = xcb_translate_coordinates (xcb_object->conn, xcb_object->win,
                                   geom->root, -(geom->border_width),
                                   (geom->border_width));

  trans_coords = xcb_translate_coordinates_reply (xcb_object->conn,
                                                  trans_coords_cookie, NULL);
  if (!trans_coords)
    {
      log_message (3, "Can't get translated coordinates.");
      goto error_trans;
    }

  rx = (int16_t)trans_coords->dst_x;
  ry = (int16_t)trans_coords->dst_y;

  xright = (xcb_object->screen->width_in_pixels - rx - geom->border_width * 2
            - geom->width);

  double gravity = pidget_configs->gravity;
  int steps = 32;
  double v0 = 1.6 + ((double)rand () / (RAND_MAX + 1.0)) * (5 - 1.6);
  double theta = 0.8;
  double vx0 = v0 * cos (theta);
  double vy0 = v0 * sin (theta);
  double range = 0.0;
  double time_of_flight = (2 * v0 * sin (theta)) / gravity;
  double step_time = time_of_flight / (double)steps;
  uint32_t mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;

  range = 100 * ((v0 * v0) * sin (2 * theta) / gravity);

  /* Choose direction */
  srand (time (NULL));
  int left;
  if (xright - range < 0)
    {
      left = 1;
    }
  else if (rx - range < 0)
    {
      left = 0;
    }
  else
    {
      left = (rand () % (1 - 0 + 1));
    }
  mirrored = left;

  pidget_xcb_load_image (xcb_object, png_buffer[2], mirrored);
  for (int i = 0; i <= steps; i++)
    {
      double x, y;
      double current_time = i * step_time;
      /* Displacement equations omit starting position since it's always 0 */
      x = 100 * (vx0 * current_time);
      y = 100
          * ((vy0 * current_time)
             - (0.5 * gravity * (current_time * current_time)));

      uint32_t values[] = { rx + x, ry - y };

      if (current_time > time_of_flight * .5)
        {
          pidget_xcb_load_image (xcb_object, png_buffer[1], mirrored);
        }

      if (left)
        {
          values[0] = rx - x;
        }

      xcb_configure_window (xcb_object->conn, xcb_object->win, mask, values);
      xcb_flush (xcb_object->conn);
      usleep ((int)(1000000 * step_time));
    }
  pidget_xcb_load_image (xcb_object, png_buffer[0], mirrored);

  free (trans_coords);
error_trans:
  free (geom);
  return;
}

void
handle_event (XcbObject *xcb_object, xcb_generic_event_t *e,
              struct PixelBuffer *png_buffer,
              struct PidgetConfigs *pidget_configs)
{
  switch (e->response_type)
    {
    case 0:
      {
        xcb_generic_error_t *err = (xcb_generic_error_t *)e;
        xcb_errors_context_t *err_ctx;
        xcb_errors_context_new (xcb_object->conn, &err_ctx);
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
        pidget_hop_random (xcb_object, png_buffer, pidget_configs);
        xcb_flush (xcb_object->conn);
        break;
      }
    case XCB_EXPOSE:
      {
        xcb_expose_event_t *ev = (xcb_expose_event_t *)e;
        xcb_copy_area (xcb_object->conn, xcb_object->backing_pixmap,
                       xcb_object->win, xcb_object->gc, 0, 0, 0, 0, ev->width,
                       ev->height);
        xcb_flush (xcb_object->conn);
        break;
      }
    default:
      /* Unknown event type, ignore it */
      log_message (0, "Unknown event: %d\n", e->response_type);
      break;
    }
}
