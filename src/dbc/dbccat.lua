#!/usr/bin/env lua

local dbc = require("dbc")

local _M = {}

function _M.parse_args(self)
  self.args.dbcs = {}
  for _, v in pairs(arg) do
    if v:len() > 4 and v:match("^.+(%..+)$") == ".dbc" then
      table.insert(self.args.dbcs, v)
    end
    if v == "-x" or v == "--hex" then
      self.args.raw = true
    end
    if v == "-v" or v == "--verbose" then
      self.args.verbose = true
    end
  end
end

function _M.parse_record(self, record, header)
  local res = ""
  local i = 0
  repeat
    local field, err
    if self.args.raw then
      field, err = record:get_raw()
    else
      field, err = record:get_int()
      field = string.format("%i", field)
    end
    if err then field = err end
    if res:len() == 0 then
      res = field
    else
      res = string.format("%s,%s", res, field)
    end
    i = i + 1
  until i == header.fcount
  return res
end

function _M.parse_record_blueprint(self, record, header, blueprint)
  local res = ""

  for _, v in pairs(blueprint) do
    local i = 0
    while i < v.count do
      local field, err
      if v.type == "int32_t" then
        field, err = record:get_int()
        field = string.format("%i", field)
      end
      if v.type == "string" then
        field, err = record:get_string()
        field = string.format("\"%s\"", field)
      end
      if v.type == "uint32_t" then
        field, err = record:get_uint()
        field = string.format("%u", field)
      end
      if err then field = err end
      if res:len() == 0 then
        res = field
      else
        res = string.format("%s,%s", res, field)
      end
      i = i + 1
    end
  end

  return res
end

function _M.print_headline(self, blueprint)
  local headline = ""
  for _, v in pairs(blueprint) do
    local i = 0
    while i < v.count do
      local title = v.name
      if v.count > 1 then
        title = string.format("%s%d", v.name, i)
      end
      if headline:len() == 0 then
        headline = tostring(title)
      else
        headline = string.format("%s,%s", headline, tostring(title))
      end
      i = i + 1
    end
  end
  print(headline)
end

function _M.read_dbc(self, file)
  if self.args.verbose then
    print(string.format("[I] reading %s", file))
  end

  local dbcfile, err = dbc.open(file)
  if err then
    print(err)
    os.exit(1)
  end

  local signature, rcount, fcount, rsize, ssize = dbcfile:get_header()
  local header = {
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

  local specname = file:match("^.+/(.+).dbc$"):lower()
  if self.args.verbose then
    print(string.format("[I] Loading specfile %s", specname))
  end
  local modload, spec = pcall(require, "dbcspec." .. specname)
  local blueprint
  if modload then
    for k, v in pairs(spec) do
      if v.rcount == rcount and v.fcount == fcount and v.rsize == rsize then
        if self.args.verbose then
          print(string.format("[I] Using spec for client build %d", k))
        end
        blueprint = v.fields
      end
    end
  end

  if blueprint then
    self:print_headline(blueprint)
  end

  local i = 0
  repeat
    local record, err = dbcfile:get_record()
    if err then
      print(string.format("[E] %s", err))
    else
      if self.args.raw or not blueprint then
        print(self:parse_record(record, header))
      else
        print(self:parse_record_blueprint(record, header, blueprint))
      end
    end
    i = i + 1
  until i == rcount

  dbcfile:close()
end

function _M.run(self)
  self.args = {}
  self:parse_args()

  for _, v in pairs(self.args.dbcs) do
    self:read_dbc(v)
  end
end

_M:run()
