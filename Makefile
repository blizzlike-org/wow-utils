ifndef LUA
LUA = lua54
endif

CFLAGS += -fpic -I./src/dbc/include
prefix ?= /usr
bindir ?= ${prefix}/bin
interpreter ?= /usr/bin/env lua$(shell pkg-config --variable=V ${LUA})

all: dbc.so dbccat mkunitfieldbyte0

dbc.o:
	${CC} -c ./src/dbc/dbc.c ${CFLAGS}

# lua bindings

dbc.so: dbc.o
	${CC} -o ./luadbc.o -c ./src/dbc/lua/dbc.c ${CFLAGS} $(shell pkg-config --cflags ${LUA})
	${CC} -o ./dbc.so ./luadbc.o ./dbc.o ${CFLAGS} -shared ${LDFLAGS} $(shell pkg-config --libs ${LUA})

dbccat: dbc.so
	install -D -m 0755 src/dbc/dbccat.lua dbccat
	sed -i "s_/usr/bin/env lua_${interpreter}_" dbccat

mkunitfieldbyte0:
	${CC} -o ./mkunitfieldbyte0 ./src/mkunitfieldbyte0.c

install:
	install -D dbc.so ${DESTDIR}$(shell pkg-config --variable=INSTALL_CMOD ${LUA})/dbc.so
	install -D -m 0755 dbccat ${DESTDIR}${bindir}/dbccat
	install -D -t ${DESTDIR}$(shell pkg-config --variable=INSTALL_LMOD ${LUA})/dbcspec dbcspec/*.lua

clean:
	rm -rf ./*.o ./*.so ./dbccat ./mkunitfieldbyte0
