#include "config_parser.h"
#include "image_proc.h"
#include "logger.h"
#include <cyaml/cyaml.h>
#include <stdlib.h>
#include <unistd.h>

int
parse_config_file (struct PidgetConfigs *configs)
{
  /* TODO: Find a better way of doing this, doing this for now
   * since libCYAML is freeing this memory automatically
   */
  struct PidgetConfigs *temp_configs;
  temp_configs = malloc (sizeof (struct PidgetConfigs));
  temp_configs->file_name = configs->file_name;

  /* CYAML value schema for entries of the data sequence. */
  static const cyaml_schema_value_t data_entry = {
    CYAML_VALUE_STRING (CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
  };

  /* CYAML mapping schema fields array for the top level mapping. */
  static const cyaml_schema_field_t top_mapping_schema[]
      = { CYAML_FIELD_FLOAT ("gravity", CYAML_FLAG_DEFAULT,
                             struct PidgetConfigs, gravity),
          CYAML_FIELD_SEQUENCE ("images", CYAML_FLAG_POINTER,
                                struct PidgetConfigs, images, &data_entry, 0,
                                CYAML_UNLIMITED),
          CYAML_FIELD_END };

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

  *configs = *temp_configs;

  /* Free the data */
  cyaml_free (&cyaml_config, &top_schema, NULL, 0);
  free (temp_configs);

  return 0;
}
