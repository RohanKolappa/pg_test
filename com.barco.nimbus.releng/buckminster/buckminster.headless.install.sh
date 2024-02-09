# this script installs the headless buckminster product (a cli to buckminster) into $dest
# after completion you can run buckminster like $dest/buckminster -data /tmp/ws help
# To script buckminster, see buckminster.nimbus.product.export.script.txt

dest=/usr/local/headless-buckminster

tmp=$RANDOM
mkdir $dest
cd /tmp
mkdir $tmp
cd $tmp

wget http://eclipse.cu.be/tools/buckminster/products/director_latest.zip
wget -O headless.zip http://archive.eclipse.org/tools/buckminster/archivedsites-4.2/org.eclipse.buckminster.site.eclipse.headless_1.5.0.v20130407-2015.zip 
wget -O cloudsmith.zip http://download.cloudsmith.com/buckminster/archivedsites/4.2/org.eclipse.buckminster.site.external_1.5.0.v20121118-2330.zip 

unzip director_latest.zip
unzip headless.zip -d headless
unzip cloudsmith.zip -d cloudsmith

director/director -r file://`pwd`/headless -d $dest -p Buckminster -i org.eclipse.buckminster.cmdline.product
director/director -r file://`pwd`/headless -d $dest -p Buckminster -i org.eclipse.buckminster.core.headless.feature.feature.group
director/director -r file://`pwd`/headless -d $dest -p Buckminster -i org.eclipse.buckminster.pde.headless.feature.feature.group
director/director -r file://`pwd`/headless -d $dest -p Buckminster -i org.eclipse.buckminster.git.headless.feature.feature.group
director/director -r file://`pwd`/cloudsmith -d $dest -p Buckminster -i org.eclipse.buckminster.subversive
director/director -r file://`pwd`/cloudsmith -d $dest -p Buckminster -i org.eclipse.buckminster.subversive.headless.feature.feature.group
director/director -r file://`pwd`/cloudsmith -d $dest -p Buckminster -i org.eclipse.buckminster.emma.headless.feature.feature.group



