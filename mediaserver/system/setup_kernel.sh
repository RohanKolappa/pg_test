#!/bin/sh

# Source common library
. /etc/scripts/common.rc

# Mount the proc file system
/bin/busybox mount -t proc proc /proc

# Mount the sys file system
/bin/busybox mount -t sysfs sysfs /sys

# Remount the rootfs file system as read/write
/bin/busybox mount -n -o remount,rw  /


# Install busybox utilities
/bin/busybox --install -s


# Override busybox's version of df with full blown version version
if [ ! -f /etc/config/arm_system ] ; then
    if [ -f /bin/df.system ] ; then
        /bin/rm -f /bin/df
        /bin/mv /bin/df.system /bin/df
        # Fedora's df command needs /etc/mtab file
        # Creating a link to /proc/mounts suffices
        /bin/ln -sf /proc/mounts /etc/mtab
    fi
fi

# Remove busybox poweroff command so that the kernel can
# never shut us down (ACPI thermal trip-points)
/bin/rm -f /sbin/poweroff

# Setup devices in /dev using udev facility 
if [ -f /etc/config/arm_system ] ; then
    /bin/mount -t tmpfs mdev /dev
fi

/bin/mkdir /dev/pts
/bin/mount -t devpts devpts /dev/pts
/bin/echo /sbin/mdev > /proc/sys/kernel/hotplug
/sbin/mdev -s
/bin/chmod og+rw /dev/null

# Create kernel loadable module dependencies
/sbin/depmod -a

# Assign local host IP address
/sbin/ifconfig lo 127.0.0.1

# Create the /tmp partition using tmpfs limited to 16 MB
echo "Creating /tmp partition"
/bin/mkdir -p /tmp
/bin/mount  -t tmpfs -o size=16m tmpfs /tmp

# Create the /var partition using tmpfs limited to 16 MB
# We can't do this on xpi since /var/www hosts the gui web pages
if [ ! -f /etc/config/arm_system ] ; then
    echo "Creating /var partition"
    /bin/mkdir -p /var
    /bin/mount  -t tmpfs -o size=16m tmpfs /var
    echo "Creating /var/log partition"
    /bin/mkdir -p /var/log
    /bin/mount  -t tmpfs -o size=16m tmpfs /var/log
fi
/bin/mkdir -p /var/run
/bin/mkdir -p /var/lib

# Create the /home/onair partition using tmpfs limited to 128 MB
# echo "Creating /home/onair partition"
# /bin/mount  -t tmpfs -o size=128m tmpfs /home/onair

echo "Creating /usr/local partition"
if [ ! -f /etc/config/arm_system ] ; then
    /bin/mkdir -p /usr/local
    /bin/mount  -t tmpfs -o size=128m tmpfs /usr/local
fi

if [ ! -f /etc/config/arm_system ] ; then
    # This is needed for USB LCD panel to work
    /bin/mount -t usbfs none /proc/bus/usb
fi

# Setup kernel variables
if [ ! -f /etc/config/arm_system ] ; then
    /sbin/sysctl -w kernel.hostname="localhost"
    /sbin/sysctl -w net.core.rmem_max=52428800
    /sbin/sysctl -w net.core.wmem_max=52428800
    /sbin/sysctl -w net.core.netdev_max_backlog=2000
    /sbin/sysctl -w kernel.sem="250 64000 32 256"
fi

# Load all needed kernel loadable modules

KERNEL_VERSION=`uname -r`
echo "Regenerating module information for ${KERNEL_VERSION}..."
/sbin/depmod -a

