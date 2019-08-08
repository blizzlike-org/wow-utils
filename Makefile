ifndef LUA
LUA = lua53
endif

CFLAGS += -fpic -I./src/dbc/include
prefix ?= /usr
bindir ?= ${prefix}/bin
interpreter ?= /usr/bin/env lua$(shell pkg-config --variable=V ${LUA})

all: dbc.so

dbc.o:
	${CC} -c ./src/dbc/dbc.c ${CFLAGS}

# lua bindings

dbc.so: dbc.o
	${CC} -o ./luadbc.o -c ./src/dbc/lua/dbc.c ${CFLAGS} $(shell pkg-config --cflags ${LUA})
	${CC} -o ./dbc.so ./luadbc.o ./dbc.o ${CFLAGS} -shared ${LDFLAGS} $(shell pkg-config --libs ${LUA})

install:
	install -D dbc.so ${DESTDIR}$(shell pkg-config --variable=INSTALL_CMOD ${LUA})/dbc.so
	install -D -m 0755 src/dbc/dbccat.lua ${DESTDIR}${bindir}/dbccat
	sed -i "s_/usr/bin/env lua_${interpreter}_" ${DESTDIR}${bindir}/dbccat
	install -D dbcspec/faction.dbc ${DESTDIR}$(shell pkg-config --variable=INSTALL_LMOD ${LUA})/dbcspec/faction.lua

clean:
	rm -rf ./*.o ./*.so
