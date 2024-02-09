#
# Top level Makefile for V2D build for DMS 3.0

ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_LIB = @echo "LIB $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET_DOC = @echo "DOC $@";
QUIET = @ 
endif

all: modules

modules:
	${MAKE} -C $@


DOCUMENTS = fpga_driver.txt \
            device_list.txt \
            board.txt \
            hald.txt \
            keyboard_mouse.txt \
            hal.txt \
            clidoc.txt \

DOC_SRCS = ${patsubst %.txt, docs/%.txt, ${DOCUMENTS}}
DOC_HTML = $(DOC_SRCS:.txt=.html)

${DOC_HTML}: %.html: %.txt
	${QUIET_DOC} asciidoc -a max-width=55em -a toc -a toclevels=3 $^
	${QUIET} a2x -a toc -a toclevels=3 --fop $^

docs: ${DOC_HTML}

ifdef DOCSINSTALLBASEDIR
DOCS_INSTALL_BASEDIR = ${DOCSINSTALLBASEDIR}
else
DOCS_INSTALL_BASEDIR = ${HOME}/public_html/docs
endif

DOCS_INSTALL_DIR = ${DOCS_INSTALL_BASEDIR}/V2O

docsinstall: docs
	${QUIET} echo "INSTALLDOCS"
	${QUIET} mkdir -p ${DOCS_INSTALL_DIR} ${DOCS_INSTALL_DIR}/images
	${QUIET} /bin/cp -f docs/*.txt docs/*.pdf docs/*.html ${DOCS_INSTALL_DIR}
	${QUIET} for file in `/bin/ls docs/images/*.png 2>/dev/null`; \
		do /bin/cp  $${file} ${DOCS_INSTALL_DIR}/images; done

test: 
	${MAKE} -C modules test

clean: docsclean
	${MAKE} -C modules clean
	rm -f docs/*.html docs/*.pdf

docsclean:
	${QUIET} /bin/rm -f docs/*.html docs/*.pdf docs/images/gen_*.png


.PHONY:	modules clean docs docsinstall docsclean 
