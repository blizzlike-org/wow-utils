#include <lua.h>
#include <lauxlib.h>

#include <string.h>

#include "dbc.h"

#if LUA_VERSION_NUM < 502
  #define luaL_newlib(L, m) \
    (lua_newtable(L), luaL_register(L, NULL, m))

// adapted from lua-users.org
static void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for(; l->name != NULL; l++) {
    int i;
    lua_pushstring(L, l->name);
    for(i = 0; i < nup; i++)
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);
}
#endif

typedef struct {
  dbc_file_t dbc;
  const unsigned char *dbcfile;
} ldbc_userdata_t;

typedef struct {
  dbc_file_t *dbc;
  dbc_record_t record;
} ldbc_record_userdata_t;

typedef struct {
  dbc_file_t *dbc;
  dbc_stringblock_t stringblock;
} ldbc_stringblock_userdata_t;

static int ldbc_get_int(lua_State *L);
static int ldbc_get_uint(lua_State *L);

static int ldbc_close(lua_State *L) {
  ldbc_userdata_t *udata = (ldbc_userdata_t *) luaL_checkudata(L, 1, "dbc");
  dbc_close(&udata->dbc);
  return 0;
}

static int ldbc_get_float(lua_State *L) {
  ldbc_record_userdata_t *udata =
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  float field = 0;
  if(dbc_read_float(udata->dbc, &udata->record, &field) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read float from record");
    return 2;
  }

  lua_pushnumber(L, field);
  return 1;
}

static int ldbc_get_header(lua_State *L) {
  ldbc_userdata_t *udata = (ldbc_userdata_t *) luaL_checkudata(L, 1, "dbc");
  lua_pushstring(L, udata->dbc.header.signature);
  lua_pushnumber(L, udata->dbc.header.rcount);
  lua_pushnumber(L, udata->dbc.header.fcount);
  lua_pushnumber(L, udata->dbc.header.rsize);
  lua_pushnumber(L, udata->dbc.header.ssize);
  return 5;
}

static int ldbc_get_int32(lua_State *L) {
  ldbc_record_userdata_t *udata =
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  int32_t field = 0;
  if(dbc_read_int32(udata->dbc, &udata->record, &field) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read int32 from record");
    return 2;
  }

  lua_pushnumber(L, field);
  return 1;
}

static int ldbc_get_raw(lua_State *L) {
  ldbc_record_userdata_t *udata = 
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  uint32_t l = luaL_checknumber(L, 2) * sizeof(unsigned char);
  uint32_t lhex = l * 2 + (sizeof(unsigned char));
  unsigned char field[l + 1], hex[lhex];
  int i;
  if(dbc_read_raw(udata->dbc, &udata->record, &field[0], l) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read raw values from record");
    return 2;
  }

  memset(&hex[0], 0, lhex);
  for(i = 0; i < l; ++i) {
    sprintf(hex + 2 * i, "%02x", field[i]);
  }

  lua_pushstring(L, hex);
  return 1;
}

static int ldbc_get_record(lua_State *L) {
  ldbc_userdata_t *udata = (ldbc_userdata_t *) luaL_checkudata(L, 1, "dbc");
  ldbc_record_userdata_t *udata_r =
    (ldbc_record_userdata_t *) lua_newuserdata(L, sizeof(ldbc_record_userdata_t) + udata->dbc.header.rsize);

  switch(dbc_read_record(&udata->dbc, &udata_r->record)) {
    case -3:
      lua_pushnil(L);
      lua_pushstring(L, "cannot read record from dbc");
      return 2;
    case -2:
      lua_pushnil(L);
      lua_pushstring(L, "no records left");
      return 2;
    case -1:
      lua_pushnil(L);
      lua_pushstring(L, "invalid file handle");
      return 2;
  }

  udata_r->dbc = &udata->dbc;

  luaL_getmetatable(L, "dbcrecord");
  lua_setmetatable(L, -2);

  return 1;
}

static int ldbc_get_next_string(lua_State *L) {
  ldbc_stringblock_userdata_t *udata =
    (ldbc_stringblock_userdata_t *) luaL_checkudata(L, 1, "dbcstringblock");
  uint32_t l = dbc_sizeof_string(&udata->stringblock);
  unsigned char field[l];
  if(dbc_read_next_string(udata->dbc, &udata->stringblock, &field[0]) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read string from stringblock");
    return 2;
  }

  lua_pushstring(L, field);
  return 1;
}

