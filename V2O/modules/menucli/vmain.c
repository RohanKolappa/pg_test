#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/stat.h>

#include "hal.h"
#include "commonutils.h"
#include "vlog.h"
#include "loadnet.h"
#include "vutils.h"
#include "ddcedid.h"
#include "generatedHeaders.h"
#include "plx9056pciif.h"
#include "v2d.h"
#include "vlib.h"
#include "loadfpga.h"
#include "hald.h"
#include "dump.h"
#include "setconsole.h"
#include "override_list_menu.h"
#include "edid_list_menu.h"
#include "configureEdid.h"


static int vutils_main(int argc, char **argv);
static void run_util(const char *util_name, int argc, char **argv);
static void register_init_hooks(void);

// List of command line utilites
const struct v_util utils[] = {

    {"tester", tester_main},
    {"getuserinput", getuserinput_main},
    {"loadlut", loadlut_main},
    {"modlut", modlut_main},
    {"loadnet", loadnet_main},
    {"vutils", vutils_main},
    {"menu", menu_main},
    {"cli", cli_main},
    {"dreg", dreg_main},
    {"dump_config", dump_config_main},
    {"get_auth_table", get_auth_table_main},
    {"describe_auth_table", describe_auth_table_main},
    {"add_auth_entry", add_auth_entry_main},
    {"del_auth_entry", del_auth_entry_main},
    {"mod_auth_entry", mod_auth_entry_main},
    {"add_conn_entry", add_conn_entry_main},
    {"del_conn_entry", del_conn_entry_main},
    {"mod_conn_entry", mod_conn_entry_main},
    {"get_conn_table", get_conn_table_main},
    {"describe_conn_table", describe_conn_table_main},
    {"make_conn", make_conn_main},
    {"break_conn", break_conn_main},
    {"add_route_entry", add_route_entry_main},
    {"del_route_entry", del_route_entry_main},
    {"mod_route_entry", mod_route_entry_main},
    {"get_route_table", get_route_table_main},
    {"describe_route_table", describe_route_table_main},
    {"save_config", save_config_main},
    {"save_video_params_to_lut", save_video_params_to_lut_main},
    {"revert_config", revert_config_main},
    {"get_version", get_version_main},
    {"auto_phase_adjust", auto_phase_adjust_main},
    {"sync_cp", sync_cp_main},
    {"kmtest", kmtest_main},
    {"banner_set", configure_banner_login_main},
    {"banner_get", show_banner_login_main},
    {"urlencoder", urlencoder_main},
    {"urldecoder", urldecoder_main},
    {"upgrade_device", upgrade_device_main},
    {"reboot_system", reboot_system_main},
    {"shutdown_system", shutdown_system_main},
    {"ha_takeover", ha_takeover_main},
    {"ha_standby", ha_standby_main},
    {"set_time", set_time_main},
    {"loadfpga", load_fpga_main},
    {"dumpsram", dumpsram_main},
    {"dump", dump_main},
    {"setconsole", setconsole_main},
    {"hald", hald_main},
    {"standalone", standalone_main},
    {"describe_override_list", describe_override_list_main},
    {"get_override_list", get_override_list_main},
    {"add_override_list", add_override_list_main},
    {"mod_override_list", mod_override_list_main},
    {"del_override_list", del_override_list_main},
    {"load_override_list", load_override_list_main},
    {"describe_edid_list", describe_edid_list_main},
    {"get_edid_list", get_edid_list_main},
    {"add_edid_list", add_edid_list_main},
    {"mod_edid_list", mod_edid_list_main},
    {"del_edid_list", del_edid_list_main},
    {"load_edid_list", load_edid_list_main},
    {"clear_edid_list", clear_edid_list_main},
    {"loadedideeprom", load_edid_eeprom_main},
    {"readedideeprom", read_edid_eeprom_main},
    {"send_notification", send_notification_main},
    {"dhcpc_event", dhcpc_event_main},
    {"check_device", check_device_main},
    {"usbhotplug", usbhotplug_main},
    {"describe_edid_videoinput", describe_edid_videoinput_main},
    {"get_edid_videoinput", get_edid_videoinput_main},
    {"set_edid_videoinput", set_edid_videoinput_main},
    {"get_license_pin", get_license_pin_main},
    {"set_license_file", set_license_file_main},
    {"verify_license", verify_license_main},

#ifndef __XPI__
    {"lseventdev", lseventdev_main},
    {"lsttyusb", lsttyusb_main},
#endif
    /* These are created using Menu/CLI code generation but we
     * manually map to custom function names */
    {"rebuild_disk", set_disk_rebuild_main},
    {"initialize_disk", set_disk_init_main},


    /* The remaining CLI to function name mappings are automatically generated */
     #include "generated/generatedCliCommandMap.c"

    {NULL, NULL}
};

/**********************************
 * Main entry point for all utils *
 **********************************/
