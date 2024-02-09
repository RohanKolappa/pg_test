# Makefile for V2O build

ifneq ($(VERBOSE),1)
QUIET_CC = @echo "CC $@";
QUIET_DEP = @echo "DEP $@";
QUIET_BIN = @echo "BIN $@";
QUIET_CLEAN = @echo "CLEAN";
QUIET_GEN = @echo "GEN $@";
QUIET_STRIP = @echo "STRIP $@";
QUIET_LIB = @echo "LIB $@";
QUIET = @ 
endif

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
DEV_DIR=dev/dev_arm
else
DEV_DIR=dev/dev_i386
endif

GENERATED_DIR := generated

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CFLAGS = -D__XPI__
else
CFLAGS =
endif

XERCESDIR = ../../${DEV_DIR}/xerces
XERCESINCDIR = ${XERCESDIR}/include
XERCES_INCLUDE = -I ${XERCESINCDIR}
LIBXERCESDIR = ${XERCESDIR}/lib
#XERCESLIBS = -L ${LIBXERCESDIR} -Wl,-Bstatic -lxerces-c -Wl,-Bdynamic
XERCESLIBS = -L ${LIBXERCESDIR} -lxerces-c

LOG4CDIR = ../../${DEV_DIR}/log4c
LOG4CINCDIR = ${LOG4CDIR}/include
LOG4C_INCLUDE = -I ${LOG4CINCDIR}
LIBLOG4CDIR = ${LOG4CDIR}/lib
LOG4CLIBS = -L ${LIBLOG4CDIR} -llog4c

LIBXKBCOMMONDIR = ../../${DEV_DIR}/libxkbcommon
LIBXKBCOMMONINCDIR = ${LIBXKBCOMMONDIR}/include
LIBXKBCOMMON_INCLUDE = -I ${LIBXKBCOMMONINCDIR}
LIBLIBXKBCOMMONDIR = ${LIBXKBCOMMONDIR}/lib
LIBXKBCOMMONLIBS = -L ${LIBLIBXKBCOMMONDIR} -lxkbcommon

#BLMDIR = ../../${DEV_DIR}/BLM
#BLMINCDIR = ${BLMDIR}/
#BLM_INCLUDE = -I ${BLMINCDIR}
#LIBBLMDIR = ${BLMDIR}/lib
#BLMLIBS = -L ${LIBBLMDIR} -lblm

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
CDKDIR = ../../${DEV_DIR}/cdk
else
CDKDIR = ../cdk
endif
CDKINCDIR = ${CDKDIR}/include
CDK_INCLUDE = -isystem ${CDKINCDIR}
LIBCDKDIR = ${CDKDIR}/lib
CDKLIBS = -L ${LIBCDKDIR} -lcdk

GDDIR = ../../${DEV_DIR}/gd
GDINCDIR = ${GDDIR}/include
GD_INCLUDE = -I ${GDINCDIR}
GDLIBS = -L ${GDDIR}/lib -lgd 

KERNELDIR = ../../${DEV_DIR}/linux-kernel-headers
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
DRIVER_INCLUDE := -I ../../xpidrivers/include -I ../../xpidrivers/driver -I ../../${DEV_DIR}/i2c-tools/include/linux -I ${KERNELDIR}/include/media  -I drivers
else
DRIVER_INCLUDE := -Idrivers
endif

GENERATED_DIR := generated

GENERATED_INCLUDE := -I ${GENERATED_DIR}

MENUCLI_DIR := menucli

MENUCLI_INCLUDE := -I ${MENUCLI_DIR}

UTIL_INCLUDE := -I../../v2dlib ${CDK_INCLUDE} ${GD_INCLUDE} ${LOG4C_INCLUDE} ${LIBXKBCOMMON_INCLUDE} ${DRIVER_INCLUDE} ${GENERATED_INCLUDE} ${MENUCLI_INCLUDE} -I .

#UTIL_INCLUDE += -I../../${DEV_DIR}/omnitek/include -I../../${DEV_DIR}/i2c-tools/include/linux



CPP_INCLUDE :=  ${XERCES_INCLUDE} ${LOG4C_INCLUDE} ${DRIVER_INCLUDE} ${GENERATED_INCLUDE} 


