###############################################################################
###                                                                         ###
### Copyright (c) 2003-2014 Barco N.V.                                      ###
###                                                                         ###
###############################################################################
###                                                                         ###
### This file is part of "JBS".                                             ###
###                                                                         ###
### "JBS" is free software; you can redistribute it and/or modify it        ###
### under the terms of the GNU Lesser General Public License as published   ###
### by the Free Software Foundation; either version 2.1 of the License, or  ###
### (at your option) any later version.                                     ###
###                                                                         ###
### "JBS" is distributed in the hope that it will be useful, but            ###
### WITHOUT ANY WARRANTY; without even the implied warranty of              ###
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       ###
### Lesser General Public License for more details.                         ###
###                                                                         ###
### You should have received a copy of the GNU Lesser General Public        ###
### License along with "JBS"; if not, write to the Free Software            ###
### Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  2111-1307 USA ###
###                                                                         ###
###############################################################################

###############################################################################
### This is the entry point of the building system, i.e., the file that is
### first read when "make" is invoked in any directory of the development tree.

###############################################################################
### This is the default target, i.e. the one that get rebuilt if no other
### target is passed on the command line (the "all" target is defined in the
### "Make.common.stuff" file).

.PHONY : default-entry-point

default-entry-point : all

###############################################################################
### The following variables are used to define the name of the special files
### JBS uses to do its job. If you want to change some of them, update the
### Makefile in the root directory, and your changes will get propagated
### automatically!
###
### +-------------------+-----------------------------------------------------+
### | JBS_TOP_DIR_MARK  | Name of the file marking the top-level directory.   |
### | JBS_DEFS_BASE     | Prefix of the name of the auto-generated file       |
### |                   | containing project definitions.                     |
### | JBS_DEPS_BASE     | Prefix of the name of the auto-generated file       |
### |                   | containing the traditional source/header files      |
### |                   | inter-dependencies.                                 |
### | JBS_BUILD_BASE    | Directory to hold temp files (will be used in 5.x). |
### | JBS_NOENTER_FILE  | Name of the file telling JBS to never enter that    |
### |                   | directory.                                          |
### | JBS_NOBUILD_FILE  | Name of the file telling JBS to never build         |
### |                   | anything in that particular directory.              |
### +-------------------+-----------------------------------------------------+

JBS_TOP_DIR_MARK	:=	.jbs.root

JBS_DEFS_BASE		:=	.jbs.defs
JBS_DEPS_BASE		:=	.jbs.deps
JBS_BUILD_BASE		:=	.jbs.build

JBS_NOENTER_FILE	:=	.jbs.noenter
JBS_NOBUILD_FILE	:=	.jbs.nobuild

###############################################################################
### If this "make" run is launched by the user (i.e. it is not a recursive call
### to "make" done when entering sub-directories), we do the following:
### - compute the path to the top-level directory
### - determine the environment we are using
### - export those values for use by further recursive calls to "make".
###
### Determining if this is a user call can be done in at least 2 ways:
### - use the MAKELEVEL variable (automatically set by GNU make)
### - use our own 'flags'.
###
### We use the second solution and define the following set of constants:
### +-------------------+-----------------------------------------------------+
### | JBS_CONFIGURED    | Set to "YES" during the first call to "make".       |
### | JBS_TOP_DIR       | Path of the top-level directory of this development |
### |                   | tree (where $(JBS_TOP_DIR_MARK) was found).         |
### | JBS_PLATFORM      | One of "WIN32", "LINUX", "CYGWIN".                  |
### | JBS_DEFS_FILE     | $(JBS_DEFS_BASE) suffixed by the platform name.     |
### | JBS_DEPS_FILE     | $(JBS_DEPS_BASE) suffixed by the platform name.     |
### | JBS_BUILD_DIR     | Where to put temporary files (will be used in 5.x). |
### +-------------------+-----------------------------------------------------+
###
### Those variables are then exported so that they are available to the
### subsequent recursive calls to "make".

ifndef JBS_CONFIGURED

### So this is no recursive call to "make"! Let's start discovering our
### environment!

export JBS_CONFIGURED	:=	yes

### We compute the path of the top-level directory. To do so, we start from
### the pathname of the current directory (as given by "$(CURDIR)"), and go up
### one level until we find the "$(JBS_TOP_DIR_MARK)" file.

remove-one-level	=	$(patsubst %/,%,$(dir $(1)))
find-top-from		=	$(if $(wildcard $(1)/$(JBS_TOP_DIR_MARK)),$(1),$(call find-top-from,$(call remove-one-level,$(1))))

export JBS_TOP_DIR	?=	$(call find-top-from,$(CURDIR))

