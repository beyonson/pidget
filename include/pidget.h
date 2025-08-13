#ifndef PIDGET_H
#define PIDGET_H

#include "common.h"

typedef struct PidgetCallbackData
{
  struct PidgetConfigs *pidget_configs;
  struct PixelBuffer *png_buffer;
  struct XcbObject *xcb_object;
} PidgetCallbackData;

int launch_pidget (struct PidgetConfigs *pidget_configs, float start_timeout,
                   float movement_timeout);

#endif /* PIDGET_H */
