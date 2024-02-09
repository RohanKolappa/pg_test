CC=gcc


all: reset oacrypt

oacrypt: oacrypt.c
	${CC} -lcrypt -o oacrypt oacrypt.c

reset: reset.c
	${CC} -o reset reset.c

clean:
	${RM} *.o reset oacrypt