ifneq (,$(findstring arm,$(CROSS_COMPILE)))
LIBS= -Ldrivers -lv2okernel -L../../${DEV_DIR}/png/lib/ -lpng -L../../${DEV_DIR}/libusb/lib -lusb-1.0 ${LOG4CLIBS} ${XERCESLIBS} ${LIBXKBCOMMONLIBS} -lcrypt -lmenu -lform ${CDKLIBS} ${GDLIBS} -L../../${DEV_DIR}/fontconfig/lib -lfontconfig -lncurses -lz -lm 
else
LIBS= -Ldrivers -lv2okernel ${LOG4CLIBS} ${XERCESLIBS} ${CDKLIBS} ${LIBXKBCOMMONLIBS} -L../../${DEV_DIR}/fontconfig/lib -lfontconfig -lusb -lcrypt -lmenu -lform -lncurses -lz -lm 
endif


I386LAGS := -DCONFIG_FEATURE_SHADOWPASSWDS

WARNFLAGS  := -Wall -Wstrict-prototypes -Wmissing-prototypes -Wno-unused
CPPWARNFLAGS  := -Wall
MOPTFLAGS  := -O
UOPTFLAGS  := -g -O
I386FLAGS  := 

ifeq ($(DEBUG),1)
UOPTFLAGS += -DV2ODEBUG=1
endif

CC = ${CROSS_COMPILE}gcc
CPP = ${CROSS_COMPILE}g++
AR = ${CROSS_COMPILE}ar
LD = ${CROSS_COMPILE}ld
STRIP = ${CROSS_COMPILE}strip

#DMALLOC_FLAGS := -DDMALLOC -DDMALLOC_FUNC_CHECK
DMALLOC_FLAGS :=

UTIL_FLAGS  := ${CFLAGS} ${DMALLOC_FLAGS} ${WARNFLAGS} ${UOPTFLAGS} ${I386FLAGS} ${UTIL_INCLUDE} 

CPP_FLAGS  := ${DMALLOC_FLAGS} ${CPPWARNFLAGS} ${UOPTFLAGS} ${I386FLAGS} ${CPP_INCLUDE}

ifneq (,$(DMALLOC_FLAGS))
LIBS += -ldmalloc 
endif

GTESTDIR = ../../${DEV_DIR}/gmock
GTESTCFLAGS = -I ${GTESTDIR}/include ${DRIVER_INCLUDE} ${GENERATED_INCLUDE} ${MENUCLI_INCLUDE}
GTESTLIB = ${GTESTDIR}/lib/gmock_main.a ${GTESTDIR}/lib/gtest_utils.a 
GTESTLIB += -lpthread


ifneq (,$(findstring arm,$(CROSS_COMPILE)))
BOOSTLIB = -L../../${DEV_DIR}/boost/lib -lboost_filesystem -lboost_system 
else
BOOSTLIB = -lboost_filesystem  
endif

TESTOBJS = test_commonutils.o \
           test_system_utils.o \
           test_slice_size.o \
           test_text_utils.o \
           test_validations.o \
           test_frame_counter_utils.o \
           test_xml_utils.o

TEST_OBJS = ${patsubst %.o, test/%.o, ${TESTOBJS}}
TEST_SRCS = $(TEST_OBJS:.o=.cpp)

OBJS    := ${patsubst %.c, %.o, ${wildcard *.c}}

KBD_SRCS    := ${patsubst %.o, %.c, ${KBD_OBJS}}

MENUCLI_OBJS := ${MENUCLI_DIR}/vmain.o \
                ${MENUCLI_DIR}/custmenu.o \
                ${MENUCLI_DIR}/cli.o \
                ${MENUCLI_DIR}/nargv.o


ifeq (,$(findstring arm,$(CROSS_COMPILE)))
MAINT_OBJS := 	maint_xp.o  
else
MAINT_OBJS := 	maint_xpi.o  
endif

