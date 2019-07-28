#include <stdio.h>
#include <string.h>

#include "dbc.h"
#include "dbc/types.h"

uint32_t dbctypes_discover(char *filename) {
  uint32_t type_l = strlen(filename) - 4;

  if(strncmp(filename, "Faction", type_l) == 0)
    return DBC_TYPE_FACTION;

  return -1;
}

int dbctypes_parse_faction(dbc_file_t *dbc, unsigned char *record) {
  dbc_record_faction_t fields;
  memset(&fields, 0, sizeof(dbc_record_faction_t));

  dbc_read_uint(dbc, record, &fields.id);
  record += sizeof(uint32_t);
  dbc_read_int(dbc, record, &fields.reputation_index);
  record += sizeof(int32_t);
  fprintf(stdout, "id: %u, repindex: %d\n", fields.id, fields.reputation_index);
  return 0;
}
