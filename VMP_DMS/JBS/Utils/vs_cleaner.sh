#!/bin/sh

###############################################################################
###                                                                         ###
### Copyright (c) 2003-2010 Barco N.V.                                      ###
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

find .     \( -path "*/Win32/*.exe" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.idb" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.ilk" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.lib" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.map" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.ncb" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.obj" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.opt" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.pch" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.pdb" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.plg" -exec rm \{\} \; \) \
	-o \( -path "*/Win32/*.sbr" -exec rm \{\} \; \)