UTIL_OBJS :=    loadlut.o \
                loadnet.o \
                configureServerlists.o \
                configureVideo.o \
                displayVersion.o \
                configureDefaults.o \
                commandSaveToFlash.o \
                saveVideoParamsToLUT.o \
                testLEDs.o \
                configureLog.o \
                portSelector.o \
                adjustVideoParameters.o \
                tester.o \
                getuserinput.o \
                dumpsram.o \
                dump_config.o \
                auth_list_menu.o \
                routeList.o \
                dhcpc_utils.o \
                vlog.o \
                menuutils.o \
                kmtest.o \
                sync_cp.o \
                serverutils.o \
                clientutils.o \
                xkb_utils.o \
                commonutils.o \
                cli_utils.o \
                validationroutines.o \
				text_utils.o \
                system_utils.o \
                device_detection.o \
                configfile_parser.o \
                kbms_utils.o \
                strmsrv_comm.o \
                upgrade_software_command.o \
                reboot_system_command.o \
                ha_commands.o \
                set_time_command.o \
                dreg.o \
                hal.o \
                hal_console.o \
                hal_serial.o \
                hal_event_device.o \
                hal_hwinfo.o \
                hal_LED.o \
		        hal_edid.o \
		        hal_v4l2.o \
                hal_vpd.o \
		        hal_nethdr.o \
		        hal_rtpfilter.o \
		        xpi_i2c_control.o \
		        xpi_gpio_control.o \
		        xpi_pll_control.o \
                frame_counter_utils.o \
                vlib.o \
		        xp_i2c_control.o \
		        osd_control.o \
		        temperature_control.o \
                loadfpga.o \
                hald.o \
                dump.o \
                license_verifier.o \
                setconsole.o \
                configureDatabase.o \
                override_list_menu.o \
                override_utils.o \
                splash_list_menu.o \
				loadedideeprom.o \
                edid_list_menu.o \
                clear_edid_list_menu.o \
                urlutils.o \
                edid_utils.o \
                configureEdid.o \
				chromakey_control.o \
                displayGuideRuler.o \
				lut_utils.o \
				usbutils.o \
				if_list_config.o \
                standalone.o \
                send_notification.o \
                check_device_command.o \
                get_license_pin.o \
                set_license_file.o \
                verify_license.o 

LIB_OBJS :=     commonutils.o \
                cli_utils.o \
                validationroutines.o \
				text_utils.o \
                system_utils.o \
                device_detection.o \
                configfile_parser.o \
                kbms_utils.o \
                vlog.o \
                clientutils.o \
                serverutils.o \
                xkb_utils.o \
                hal.o \
                hal_console.o \
                hal_serial.o \
                hal_event_device.o \
                hal_hwinfo.o \
                hal_LED.o \
				hal_edid.o \
				hal_v4l2.o \
                hal_vpd.o \
		        hal_nethdr.o \
		        hal_rtpfilter.o \
                frame_counter_utils.o \
                vlib.o \
		        xp_i2c_control.o \
		        osd_control.o \
		        temperature_control.o \
		        xpi_i2c_control.o \
		        xpi_gpio_control.o \
		        xpi_pll_control.o \
                verify_license.o  \
                get_license_pin.o \
                rx_io_params_menu_gen_lib.o \
                rx_io_params_menu.o \
                tx_io_params_menu_gen_lib.o \
                tx_io_params_menu.o \
                tx_kbm_params_menu_gen_lib.o \
                tx_kbm_params_menu.o \
                kbcons_menu_gen_lib.o \
                kbcons_menu.o \
                tx_kblocal_menu_gen_lib.o \
                tx_kblocal_menu.o \
                tx_thumbnail_params_menu_gen_lib.o \
                tx_thumbnail_params_menu.o \
                rx_kbm_params_menu_gen_lib.o \
                rx_kbm_params_menu.o \
                rx_kblocal_menu_gen_lib.o \
                rx_kblocal_menu.o \
                rx_kbremote_menu_gen_lib.o \
                rx_kbremote_menu.o \
                strmsrvAccess.o \
                xmlUtils.o \
                strmsrv_comm.o \
                tx_splash_params_menu_gen_lib.o \
                tx_splash_params_menu.o \
                rx_splash_params_menu_gen_lib.o \
                rx_splash_params_menu.o \
		        color_params_menu_gen_lib.o \
		        color_params_menu.o \
		        xpi_color_params_menu_gen_lib.o \
		        xpi_color_params_menu.o \
		        lut_list_menu_gen_lib.o \
				lut_utils.o \
				override_list_menu_lib.o \
                override_utils.o \
                display_params_menu_gen_lib.o \
                display_params_menu.o \
				override_params_menu_gen_lib.o \
				override_params_menu.o \
				edid_list_menu_lib.o \
                edid_utils.o \
				chromakey_control.o \
                usbutils.o \
                tx_rfb_params_menu_gen_lib.o \
                tx_rfb_params_menu.o \
                sync_edid_eeprom_menu_gen_lib.o \
                sync_edid_eeprom_menu.o \
                if_list_menu_gen_lib.o \
                if_list_config.o \
                tx_connection_params_menu_gen_lib.o \
                tx_connection_params_menu.o \
                stand_alone_mode_menu_gen_lib.o \
                kbconsole_menu_gen_lib.o \
				kbconsole_menu.o \
                xmpp_linklocal_menu_gen_lib.o \
                xmpp_agent_menu_gen_lib.o \
                xmpp_server_menu_gen_lib.o \
                xmpp_linklocal_menu.o \
                xmpp_agent_menu.o \
                linuxha_menu_gen_lib.o \
                codec_version_menu_gen_lib.o \
				tx_kbm_layout_menu_gen_lib.o \
				tx_kbm_layout_menu.o \
				rx_kbm_layout_menu_gen_lib.o \
				rx_kbm_layout_menu.o \
				rx_local_mouse_pointer_gen_lib.o \
				rx_local_mouse_pointer.o

