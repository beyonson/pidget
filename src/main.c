#include "config_parser.h"
#include "image_proc.h"
#include "logger.h"
#include "pidget.h"
#include "xcb.h"
#include <assert.h>
#include <ev.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xcb/xcb.h>

struct PidgetConfigs pidget_configs;
struct PidgetConfigs pidget_configs1;

void *
pidget_thread (void *arg)
{
  struct PidgetConfigs *configs = (struct PidgetConfigs *)arg;
  launch_pidget (configs);
  return NULL;
}

int
main (int argc, char *argv[])
{
  /* Parse CLI options */
  int err, opt;
  char *config_file = NULL;

  while ((opt = getopt (argc, argv, "hc:")) != -1)
    {
      switch (opt)
        {
        case 'c':
          config_file = optarg;
          break;
        case 'h':
          log_message (3, "Usage: %s [-c <config file>]\n", argv[0]);
          exit (EXIT_SUCCESS);
        case '?':
          log_message (3, "Usage: %s [-c <config file>]\n", argv[0]);
          exit (EXIT_FAILURE);
        }
    }

  set_log_level (0);

  /* Config file parsing and checking */
  if (config_file != NULL)
    {
      log_message (0, "Config file: %s\n", config_file);
      pidget_configs.file_name = config_file;
      err = parse_config_file (&pidget_configs);
    }
  else
    {
      log_message (1, "Using default configuration.\n", config_file);
      pidget_configs.file_name = DEFAULT_CONFIG;
      err = parse_config_file (&pidget_configs);
    }

  if (err)
    {
      log_message (3, "Error in configuration file.\n");
      return 1;
    }

  pidget_configs1 = pidget_configs;

  // launch_pidget (&pidget_configs);

  pthread_t thread1, thread2;

  pthread_create (&thread1, NULL, pidget_thread, &pidget_configs);
  pthread_create (&thread2, NULL, pidget_thread, &pidget_configs1);

  pthread_join (thread1, NULL);
  pthread_join (thread2, NULL);

  return 0;
}