-include $(JBS_TOP_DIR)/$(JBS_TOP_DIR_MARK)

ifdef JBS_BASE_DIR
export JBS_APPS_DIR	:=	$(JBS_BASE_DIR)/Apps
export JBS_MAKE_DIR	:=	$(JBS_BASE_DIR)/Make
export JBS_TMPL_DIR	:=	$(JBS_BASE_DIR)/Templates
export JBS_UTIL_DIR	:=	$(JBS_BASE_DIR)/Utils
else
export JBS_APPS_DIR	:=	$(JBS_TOP_DIR)/JBS/Apps
export JBS_MAKE_DIR	:=	$(JBS_TOP_DIR)/JBS/Make
export JBS_TMPL_DIR	:=	$(JBS_TOP_DIR)/JBS/Templates
export JBS_UTIL_DIR	:=	$(JBS_TOP_DIR)/JBS/Utils
endif

### [KAE] Get already-built-deps from file: clear it on startup.

ifndef JBS_NO_ALREADY_BUILT
export JBS_PROGR_FILE	:=	$(JBS_TOP_DIR)/.jbs.progress
$(shell [ -e "$(JBS_PROGR_FILE)" ] && rm $(JBS_PROGR_FILE))
endif

### We now discover the platform type. To do so, we invoke the "uname -s"
### command in a sub-shell.

uname-system		:=	$(shell uname -s)
JBS_UNIX_SYSTEM		:= 0
ifeq "$(findstring Linux,$(uname-system))" "Linux"
system-target		:=	LINUX
JBS_UNIX_SYSTEM		:=	1
else
ifeq "$(findstring Darwin,$(uname-system))" "Darwin"
system-target		:=	DARWIN
JBS_UNIX_SYSTEM		:=	1
else
$(error Unsupported system! ($(uname-system)))
endif
endif

### Let the user override our choice, for example to force using the GNU
### compiler and linker on Cygwin...

ifdef JBS_TARGET
system-target		:=	$(JBS_TARGET)
endif

### Last check of the platform type (maybe directly set by the user).

JBS_SUPP_PLATFS		:=	LINUX WIN32 DARWIN

ifeq "$(findstring $(system-target),$(JBS_SUPP_PLATFS))" ""
$(error Unsupported platform "$(system-target)")
endif

export JBS_UNIX_SYSTEM
export JBS_PLATFORM	:=	$(system-target)
export JBS_DEFS_FILE	:=	$(JBS_DEFS_BASE).$(JBS_PLATFORM)
export JBS_DEPS_FILE	:=	$(JBS_DEPS_BASE).$(JBS_PLATFORM)
export JBS_BUILD_DIR	:=	$(JBS_BUILD_BASE).$(JBS_PLATFORM)

### We use this unique occasion to perform some additional initialization.

include $(JBS_MAKE_DIR)/Make.module.init

### For debugging purposes...

dumpable += JBS_TOP_DIR JBS_PLATFORM
dumpable += JBS_DEFS_FILE JBS_DEPS_FILE JBS_BUILD_DIR

endif

###############################################################################
### Now, we compute the directory we are in, and the path from the top-level
### directory to this directory:
### +-------------------+-----------------------------------------------------+
### | JBS_CUR_DIR       | Where we are.                                       |
### | JBS_REL_DIR       | The path to follow from the top-level dir to go to  |
### |                   | this dir, such that all the following paths would   |
### |                   | be valid, and point to this directory:              |
### |                   |        $(JBS_TOP_DIR)/$(JBS_REL_DIR)                |
### |                   |        $(JBS_CUR_DIR)                               |
### +-------------------+-----------------------------------------------------+
###
### Example 1:
###	$(JBS_TOP_DIR)		:= /home/<username>/DevTree
###	$(JBS_CUR_DIR)		:= /home/<username>/DevTree/BFC/Common
###	$(JBS_REL_DIR)		:= BFC/Common
###
### Example 2:
###	$(JBS_TOP_DIR)		:= /home/<username>/DevTree
###	$(JBS_CUR_DIR)		:= /home/<username>/DevTree
###	$(JBS_REL_DIR)		:= .

JBS_CUR_DIR		:=	$(CURDIR)
JBS_REL_DIR		:=	$(patsubst %/.,%,$(patsubst $(JBS_TOP_DIR)/%,%,$(JBS_CUR_DIR)/.))

### For debugging purposes...

dumpable += JBS_CUR_DIR JBS_REL_DIR

###############################################################################
### Now, it's time to include the rest of the building system (static parts),
### which are located in the $(JBS_MAKE_DIR) directory...

include $(JBS_MAKE_DIR)/Make.bootstrap

###############################################################################