GENERATOR_XML := sample_menu.xml \
                 sample_immediate_menu.xml \
                 dhcp_server_menu.xml \
                 xmpp_agent_menu.xml \
                 xmpp_linklocal_menu.xml \
                 xmpp_server_menu.xml \
                 max_media_clients.xml \
                 ntp_menu.xml \
                 ntp_server_menu.xml \
                 admin_menu.xml \
                 banner_menu.xml \
                 cpu_status_menu.xml \
                 memory_status_menu.xml \
                 route_status_menu.xml \
                 streaming_status_menu.xml \
                 software_image_status_menu.xml \
                 front_panel_status_menu.xml \
                 system_clock_status_menu.xml \
                 firmware_image_status_menu.xml \
                 network_ports_status_menu.xml \
                 dns_menu.xml \
                 telnet_server_menu.xml \
                 tx_rfb_params_menu.xml \
                 sync_edid_eeprom_menu.xml \
                 tx_video_params_menu.xml \
                 xp_tx_video_params_menu.xml \
                 rx_video_params_menu.xml \
                 rx_input_video_params_menu.xml \
                 tx_io_params_menu.xml \
                 rx_io_params_menu.xml \
                 tx_control_ports_menu.xml \
                 rx_av_ports_menu.xml \
                 mpegts_rx_avparams_menu.xml \
                 udp_rx_avparams_menu.xml \
                 rtp_rx_avparams_menu.xml \
                 multicast_params_menu.xml \
                 xmpp_agent_status_menu.xml \
                 disk_status_menu.xml \
                 xmpp_server_status_menu.xml \
                 linuxha_status_menu.xml \
                 stand_alone_mode_menu.xml \
                 kbconsole_menu.xml \
                 display_params_menu.xml \
                 board_info_menu.xml \
                 override_params_menu.xml \
                 if_list_menu.xml \
                 tx_connection_params_menu.xml \
                 tx_kbm_params_menu.xml \
                 kbcons_menu.xml \
                 tx_kblocal_menu.xml \
                 tx_thumbnail_params_menu.xml \
                 xp_audio_mixer_menu.xml \
                 rx_kbm_params_menu.xml \
                 rx_kblocal_menu.xml \
                 rx_kbremote_menu.xml \
                 sample_status_menu.xml \
                 temperature_status_menu.xml \
                 display_status_menu.xml \
                 tx_connection_status_menu.xml \
                 rx_connection_status_menu.xml \
                 tx_splash_params_menu.xml \
                 rx_splash_params_menu.xml \
                 color_params_menu.xml \
                 xpi_color_params_menu.xml \
                 xp_firmware_image_status_menu.xml \
                 xpi_firmware_image_status_menu.xml \
                 force_splash_screen_menu.xml \
                 tx_addside_reg_status_menu.xml \
                 rx_dropside_reg_status_menu.xml \
                 tx_fpga_reg_status_menu.xml \
                 rx_fpga_reg_status_menu.xml \
                 sample_display.xml \
                 nas_list_menu.xml \
                 test_menu_one.xml \
                 test_list_menu.xml \
                 natzone_list_menu.xml \
                 host_list_menu.xml \
                 static_route_list_menu.xml \
                 tx_display_io_info_status_menu.xml \
                 rx_display_io_info_status_menu.xml \
                 nas_status_menu.xml \
                 database_management_menu.xml \
                 database_management_status_menu.xml \
                 sample_pager_status_menu.xml \
                 eeprom_edid_status_menu.xml \
                 monitor_edid_status_menu.xml \
                 lut_list_menu.xml \
                 sample_list_menu.xml \
                 var_log_status_menu.xml \
                 read_video_statistics_status_menu.xml \
                 read_audio_statistics_status_menu.xml \
                 write_video_statistics_status_menu.xml \
                 write_audio_statistics_status_menu.xml \
                 frame_counter_status_menu.xml \
                 debug_reg_parms_menu.xml \
                 debug_reg_status_menu.xml \
                 debug_regII_status_menu.xml \
                 omnitek_reg_parms_menu.xml \
                 omnitek_reg_status_menu.xml \
                 omnitek_regII_status_menu.xml \
                 s6_reg_parms_menu.xml \
                 s6_reg_status_menu.xml \
                 s6_regII_status_menu.xml \
                 disk_init_menu.xml \
                 disk_rebuild_menu.xml \
                 tx_av_buffers_menu.xml \
                 rx_av_buffers_menu.xml \
                 rfb_connection_list_menu.xml \
                 chromakey_status_menu.xml \
                 chromakey_params_menu.xml \
                 sample_log.xml \
                 logging_level_menu.xml \
                 linuxha_menu.xml \
                 reverse_audio_menu.xml \
                 fan_speed_menu.xml \
                 fan_status_menu.xml \
                 xpi_fan_status_menu.xml \
                 protocol_version_menu.xml \
                 v2d_connect_menu.xml \
                 v2d_streaming_profile_menu.xml \
                 codec_version_menu.xml \
                 tx_kbm_layout_menu.xml \
                 rx_kbm_layout_menu.xml \
                 rx_local_mouse_pointer.xml \
                 demos_menu.xml

