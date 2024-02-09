#
# Makefile for compiling authtool tool
#

CC = g++
CCSYSTEM = $(shell $(CC) -dumpmachine)
MINGW = 0
ifeq ($(findstring mingw32, $(CCSYSTEM)), mingw32)
MINGW = 1
endif

ifeq ($(MINGW),1)
all: mediaconverter IPVSTranscode
else
all: mediaconverter IPVSTranscode
endif

clean: mediaconverter_clean IPVSTranscode_clean
	
mediaconverter:
	$(MAKE) -C mediaconverter

mediaconverter_clean:
	$(MAKE) -C mediaconverter clean

IPVSTranscode: 
	$(MAKE) -C IPVSTranscode/IPVSTranscode

IPVSTranscode_clean: 
	$(MAKE) -C IPVSTranscode/IPVSTranscode clean

.PHONY: mediaconverter mediaconverter_clean IPVSTranscode IPVSTranscode_clean