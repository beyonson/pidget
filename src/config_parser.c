#include "config_parser.h"
#include "common.h"
#include "image_proc.h"
#include "logger.h"
#include <cyaml/cyaml.h>
#include <stdlib.h>
#include <unistd.h>

int
parse_config_file (struct PidgetConfigs *pidget_configs)
{
  /* TODO: Find a better way of doing this, doing this for now
   * since libCYAML is freeing this memory automatically
   */
  struct PidgetConfigs *temp_configs;
  temp_configs = malloc (sizeof (struct PidgetConfigs));
  temp_configs->file_name = pidget_configs->file_name;

  /* CYAML value schema for entries of the data sequence. */
  static const cyaml_schema_value_t data_entry = {
    CYAML_VALUE_STRING (CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
  };

  /* CYAML mapping schema fields array for the top level mapping. */
  static const cyaml_schema_field_t top_mapping_schema[] = {
    CYAML_FIELD_FLOAT ("gravity", CYAML_FLAG_DEFAULT, struct PidgetConfigs,
                       gravity),
    CYAML_FIELD_INT ("num_pidgets", CYAML_FLAG_OPTIONAL, struct PidgetConfigs,
                     num_pidgets),
    CYAML_FIELD_STRING_PTR ("color", CYAML_FLAG_OPTIONAL, struct PidgetConfigs,
                            color, 0, CYAML_UNLIMITED),
    CYAML_FIELD_STRING_PTR ("image_path", CYAML_FLAG_OPTIONAL,
                            struct PidgetConfigs, image_path, 0,
                            MAX_FILEPATH_LENGTH),
    CYAML_FIELD_SEQUENCE ("images", CYAML_FLAG_POINTER, struct PidgetConfigs,
                          images, &data_entry, 0, MAX_FILENAME_LENGTH),
    CYAML_FIELD_END
  };

  /* CYAML value schema for the top level mapping. */
  static const cyaml_schema_value_t top_schema = {
    CYAML_VALUE_MAPPING (CYAML_FLAG_POINTER, struct PidgetConfigs,
                         top_mapping_schema),
  };

  static const cyaml_config_t cyaml_config = {
    .log_fn = cyaml_log,            /* Use the default logging function. */
    .mem_fn = cyaml_mem,            /* Use the default memory allocator. */
    .log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
  };

  cyaml_err_t err;
  err = cyaml_load_file (temp_configs->file_name, &cyaml_config, &top_schema,
                         (cyaml_data_t **)&temp_configs, NULL);
  if (err != CYAML_OK)
    {
      log_message (3, "ERROR: %s\n", cyaml_strerror (err));
      return 1;
    }

  if (!temp_configs->color)
    {
      log_message (2, "No color provided, using default.\n");
      temp_configs->color = "default";
    }

  if (!temp_configs->image_path)
    {
      log_message (2, "No image path provided, using default.\n");
      temp_configs->image_path = DEFAULT_IMAGE_PATH;
    }

  if (!temp_configs->num_pidgets || temp_configs->num_pidgets > 10)
    {
      log_message (2, "Warning: invalid num_pidgets: %d, defaulting to 1\n",
                   temp_configs->num_pidgets);
      temp_configs->num_pidgets = 1;
    }

  log_message (1, "Using images:\n");
  for (int i = 0; i < temp_configs->images_count; i++)
    {
      log_message (1, "%s\n", temp_configs->images[i]);
    }

  *pidget_configs = *temp_configs;

  /* Free the data */
  cyaml_free (&cyaml_config, &top_schema, NULL, 0);
  free (temp_configs);

  return 0;
}