CPP_OBJS :=     remoteConfig.o remoteStatus.o strmsrvAccess.o xmlUtils.o

MENUCLI_SRCS    := ${patsubst %.o, %.c, ${MENUCLI_OBJS}} 

MAINT_SRCS    := ${patsubst %.o, %.c, ${MAINT_OBJS}} 

UTIL_SRCS    := ${patsubst %.o, %.c, ${UTIL_OBJS}} 

MENU_OBJS    := ${patsubst %.xml, %.o, ${GENERATOR_XML}} 

MENU_SRCS    := ${patsubst %.o, %.c, ${MENU_OBJS}} 

CPP_SRCS     := ${patsubst %.o, %.cpp, ${CPP_OBJS}} 

GENERATED_SRCS  := ${patsubst %.xml, ${GENERATED_DIR}/%_gen.c, ${GENERATOR_XML}} 
GENERATED_OBJS  := ${patsubst %.xml, ${GENERATED_DIR}/%_gen.o, ${GENERATOR_XML}} 
GENERATED_HDRS  := ${patsubst %.xml, ${GENERATED_DIR}/%_gen.h, ${GENERATOR_XML}} 
GENERATED_LIBOBJS  := ${patsubst %.xml, %_gen_lib.o, ${GENERATOR_XML}} 

GENERATED_DOCS  := ${patsubst %.xml, ${GENERATED_DIR}/%_gen.txt, ${GENERATOR_XML}} 

.PHONY: clean drivers 

ifneq (,$(findstring arm,$(CROSS_COMPILE)))
all: drivers ${GENERATED_DIR}/generatedHeaders.h vutils maint libv2o.so V2OTests 
else
all: drivers ${GENERATED_DIR}/generatedHeaders.h vutils maint libv2o.so V2OTests 
endif