if [ ! -f /etc/config/arm_system ] ; then
    # This is a bad way to probe for modules, it is converted to
    # modprobe but still not very good
    /sbin/modprobe mmc_core
    /sbin/modprobe mmc_block
    /sbin/modprobe sdhci

    /sbin/modprobe xt_state
    /sbin/modprobe e1000
    /sbin/modprobe e1000e
    /sbin/modprobe igb
    /sbin/modprobe tg3
    /sbin/modprobe r8169
    /sbin/modprobe usbserial
    /sbin/modprobe ftdi_sio
    /sbin/modprobe 3w-9xxx
    /sbin/modprobe 3w-sas

    # Modules need for DMS1550 
    /sbin/modprobe megaraid

    # Modules needed for XP220 compact flash
    /sbin/modprobe ata_piix

    # Load EHCI controller needed for DMS USB keyboard -- But don't load
    # on Pentium 4 SBCs (XP200) it will break USB keyboard/mouse
    /bin/cat /proc/cpuinfo | grep "Pentium(R) 4 CPU 2.40GHz" > /dev/null 2>&1
    if [ x$? != "x0" ] ; then
        /sbin/modprobe ehci-hcd
    fi

    # Modules needed for DMS USB frontpanel and XP USB serial & Keyboard/Mouse
    /sbin/modprobe uhci-hcd
                                                                                
    # Modules needed for VMWare based virtualization
    /sbin/modprobe pcnet32
    /sbin/modprobe scsi_transport_spi
    /sbin/modprobe mptbase
    /sbin/modprobe mptscsih
    /sbin/modprobe mptspi
    # Modules needed for XP100 SD Disks
    /sbin/modprobe sdhci-pci


    # load a bunch of standard modules. These modules were loaded by
    # EMS on an x64 system.
    for i in ablk_helper acpi_power_meter aesni_intel aes_x86_64 ahci autofs4 binfmt_misc bluetooth btrfs button coretemp crc16 crc32c_generic crc32c_intel crc32_pclmul crct10dif_pclmul cryptd dcdbas dell_smbios dm_mod drm ecb edac_core evdev ext4 fat fscrypto fuse gf128mul ghash_clmulni_intel glue_helper hfs hfsplus hid hid_generic i2c_i801 i2c_smbus intel_powerclamp intel_rapl intel_rapl_perf intel_uncore ip6table_filter ip6_tables ipmi_msghandler ipmi_si iptable_filter iptable_mangle ip_tables ipt_REJECT irqbypass iTCO_vendor_support iTCO_wdt jbd2 jfs kvm kvm_intel libahci libata libcrc32c libphy lpc_ich lrw mbcache megaraid_sas mei mei_me mfd_core minix msdos nf_conntrack nf_conntrack_ftp nf_conntrack_ipv4 nf_conntrack_irc nf_conntrack_pptp nf_conntrack_proto_gre nf_conntrack_sip nf_defrag_ipv4 nfnetlink nfnetlink_log nfnetlink_queue nf_reject_ipv4 ntfs pcspkr pps_core ptp qnx4 raid6_pq rfkill sch_sfq sch_tbf scsi_mod sd_mod sg shpchp skx_edac sparse_keymap tg3 ufs usb_common usbcore usbhid vfat video wmi x86_pkg_temp_thermal xfs xhci_hcd xhci_pci xor x_tables xt_connmark xt_conntrack xt_helper xt_limit xt_multiport xt_NFLOG xt_tcpudp squashfs loop zstd_decompress
    do
        echo "Inserting module ${i}, ..."
        /sbin/modprobe ${i}
    done	

    # mount /tmp/sysrcd.dat /media/usb0/ -t squashfs -o loop
fi

# FIXUPs
echo "Creating device nodes..."

c=0
for d in /dev/sda /dev/sdb /dev/sdc /dev/sdd /dev/sde /dev/sdf /dev/sdg /dev/sdh /dev/sdi /dev/sdj
do
  let m=${c}*16
  /bin/mknod ${d} b 8 ${m}
  echo "Creating device block ${d} with 8 ${m}"
  let c=${c}+1
  for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
  do
    let dm=${m}+${i}
    echo "Creating device block ${d}${i} with 8 ${dm}"
    /bin/mknod ${d}${i} b 8 ${dm}
  done
done

echo "Correct busybox permissions..."
chmod u+s /bin/busybox

echo "Increase memory for root tmpfs to 768M"
mount -o remount,size=728m tmpfs /

# Create securetty file for root access
/bin/touch /etc/securetty
