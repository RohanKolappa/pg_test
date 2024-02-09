
ifneq ("$(CROSS_COMPILE)", "")
	ASM     := $(CROSS_COMPILE)as
	LD      := $(CROSS_COMPILE)ld
	CC      := $(CROSS_COMPILE)gcc
	CPP     := $(CROSS_COMPILE)c++
	AR      := $(CROSS_COMPILE)ar
	STRIP   := $(CROSS_COMPILE)strip
	OBJCOPY := $(CROSS_COMPILE)objcopy
	OBJDUMP := $(CROSS_COMPILE)objdump
	RANLIB  := $(CROSS_COMPILE)ranlib
endif

PROGRAM_NAME = net2usb
SRC := .
OBJ := $(SRC)/obj
Q=@


E = echo \\\# `date +%Y.%m.%d.%H:%M:%S` ---

CINCLUDES = $(SRC)/vncrfbconfig.h \
            $(SRC)/vncrfbproto.h \
            $(SRC)/vncrfb.h  \
            $(SRC)/vnc2usb.h  \
            $(SRC)/vncrfbint.h \
            $(SRC)/vnckeysym.h \
            $(SRC)/curses.h \
            $(SRC)/config.h \
            $(SRC)/dev-hidraw.h

OBJECTS = $(OBJ)/vnchttpd.c.o \
          $(OBJ)/vncmain.c.o \
          $(OBJ)/vncserver.c.o \
          $(OBJ)/vncsockets.c.o \
          $(OBJ)/vncstats.c.o \
          $(OBJ)/vnc2usb.c.o \
          $(OBJ)/dev-hidraw.c.o

INCLUDE_PATHS = -I . \
		-I $(SRC)/inc \
		-I $(SRC)/../inc

CCFlags = $(INCLUDE_PATHS) -g -c -Wall

GCC_VER = $(shell $(CROSS_COMPILE)gcc --version)
#LFLAGS = -L. -L/lib -lnsl -lpthread -static
LFLAGS = -L. -lnsl -lpthread 


# +----------------------------------------
# | Rules:
# | "default" comes first so that if you just type "make" it is the default target.

default_target : $(PROGRAM_NAME)
	@$(E) .
	@$(E) . Built the program $(PROGRAM_NAME)

deb-pkg : $(PROGRAM_NAME)
	@$(E) Creating .deb package
	$(Q)cp $(PROGRAM_NAME) $(SRC)/update/tmp/update/
	$(Q)mv $(SRC)/update/.svn $(SRC)/.svn-dir-update
	$(Q)fakeroot dpkg-deb --build update
	$(Q)mv $(SRC)/.svn-dir-update $(SRC)/update/.svn
	$(Q)mv update.deb vnc2usb-netviz2.deb

$(PROGRAM_NAME): $(OBJ) $(OBJECTS)
	@$(E) Linking $@
	$(Q)$(CC) $(OBJECTS) $(LFLAGS) -o$(PROGRAM_NAME)
	@$(E) Dumping to "$(OBJ)/$(PROGRAM_NAME).dump"
	$(Q)$(OBJDUMP) -d --source "$(PROGRAM_NAME)" > "$(OBJ)/$(PROGRAM_NAME).dump"
	@$(E) Before strip:
	$(Q)ls -l "$(PROGRAM_NAME)"
	$(Q)$(STRIP) "$(PROGRAM_NAME)"
	@$(E) "After strip:"
	$(Q)ls -l "$(PROGRAM_NAME)"

$(OBJ)/%.c.o : $(SRC)/%.c $(CINCLUDES)
	@$(E) Compiling $<
	$(Q)$(CC) $(CCFlags) $< -o$@

$(OBJ) :
	@$(E) Making $@/ directory
	$(Q)mkdir $@

clean : $(OBJ)
	@$(E) Removing objects
	$(Q)rm -rf $(OBJ) $(PROGRAM_NAME) $(SRC)/update/tmp/update/$(PROGRAM_NAME) vnc2usb-netviz2.deb

help :
	@echo
	@echo Program name: $(PROGRAM_NAME)
	@echo
	@echo Available makefile targets:
	@echo
	@echo "  make clean -- erase intermediate files"
	@echo "  make srec  -- compile, link"
	@echo "  make deb-pkg - create vnc2usb.deb"

