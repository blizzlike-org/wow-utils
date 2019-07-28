#!/usr/bin/env lua

local dbc = require("dbc")

local dbcfile, err = dbc.open(arg[1])
if err then
  print(err)
  os.exit(1)
end

local signature, rcount, fcount, rsize, ssize = dbcfile:get_header()
print(string.format(
  "signature: %s, records: %d per %d byte, fields: %d, stringblock: %d",
  signature, rcount, rsize, fcount, ssize))
dbcfile:close()
