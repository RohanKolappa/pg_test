#!/bin/bash

###############################################################################

tgt_dir="`pwd`"
cd "`dirname $0`"
jbs_dir="`pwd`"
cd "${tgt_dir}"

echo "Dst dir: ${tgt_dir}"
echo "JBS dir: ${jbs_dir}"

###############################################################################
# Names gathering:
# - folder_name
# - dll_name

# -----------------------------------------------------------------------------

read -e -p "Folder name ? " answer

case "$answer" in
	"") echo "Invalid answer!"; exit 1 ;;
	* ) folder_name="$answer" ;;
esac

# -----------------------------------------------------------------------------

read -e -p "DLL name    ? " answer

case "$answer" in
	"") echo "Invalid answer!"; exit 1 ;;
	* ) dll_name="$answer" ;;
esac

###############################################################################

# cp -R ${jbs_dir}/DllTmpl ${tgt_dir}/${folder_name}

tar xfz ${jbs_dir}/DllTmpl.tgz
mv DllTmpl "${folder_name}"

cd "${folder_name}"

${jbs_dir}/../Utils/jbs_rename_token.sh __dll_name_colons__ "${dll_name//_/::}"
${jbs_dir}/../Utils/jbs_rename_token.sh __dll_name_dots__ "${dll_name//_/.}"
${jbs_dir}/../Utils/jbs_rename_token.sh __dll_name_underscores__ "${dll_name}"
${jbs_dir}/../Utils/jbs_rename_token.sh __dllname__ "${dll_name}"

for f in `find . -name Make.target`
do
	${jbs_dir}/../Utils/jbs_rename_token.sh __uuid__ `uuidgen` $f
done

###############################################################################