int main(int argc, char **argv)
{

    const char *s;
    const char *util_name;

    util_name = argv[0];

    /*****************************************
     * Strip path name prefix from util name *
     *****************************************/
    for (s = util_name; *s != '\0';) {
        if (*s++ == '/')
            util_name = s;
    }

    /************************************************************
     * If we have admin privileges, set the user and group to   *
     * root. This will only work if the SUID bit is set on this *
     * executable and non-root user is using this (e.g. admin)  *
     ************************************************************/
    int gid = getgid();
    if (gid == 0 || gid == 100) {
        setuid(0);
        setgid(0);
    }

    /* Initialise our logging library */
    vlog_init(util_name);

    /* Set up auto-generated Init functions */
    register_init_hooks();

    /* Run the invoked utility -- never ruturns if utility found */
    run_util(util_name, argc, argv);

    printf("Utility %s not found!\n", util_name);

    return -1;
}


static int vutils_main(int argc, char **argv)
{
    int col = 0, len, i;
    static int visited = 0;

    /***************************************************************
     * If --install requested, automatically create symbolic links *
     * for all current utils                                       *
     ***************************************************************/
    if (argc > 1 && (strcmp(argv[1], "--install") == 0)) {
        const struct v_util *u = utils;
        char buf[80];
        while (u->name != 0) {
            if (strcmp(u->name, "vutils") != 0) {
                sprintf(buf, "%s/bin/%s", INSTALL_PATH, u->name);
                symlink(INSTALL_PATH "/bin/vutils", buf);
            }
            u++;
        }

        return 0;
    }

    argc--;

    /*********************************************
     * If we've already been here once, exit now *
     *********************************************/
    if (visited == 1 || argc < 1) {
        const struct v_util *u = utils;

        fprintf(stderr, "vutils \n\n"
                "Usage: vutils [function] [arguments]...\n"
                "   or: [function] [arguments]...\n\n"
                "Currently defined functions:\n");

        while (u->name != 0) {
            col +=
                fprintf(stderr, "%s%s", ((col == 0) ? "\t" : ", "),
                        (u++)->name);
            if (u->name != 0 && (strlen(u->name) + col) > 65) {
                fprintf(stderr, ",\n");
                col = 0;
            }
        }
        fprintf(stderr, "\n\n");
        exit(0);
    }


    /*************************************
     * Flag that we've been here already *
     *************************************/
    visited = 1;

    /**************************************
     * Move the command line down a notch *
     **************************************/
    len = argv[argc] + strlen(argv[argc]) - argv[1];
    memmove(argv[0], argv[1], len);
    memset(argv[0] + len, 0, argv[1] - argv[0]);

    /****************************
     * Fix up the argv pointers *
     ****************************/
    len = argv[1] - argv[0];
    memmove(argv, argv + 1, sizeof(char *) * (argc + 1));
    for (i = 0; i < argc; i++)
        argv[i] -= len;

    /***********************************************************
     * Call ourselves again to find the real utility requested *
     ***********************************************************/
    return (main(argc, argv));
}

/**************************************
 * Find and run the requested utility *
 **************************************/
static void run_util(const char *util_name, int argc, char **argv)
{
    const struct v_util *u = utils;

    while (u->name != 0) {
        if (strcmp(util_name, u->name) == 0) {
            vlog_reinit();              /* re-init in case config changed */
            exit(u->main(argc, argv));  /* each utility exits on its own */
        }
        u++;
    }

}

util_entry_point find_cli_util(const char *util_name)
{
    const struct v_util *u = utils;

    while (u->name != 0) {
        if (strcmp(util_name, u->name) == 0) {
            return u->main;
        }
        u++;
    }
    return NULL;
}

/*************************************************************
 * Invoke all Init functions that clients use to register to *
 * customized auto-generated functions                       *
 *************************************************************/
static void register_init_hooks() {

    SampleConfigInit();
    TestMenuOneConfigInit();
    ChromakeyParamInit();
    TestListConfigInit();
    SampleListInit();
	CodecVersionInit();
    SampleImmediateConfigInit();
    DHCPServerInit();
    TxControlPortsInit();
    NTPClientInit();
    RxAVPortsInit();
    RxVideoParametersInit();
    RxInputVideoParametersInit();
    TxVideoParametersInit();
    TxSplashParametersInit();
    TxThumbnailParametersInit();
    RxSplashParametersInit();
    XpTxVideoParametersInit();
    MulticastParametersInit();
#ifndef __XPI__
    ColorParametersInit();
#else
    XpiColorParametersInit();
#endif
    TxConnectionParametersInit();
    XPAudioMixerParametersInit();
    XMPPAgentInit();
    XMPPServerInit();
    HAInit();
    HostListInit();
    DNSClientInit();
    IFListInit();
    LUTListInit();
    DatabaseManagementInit();
    BoardInfoInit();
    TxAVBuffersInit();
    RxAVBuffersInit();
    OverrideListInit();
    LoggingLevelInit();
    ReverseAudioParametersInit();
    ProtocolVersionParametersInit();
    BannerInit();
}


