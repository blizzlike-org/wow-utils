#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbc.h"

int dbc_read_header(dbc_file_t *dbc);

void dbc_close(dbc_file_t *dbc) {
  if(dbc->fd != NULL) {
    fclose(dbc->fd);
    dbc->fd = NULL;
  }
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

  if(dbc->fd == NULL)
    return -1;

  if(fread(header, 1, DBC_HEADER_SIZE, dbc->fd) != DBC_HEADER_SIZE)
    return -2;

  if(strncmp(header, "WDBC", 4) != 0)
    return -3;

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
  if(record->p == NULL)
    return -1;

  if((dbc->header.rsize - record->_offset) < sizeof(int32_t))
    return -2;

  memcpy(field, record->p + record->_offset, sizeof(int32_t));
  record->_offset += sizeof(int32_t);
  return 0;
}

int dbc_read_raw(dbc_file_t *dbc, dbc_record_t *record, unsigned char *field) {
  if(record->p == NULL)
    return -1;

  if((dbc->header.rsize - record->_offset) < sizeof(char) * 4)
    return -2;

  memset(field, 0, sizeof(char) * 5);
  memcpy(field, record->p + record->_offset, sizeof(char) * 4);
  record->_offset += sizeof(char) * 4;
  return 0;
}

int dbc_read_record(dbc_file_t *dbc, dbc_record_t *record) {
  if(dbc->fd == NULL)
    return -1;

  if(dbc->_iter_r >=  dbc->header.rcount)
    return -2;

  memset(record, 0, dbc_sizeof_record(dbc));
  if(fread(record->payload, 1, dbc->header.rsize, dbc->fd) != dbc->header.rsize)
    return -3;

  dbc->_iter_r += 1;
  record->_offset = 0;
  record->p = &record->payload[0];
  return 0;
}

int dbc_read_string(dbc_file_t *dbc, dbc_stringblock_t *stringblock, unsigned char *field) {
  int l = dbc_sizeof_string(stringblock);

  if(l > dbc->header.ssize)
    return -1;

  if(stringblock->_offset >= dbc->header.ssize)
    return -2;

  memcpy(field, stringblock->p + stringblock->_offset, l);
  stringblock->_offset += l;
  return 0;
}

int dbc_read_stringblock(dbc_file_t *dbc, dbc_stringblock_t *stringblock) {
  long int current = ftell(dbc->fd);
  int i, j;
  fseek(dbc->fd, DBC_HEADER_SIZE + dbc->header.rcount * dbc->header.rsize, SEEK_SET);

  memset(stringblock, 0, sizeof(dbc_stringblock_t) + dbc->header.ssize);
  if(fread(stringblock->payload, 1, dbc->header.ssize, dbc->fd) != dbc->header.ssize) {
    fseek(dbc->fd, current, SEEK_SET);
    return -1;
  }

  for(i = 0, j = 0; i <= dbc->header.ssize; ++i)
    if(stringblock->payload[i] == '\0')
      ++j;

  fseek(dbc->fd, current, SEEK_SET);
  stringblock->_offset = 0;
  stringblock->fcount = j;
  stringblock->p = &stringblock->payload[0];

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

uint32_t dbc_sizeof_string(dbc_stringblock_t *stringblock) {
  int i = stringblock->_offset;
  while(stringblock->payload[i] != '\0')
    ++i;

  return i + 1 - stringblock->_offset;
}

uint32_t dbc_sizeof_stringblock(dbc_file_t *dbc) {
  return sizeof(dbc_stringblock_t) + dbc->header.ssize;
}
