#!/usr/bin/env lua

local dbc = require("dbc")

local _M = {}

function _M.parse_record(self, record)
  local res = ""
  local i = 0
  repeat
    local field, err
    if self.args.raw then
      field, err = record:get_raw()
    else
      field, err = record:get_int()
    end
    if err then return err end
    if res:len() == 0 then
      res = tostring(field)
    else
      res = string.format("%s,%s", res, tostring(field))
    end
    i = i + 1
  until i == self.header.fcount
  return res
end

function _M.run(self)
  local err = nil
  self.args = {}
  self.dbc, err = dbc.open(arg[1])
  if err then
    print("No such file or directory")
    os.exit(1)
  end

  local i = 1
  while i <= #arg do
    if arg[i] == "raw" then
      self.args.raw = true
    end
    if arg[i] == "-v" then
      self.args.verbose = true
    end
    i = i + 1
  end

  local signature, rcount, fcount, rsize, ssize = self.dbc:get_header()
  self.header = {
    signature = signature,
    rcount = rcount,
    fcount = fcount,
    rsize = rsize,
    ssize = ssize
  }
  if self.args.verbose then
    print(string.format("> dbc file header"))
    print(string.format("signature: %s", signature))
    print(string.format("results: %s", rcount))
    print(string.format("fields/result: %s", fcount))
    print(string.format("result size: %s", rsize))
    print(string.format("stringblock size: %s", ssize))
  end

  local i = 0
  repeat
    local record, err = self.dbc:get_record()
    if err then
      print(string.format("[E] %s", err))
    end
    print(self:parse_record(record))
    i = i + 1
  until i == rcount

  self.dbc:close()
end

_M:run()
