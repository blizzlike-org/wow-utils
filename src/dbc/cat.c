#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "dbc.h"
#include "dbc/types.h"

typedef struct {
  char *dbc_file;
} config_t;

int parse_args(int argc, char *argv[], config_t *config) {
  if(argc != 2)
    return -1;

  if(access(argv[1], R_OK) != 0)
    return -2;

  config->dbc_file = argv[1];
  return 0;
}

int parse_dbc(dbc_file_t *dbc, uint32_t type) {
  int i;
  for(i = 0; i < dbc->header.rcount; ++i) {
    if(dbc_read_record(dbc, dbctypes_parse_faction) != 0)
      return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  char *filename;
  uint32_t type = 0;
  config_t config;
  dbc_file_t dbc;

  if(parse_args(argc, argv, &config) != 0) {
    fprintf(stdout, "USAGE: %s <dbc-file>\n", argv[0]);
    return 1;
  }

  filename = basename(config.dbc_file);
  type = dbctypes_discover(filename);

  fprintf(stdout, "Using %s\n", filename);
  memset(&dbc, 0, sizeof(dbc_file_t));
  if(dbc_open(config.dbc_file, &dbc) != 0)
    return 2;

  fprintf(
    stdout, "%s: record count %d, field count %d, record size %d, string size %d\n",
    dbc.header.signature,
    dbc.header.rcount,
    dbc.header.fcount,
    dbc.header.rsize,
    dbc.header.ssize);
  parse_dbc(&dbc, type);
  dbc_close(&dbc);

  return 0;
}
