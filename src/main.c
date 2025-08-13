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

#define NUM_THREADS 10

void *
pidget_thread (void *arg)
{
  float start_timeout = 1.6 + ((double)rand () / (RAND_MAX + 1.0)) * (5 - 1.6);
  float movement_timeout
      = 2.0 + ((double)rand () / (RAND_MAX + 1.0)) * (10 - 2.0);
  struct PidgetConfigs *configs = (struct PidgetConfigs *)arg;
  launch_pidget (configs, start_timeout, movement_timeout);
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

  pthread_t *threads;
  threads = malloc (NUM_THREADS * sizeof (pthread_t));

  for (int i = 0; i < NUM_THREADS; i++)
    {
      pthread_create (&threads[i], NULL, pidget_thread, &pidget_configs);
    }

  for (int i = 0; i < NUM_THREADS; i++)
    {
      pthread_join (threads[i], NULL);
    }

  return 0;
}
