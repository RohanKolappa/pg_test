#!/bin/bash

###############################################################################

read -e -p "Folder name                                          ? " folder_name

if ! [[ "${folder_name}" =~ ^[A-Za-z0-9_]+$ ]]
then
	echo "Folder name may only contain alphanumeric characters!"
	exit 1
fi

if [ -e "${folder_name}" ]
then
	echo "\"${folder_name}\" already exists! aborting!"
	exit 1
fi

# -----------------------------------------------------------------------------

read -e -i "${folder_name}" -p "Project name                                         ? " project_name

if ! [[ "${project_name}" =~ ^[A-Za-z0-9_]+$ ]]
then
	echo "Project name may only contain alphanumeric characters!"
	exit 1
fi

# -----------------------------------------------------------------------------

read -e -i BFC_Light -p "Main dependency                                      ? " target_dep

if ! [[ "${target_dep}" =~ ^[A-Za-z0-9_]+$ ]]
then
	echo "Main dependency may only contain alphanumeric characters!"
	exit 1
fi

###############################################################################
### Templates...

make_target_template="\
###############################################################################
###                                                                         ###
### Copyright (c) 2003-@@@YEAR@@@ Barco N.V.                                      ###
###                                                                         ###
###############################################################################
###                                                                         ###
### This file is part of \"BCP\".                                             ###
###                                                                         ###
### \"BCP\" is free software; you can redistribute it and/or modify it        ###
### under the terms of the GNU Lesser General Public License as published   ###
### by the Free Software Foundation; either version 2.1 of the License, or  ###
### (at your option) any later version.                                     ###
###                                                                         ###
### \"BCP\" is distributed in the hope that it will be useful, but            ###
### WITHOUT ANY WARRANTY; without even the implied warranty of              ###
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       ###
### Lesser General Public License for more details.                         ###
###                                                                         ###
### You should have received a copy of the GNU Lesser General Public        ###
### License along with \"BCP\"; if not, write to the Free Software            ###
### Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  2111-1307 USA ###
###                                                                         ###
###############################################################################

###############################################################################
###                                                                         ###
### This is an auto-generated file.                                         ###
### Please consult the \"JBS\" docs to understand its content.                ###
###                                                                         ###
###############################################################################

target-type	:=	exe
target-name	:=	@@@NAME@@@
target-ver-base	:=	@@@BASE@@@
target-sources	:=	*
target-deps	:=	@@@BASE@@@
target-incldirs	:=
target-libs	:=
target-libdirs	:=
target-docdir	:=	# Doc	# Base folder for documentation.
target-doxydir	:=	# Classes	# Auto-gen. files go to ./Doc/Classes
target-doxyimg	:=	# Images	# User images go to ./Doc/Images
target-cluster	:=

###############################################################################

target-uuid	:=	@@@UUID@@@

###############################################################################
"

source_template="\
// ============================================================================
//
// Copyright (c) 2003-@@@YEAR@@@ Barco N.V.
//
// ============================================================================
// 
// This file is part of \"BCP\".
// 
// \"BCP\" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// \"BCP\" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with \"BCP\"; if not, write to the Free Software Foundation, 59 Temple
// Place, Suite 330, Boston, MA 02111-1307 USA.
// 
// ============================================================================

#include <iostream>

#include \"BFC.Base.Buffer.h\"
#include \"BFC.Base.BufferIO.h\"
#include \"BFC.Base.Exception.h\"

#include \"BFC.TL.BufferArray.h\"

// ============================================================================

using namespace std;
using namespace BFC;

// ============================================================================

static Buffer progName;

// ============================================================================

static void usage() {

	cerr << \"Usage: \" << progName << \" <options>\" << endl
		<< \"where <options> are:\" << endl;

}

// ============================================================================

static void doIt(
		BufferArray &	pArgs ) {

	progName = pArgs.pop( 0 );

	// Do something useful here!

}

// ============================================================================

int main( int argc, char * argv[] ) {

	try {
		BufferArray args( argc, argv );
		doIt( args );
	}
	catch ( Exception & e ) {
		cerr << e.what() << endl;
		usage();
		return -2;
	}

	return 0;

}

// ============================================================================
"

###############################################################################

YEAR="$( date +%Y )"
NAME="${project_name}"
BASE="${target_dep}"
UUID="$( uuidgen )"

mkdir "${folder_name}"
cp Makefile "${folder_name}"/Makefile
echo "${make_target_template}" \
	| sed	-e "s/@@@YEAR@@@/${YEAR}/" \
		-e "s/@@@NAME@@@/${NAME}/" \
		-e "s/@@@BASE@@@/${BASE}/" \
		-e "s/@@@UUID@@@/${UUID}/" \
	> "${folder_name}"/Make.target
echo "${source_template}" \
	| sed	-e "s/@@@YEAR@@@/${YEAR}/" \
		-e "s/@@@NAME@@@/${NAME}/" \
		-e "s/@@@BASE@@@/${BASE}/" \
		-e "s/@@@UUID@@@/${UUID}/" \
	> "${folder_name}"/"${project_name}.cpp"

echo "Done!"

###############################################################################
