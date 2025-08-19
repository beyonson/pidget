#include "common.h"
#include "image_proc.h"
#include <xcb/xcb.h>

#ifndef XCB_H
#define XCB_H

typedef struct MotifHints
{
  uint32_t flags;
  uint32_t functions;
  uint32_t decorations;
  int32_t input_mode;
  uint32_t status;
} MotifHints;

xcb_visualtype_t *find_argb_visual (xcb_connection_t *conn,
                                    xcb_screen_t *screen);

void pidget_set_origin (XcbObject *xcb_object);

int pidget_set_origin_on_win (XcbObject *xcb_object,
                              struct PixelBuffer *png_buffer);

void pidget_hop_lockscreen (struct XcbObject *xcb_object,
                            struct PixelBuffer *png_buffer,
                            struct PidgetConfigs *pidget_configs);

void pidget_hop_random (XcbObject *xcb_object, struct PixelBuffer *png_buffer,
                        struct PidgetConfigs *pidget_configs);

void pidget_move_random (XcbObject *xcb_object);

int lockscreen_xcb_init (XcbObject *xcb_object);

int pidget_xcb_init (XcbObject *xcb_object);

int pidget_xcb_erase_area (struct XcbObject *xcb_object,
                           struct PixelBuffer png_buffer);

int pidget_xcb_load_image_on_win (XcbObject *xcb_object,
                                  struct PixelBuffer png_buffer, int mirrored);

int pidget_xcb_load_image (XcbObject *xcb_object,
                           struct PixelBuffer png_buffer, int mirrored);

void handle_event (XcbObject *xcb_object, xcb_generic_event_t *e,
                   struct PixelBuffer *png_buffer,
                   struct PidgetConfigs *pidget_configs);

#endif // XCB_H
