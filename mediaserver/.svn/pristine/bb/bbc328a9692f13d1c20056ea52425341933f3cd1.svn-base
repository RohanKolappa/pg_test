#!/bin/sh
URL=http://192.168.1.15/~nightly/NIGHTLY/DMS-3.9/IMAGES
if [ x"$1" != "x" ]; then
	URL=$1
fi
SCRIPTPATH=/root/scripts
echo Getting Images from $URL
mkdir -p /home/onair
for i in  base.tar firmwareapp.tgz mediaroomapp.tgz onairapp.tgz onlibapp.tgz guiapp.tgz strmsrvapp.tgz jdkapp.tgz plugin.tgz
do
  wget ${URL}/$i -O /home/onair/$i
done

cd /
tar -C /home/onair -xvf /home/onair/base.tar
/bin/rm -rf /etc/scripts /etc/config
zcat /home/onair/onair.gz | cpio --make-directories -i -H newc  etc/scripts/* etc/config/*


for i in  firmwareapp.tgz mediaroomapp.tgz onairapp.tgz onlibapp.tgz guiapp.tgz strmsrvapp.tgz
do
	echo "Installing " $i
	tar xzf /home/onair/$i
done


chkconfig --del Orb
cp /etc/scripts/Orb /etc/init.d
chkconfig --add Orb
echo "Installing plugin"
tar -C /home/orb --strip=2 -xzf /home/onair/plugin.tgz
chown -R orb:100 /home/orb/.mozilla

mkdir -p /V2O
ln -sf /home/onair/V2O/bin /V2O
ln -sf /home/onair/V2O/modules /V2O
ln -sf /home/onair/V2O/firmware /V2O
/V2O/bin/vutils --install
echo "Now working around the sticky bit problem"
rm /V2O/bin/set_sharedvideo /V2O/bin/get_sharedvideo
cp /V2O/bin/vutils /V2O/bin/set_sharedvideo 
cp /V2O/bin/vutils /V2O/bin/get_sharedvideo
chmod +xs /V2O/bin/set_sharedvideo /V2O/bin/get_sharedvideo


mkdir -p /var/log/deviceapp
echo "Installing hard coded configs"
touch /etc/config/sw_profile_orb.txt
echo "ORB-500-Device" > /etc/config/hw_profile_orb.txt
cp /home/onair/objectXml/ORB-500-Device.xml /etc/config/Device.xml
mkdir -p /V2O/config
if [ -f /etc/script/orbupgrade.sh ]; then
	echo "Skipping installtion of default config files"
else
	cp ${SCRIPTPATH}/lutlistconf_0.txt /V2O/config
	cp ${SCRIPTPATH}/iflistconf.txt /V2O/config
fi

echo "Checking for paths"
grep "V2O" /root/.bashrc 
if [ x"$?" != x"0" ]; then
	echo "export PATH=\${PATH}:/V2O/bin" >> /root/.bashrc
fi
grep "V2O" /home/orb/.bashrc 
if [ x"$?" != x"0" ]; then
	echo "export PATH=\${PATH}:/V2O/bin" >> /home/orb/.bashrc
fi


# Now install the clients
mkdir -p /home/orb/ipvs
if [ -f /home/orb/ipvs/web/JSXAPPS/IPVS-Common/xml/defaults/Preferences.xml ]; then
	tar --strip=5 -C /home/orb/ipvs -xzf /home/onair/guiapp.tgz home/onair/modules/clients/public/web --exclude Preferences.xml
else
	tar --strip=5 -C /home/orb/ipvs -xzf /home/onair/guiapp.tgz home/onair/modules/clients/public/web 
fi
cp /home/orb/ipvs/web/jsx3.gui.window.html /home/orb/ipvs/web/JSXAPPS/IPVS-Common/mediaroom
cp /etc/scripts/launchumc.sh /home/orb
chown -R orb:users /home/orb/ipvs
chown orb:users /home/orb/launchumc.sh






