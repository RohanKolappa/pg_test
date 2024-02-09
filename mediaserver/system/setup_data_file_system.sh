#!/bin/sh

# Source common library
. /etc/scripts/common.rc

############## import _db_server.rc for DB_SERVER=sedna_native/sedna_xmldb/exist_xmldb ##############
. /etc/scripts/_db_server.rc
############## import _db_server.rc for DB_SERVER=sedna_native/sedna_xmldb/exist_xmldb ##############

# Setup Open Fire XMPP Server file system from files in /data
if [  -f /etc/config/sw_profile_dms.txt ] || [  -f /etc/config/sw_profile_ms.txt ] ; then

    # create mediadir import/export directories
    /bin/mkdir -p /data/mediadir/export
    /bin/mkdir -p /data/mediadir/import
    /bin/mkdir -p /data/mediadir/inject
    /bin/chown nobody /data/mediadir/inject

    # Create thumbnail cache directories
    /bin/mkdir -p /data/tncache
    /bin/mkdir -p /data/tncache/live
    /bin/mkdir -p /data/tncache/recordings
    /bin/chown admin /data/tncache/live
    /bin/chown admin /data/tncache/recordings

    # Openfire file system setup
    /bin/mkdir -p /data/xmppserver/conf
    /bin/mkdir -p /data/xmppserver/resources/security
    /bin/mkdir -p /data/xmppserver/embedded-db
    /bin/mkdir -p /data/xmppserver/logs
    /bin/rm -rf /home/onair/openfire/conf
    /bin/rm -rf /home/onair/openfire/resources/security
    /bin/rm -rf /home/onair/openfire/logs


    /bin/ln -s /data/xmppserver/conf /home/onair/openfire
    /bin/ln -s /data/xmppserver/embedded-db /home/onair/openfire
    /bin/ln -s /data/xmppserver/logs /home/onair/openfire
    /bin/ln -s /data/xmppserver/resources/security /home/onair/openfire/resources

     #copy openfire initdb files on every system restart (this could have done of every openfire restart instead)
     #removed copy files in restore database, but did not take out the same from save_database yet.
     /bin/cp /home/onair/templates/openfire.xml.localdb /data/xmppserver/conf/openfire.xml
     /bin/cp /home/onair/templates/openfire.properties.localdb /data/xmppserver/embedded-db/openfire.properties
     /bin/cp /home/onair/templates/openfire.script.localdb /data/xmppserver/embedded-db/openfire.script

    if [ "x${DB_SERVER}" = "xexist_xmldb" ] ; then
		#link exist persistence/growing folders
		exist_data=/data/existdbserver
		exist_install=/usr/local/exist
		exist_install_webapp=${exist_install}/webapp/WEB-INF
		exist_install_tools=${exist_install}/tools
		/bin/mkdir -p $exist_data
		/bin/mkdir -p ${exist_data}/data
		/bin/mkdir -p ${exist_data}/logs
		/bin/mkdir -p ${exist_data}/entities
		/bin/mkdir -p ${exist_data}/attachments
		/bin/mkdir -p ${exist_data}/classes
		/bin/mkdir -p ${exist_data}/jetty/logs
		/bin/mkdir -p ${exist_data}/jetty/work
		/bin/mkdir -p ${exist_data}/wrapper/logs
		/bin/mkdir -p ${exist_data}/wrapper/work

		[ -d ${exist_install_webapp}/logs/ ] && /bin/cp -r ${exist_install_webapp}/logs/* $exist_data/logs
		[ -d ${exist_install_webapp}/entities/ ] && /bin/cp -r ${exist_install_webapp}/entities/* ${exist_data}/entities
		[ -d ${exist_install_webapp}/attachments/ ] && /bin/cp -r ${exist_install_webapp}/attachments/* ${exist_data}/attachments
		[ -d ${exist_install_webapp}/classes/ ] && /bin/cp -r ${exist_install_webapp}/classes/* ${exist_data}/classes

		/bin/rm -rf ${exist_install_webapp}/data
		/bin/rm -rf ${exist_install_webapp}/logs
		/bin/rm -rf ${exist_install_webapp}/entities
		/bin/rm -rf ${exist_install_webapp}/attachments
		/bin/rm -rf ${exist_install_webapp}/classes
		/bin/rm -rf ${exist_install_tools}/jetty/logs
		/bin/rm -rf ${exist_install_tools}/jetty/work
		/bin/rm -rf ${exist_install_tools}/wrapper/logs
		/bin/rm -rf ${exist_install_tools}/wrapper/work

		/bin/ln -s  ${exist_data}/data         ${exist_install_webapp}/data
		/bin/ln -s  ${exist_data}/logs         ${exist_install_webapp}/logs
		/bin/ln -s  ${exist_data}/entities     ${exist_install_webapp}/entities
		/bin/ln -s  ${exist_data}/attachments  ${exist_install_webapp}/attachments
		/bin/ln -s  ${exist_data}/classes      ${exist_install_webapp}/classes
		/bin/ln -s  ${exist_data}/jetty/logs   ${exist_install_tools}/jetty/logs
		/bin/ln -s  ${exist_data}/jetty/work   ${exist_install_tools}/jetty/work
		/bin/ln -s  ${exist_data}/wrapper/logs ${exist_install_tools}/wrapper/logs
		/bin/ln -s  ${exist_data}/wrapper/work ${exist_install_tools}/wrapper/work

	else

		# Sedna file system setup
		/bin/mkdir -p /dev/shm
		/bin/mount -t tmpfs tmpfs /dev/shm

		/bin/mkdir -p /data/sednaserver/sedna-db
		/bin/mkdir -p /data/sednaserver/sedna-db/data
		/bin/mkdir -p /data/sednaserver/sedna-db/cfg
		/bin/mkdir -p /data/sednaserver/sedna-db/share

		/bin/rm -rf /home/onair/sedna/data
		/bin/ln -s /data/sednaserver/sedna-db/data /home/onair/sedna/data

		rm  -rf /home/onair/sedna/cfg
		ln -s /data/sednaserver/sedna-db/cfg /home/onair/sedna/cfg

		/bin/cp /home/onair/sedna/share/sedna_auth_md.xml /data/sednaserver/sedna-db/share
		rm  -rf /home/onair/sedna/share
		ln -s /data/sednaserver/sedna-db/share /home/onair/sedna/share

    fi


fi

# Client Build requirements: cleanout previous build area
/bin/rm -rf /data/client_build

# NAS requirements: cleanout previous mount points
rm -rf /data/DSS/data/portal/site/NAS

#create links for dms upgrade build
/bin/mkdir -p /data/mediaroom/upgrade/IMAGES
ln -s /data/mediaroom/upgrade/IMAGES /home/onair/modules/mediaroom/public/upgrade_IMAGES

#create links for ngs upgrade build
/bin/mkdir -p /data/mediaroom/upgrade/ngs/IMAGES
ln -s /data/mediaroom/upgrade/ngs/IMAGES /home/onair/modules/mediaroom/public/ngs_IMAGES

