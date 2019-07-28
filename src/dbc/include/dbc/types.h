#ifndef _DBC_TYPES_H
#define _DBC_TYPES_H

#include <stdint.h>

#include "dbc.h"

enum {
  DBC_TYPE_GENERIC = 0,
  DBC_TYPE_FACTION = 1
};

typedef struct {
  uint32_t id;
  int32_t reputation_index;
  uint32_t reputation_race_mask[4];
  uint32_t reputation_class_mask[4];
  int32_t reputation_base[4];
  uint32_t reputation_flags[4];
  uint32_t parent_faction_id;
} dbc_record_faction_t;

uint32_t dbctypes_discover(char *filename);

int dbctypes_parse_faction(dbc_file_t *dbc, unsigned char *record);

#endif
