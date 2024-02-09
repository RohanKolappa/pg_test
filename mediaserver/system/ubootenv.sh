#!/bin/sh
#First clear out everything get it to a known state
sernum=`fw_printenv sernum`
echo "Setting default boot arguments, serial, console etc"
fw_setenv sernum ${sernum}
fw_setenv bootargs "init=rootfs.gz rootwait console=ttyS2,115200 uart_dma"
echo "Setting default boot delay"
fw_setenv bootdelay 10
echo "Setting up nor to load kernel, fpga and root file system"
fw_setenv norboot "watchdog stop; smc upload 0x800a0000 3300a0; cp.l 0x80aa0000 0x3000000 c0000;cp.l 0x80da0000 0x6000000 A00000; bootm 0x3000000 0x6000000"
echo "Setting up usb to load kernel, fpga and root file system"
fw_setenv usbboot "watchdog stop; usb start; fatload usb 0:1 0x3000000 \${sys_type}.bit; smc upload 0x3000000 33000a0; fatload usb 0:1 0x3000000 OA; fatload usb 0:1 0x6000000 onair.gz; bootm 0x3000000 0x6000000"
echo "Setting up SD card to load kernel, fpga and root file system"
fw_setenv sdboot "watchdog stop; mmcinfo; fatload mmc 0:1 0x3000000 \${sys_type}.bit; smc upload 0x3000000 33000a0; fatload mmc 0:1 0x3000000 OA; fatload mmc 0:1 0x6000000 onair.gz; bootm 0x3000000 0x6000000"
echo "Set the defaults boot command to boot from SD only"
fw_setenv bootcmd "run sdboot"
