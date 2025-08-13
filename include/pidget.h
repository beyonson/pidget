#ifndef PIDGET_H
#define PIDGET_H

#include "common.h"

typedef struct PidgetCallbackData
{
  struct PidgetConfigs *pidget_configs;
  struct PixelBuffer *png_buffer;
  struct XcbObject *xcb_object;
} PidgetCallbackData;

int 
launch_pidget (struct PidgetConfigs *pidget_configs);

#endif /* PIDGET_H */
