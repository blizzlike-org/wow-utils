#ifndef _DBC_TYPES_H
#define _DBC_TYPES_H

#include <stdint.h>

#include "dbc.h"

enum {
  DBC_TYPE_GENERIC = 0,
  DBC_TYPE_FACTION = 1
};

uint32_t dbctypes_discover(char *filename);

int dbctypes_parse_faction(dbc_file_t *dbc, unsigned char *record);

#endif
