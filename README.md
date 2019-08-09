# wow-utils

useful tools to inspect wow client files.

## dbccat

    dbccat <dbcfile> [<dbcfile>] [-x|--hex] [-v|--verbose]

### file format (header)

| signature | result count | field count | result size | stringblock size |
| --------- | ------------ | ----------- | ----------- | ---------------- |
| 4 byte    | 4 byte       | 4 byte      | 4 byte      | 4 byte           |

the signature is defined as **WDBC**

### file format (results)

| field  | field  | field  | ...    |
| ------ | ------ | ------ | ------ |
| 4 byte | 4 byte | 4 byte | 4 byte |

every result consists ususally of 4 byte large fields.
The result size as well as the number of results is defined in the header.

Known field types are:

* uint8_t
* uint32_t
* int8_t
* int32_t
* float (4 byte)
* uint32_t as offset relative to the beginning of the stringblock
* bitmask (4 byte)

> there is no definition in dbc that points out the field types.

localized strings usually are attached to each other.

| loc0      | loc1 | loc2 | loc3 | loc4 | loc5 | loc6 | loc7 | loc8 (>=TBC) |
| --------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ------------ |
| enUS/ruRU | koKR | frFR | deDE | zhCN | zhTW | esES | esMX | ruRU         |

> the developer of the ruRU client files for vanilla chose loc0.

### file format (stringblock)

the stringblock is a large block containing `\0` delimeted strings.
It starts with a `\0` even if there are no strings stored in a dbc file.

# credits

* [cmangos](https://github.com/cmangos/issues/wiki/Dbc-files)
* [getmangos](https://mangoszero-docs.readthedocs.io/en/latest/file-formats/dbc/index.html)
* [shagu](https://gitlab.com/shagu/pfQuest/blob/master/toolbox/load-client-data.sh)
