#!/bin/sh

NFSSERVER=10.1.5.12
MOUNTPOINT=/home/nfstest
MOUNTDIR=/mnt/data


mount_datastore() {

    # Have we already mounted the NFS volume??
    /bin/mount | grep ${NFSSERVER}:${MOUNTPOINT} > /dev/null 2>&1
    if [ x$? = "x0" ]; then
        echo "Already Mounted:  ${NFSSERVER}:${MOUNTPOINT} on ${MOUNTDIR}" 
        echo "Nothing to do!"
        return
    fi

    # Mount the NFS directory
    /bin/mkdir -p ${MOUNTDIR}

    /bin/mount -t nfs -o hard,intr,nolock ${NFSSERVER}:${MOUNTPOINT} ${MOUNTDIR}
    if [ x$? = "x0" ]; then
        echo "Mounted  ${NFSSERVER}:${MOUNTPOINT} on ${MOUNTDIR}" 
    fi

}

unmount_datastore() {

    # Are we currently mounted??
    /bin/mount | grep ${NFSSERVER}:${MOUNTPOINT} > /dev/null 2>&1
    if [ x$? = "x1" ]; then
        echo "Not currently mounted. Nothing to do."
        return
    fi

    # Unmounted
    /bin/umount ${MOUNTDIR}

    # Did we unmount sucessfully??
    /bin/mount | grep ${NFSSERVER}:${MOUNTPOINT} > /dev/null 2>&1
    if [ x$? = "x0" ]; then
        echo "Failed to unmount ${NFSSERVER}:${MOUNTPOINT} from ${MOUNTDIR}."
        return
    fi

    echo "Unmounted ${NFSSERVER}:${MOUNTPOINT} from ${MOUNTDIR}"
}

case "$1" in
  start)
        mount_datastore
        ;;


  stop)
        unmount_datastore
        ;;

  *)
        echo "Usage: $0 [start|stop]"
        exit 1

esac
