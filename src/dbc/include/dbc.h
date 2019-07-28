#ifndef _DBC_H
#define _DBC_H

#include <stdint.h>

#define DBC_HEADER_SIZE sizeof(char) * 4 + 16
#define DBC_HEADER_SIGNATURE_SIZE sizeof(char) * 4
#define DBC_HEADER_RCOUNT_SIZE sizeof(uint32_t)
#define DBC_HEADER_FCOUNT_SIZE sizeof(uint32_t)
#define DBC_HEADER_RSIZE_SIZE sizeof(uint32_t)
#define DBC_HEADER_SSIZE_SIZE sizeof(uint32_t)

typedef struct dbc_header_t dbc_header_t;
typedef struct dbc_record_t dbc_record_t;

struct dbc_header_t {
  char signature[5];  // always 'WDBC'
  uint32_t rcount; // record count
  uint32_t fcount; // field count
  uint32_t rsize;  // record size
  uint32_t ssize;  // string block size
};

typedef struct {
  FILE *fd;
  dbc_header_t header;
} dbc_file_t;

void dbc_close(dbc_file_t *dbc);
int dbc_open(dbc_file_t *dbc, const unsigned char *file);

int dbc_read_int(dbc_file_t *dbc, unsigned char *record, int32_t *field);
int dbc_read_uint(dbc_file_t *dbc, unsigned char *record, uint32_t *field);
int dbc_read_record(dbc_file_t *dbc, int (*callback)(dbc_file_t *dbc, unsigned char *record));

#endif
