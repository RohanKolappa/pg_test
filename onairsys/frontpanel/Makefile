CC=gcc

LCDOBJS=633_linux/633_packet.o 633_linux/show_packet.o 633_linux/serial.o

all:  frontpanel burn_lcd


frontpanel:frontpanel.o ${LCDOBJS}
	${CC} frontpanel.o ${LCDOBJS} -o $@ -Lsupermicro_lcd -lsmc

frontpanel.o: frontpanel.c
	${CC} -Wall -c -o $@ frontpanel.c

burn_lcd: burn_lcd.c ${LCDOBJS}
	${CC} -Wall  -o $@ ${LCDOBJS} burn_lcd.c

${LCDOBJS}:
	${MAKE} -C 633_linux

clean:
	${RM} frontpanel.o  frontpanel burn_lcd
	${MAKE} -C 633_linux clean

