#include <stdio.h>
#include <string.h>

#include "dbc.h"

int read_dbc_header(FILE *fd, dbc_file_t *dbc);

int read_dbc(char *file, dbc_file_t *dbc) {
  FILE *fd = fopen(file, "r");

  if(!fd)
    return -1;

  if(read_dbc_header(fd, dbc) != 0) {
    fclose(fd);
    return -2;
  }

  fclose(fd);
  return 0;
}

int read_dbc_header(FILE *fd, dbc_file_t *dbc) {
  char header[DBC_HEADER_SIZE];
  char *p = &header[0];
  memset(header, 0, DBC_HEADER_SIZE);

  if(fread(header, 1, DBC_HEADER_SIZE, fd) != DBC_HEADER_SIZE) {
    return -1;
  }

  if(strncmp(header, "WDBC", 4) != 0) {
    return -2;
  }

  memcpy(
    &dbc->header.signature, p,
    DBC_HEADER_SIGNATURE_SIZE);
  p += DBC_HEADER_SIGNATURE_SIZE;
  memcpy(
    &dbc->header.rcount, p,
    DBC_HEADER_RCOUNT_SIZE);
  p += DBC_HEADER_RCOUNT_SIZE;
  memcpy(
    &dbc->header.fcount, p,
    DBC_HEADER_FCOUNT_SIZE);
  p += DBC_HEADER_RSIZE_SIZE;
  memcpy(
    &dbc->header.rsize, p,
    DBC_HEADER_RSIZE_SIZE);
  p += DBC_HEADER_SSIZE_SIZE;
  memcpy(
    &dbc->header.ssize, p,
    DBC_HEADER_SSIZE_SIZE);

  return 0;
}