CODE_GENERATORS := ${MENUCLI_DIR}/menu_builder.tcl ${MENUCLI_DIR}/config_menu_builder.tcl ${MENUCLI_DIR}/status_menu_builder.tcl ${MENUCLI_DIR}/list_menu_builder.tcl 

PWD := $(shell pwd)


drivers:
	${MAKE} -C drivers

clidoc: ${GENERATED_DOCS}
	${QUIET_GEN} cat ${GENERATED_DIR}/*_gen.txt > ${GENERATED_DIR}/clidoc.txt 
	${QUIET} rm -f generated/*_gen.txt 
	${QUIET} cp ${GENERATED_DIR}/clidoc.txt ../docs/.

vutils: ${MENUCLI_OBJS} ${UTIL_OBJS} ${MENU_OBJS} ${GENERATED_OBJS} ${CPP_OBJS} libv2o.so
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
	${QUIET_BIN} ${CPP}  -g -o $@ ${MENUCLI_OBJS} ${UTIL_OBJS} ${MENU_OBJS} ${GENERATED_OBJS} ${CPP_OBJS} ${LIBS} 
else
	${QUIET_BIN} ${CC}  -g -o $@ ${MENUCLI_OBJS} ${UTIL_OBJS} ${MENU_OBJS} ${GENERATED_OBJS} ${CPP_OBJS} ${LIBS} -lusb -lpng; 
endif


maint: ${MAINT_OBJS} ${MENUCLI_DIR}/custmenu.o ${UTIL_OBJS} ${MENU_OBJS} ${GENERATED_OBJS} ${CPP_OBJS} libv2o.so
	${QUIET_BIN} ${CPP} -g -o $@ ${MAINT_OBJS} ${MENUCLI_DIR}/custmenu.o ${UTIL_OBJS} ${MENU_OBJS} ${GENERATED_OBJS} ${CPP_OBJS} ${LIBS}; \

chromakey_params_menu_lib.o: chromakey_params_menu.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -DLIBONLY -c -o $@ $< 

override_list_menu_lib.o: override_list_menu.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -DLIBONLY -c -o $@ $< 

edid_list_menu_lib.o: edid_list_menu.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -DLIBONLY -c -o $@ $< 

libv2o.so: ${LIB_OBJS}
	${QUIET_LIB} ${CC} ${CFLAGS} -shared -o $@ ${LIBS} $^ 

${TEST_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CPP} -c ${CFLAGS} -I . ${GTESTCFLAGS} -o $@ $<

clean:
	${QUIET_CLEAN} /bin/rm -f ${UTIL_OBJS} ${MENUCLI_OBJS} ${MENU_OBJS} ${CPP_OBJS} ${GENERATED_OBJS} ${GENERATED_SRCS}  ${GENERATED_HDRS} fpga_module.o vutils .depend modules.order *.d libv2o.so V2OTests vutils maint generated/* menucli/*.o *.o  menucli/*.d test/*.d test/*.o
	${MAKE} -C drivers clean

distclean: clean
	${QUIET_CLEAN} /bin/rm -f TAGS *~ tut *.bak

V2OTests : ${TEST_OBJS} ${UTIL_OBJS} ${CPP_OBJS} ${MENU_OBJS} ${MENUCLI_DIR}/custmenu.o  ${GENERATED_OBJS} libv2o.so
ifneq (,$(findstring arm,$(CROSS_COMPILE)))
	${QUIET_BIN} ${CPP} -Wall -o $@ -L"./" ${TEST_OBJS} \
	${GTESTLIB} ${BOOSTLIB} ${UTIL_OBJS} ${CPP_OBJS} ${MENU_OBJS} ${MENUCLI_DIR}/custmenu.o  ${GENERATED_OBJS} ${LIBS} \
	-L. -lv2o 
else
	${QUIET_BIN} ${CPP} -Wall -o $@ -L"./" ${TEST_OBJS} \
	${GTESTLIB} ${BOOSTLIB} ${UTIL_OBJS} ${CPP_OBJS} ${MENU_OBJS} ${MENUCLI_DIR}/custmenu.o  ${GENERATED_OBJS} ${LIBS} -lusb -lxerces-c -lcrypt -lmenu -lform -lcdk -lncurses -lpng -lz -lm 
endif

test: V2OTests
	export LD_LIBRARY_PATH=.:drivers:${LIBXERCESDIR}:${LIBLOG4CDIR}:${LIBLIBXKBCOMMONDIR}; \
	./V2OTests ${TESTPARAMS}

tags:
	etags ${UTIL_SRCS} *.h

${MENUCLI_OBJS}: ${MENUCLI_DIR}/%.o: ${MENUCLI_DIR}/%.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -c -o $@ $< 

${MAINT_OBJS}: %.o: %.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -c -o $@ $< 

${UTIL_OBJS}: %.o: %.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -c -o $@ $< 

${MENU_OBJS} custmenu.o:  ${GENERATED_SRCS} ${GENERATED_DIR}/generatedHeaders.h

${MENU_OBJS}: %.o: %.c 
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -c -o $@ $< 

${CPP_OBJS}: %.o: %.cpp
	${QUIET_CC} ${CPP} ${CPP_FLAGS} -c -o $@ $< 

${GENERATED_OBJS} :%.o: %.c 
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -c -o $@ $< 

${GENERATED_LIBOBJS} :%_lib.o: ${GENERATED_DIR}/%.c
	${QUIET_CC} ${CC} ${UTIL_FLAGS} -DLIBONLY -c -o $@ $<  

${GENERATED_SRCS}: ${GENERATED_DIR}/%_gen.c: %.xml ${CODE_GENERATORS}
	${QUIET_GEN} ${MENUCLI_DIR}/menu_builder.tcl $< generated/  
	${QUIET} indent -kr -nce -nut ${GENERATED_DIR}/$*_gen.c ${GENERATED_DIR}/$*_gen.h

${GENERATED_DOCS}: ${GENERATED_DIR}/%_gen.txt: %.xml ${MENUCLI_DIR}/menu_builder.tcl
	 ${QUIET_GEN} ${MENUCLI_DIR}/menu_builder.tcl $< generated/ create_doc

${GENERATED_DIR}/generatedHeaders.h: Makefile ${GENERATED_SRCS}
	${QUIET_GEN} cat /dev/null >  ${GENERATED_DIR}/generatedHeaders.h; \
	for header in ${GENERATED_HDRS}; do \
	   echo "#include \"$$header\"" >> ${GENERATED_DIR}/generatedHeaders.h; \
	done

${GENERATED_DIR}/generatedCliCommandMap.c: Makefile ${GENERATED_SRCS}
	${QUIET_GEN} cat /dev/null >  ${GENERATED_DIR}/generatedCliCommandMap.c; \
	for header in ${GENERATED_HDRS}; do \
	   grep CLI_COMMAND_MAP $$header | sed -e 's/ *CLI_COMMAND_MAP//' >> ${GENERATED_DIR}/generatedCliCommandMap.c; \
	done

# Create dependencies files 
C_SRCS = ${UTIL_SRCS} ${MENU_SRCS}
$(C_SRCS:.c=.d): %.d: %.c ${GENERATED_DIR}/generatedHeaders.h ${GENERATED_DIR}/generatedCliCommandMap.c
	${QUIET_DEP} set -e; $(CC) -MM  ${UTIL_FLAGS} $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@

$(CPP_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM  ${CPP_FLAGS} $< | sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	   [ -s $@ ] || rm -f $@

$(MENUCLI_SRCS:.c=.d): %.d: %.c ${GENERATED_DIR}/generatedHeaders.h ${GENERATED_DIR}/generatedCliCommandMap.c
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  ${UTIL_FLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@

$(TEST_SRCS:.cpp=.d): %.d: %.cpp
	${QUIET_DEP} set -e; $(CC) -MM -MT $@ -MT ${patsubst %.d, %.o, $@} $(CFLAGS) $<  -I . ${GTESTCFLAGS} -o $@; \
	   [ -s $@ ] || rm -f $@


ifneq ($(MAKECMDGOALS),clean)
-include $(C_SRCS:.c=.d)
-include $(CPP_SRCS:.cpp=.d)
-include $(TEST_SRCS:.cpp=.d)
-include $(MENUCLI_SRCS:.c=.d)
endif

.PHONY: test clean
 
