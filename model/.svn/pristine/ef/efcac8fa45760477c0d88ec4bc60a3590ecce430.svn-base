#!/bin/sh
#if argument not there return
fromVersion=$1
toVersion=$2
if [ -z $fromVersion -o -z $toVersion ]; then
	echo "usage: ./createRevision.sh fromVersion toVersion."
	echo "e.g top create folder for serviceVer from '1.0' to '1.3' : ./createRevision.sh \"1.0\" \"1.3\" "
	exit 1
fi	
rm -rf migration/${fromVersion}
mkdir migration/${fromVersion}
cp -r migration/templates/* migration/${fromVersion}
find migration/${fromVersion} -name ".svn" | xargs rm -rf
for i in `find migration/${fromVersion} -type f`
do
	sed -e s/OLDSERVICEVERSION/${fromVersion}/ $i > $i.tmp
	mv $i.tmp $i
	sed -e s/NEWSERVICEVERSION/$toVersion/ $i > $i.tmp

	mv $i.tmp $i
	echo $i
done

sed -i -e "s/$fromVersion/$toVersion/g" ./sdk/rest/src/lib/barcoUtils.js 

echo "Updated service version in file ./sdk/rest/src/lib/barcoUtils.js"
echo "Created migration scripts into folder 'migration/${fromVersion}' for migrating serviceVer from '${fromVersion}' to '${toVersion}' .  "
echo "IMPORTANT: Please do not forget to commit the changes to svn.."