static int ldbc_get_string(lua_State *L) {
  ldbc_record_userdata_t *udata =
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  uint32_t offset = 0;
  uint32_t l = 0;
  if(dbc_read_uint32(udata->dbc, &udata->record, &offset) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read string offset from record");
    return 2;
  }

  l = dbc_strlen(udata->dbc, offset);
  unsigned char field[l + 1];
  if(dbc_read_string(udata->dbc, offset, &field[0], l + 1) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read string");
    return 2;
  }

  lua_pushstring(L, field);
  return 1;
}

static int ldbc_get_stringblock(lua_State *L) {
  ldbc_userdata_t *udata = (ldbc_userdata_t *) luaL_checkudata(L, 1, "dbc");
  ldbc_stringblock_userdata_t *udata_s =
    (ldbc_stringblock_userdata_t *) lua_newuserdata(L,
      sizeof(ldbc_stringblock_userdata_t) + udata->dbc.header.ssize);

  switch(dbc_read_stringblock(&udata->dbc, &udata_s->stringblock)) {
    case -2:
      lua_pushnil(L);
      lua_pushstring(L, "cannot read stringblock from dbc");
      return 2;
    case -1:
      lua_pushnil(L);
      lua_pushstring(L, "no string left");
      return 2;
  }

  udata_s->dbc = &udata->dbc;

  luaL_getmetatable(L, "dbcstringblock");
  lua_setmetatable(L, -2);

  return 1;
}

static int ldbc_get_uint8(lua_State *L) {
  ldbc_record_userdata_t *udata =
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  uint8_t field = 0;
  if(dbc_read_uint8(udata->dbc, &udata->record, &field) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read uint8 from record");
    return 2;
  }

  lua_pushnumber(L, field);
  return 1;
}

static int ldbc_get_uint32(lua_State *L) {
  ldbc_record_userdata_t *udata =
    (ldbc_record_userdata_t *) luaL_checkudata(L, 1, "dbcrecord");
  uint32_t field = 0;
  if(dbc_read_uint32(udata->dbc, &udata->record, &field) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "cannot read uint32 from record");
    return 2;
  }

  lua_pushnumber(L, field);
  return 1;
}

static int ldbc_open(lua_State *L) {
  ldbc_userdata_t *udata = (ldbc_userdata_t *) lua_newuserdata(L, sizeof(ldbc_userdata_t));
  memset(&udata->dbc, 0, sizeof(dbc_file_t));
  udata->dbcfile = luaL_checkstring(L, 1);

  if(dbc_open(&udata->dbc, udata->dbcfile) != 0) {
    lua_pushnil(L);
    lua_pushstring(L, "no such file or directory");
    return 2;
  }

  luaL_getmetatable(L, "dbc");
  lua_setmetatable(L, -2);

  return 1;
}

static const struct luaL_Reg ldbc[] = {
  { "open", ldbc_open },
  { NULL, NULL }
};

int luaopen_dbc(lua_State *L) {
  if(luaL_newmetatable(L, "dbc")) {
    static struct luaL_Reg dbc_methods[] = {
      { "__gc", ldbc_close },
      { "close", ldbc_close },
      { "get_header", ldbc_get_header },
      { "get_record", ldbc_get_record },
      { "get_stringblock", ldbc_get_stringblock },
      { NULL, NULL }
    };
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, dbc_methods, 0);
  }

  if(luaL_newmetatable(L, "dbcrecord")) {
    static struct luaL_Reg dbcrecord_methods[] = {
      { "get_float", ldbc_get_float },
      { "get_int32", ldbc_get_int32 },
      { "get_raw", ldbc_get_raw },
      { "get_string", ldbc_get_string },
      { "get_uint8", ldbc_get_uint8 },
      { "get_uint32", ldbc_get_uint32 },
      { NULL, NULL }
    };
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, dbcrecord_methods, 0);
  }

  if(luaL_newmetatable(L, "dbcstringblock")) {
    static struct luaL_Reg dbcstringblock_methods[] = {
      { "get_string", ldbc_get_next_string },
      { NULL, NULL }
    };
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, dbcstringblock_methods, 0);
  }

  luaL_newlib(L, ldbc);
  return 1;
}
