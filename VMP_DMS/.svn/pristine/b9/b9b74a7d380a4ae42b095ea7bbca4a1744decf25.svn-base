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



if [ $# -lt 2 ]
then
	echo "Usage: $0 <old_ns> <new_ns>"
	exit 1
fi

old_ns="$1"
new_ns="$2"

old_ns_dot=${old_ns//::/.}
new_ns_dot=${new_ns//::/.}
old_ns_col=${old_ns_dot//\./::}
new_ns_col=${new_ns_dot//\./::}
old_ns_cma=${old_ns_dot//\./, }
new_ns_cma=${new_ns_dot//\./, }
old_ns_und=${old_ns_dot//\./_}
new_ns_und=${new_ns_dot//\./_}

old_ns_dot=${old_ns_dot//\./\\.}

# echo "Converting ${old_ns_dot} to ${new_ns_dot}"
# echo "Converting $old_ns_col to $new_ns_col"

if [ $# -gt 2 ]
then
	shift
	shift
	list="$@"
else
	list=$( find . -type f -not -path './*/.svn' \
		'(' -name "${old_ns_dot}.*.cpp" -o -name "${old_ns_dot}.*.h" ')' )
fi

# echo "Renaming in $list"

for file_old_pth in $list
do

#	echo "Processing ${file_old_pth}..."

	file_cur_dir="`dirname ${file_old_pth}`"
	file_old_nme="`basename ${file_old_pth}`"

	# filename ?

	echo "${file_old_nme}" | egrep "${new_ns_dot}" > /dev/null 2> /dev/null

	if [ $? -eq 0 ]
	then
		echo "File ${file_old_nme} already contains new namespace pattern. Not renaming."
		file_new_nme=${file_old_nme}
		file_new_pth=${file_old_pth}
	else
		# rename !
		file_new_nme=${file_old_nme/${old_ns_dot}/${new_ns_dot}}
		file_new_pth=${file_cur_dir}/${file_new_nme}
		echo "Renaming ${file_old_pth} to ${file_new_pth}..."
		( svn info ${file_old_pth} | grep Revision ) > /dev/null 2>&1
		if [ $? -eq 0 ]
		then
			( cd ${file_cur_dir} && svn commit -m "File renaming." ${file_old_nme} )
			svn move "${file_old_pth}" "${file_new_pth}"
		else
			mv "${file_old_pth}" "${file_new_pth}"
		fi
	fi

	# Replace some known patterns...

	echo "Converting in \"${file_new_pth}\"..."

	tmp_new_pth=${file_new_pth}.new

	old_ns_col_strp_bfc=${old_ns_col/#BFC::/}
	new_ns_col_strp_bfc=${new_ns_col/#BFC::/}
	old_ns_col_strp_vmp=${old_ns_col/#VMP::/}
	new_ns_col_strp_vmp=${new_ns_col/#VMP::/}

	old_ns_cma_strp_bfc=${old_ns_cma/#BFC, /}
	new_ns_cma_strp_bfc=${new_ns_cma/#BFC, /}
	old_ns_cma_strp_vmp=${old_ns_cma/#VMP, /}
	new_ns_cma_strp_vmp=${new_ns_cma/#VMP, /}

	( \
		cat ${file_new_pth} \
		| \
		sed -r -e "{ \
			s/\b${old_ns_dot}\b/${new_ns_dot}/g ; t ; \
			s/\b${old_ns_col}\b/${new_ns_col}/g ; t ; \
			s/\b${old_ns_col_strp_bfc}::\b/${new_ns_col_strp_bfc}::/g ; t ; \
			s/\b${old_ns_col_strp_vmp}::\b/${new_ns_col_strp_vmp}::/g ; t ; \
			s/BFC_PTR_IMPL_NS. ${old_ns_cma_strp_bfc}/BFC_PTR_IMPL_NS2( ${new_ns_cma_strp_bfc}/g ; t ; \
			s/BFC_PTR_IMPL_NS_CC. ${old_ns_cma_strp_bfc}/BFC_PTR_IMPL_NS2_CC( ${new_ns_cma_strp_bfc}/g ; t ; \
			s/BFC_PTR_IMPL_NS. ${old_ns_cma_strp_vmp}/BFC_PTR_IMPL_NS2( ${new_ns_cma_strp_vmp}/g ; t ; \
			s/BFC_PTR_IMPL_NS_CC. ${old_ns_cma_strp_vmp}/BFC_PTR_IMPL_NS2_CC( ${new_ns_cma_strp_vmp}/g ; t ; \
			s/\b${old_ns_und}\b/${new_ns_und}/g ; t ; \
			s/\b${old_ns_und}_DLL\b/${new_ns_und}_DLL/g ; t ; \
			s/_${old_ns_und}_/_${new_ns_und}_/g ; t ; \
		}" > ${tmp_new_pth} \
	) \
	&& \
	mv -f ${tmp_new_pth} ${file_new_pth}

	old_ns_blk_prf="$( echo -n ${old_ns_col} | sed -e 's/\([A-Za-z0-9]\+\)\(::\)*/namespace \1 \{\\n/g' )"
	new_ns_blk_prf="$( echo -n ${new_ns_col} | sed -e 's/\([A-Za-z0-9]\+\)\(::\)*/namespace \1 \{\\n/g' )"

#	echo -E "${old_ns_blk_prf}"
#	echo -E "${new_ns_blk_prf}"

	( \
		cat ${file_new_pth} \
		| \
		sed -e ":a;N;$"'!'"ba;s/${old_ns_blk_prf}/${new_ns_blk_prf}/g" > ${tmp_new_pth} \
	) \
	&& \
	mv -f ${tmp_new_pth} ${file_new_pth}

	old_ns_blk_sfx="$( echo -n ${old_ns_col} | sed -e 's/\([A-Za-z0-9]\+\)\(::\)*/\} \\\/\\\/ namespace \1\n/g' | tac | sed -e ':a;N;$!ba;s/\n/\\n/g' )"
	new_ns_blk_sfx="$( echo -n ${new_ns_col} | sed -e 's/\([A-Za-z0-9]\+\)\(::\)*/\} \\\/\\\/ namespace \1\n/g' | tac | sed -e ':a;N;$!ba;s/\n/\\n/g' )"

#	echo -E "${old_ns_blk_sfx}"
#	echo -E "${new_ns_blk_sfx}"

	( \
		cat ${file_new_pth} \
		| \
		sed -e ":a;N;$"'!'"ba;s/${old_ns_blk_sfx}/${new_ns_blk_sfx}/g" > ${tmp_new_pth} \
	) \
	&& \
	mv -f ${tmp_new_pth} ${file_new_pth}

done



