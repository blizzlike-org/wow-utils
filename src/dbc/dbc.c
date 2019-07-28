#include <stdio.h>
#include <string.h>

#include "dbc.h"

int dbc_read_header(dbc_file_t *dbc);

int dbc_open(char *file, dbc_file_t *dbc) {
  dbc->fd = fopen(file, "r");

  if(!dbc->fd)
    return -1;

  if(dbc_read_header(dbc) != 0) {
    fclose(dbc->fd);
    return -2;
  }

  fclose(dbc->fd);
  return 0;
}

int dbc_read_header(dbc_file_t *dbc) {
  char header[DBC_HEADER_SIZE];
  char *p = &header[0];
  memset(header, 0, DBC_HEADER_SIZE);

  if(fread(header, 1, DBC_HEADER_SIZE, dbc->fd) != DBC_HEADER_SIZE) {
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
