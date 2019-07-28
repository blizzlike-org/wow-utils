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

struct dbc_header_t {
  char signature[5];  // always 'WDBC'
  uint32_t rcount; // record count
  uint32_t fcount; // field count
  uint32_t rsize;  // record size
  uint32_t ssize;  // string block size
};

typedef struct {
  dbc_header_t header;
} dbc_file_t;

int read_dbc(char *file, dbc_file_t* dbc);

#endif
