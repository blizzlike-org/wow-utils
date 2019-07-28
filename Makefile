CFLAGS += -fPIC

ifndef LUA
LUA = lua5.3
endif

INC += -I./src/dbc/include
ifdef LUA_INCDIR
INC += -I${LUA_INCDIR}
else
INC += -I/usr/include/${LUA}
endif

all: dbccat dbc.so

dbc.o:
	${CC} -c ./src/dbc/dbc.c ${CFLAGS} ${INC}

dbctypes.o:
	${CC} -c ./src/dbc/dbctypes.c -I./src/dbc/include

dbccat: dbc.o dbctypes.o
	${CC} -c ./src/dbc/cat.c -I./src/dbc/include
	${CC} -o ./dbccat ./cat.o ./dbc.o ./dbctypes.o ${LDFLAGS}

# lua bindings

dbc.so: dbc.o
	${CC} -o ./luadbc.o -c ./src/dbc/lua/dbc.c ${CFLAGS} ${INC}
	${CC} -o ./dbc.so ./luadbc.o ./dbc.o ${CFLAGS} ${INC} -shared ${LDFLAGS} -l${LUA}

clean:
	rm -rf ./*.o ./*.so ./dbccat
