# File        : Makefile
# Author      : Eldridge M. Mount IV (eldridge.mount@labxtechnologies.com)
# Description : Makefile for the Lab X AVB class library.  The environment
#               variable "PROJ_TOP" must be defined for Make to locate
#               its options file for inclusion.
#
# Copyright (c) 2010, Lab X Technologies, LLC.  All rights reserved.

# Include project-specific build options
include $(PROJ_TOP)/build/options.mk

# Tool executables
CC   = $(CCFRONT) $(CROSS)gcc
CPP  = $(CXXFRONT) $(CROSS)g++
AR   = $(CROSS)ar
DOXY = doxygen

# Utility classes built unconditionally
SRCS  = PtpEvent.cpp

SRCS += AvbPort.cpp
SRCS += LinkMonitor.cpp
SRCS += PtpDomain.cpp

DEBUG       = -g
# DEBUG      += -D_LABXDEBUG

INC_DIRS   += ./include
INC_DIRS   += $(LABXFC)/include
INC_DIRS   += $(LINUX_INC)
CFLAGS     += $(DEBUG)
CFLAGS     += -Wall
LD	        = $(CC)
LDFLAGS     = 
RM	        = rm
LIBRARY     = libLabXAvb.a

# Default target by convention is ``all''
all : $(LIBRARY)

# Produce objects and dependency files from the source list
OBJS        = ${SRCS:%.cpp=%.o}
DEPFILES    = ${SRCS:%.cpp=%.d}

# clear out all suffixes
.SUFFIXES:

# list only those we use
.SUFFIXES: .o .cpp .d

# Phony targets
.PHONY: docs

# Pattern rule for compiling C++ sources to objects, producing dependency
# rule files on-the-fly as required.
%.o: %.cpp
	$(CPP) -MMD $(CFLAGS) $(CPPFLAGS) $(INC_DIRS:%=-I %) -c $<

$(LIBRARY): $(OBJS) Makefile
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJS)

clean:
	-$(RM) -f $(LIBRARY) $(OBJS) $(DEPFILES) $(IDL_GEN)

docs:
	$(DOXY) Doxyfile

# Include dependency files for each source file, once they've been built.
-include ${SRCS:%.cpp=%.d}
