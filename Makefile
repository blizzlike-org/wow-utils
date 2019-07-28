all: dbccat

dbc.o:
	${CC} -c ./src/dbc/dbc.c -I./src/dbc/include

dbctypes.o:
	${CC} -c ./src/dbc/dbctypes.c -I./src/dbc/include

dbccat: dbc.o dbctypes.o
	${CC} -c ./src/dbc/cat.c -I./src/dbc/include
	${CC} -o ./dbccat ./cat.o ./dbc.o ./dbctypes.o ${LDFLAGS}

clean:
	rm -rf ./*.o ./dbccat
