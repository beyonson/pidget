#include "common.h"
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>

/* Returns index of max value */
uint8_t
arr_max (uint8_t num_array[], uint8_t length)
{
  uint8_t max = 0;
  for (int i = 0; i < length; i++)
    {
      if (num_array[max] < num_array[i])
        {
          max = i;
        }
    }

  return max;
}

/* Returns index of min value */
uint8_t
arr_min (uint8_t num_array[], uint8_t length)
{
  uint8_t min = 0;
  for (int i = 0; i < length; i++)
    {
      if (num_array[min] > num_array[i])
        {
          min = i;
        }
    }

  return min;
}
