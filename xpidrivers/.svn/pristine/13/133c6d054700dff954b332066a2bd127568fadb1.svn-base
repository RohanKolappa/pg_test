#!/bin/sh
/usr/sbin/fw_setenv loadfpga 'watchdog stop; mmcinfo ; if fatload mmc 0:1 ${default_load_addr} sys_type_xpi_decoder ; then setenv sys_type "xpi_decoder"; else  setenv sys_type "xpi_encoder"; fi ; if fatload mmc 0:1 ${default_load_addr} sys_type_xpi_bringup ; then setenv sys_type "xpi_bringup"; else ; fi ; fatload mmc 0:1 ${default_load_addr} ${sys_type}.bit;smc upload ${default_load_addr} ${fpga_sz}'
/usr/sbin/fw_setenv bootcmd 'gpio clear 95;smc regwrite 0xf044 0;run loadfpga;run mmc_irfs_boot'
