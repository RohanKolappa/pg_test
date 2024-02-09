#
# Makefile for compiling mediaconvertor tool
#

CONVERTERLIBDIR = libmediaconverter
CONVERTEREXEDIR = mediaconverter

all: mediaconverterexe

mediaconverterexe:
	$(MAKE) -C ${CONVERTEREXEDIR}

mediaconverterexe_clean:
	${MAKE} -C ${CONVERTEREXEDIR} clean

clean: mediaconverterexe_clean

.PHONY: mediaconverterexe clean  mediaconverterexe_clean
	
