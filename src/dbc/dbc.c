#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbc.h"

int dbc_read_header(dbc_file_t *dbc);

void dbc_close(dbc_file_t *dbc) {
  fclose(dbc->fd);
}

int dbc_open(dbc_file_t *dbc, const unsigned char *file) {
  dbc->fd = fopen(file, "r");

  if(!dbc->fd)
    return -1;

  if(dbc_read_header(dbc) != 0) {
    fclose(dbc->fd);
    return -2;
  }

  dbc->_iter_r = 0;

  return 0;
}

int dbc_read_header(dbc_file_t *dbc) {
  unsigned char header[DBC_HEADER_SIZE];
  unsigned char *p = &header[0];
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

int dbc_read_int(dbc_file_t *dbc, dbc_record_t *record, int32_t *field) {
  if((dbc->header.rsize - record->_offset) < sizeof(int32_t))
    return -1;

  memcpy(field, record->p + record->_offset, sizeof(int32_t));
  record->_offset += sizeof(int32_t);
  return 0;
}

int dbc_read_record(dbc_file_t *dbc, dbc_record_t *record) {
  if(dbc->_iter_r >=  dbc->header.rcount)
    return -1;

  memset(record, 0, dbc_sizeof_record(dbc));
  if(fread(record->payload, 1, dbc->header.rsize, dbc->fd) != dbc->header.rsize)
    return -2;

  dbc->_iter_r += 1;
  record->_offset = 0;
  record->p = &record->payload[0];

  return 0;
}

int dbc_read_uint(dbc_file_t *dbc, dbc_record_t *record, uint32_t *field) {
  if((dbc->header.rsize - record->_offset) < sizeof(uint32_t))
    return -1;

  memcpy(field, record->p + record->_offset, sizeof(uint32_t));
  record->_offset += sizeof(uint32_t);
  return 0;
}

uint32_t dbc_sizeof_record(dbc_file_t *dbc) {
  return sizeof(dbc_record_t) + dbc->header.rsize;
}
