#!/usr/bin/env lua5.1

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

local dbcrecord = nil
repeat
  dbcrecord, err = dbcfile:get_record()
  if err then
    print(err)
  else
    print(dbcrecord:get_uint())
  end
until dbcrecord == nil

dbcfile:close()
