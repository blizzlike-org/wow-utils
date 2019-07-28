#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#include "dbc.h"

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

int main(int argc, char *argv[]) {
  config_t config;
  dbc_file_t dbc;

  if(parse_args(argc, argv, &config) != 0) {
    fprintf(stdout, "USAGE: %s <dbc-file>\n", argv[0]);
    return 1;
  }

  fprintf(stdout, "Using %s\n", basename(config.dbc_file));
  memset(&dbc, 0, sizeof(dbc_file_t));
  read_dbc(config.dbc_file, &dbc);
  fprintf(
    stdout, "%s: record count %d, field count %d, record size %d, string size %d\n",
    dbc.header.signature,
    dbc.header.rcount,
    dbc.header.fcount,
    dbc.header.rsize,
    dbc.header.ssize);

  return 0;
}
