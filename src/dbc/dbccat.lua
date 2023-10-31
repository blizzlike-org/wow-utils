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

function _M.load_blueprint(self, name, header)
  if self.args.verbose then
    print(string.format("[I] Loading specfile %s", name))
  end
  local modload, specs = pcall(require, "dbcspec/" .. name)

  local fields = {}
  local blueprint = { build = 0, precission = 0, fields = {} }
  if not modload then return blueprint end

  local fields = specs.fields
  specs.fields = nil

  for buildno, spec in pairs(specs) do
    local precission = 1
    if spec.rsize == header.rsize then precission = precission + 33 end
    if spec.rcount == header.rcount then precission = precission + 33 end
    if spec.ssize == header.ssize then precission = precission + 33 end
    if #spec.fields ~= header.fcount then precission = 0 end

    if precission > 1 and blueprint.precission < precission then
      blueprint.build = buildno
      blueprint.precission = precission
      for k, v in pairs(spec.fields) do
        for _, n in pairs(fields) do
          if v == n.name then
            blueprint.fields[k] = n
            if not blueprint.fields[k].count then blueprint.fields[k].count = 1 end
          end
        end
      end
    end
  end

  return blueprint
end

function _M.parse_record(self, record, header)
  local res = ""
  local i = 0
  repeat
    local field, err
    field, err = record:get_raw(1)
    if err then field = err end
    if res:len() == 0 then
      res = field
    else
      res = string.format("%s,%s", res, field)
    end
    i = i + 1
  until i == header.rsize
  return res
end

function _M.parse_record_blueprint(self, record, header, blueprint)
  local res = {}

  local i = 1
  for _, v in pairs(blueprint.fields) do
    local j = 1
    while j <= v.count do
      local field, err
      if v.type == "float" then
        field, err = record:get_float()
      end
      if v.type == "hex" then
        field, err = record:get_raw(v.size)
      end
      if v.type == "int32_t" then
        field, err = record:get_int32()
      end
      if v.type == "string" then
        field, err = record:get_string()
        field = string.format("\"%s\"", field)
      end
      if v.type == "uint8_t" then
        field, err = record:get_uint8()
      end
      if v.type == "uint32_t" then
        field, err = record:get_uint32()
      end
      if err then field = err end
      res[i] = field
      i = i + 1
      j = j + 1
    end
  end

  return res
end

function _M.print_headline(self, blueprint)
  local headline = ""
  for _, v in pairs(blueprint.fields) do
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

function _M.print_records(self, records)
  local separator = 0
  for k, v in pairs(records) do
    if separator == 1 then io.write(",") end
    io.write(v)
    if separator == 0 then separator = 1 end
  end
  io.write("\n")
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
    print(string.format(
      "[I] signature: %s, results: %u, fields: %u, rcount: %u, rsize: %u, ssize: %u",
      signature, rcount, fcount, rcount, rsize, ssize))
  end

  local specname = file:match("([^/]+)$"):match("(%w+).*"):lower()
  local blueprint = self:load_blueprint(specname, header)
  if blueprint.build > 0 then
    if self.args.verbose then
      print(string.format("[I] Using spec for client build %d (precission: %d)", blueprint.build, blueprint.precission))
    end
    self:print_headline(blueprint)
  end

  local i = 0
  repeat
    local record, err = dbcfile:get_record()
    if err then
      print(string.format("[E] %s", err))
    else
      if self.args.raw or blueprint.build == 0 then
        self:print_records(self:parse_record(record, header))
      else
        self:print_records(self:parse_record_blueprint(record, header, blueprint))
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
