# File        : Makefile
# Author      : Eldridge M. Mount IV (eldridge.mount@labxtechnologies.com)
# Description : Makefile for the Lab X Foundation class library.  The environment
#               variable "PROJ_TOP" must be defined for Make to locate
#               its options file for inclusion.
#
# Copyright (c) 2010, Lab X Technologies, LLC.  All rights reserved.

# Include project-specific build options
include $(PROJ_TOP)/build/options.mk

CC =  $(CCFRONT) $(CROSS)gcc
CPP = $(CXXFRONT) $(CROSS)g++
AR =  $(CROSS)ar

SRCS      = CleanupTarget.cpp
SRCS     += Debug.cpp 
SRCS     += Event.cpp
SRCS     += EventHandler.cpp
SRCS     += EventNexus.cpp
SRCS     += Exception.cpp
SRCS     += IDLResponder.cpp
SRCS     += LabX.cpp
SRCS     += Lockable.cpp
SRCS     += MacAddress.cpp
SRCS     += MacRange.cpp
SRCS     += PropertyTable.cpp
SRCS     += RemoteIDLClient.cpp
SRCS     += RemoteIDLServer.cpp
SRCS     += Semaphore.cpp
SRCS     += ShutdownHandler.cpp
SRCS     += SignalSniffer.cpp
SRCS     += TcpClientSocket.cpp
SRCS     += TcpServerSocket.cpp
SRCS     += Thread.cpp
SRCS     += Timer.cpp
SRCS     += XmlHandler.cpp
SRCS     += XmlParser.cpp

# Some aspects of the library are not supported universally
ifndef CYGWIN
SRCS     += NetlinkClient.cpp
SRCS     += RawSocket.cpp
SRCS     += SpiMaster.cpp
endif

DEBUG     = -g
INC_DIRS  = ./include
INC_DIRS += $(LINUX_INC)
CFLAGS   += $(DEBUG) # -D_LABXDEBUG
CFLAGS   += -Wall
LD        = $(CC)
LDFLAGS	  = 
RM        = rm
OBJS	  = ${SRCS:.cpp=.o}
DEPFILES  = ${SRCS:.cpp=.d}
LIBRARY   = libLabXFc.a

# clear out all suffixes
.SUFFIXES:

# list only those we use
.SUFFIXES: .o .cpp

# define a suffix rule for .cpp -> .o

%.o: %.cpp
	$(CPP) -MMD $(CFLAGS) $(CPPFLAGS) $(INC_DIRS:%=-I %) -c $<

# default target by convention is ``all''
all : $(LIBRARY)

$(LIBRARY): $(OBJS) Makefile
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJS)

clean :
	-$(RM) -f $(LIBRARY) $(OBJS) $(DEPFILES)

# Include dependency files for each source file, once they've been built.
-include $(SRCS:%.cpp=%.d)
