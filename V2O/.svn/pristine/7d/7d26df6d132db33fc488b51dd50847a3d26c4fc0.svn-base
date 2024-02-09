
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_console.h"
#include "hal_LED.h"
#include "custmenu.h"
#include "configureSystem.h"
#include "displayVersion.h"
#include "configureDefaults.h"
#include "configureLog.h"
#include "commandSaveToFlash.h"
#include "saveVideoParamsToLUT.h"
#include "testLEDs.h"
#include "configureServerlists.h"
#include "configureVideo.h"
#include "displayAudioVideoStatistics.h"
#include "displayWriteAudioVideoStatistics.h"
#include "adjustVideoParameters.h"
#include "override_list_menu.h"
#include "edid_list_menu.h"
#include "clear_edid_list_menu.h"
#include "configureEdid.h"
#include "configureUser.h"
#include "portSelector.h"
#include "routeList.h"
#include "displayGuideRuler.h"
#include "commonutils.h"
#include "device_detection.h"
#include "vutils.h"
#include "vlog.h"
#include "generatedHeaders.h"
#include "splash_list_menu.h"

unsigned short ruid;            /* Current user id */

static void custmenu_signal(int sig);

extern char *strsignal(int sig);        /* Remove Compile Warning */

static int system_mode = 0;
static unsigned long current_device = 0;
static struct termios save_termios;
WINDOW *mwg, *iwg;

static int menuhandler(MENUINFO * minfo);

char tempbuf[512];

#define CURSES_ERR(format, args...)                             \
    do {                                                        \
        VLOG_CRITICAL_CAT("menu", format, ##args);              \
        rc = RC_ERROR;                                          \
        goto QUIT_LABEL;                                        \
    } while (0)


#define CURSES_INFO(format, args...)                    \
    VLOG_INFO_CAT("menu", format, ##args);

#define CURSES_DEBUG(format, args...)                   \
    VLOG_DEBUG_CAT("menu", format, ##args);

char *pressanykey = "Press any key to continue...";
char *formmesg;
char *formmesg0 =
    "Use </U>Up/Down<!U> arrow keys to select field to edit.\n"
    "Press </U>Enter<!U> to accept current value or </U>CTRL-Q<!U> to quit.";
char *formmesg1 =
    "Use </U>Up/Down<!U> arrow keys to select field to edit.\n"
    "Press </U>Enter<!U> to accept current value, </U>Q<!U> or </U>CTRL-Q<!U> to quit.";
char *formmesg2 =
    "This form is display only. Use </U>Q<!U> or </U>CTRL-Q<!U> to quit.";


static void print_help(void)
{
    printf("\nUsage: menu [-s]\n\n");
}

int menu_main(int argc, char **argv)
{
#define BUFF_LEN        4096
    int fd = 0;                 /* stdin */
    struct termios change_termios;
    int opt;
    int rc;

    /*****************
     * Parse options *
     *****************/
    while ((opt = getopt(argc, argv, "shdr")) > 0) {

        switch (opt) {
        case 's':
            system_mode = 1;
            break;

        case 'd':
            break;

        case '?':
        case 'h':
        default:
            print_help();
            return -1;
        }
    }

    /*************************************************
     * Initialize user id & related global variables *
     *************************************************/
    ruid = getgid();
    if (ruid == ADMIN_USER_GROUP)
        ruid = 0;

    formmesg = (ruid == 0) ? formmesg1 : formmesg2;

    /****************************************************************
     * While in menu, turn off IXON for the terminal which disables *
     * XON/XOFF flow control and makes Control-Q and Control-S      *
     * accesible for menu operations                                *
     ****************************************************************/
    if (tcgetattr(fd, &save_termios) < 0) {
        CURSES_ERR("Cannot get terminal attributes.");
    }

    signal(SIGTERM, custmenu_signal);
    signal(SIGQUIT, custmenu_signal);
    signal(SIGINT, custmenu_signal);

    change_termios = save_termios;
    change_termios.c_iflag &= ~IXON;
    CURSES_DEBUG("Terminal attributes: c_iflag = %x.",
                 save_termios.c_iflag);

    tcsetattr(fd, TCSANOW, &change_termios);

    /********************************************
     * Call the main menu operation entry point *
     ********************************************/
    mainmenu();

    /**************************
     * Reset terminal termios *
     **************************/
    tcsetattr(fd, TCSANOW, &save_termios);

    return 0;

  QUIT_LABEL:
    return rc;

}


/* Nested list of all menu items declared as a set of
   static structures */
static MENUITEMDEF testMenu[] = {

    {"Sample Menu",
     "Sample Menu.",
     MTYPE_FUNCTION, configureSampleConfig, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Sample Immediate Menu",
     "Sample Immediate Menu.",
     MTYPE_FUNCTION, configureSampleImmediateConfig, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Sample Status Menu",
     "Sample Status Menu.",
     MTYPE_FUNCTION, displaySampleStatus, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_PORTTYPE_BOTH},

    {"Auto Sample Display Parameters",
     "Select display parameters.",
     MTYPE_FUNCTION, configureSampleDisplay, NULL, NULL,
     MFLAG_DEVICES_V2D_RX | MFLAG_SYSTEM},

    {"Sample List Menu",
     "Sample List Menu.",
     MTYPE_FUNCTION, configureSampleList, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_SYSTEM | MFLAG_PORTTYPE_BOTH},

    {"Test List Menu ",
     "Test List Menu .",
     MTYPE_FUNCTION, configureTestListConfig, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_SYSTEM | MFLAG_PORTTYPE_BOTH},

    {"Sample Pager Status",
     "Sample Pager Status.",
     MTYPE_FUNCTION, displaySamplePager, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_PORTTYPE_BOTH},

    {"Sample Log Menu",
     "Sample Log Menu.",
     MTYPE_FUNCTION, displaySampleLog, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_PORTTYPE_BOTH},

    {"Test Menu One",
     "Test Menu One.",
     MTYPE_FUNCTION, configureTestMenuOneConfig, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF displayTxRegistersMenu[] = {

    {"Display/Update Add-Side Registers",
     "Display/Update Add-Side hardware registers.",
     MTYPE_FUNCTION, displayTxAddSideRegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display/Update Drop-Side Registers",
     "Display/Update Drop-Side hardware registers.",
     MTYPE_FUNCTION, displayRxDropSideRegStatus, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Display/Update Transmitter Common Registers",
     "Display/Update FPGA Common registers for V2D TX.",
     MTYPE_FUNCTION, displayTxFPGARegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Update Debug Registers",
     "Update User-Defined Debug Registers for Display.",
     MTYPE_FUNCTION, configureDebugRegParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display Debug Registers",
     "Display User Defined Debug Registers.",
     MTYPE_FUNCTION, displayDebugRegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display Debug Registers II",
     "Display User Defined Debug Registers.",
     MTYPE_FUNCTION, displayDebugRegIIStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Update OmniTek Registers",
     "Update User-Defined OmniTek Registers for Display.",
     MTYPE_FUNCTION, configureOmniTekRegParameters, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display OmniTek Registers",
     "Display User Defined OmniTek Registers.",
     MTYPE_FUNCTION, displayOmniTekRegStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display OmniTek Registers II",
     "Display User Defined OmniTek Registers.",
     MTYPE_FUNCTION, displayOmniTekRegIIStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Update Spartan 6 Registers",
     "Update User-Defined Spartan 6 Registers for Display.",
     MTYPE_FUNCTION, configureS6RegParameters, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display Spartan 6 Registers",
     "Display User Defined Spartan 6 Registers.",
     MTYPE_FUNCTION, displayS6RegStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display Spartan 6 Registers II",
     "Display User Defined Spartan 6 Registers.",
     MTYPE_FUNCTION, displayS6RegIIStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF displayTxAVStatisticsMenu[] = {
    {"Display Read Video Statistics",
     "Display Read Video Statistics.",
     MTYPE_FUNCTION, displayReadVideoStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display Read Audio Statistics",
     "Display Read Audio Statistics.",
     MTYPE_FUNCTION, displayReadAudioStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display Write Audio Statistics",
     "Display Write Audio Statistics.",
     MTYPE_FUNCTION, displayWriteAudioStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},
    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderStatisticsMenu[] = {

    {"Display Connection Information",
     "Show current connection information and video statistics.",
     MTYPE_FUNCTION, displayTxConnectionStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display IO Information",
     "Show current control/keyboard/mouse/serial statistics.",
     MTYPE_FUNCTION, displayTxDisplayIOInfoStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

     {"Display EEPROM EDID",
     "Display current EDID data loaded in the V2D EEPROM.",
     MTYPE_FUNCTION, displayEepromEdid, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

     {"Display Local Video Output Monitor EDID",
     "Display EDID data from the attached Monitor on the loopback DVI port.",
     MTYPE_FUNCTION, displayMonitorEdid, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Display Audio/Video Statistics",
     "Show audio and video read/write statistics",
     MTYPE_SUBMENU, NULL, displayTxAVStatisticsMenu,
     "Display audio and video read/write statistics",
     MFLAG_DEVICES_V2D_TX},

    {"Display/Update Registers",
     "Display/Update Addside, Dropside and Common registers,",
     MTYPE_SUBMENU, NULL, displayTxRegistersMenu,
     "Display/Update Registers Menu",
     MFLAG_DEVICES_V2D_TX},

    {"Configure Audio/Video Buffers",
     "Configure Audio/Video Buffers",
     MTYPE_FUNCTION, configureTxAVBuffers, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

     {"Frame Counter Overview",
     "Show current frame counter statistics.",
     MTYPE_FUNCTION, displayFrameStatistics, NULL, NULL,
     MFLAG_DEVICES_XP},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF displayRxRegistersMenu[] = {
    {"Display/Update Add-Side Registers",
     "Display/Update Add-Side hardware registers.",
     MTYPE_FUNCTION, displayTxAddSideRegStatus, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Display/Update Drop-Side Registers",
     "Display/Update Drop-Side hardware registers.",
     MTYPE_FUNCTION, displayRxDropSideRegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display/Update Receiver Common Registers",
     "Display/Update FPGA Common registers for V2D RX.",
     MTYPE_FUNCTION, displayRxFPGARegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display/Update Debug Registers",
     "Display/Update User-Defined Debug Registers.",
     MTYPE_FUNCTION, configureDebugRegParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Debug Registers",
     "Display User Defined Debug Registers.",
     MTYPE_FUNCTION, displayDebugRegStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Debug Registers II",
     "Display User Defined Debug Registers.",
     MTYPE_FUNCTION, displayDebugRegIIStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display/Update OmniTek Registers",
     "Display/Update User-Defined OmniTek Registers.",
     MTYPE_FUNCTION, configureOmniTekRegParameters, NULL, NULL,
     MFLAG_DEVICE_XPI_RX},

    {"Display OmniTek Registers",
     "Display User Defined OmniTek Registers.",
     MTYPE_FUNCTION, displayOmniTekRegStatus, NULL, NULL,
     MFLAG_DEVICE_XPI_RX},

    {"Display OmniTek Registers II",
     "Display User Defined OmniTek Registers.",
     MTYPE_FUNCTION, displayOmniTekRegIIStatus, NULL, NULL,
     MFLAG_DEVICE_XPI_RX},

    {"Update Spartan 6 Registers",
     "Update User-Defined Spartan 6 Registers for Display.",
     MTYPE_FUNCTION, configureS6RegParameters, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display Spartan 6 Registers",
     "Display User Defined Spartan 6 Registers.",
     MTYPE_FUNCTION, displayS6RegStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Display Spartan 6 Registers II",
     "Display User Defined Spartan 6 Registers.",
     MTYPE_FUNCTION, displayS6RegIIStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF displayRxAVStatisticsMenu[] = {
    {"Display Write Video Statistics",
     "Display Write Video Statistics.",
     MTYPE_FUNCTION, displayWriteVideoStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Write Audio Statistics",
     "Display Write Audio Statistics.",
     MTYPE_FUNCTION, displayWriteAudioStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Read Audio Statistics",
     "Display Read Audio Statistics.",
     MTYPE_FUNCTION, displayReadAudioStatisticsStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF decoderStatisticsMenu[] = {

    {"Display Connection Information",
     "Show current connection information and video statistics.",
     MTYPE_FUNCTION, displayRxConnectionStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Video Overlay Information",
     "Show current local and remote overlay configuration.",
     MTYPE_FUNCTION, displayDisplayStatus, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Display IO Information",
     "Show current control/keyboard/mouse/serial statistics.",
     MTYPE_FUNCTION, displayRxDisplayIOInfoStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Chromakey Status",
     "Show current chromakey status information",
     MTYPE_FUNCTION, displayChromakeyStatus, NULL, NULL,
     MFLAG_DEVICES_XP},

     {"Display Monitor EDID",
     "Display current EDID data from the attached Monitor.",
     MTYPE_FUNCTION, displayMonitorEdid, NULL, NULL,
     MFLAG_DEVICES_XP},

     {"Display Local Video Input EEPROM EDID",
     "Display current EDID data in the EEPROM for local video input.",
     MTYPE_FUNCTION, displayEepromEdid, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Display Audio/Video Statistics",
     "Display Audio and Video statistics,",
     MTYPE_SUBMENU, NULL, displayRxAVStatisticsMenu,
     "Display/Update Registers Menu",
     MFLAG_DEVICES_V2D_RX},

    {"Display/Update Registers",
     "Display/Update Addside, Dropside and Common registers,",
     MTYPE_SUBMENU, NULL, displayRxRegistersMenu,
     "Display/Update Registers Menu",
     MFLAG_DEVICES_V2D_RX},

    {"Configure Audio/Video Buffers",
     "Configure Audio/Video Buffers",
     MTYPE_FUNCTION, configureRxAVBuffers, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

     {"Frame Counter Overview",
     "Show current frame counter statistics.",
     MTYPE_FUNCTION, displayFrameStatistics, NULL, NULL,
     MFLAG_DEVICES_XP},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF systemMenu[] = {

    {"Encoder Statistics",
     "Encoder Statistics.",
     MTYPE_SUBMENU, NULL, encoderStatisticsMenu,
     "Encoder Statistics Menu",
     MFLAG_DEVICES_V2D_TX | MFLAG_PORTTYPE_TX},

    {"Decoder Statistics",
     "Decoder Statistics.",
     MTYPE_SUBMENU, NULL, decoderStatisticsMenu,
     "Decoder Statistics Menu",
     MFLAG_DEVICES_V2D_RX | MFLAG_PORTTYPE_RX},

    {"Change Log Level",
     "Modify the level of detail for system logs.",
     MTYPE_FUNCTION, configureLog, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Test Menu",
     "IP Video Systems Test Menu.",
     MTYPE_SUBMENU, NULL, testMenu, "System Menu",
     MFLAG_DEVICES_ALL},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};


static MENUITEMDEF systemDiagnosticsMenu[] = {

    {"Memory Information",
     "Display Memory Information.",
     MTYPE_FUNCTION, displayMemoryStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"CPU Status",
     "Display CPU Status.",
     MTYPE_FUNCTION, displayCPUStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"System Clock",
     "Display System Clock Status.",
     MTYPE_FUNCTION, displaySystemClockStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Front Panel Status",
     "Display Front Panel Status",
     MTYPE_FUNCTION, displayFrontPanelStatus, NULL, NULL,
     MFLAG_DEVICE_DMS},

    {"Temperature Status",
     "Display Tempertaure Status",
     MTYPE_FUNCTION, displayTemperatureStatus, NULL, NULL,
     MFLAG_DEVICE_XP200},

    {"Temperature and Fan Status",
     "Display Current Temperature and Fan Settings",
     MTYPE_FUNCTION, displayFanStatus, NULL, NULL,
     MFLAG_DEVICE_XP100},

    {"Temperature and Fan Status",
     "Display Current Temperature and Fan Settings",
     MTYPE_FUNCTION, displayXPiFanStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"Var Log Messages",
     "Display Var Log Messages",
     MTYPE_FUNCTION, displayVarLogMessages, NULL, NULL,
     MFLAG_DEVICES_ALL | MFLAG_SYSTEM},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF networkDiagnosticsMenu[] = {

    {"Interface Status",
     "Display Network Interface Status.",
     MTYPE_FUNCTION, displayNetworkPortsStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Route Status",
     "Display Network Route Status.",
     MTYPE_FUNCTION, displayRouteStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF storageDiagnosticsMenu[] = {

    {"Disk Status",
     "Display Disk Status.",
     MTYPE_FUNCTION, displayDiskStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"NAS Status",
     "Display NAS Status.",
     MTYPE_FUNCTION, displayNASStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF adminDiagnosticsMenu[] = {

    {"Management Server Status",
     "Management Server Status.",
     MTYPE_FUNCTION, displayXMPPAgentStatus, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"XMPP Server Status",
     "XMPP Server Status.",
     MTYPE_FUNCTION, displayXMPPServerStatus, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"Database Management Status",
     "Database Management Status.",
     MTYPE_FUNCTION, displayDatabaseManagementStatus, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"High Availability Status",
     "High Availability Status.",
     MTYPE_FUNCTION, displayHAStatus, NULL, NULL,
     MFLAG_DEVICE_MS},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderDiagnosticsMenu[] = {

    {"Display Connection Information",
     "Show current connection information and video statistics.",
     MTYPE_FUNCTION, displayTxConnectionStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Display IO Information",
     "Show current control/keyboard/mouse/serial statistics.",
     MTYPE_FUNCTION, displayTxDisplayIOInfoStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

     {"Display EEPROM EDID",
     "Display current EDID data loaded in the V2D EEPROM.",
     MTYPE_FUNCTION, displayEepromEdid, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

     {"Display Local Video Output Monitor EDID",
     "Display EDID data from the attached Monitor on the output DVI port.",
     MTYPE_FUNCTION, displayMonitorEdid, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI },

     {"Frame Counter Overview",
     "Show current frame counter statistics.",
     MTYPE_FUNCTION, displayFrameStatistics, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF decoderDiagnosticsMenu[] = {


    {"Display Connection Information",
     "Show current connection information and video statistics.",
     MTYPE_FUNCTION, displayRxConnectionStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Video Overlay Information",
     "Show current local and remote overlay configuration.",
     MTYPE_FUNCTION, displayDisplayStatus, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Display IO Information",
     "Show current control/keyboard/mouse/serial statistics.",
     MTYPE_FUNCTION, displayRxDisplayIOInfoStatus, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Display Chromakey Status",
     "Show current chromakey status information",
     MTYPE_FUNCTION, displayChromakeyStatus, NULL, NULL,
     MFLAG_DEVICES_XP},

     {"Display Monitor EDID",
     "Display current EDID data from the attached Monitor.",
     MTYPE_FUNCTION, displayMonitorEdid, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI },

     {"Display Local Video Input EEPROM EDID",
     "Display current EDID data in the EEPROM for local input port.",
     MTYPE_FUNCTION, displayEepromEdid, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

     {"Display Guide Ruler",
     "Display guide ruler icon at specified position on monitor.",
     MTYPE_FUNCTION, displayGuideRuler, NULL, NULL,
     MFLAG_DEVICES_XP| MFLAG_DEVICES_XPI},

     {"Frame Counter Overview",
     "Show current frame counter statistics.",
     MTYPE_FUNCTION, displayFrameStatistics, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF diagnosticsMenu[] = {


    {"Encoder Status",
     "Encoder Status.",
     MTYPE_SUBMENU, NULL, encoderDiagnosticsMenu,
     "Encoder Status Menu",
     MFLAG_DEVICES_V2D_TX | MFLAG_PORTTYPE_TX},

    {"Decoder Status",
     "Decoder Status.",
     MTYPE_SUBMENU, NULL, decoderDiagnosticsMenu,
     "Decoder Status Menu",
     MFLAG_DEVICES_V2D_RX | MFLAG_PORTTYPE_RX},

    {"Admin Status",
     "Admin Status.",
     MTYPE_SUBMENU, NULL, adminDiagnosticsMenu,
     "Admin Status Menu",
     MFLAG_DEVICES_ALL},

    {"Network Status",
     "Network Status.",
     MTYPE_SUBMENU, NULL, networkDiagnosticsMenu,
     "Network Status Menu",
     MFLAG_DEVICES_ALL},

    {"Storage Status",
     "Storage Status.",
     MTYPE_SUBMENU, NULL, storageDiagnosticsMenu,
     "Storage Status Menu",
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"System Status",
     "System Status.",
     MTYPE_SUBMENU, NULL, systemDiagnosticsMenu,
     "System Status Menu",
     MFLAG_DEVICES_ALL},

    {"LED Test",
     "Test LED functionality.",
     MTYPE_FUNCTION, testLEDs, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Exit", "Go Back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};


static MENUITEMDEF consoleModeConfigurationMenu[] = {

    {"Set all Keyboards to Console Mode",
     "Set all Keyboards connected to the XP to Console Mode\n",
     MTYPE_FUNCTION, configureKBConsole, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Console Mode Escape Sequence",
     "Set key sequence to control switching to console mode.",
     MTYPE_FUNCTION, configureKeyboardConsoleModeSettings, NULL, NULL,
     MFLAG_DEVICES_XP},
};

static MENUITEMDEF networkConfigurationMenu[] = {

    {"Network Interfaces",
     "Set/Change the IP address/netmask of the Interfaces\n",
     MTYPE_FUNCTION, configureIFList, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Domain Name Service (DNS)",
     "Configure Domain Name Service (DNS) Client.",
     MTYPE_FUNCTION, configureDNSClient, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Static Routes",
     "Add/Delete static routes on the system.",
     MTYPE_FUNCTION, configureRoutes, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Host Table",
     "Add/Delete host table entries on the system.",
     MTYPE_FUNCTION, configureHostList, NULL, NULL,
     MFLAG_DEVICES_ALL},

    /* Removed this as per discussion with Johan and Saurabh
    {"NAT Zone Table",
     "Add/Delete entries used to access the system via NAT.",
     MTYPE_FUNCTION, configureNATTraversalList, NULL, NULL,
     MFLAG_DEVICES_ALL},
     */

    {"NTP Client Configuration",
     "Configure NTP (Network Time Protocol) Client",
     MTYPE_FUNCTION, configureNTPClient, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"NTP Server Configuration",
     "Configure NTP (Network Time Protocol) Server",
     MTYPE_FUNCTION, configureNTPServer, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"Telnet Server Configuration",
     "Configure Telnet Server",
     MTYPE_FUNCTION, configureTelnetServer, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"DHCP Server",
     "Set Parameters for the DHCP Server.",
     MTYPE_FUNCTION, configureDHCPServer, NULL, NULL,
     MFLAG_DEVICE_DMS},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF storageConfigurationMenu[] = {

    {"NAS Configuration",
     "Configure NAS Servers",
     MTYPE_FUNCTION, configureNASServerList, NULL, NULL,
     MFLAG_DEVICE_DMS},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF streamingConfigurationMenu[] = {

    {"Connection Authorization",
     "Add/Delete Clients that this Server will accept connections from.",
     MTYPE_FUNCTION, configureAuthList, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Connection Manager",
     "Add/Delete Servers that this Client can connect to.",
     MTYPE_FUNCTION, configureServerlists, NULL, NULL,
     MFLAG_DEVICES_V2D_RX | MFLAG_PORTTYPE_RX},

     {"VNC Connection Manager",
      "Add/Delete VNC Servers that this Client can connect to.",
      MTYPE_FUNCTION, configureRFBConnectionList, NULL, NULL,
      MFLAG_DEVICES_V2D_RX | MFLAG_PORTTYPE_RX | MFLAG_SYSTEM},

    {"Disconnect",
     "Disconnect all V2D Receivers.",
     MTYPE_FUNCTION, disconnectClients, NULL, NULL,
     MFLAG_DEVICES_V2D_TX | MFLAG_PORTTYPE_TX},

    {"Control Ports Range",
     "TCP Control Ports Range.",
     MTYPE_FUNCTION, configureTxControlPorts, NULL, NULL,
     MFLAG_DEVICES_V2D_TX | MFLAG_DEVICE_DMS},

    {"Audio/Video Ports Range",
     "UDP Audio/Video Ports Range.",
     MTYPE_FUNCTION, configureRxAVPorts, NULL, NULL,
     MFLAG_DEVICES_V2D_RX | MFLAG_DEVICE_DMS},

    {"Multicast Parameters",
     "Multicast Parameters.",
     MTYPE_FUNCTION, configureMulticastParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX |  MFLAG_DEVICE_DMS},

     {"Reverse Audio",
      "Reverse Audio Parameters.",
      MTYPE_FUNCTION, configureReverseAudioParameters, NULL, NULL,
      MFLAG_DEVICES_V2D | MFLAG_PORTTYPE_BOTH},

      {"Server Protocol Version",
       "Server Protocol Version",
       MTYPE_FUNCTION, configureProtocolVersionParameters, NULL, NULL,
       MFLAG_DEVICES_V2D | MFLAG_PORTTYPE_TX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF decoderVideoParametersMenu[] = {
    {"Output Video Parameters",
     "Set Video options.",
     MTYPE_FUNCTION, configureRxVideoParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICE_XPI_RX},

    {"Input Video Parameters",
     "Set Video options.",
     MTYPE_FUNCTION, configureRxInputVideoParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICE_XPI_RX},

    {"Save Input Video Parameters to LUT",
     "Save Current Video paramenters to LUT.",
     MTYPE_FUNCTION, saveVideoParametersToLUT, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICE_XPI_RX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderColorAndEdidMenu[] = {

    {"Color Gain and Offset",
     "Set the gain and offset values for each of the three colors.",
     MTYPE_FUNCTION, configureColorParameters, NULL, NULL,
     MFLAG_DEVICES_NON_XPI},

    {"Configure EDID Timing Descriptors",
     "Set V2D EDID detailed timing descriptors to LUT entries.",
     MTYPE_FUNCTION, configureEdidDescriptorList, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Set Local Video Input EDID to Digital or Analog",
     "Set the EDID for video input to either Digital or Analog.",
     MTYPE_FUNCTION, configureEdidVideoInput, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI },

    {"Clear EDID Timing Descriptors",
     "Remove all configured EDID timing descriptors from EEPROM.",
     MTYPE_FUNCTION, clearEdidDescriptorList, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI },

    {"Copy Local Video Output Monitor EDID to EEPROM EDID",
     "Copy Local Video Output Monitor EDID to EEPROM EDID",
     MTYPE_FUNCTION, configureEdidFromMonitor, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Synchronize EDID EEPROM with Monitor EDID",
     "Synchronize EDID EEPROM with Monitor EDID",
     MTYPE_FUNCTION, configureSyncEdidEeprom, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF decoderColorAndEdidMenu[] = {

    {"Local Video Input Color Gain and Offset",
     "Set the gain and offset values for each of the three colors.",
     MTYPE_FUNCTION, configureColorParameters, NULL, NULL,
     MFLAG_DEVICES_NON_XPI},


    {"Configure Local Video Input EDID Timing Descriptors",
     "Set Local Video Input EDID detailed timing descriptors to LUT entries.",
     MTYPE_FUNCTION, configureEdidDescriptorList, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Set Local Video Input EDID to Digital or Analog",
     "Set Loocal Input EDID to either Digital or Analog.",
     MTYPE_FUNCTION, configureEdidVideoInput, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Clear EDID Timing Descriptors",
     "Remove all configured EDID timing descriptors from EEPROM.",
     MTYPE_FUNCTION, clearEdidDescriptorList, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Copy Monitor EDID to LUT and EDID EEPROM",
     "Copy Monitor EDID to LUT and EDID EEPROM.",
     MTYPE_FUNCTION, configureEdidFromMonitor, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Synchroniz EDID EEPROM with Monitor",
     "Synchronize EDID EEPROM with Monitor EDID",
     MTYPE_FUNCTION, configureSyncEdidEeprom, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF decoderSplashParametersMenu[] = {

    {"Splash Screen Parameters",
     "Set Splash Screen Parameters.",
     MTYPE_FUNCTION, configureRxSplashParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Configure Splash Parameters from LUT",
     "Configuration Splash Parameters from LUT.",
     MTYPE_FUNCTION, configureSplashList, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderVideoParametersMenu[] = {
    {"Video Parameters",
     "Set Video options.",
     MTYPE_FUNCTION, configureTxVideoParameters, NULL, NULL,
     MFLAG_DEVICE_V2D_TX},

    {"Video Parameters",
     "Set Video options.",
     MTYPE_FUNCTION, configureXpTxVideoParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICE_XPI_TX},

    {"Save Video Parameters to LUT",
     "Save Current Video paramenters to LUT.",
     MTYPE_FUNCTION, saveVideoParametersToLUT, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICE_XPI_TX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderSplashParametersMenu[] = {

    {"Splash Screen Parameters",
     "Set Splash Screen Parameters.",
     MTYPE_FUNCTION, configureTxSplashParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Configure Splash Parameters from LUT",
     "Configure Splash Parameters from LUT.",
     MTYPE_FUNCTION, configureSplashList, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Force Splash Screen",
     "Allow Force Splash Screen.",
     MTYPE_FUNCTION, configureForceSplashScreen, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF encoderConfigurationMenu[] = {

    {"Connection Parameters",
     "Set Video Sampling options.",
     MTYPE_FUNCTION, configureTxConnectionParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Video Parameters",
     "Set Video options.",
     MTYPE_SUBMENU, NULL, encoderVideoParametersMenu, "Video Parameters",
     MFLAG_DEVICES_V2D_TX},

/*  {"Codec Type",
      "Set the codecType.",
      MTYPE_FUNCTION, configureCodecVersion, NULL, NULL,
      MFLAG_DEVICES_V2D_TX}, */

    {"Serial Parameters",
     "Set Serial options.",
     MTYPE_FUNCTION, configureTxIOParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Audio Mixer",
     "Set Audio Mixer options.",
     MTYPE_FUNCTION, configureXPAudioMixerParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Auto Adjust Sampling Phase",
     "Adust Phase and Noise.",
     MTYPE_FUNCTION, adjustVideoParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Splash Parameters",
     "Set splash screen parameters.",
     MTYPE_SUBMENU, NULL, encoderSplashParametersMenu,
     "Set Splash screen and LUT parameters",
     MFLAG_DEVICES_V2D_TX},

    {"Video Lookup Tables",
     "Add/Delete/Modify V2D Transmitter video lookup table.",
     MTYPE_FUNCTION, configureLUTList, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Thumbnail Parameters",
     "Add/Change Thumbnail parameters",
     MTYPE_FUNCTION, configureTxThumbnailParameters, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Video Color and EDID Parameters",
     "Adjust video color and EDID parameters.",
     MTYPE_SUBMENU, NULL, encoderColorAndEdidMenu,
     "Video Color and EDID Parameters",
     MFLAG_DEVICES_V2D_TX & ~MFLAG_DEVICE_XPI_TX},

    {"EDID Parameters",
     "Adjust EDID parameters.",
     MTYPE_SUBMENU, NULL, encoderColorAndEdidMenu,
     "EDID Parameters",
     MFLAG_DEVICE_XPI_TX},

    {"Keyboard/Mouse Parameters",
     "Add/Change Keyboard/Mouse parameters",
     MTYPE_FUNCTION, configureTxKBMParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Keyboard/Mouse Server Parameters",
     "Add/Change Keyboard/Mouse Server parameters",
     MTYPE_FUNCTION, configureTxRFBParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_TX},

    {"Keyboard Local Mode Escape Sequence",
     "Set keyboard local mode key sequence.",
     MTYPE_FUNCTION, configureTxKeyboardLocalModeSettings, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Keyboard Layout",
     "Set Keyboard Layout",
     MTYPE_FUNCTION, configureTxKBMLayout, NULL, NULL, 
     MFLAG_DEVICES_XPI },

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF rxKeyboardControlMenu[] = {

    {"Keyboard Local Mode Settings",
     "Set keyboard local mode key sequence and icon parameters.",
     MTYPE_FUNCTION, configureRxKeyboardLocalModeSettings, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Keyboard Remote Mode Settings",
     "Set keyboard remote mode key sequence and icon parameters.",
     MTYPE_FUNCTION, configureRxKeyboardRemoteModeSettings, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF decoderConfigurationMenu[] = {

    {"Video Parameters",
     "Set Video options.",
     MTYPE_FUNCTION, configureRxVideoParameters, NULL, NULL,
     MFLAG_DEVICE_V2D_RX},

    {"Video Parameters",
     "Set Video options.",
     MTYPE_SUBMENU, NULL, decoderVideoParametersMenu,
     "Video Parameters",
     MFLAG_DEVICES_XP},

    {"Serial Parameters",
     "Set Serial options.",
     MTYPE_FUNCTION, configureRxIOParameters, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Audio Mixer",
     "Set Audio Mixer options.",
     MTYPE_FUNCTION, configureXPAudioMixerParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Splash Parameters",
     "Set splash screen parameters.",
     MTYPE_SUBMENU, NULL, decoderSplashParametersMenu,
     "Set Splash screen and LUT parameters",
     MFLAG_DEVICES_V2D_RX},

    {"Display Parameters",
     "Select display parameters.",
     MTYPE_FUNCTION, configureDisplayParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Chromakey Parameters",
     "Set chromakey options.",
     MTYPE_FUNCTION, configureChromakeyParam, NULL, NULL,
     MFLAG_DEVICES_XP},

    {"Video Lookup Tables",
     "Add/Delete/Modify V2D Receiver video lookup table.",
     MTYPE_FUNCTION, configureLUTList, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Video Override Parameters",
     "Set parameters for Remote Video Lookup Table Override.",
     MTYPE_FUNCTION, configureOverrideParameters, NULL, NULL,
     MFLAG_DEVICE_V2D_RX},

    {"Auto Override Video Lookup Tables",
     "Add/Delete/Modify V2D Receiver auto override Lookup Tables.",
     MTYPE_FUNCTION, configureOverrideList, NULL, NULL,
     MFLAG_DEVICES_V2D_RX},

    {"Local Video Input Color and EDID Parameters",
     "Adjust video color and EDID parameters for local video input.",
     MTYPE_SUBMENU, NULL, decoderColorAndEdidMenu,
     "Video Color and EDID Parameters on Local Input Port",
     MFLAG_DEVICES_XP },

    {"Local Video Input EDID Parameters",
     "EDID parameters for local video input.",
     MTYPE_SUBMENU, NULL, decoderColorAndEdidMenu,
     "EDID Parameters on Local Input Port",
      MFLAG_DEVICES_XPI },

    {"Keyboard/Mouse Parameters",
     "Display/Change Keyboard/Mouse parameters",
     MTYPE_FUNCTION, configureRxKBMParameters, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},

    {"Keyboard Control Sequences",
     "Set key sequences to control switching keyboard modes.",
     MTYPE_SUBMENU, NULL, rxKeyboardControlMenu,
     "Keyboard Control Menu",
     MFLAG_DEVICES_XP | MFLAG_DEVICES_XPI},


    {"Keyboard Layout",
    "Set Keyboard Layout",
    MTYPE_FUNCTION, configureRxKBMLayout, NULL, NULL, 
    MFLAG_DEVICES_XP |  MFLAG_SYSTEM},


    {"Keyboard Layout",
    "Set Keyboard Layout",
    MTYPE_FUNCTION, configureRxKBMLayout, NULL, NULL, 
     MFLAG_DEVICES_XPI},
    
     {"Local Mouse Pointer", 
    "Set Local Mouse Pointer",
    MTYPE_FUNCTION, configureLocalMousePointer, NULL, NULL,
    MFLAG_DEVICES_XPI | MFLAG_DEVICES_XP },

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};

static MENUITEMDEF administrationConfigurationMenu[] = {

    {"Management Server",
     "Add/Change Management Server Information.",
     MTYPE_FUNCTION, configureXMPPAgent, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"XMPP Server Configuration",
     "Add/Change XMPP Server Information.",
     MTYPE_FUNCTION, configureXMPPServer, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"Max Media Clients",
     "Max Media Clients.",
     MTYPE_FUNCTION, configureMaxMediaClients, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"High Availability Configuration",
     "Add/Change HA Information.",
     MTYPE_FUNCTION, configureHA, NULL, NULL,
     MFLAG_DEVICE_MS},

    {"Database Management",
     "Configure Database Backup/Restore Information",
     MTYPE_FUNCTION, configureDatabaseManagement, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"Admin Account",
     "Admin Account Configuration.",
     MTYPE_FUNCTION, configureAdminAccount, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Operation Mode",
     "Allows setting up direct connections.",
     MTYPE_FUNCTION, configureStandAlone, NULL, NULL,
     MFLAG_DEVICES_V2D},

    {"Demos Mode",
     "Allows Running of Demos.",
     MTYPE_FUNCTION, configureDemos, NULL, NULL,
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};


static MENUITEMDEF systemCommandsMenu[] = {

    {"Reboot System",
     "Reset the system by rebooting.",
     MTYPE_FUNCTION, rebootSystem, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Shutdown System",
     "Shutdown the system.",
     MTYPE_FUNCTION, shutdownSystem, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"High Availability Takeover",
      "HA Takeover.",
      MTYPE_FUNCTION, haTakeover, NULL, NULL,
      MFLAG_DEVICE_MS},

    {"High Availability Standby",
      "HA Standby.",
      MTYPE_FUNCTION, haStandby, NULL, NULL,
      MFLAG_DEVICE_MS},

    {"Keyboard Console Mode Configuration",
     "Configure keyboards to console mode on boot and set escape sequence",
     MTYPE_SUBMENU, NULL, consoleModeConfigurationMenu,
     "Keyboard Console Mode Configuration Menu",
     MFLAG_DEVICES_XP},

    {"Set Fan Speed",
     "Set Fan speed.",
     MTYPE_FUNCTION, configureFanSpeedParameters, NULL, NULL,
     MFLAG_DEVICE_XP100 | MFLAG_DEVICES_XPI},

    {"Set System Date/Time",
     "Set the system date/time.",
     MTYPE_FUNCTION, setTime, NULL, NULL,
     MFLAG_DEVICES_NON_XPI},

    {"Update logging level",
     "Update logging level.",
     MTYPE_FUNCTION, configureLoggingLevel, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};


static MENUITEMDEF configurationMenu[] = {

    {"Network Configuration",
     "Network Configuration Options.",
     MTYPE_SUBMENU, NULL, networkConfigurationMenu,
     "Network Configuration Menu",
     MFLAG_DEVICES_ALL},

    {"Streaming Configuration",
     "Streaming Configuration Options.",
     MTYPE_SUBMENU, NULL, streamingConfigurationMenu,
     "Streaming Configuration Menu",
     MFLAG_DEVICES_ALL},

    {"Encoder Configuration",
     "Encoder Configuration Options.",
     MTYPE_SUBMENU, NULL, encoderConfigurationMenu,
     "Encoder Configuration Menu",
     MFLAG_DEVICES_V2D_TX | MFLAG_PORTTYPE_TX},

    {"Decoder Configuration",
     "Decoder Configuration Options.",
     MTYPE_SUBMENU, NULL, decoderConfigurationMenu,
     "Decoder Configuration Menu",
     MFLAG_DEVICES_V2D_RX | MFLAG_PORTTYPE_RX},

    {"Change Board Type",
     "Change the board type",
     MTYPE_FUNCTION, configureBoardInfo, NULL, NULL,
     MFLAG_DEVICES_XP | MFLAG_PORTTYPE_BOTH | MFLAG_DEVICES_XPI },

    {"Administration",
     "Administration Options.",
     MTYPE_SUBMENU, NULL, administrationConfigurationMenu,
     "Administration Menu",
     MFLAG_DEVICES_ALL},

    {"Storage Configuration",
     "Storage Configuration Options.",
     MTYPE_SUBMENU, NULL, storageConfigurationMenu,
     "Storage Configuration Menu",
     MFLAG_DEVICE_DMS | MFLAG_DEVICE_MS},

    {"System Control",
     "System Commands.",
     MTYPE_SUBMENU, NULL, systemCommandsMenu,
     "System Control Menu",
     MFLAG_DEVICES_ALL},

    {"Save to Flash",
     "Make current configuration permanent by\n" "writing to flash.",
     MTYPE_FUNCTION, saveToFlash, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Upgrade Software",
     "Upgrade the sofware on the current device.",
     MTYPE_FUNCTION, upgradeSoftware, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Revert to Defaults",
     "Revert the system back to its original\n" "factory settings.",
     MTYPE_FUNCTION, configureDefaults, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Exit",
     "Go back to Main Menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}
};

static MENUITEMDEF mainmenuitems[] = {

    {"Configuration",
     "Display/Add/Modify various V2O configuration parameters.",
     MTYPE_SUBMENU, NULL, configurationMenu, "Configuration Menu",
     MFLAG_DEVICES_ALL},

    {"Diagnostics",
     "Perform diagnostic procedures.",
     MTYPE_SUBMENU, NULL, diagnosticsMenu, "Diagnostics Menu",
     MFLAG_DEVICES_ALL},

    {"Version Information",
     "Diplay versions information.",
     MTYPE_FUNCTION, displayVersion, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {"Firmware Version Information",
     "Diplay firmware version and\n"
     "serial number of the unit.",
     MTYPE_FUNCTION, displayFirmwareImageStatus, NULL, NULL,
     MFLAG_DEVICES_NON_XPI},

    {"Firmware Version Information",
     "Diplay firmware version and\n"
     "serial number of the unit.",
     MTYPE_FUNCTION, displayXPiFirmwareImageStatus, NULL, NULL,
     MFLAG_DEVICES_XPI},

    {"System Menu",
     "IP Video Systems Diagnostics Menu.",
     MTYPE_SUBMENU, NULL, systemMenu, "System Menu",
     MFLAG_DEVICES_ALL | MFLAG_SYSTEM},

    {"Quit", "Quit menu",
     MTYPE_EXIT, NULL, NULL, NULL,
     MFLAG_DEVICES_ALL},

    {NULL, NULL, 0, NULL, NULL, NULL, 0}

};


static char *emptymesg[] = {
    "                                                          ",
    "                                                          ",
    "                                                          ",
    "                                                          "
};

static char *helpmesg =
    "Use </U>Up/Down<!U> arrow keys to navigate the menu.\n"
    "Press </U>Enter<!U> to select menu item or </U>Q<!U> to quit.";

static char *menu_help =
    "Navigation Help:\n\n\n"
    "Press </U>Up/Down<!U> arrow keys to navigate the menu.\n"
    "Enter a number to jump to the corresponding menu item.\n"
    "Press </U>Enter<!U> to select current menu item.\n"
    "Press </U>CTRL-Q<!U> or </U>Q<!U>  to return to previous menu.\n"
    "Press </U>CTRL-L<!U> at any time to redraw screen.\n"
    "Press </U>CTRL-X<!U> at any time to quit menu system entirely.\n\n\n"
    "Press any key to exit this display and return...";

static char *form_help =
    "Help for Data Entry in Forms:\n\n\n"
    "Press </U>Up/Down<!U> arrow keys or </U>TAB<!U> to select field to edit.\n"
    "Type ASCII characters to begin updating/modifing fields.\n"
    "Press </U>Enter<!U> to accept current value.\n"
    "Pressing </U>Enter<!U> on last field accepts all values.\n"
    "Press </U>CTRL-G<!U> to accept all current values.\n"
    "Press </U>CTRL-P<!U> or </U>CTRL-Q<!U> to abort data entry and quit.\n"
    "Press </U>CTRL-H<!U> or </U>Backspace<!U> to delete previous character.\n"
    "Press </U>CTRL-D<!U> to delete current character.\n"
    "\nPress F2 again for more help or any other key to quit...";

static char *form_help2 =
    "Help for Data Entry in Forms:\n\n\n"
    "Press </U>CTRL-K<!U> to delete from cursor to end of field.\n"
    "Press </U>CTRL-A/CTRL-E<!U> moves to beginning or end of field.\n"
    "Press </U>CTRL-F/CTRL-B<!U> moves forward/backward one character.\n"
    "Press </U>CTRL-U<!U> to undo (reset) current field.\n"
    "Press </U>CTRL-R<!U> to reset all fields to original values.\n"
    "Press </U>CTRL-L<!U> at any time to redraw screen.\n"
    "Press </U>CTRL-X<!U> at any time to quit menu system entirely.\n\n"
    "Press any key to exit this display and return...";


/**************************
 * Rrefresh entire screen *
 **************************/
static void refresh_screen(void)
{
/*     ioctl(fileno(stdin), FIOSETOPTIONS, OPT_RAW); */
    wrefresh(curscr);
}

int mainmenu(void)
{
    MENUINFO menuinfo;
    CDKSCREEN *menuscreen;
    CDKSCREEN *infoscreen;
    WINDOW *menu_wind = NULL;
    WINDOW *info_win = NULL;
    int menuwidth, menuheight, menustarty, menustartx;
    int rc;
    int hardware_profile;

    char *error_message = NULL;

    CURSES_INFO("Starting menu ...");

    /* Initialize curses */
    putenv("TERM=vt100");
    initscr();
    cbreak();
    noecho();
    keypad(curscr, TRUE);
    bzero((void *) &menuinfo, sizeof(menuinfo));

    /****************************************************
     * Create the window to be associated with the menu *
     ****************************************************/
    menuwidth = 60;
    menuheight = 18;
    menustarty = 2;
    menustartx =10;

    menu_wind = newwin(menuheight, menuwidth, menustarty, menustartx);

    if (menu_wind == NULL)
        CURSES_ERR("Could not create menu window\n");

    /**********************
     * Create Info window *
     **********************/
    info_win = newwin(4, menuwidth, menustarty + menuheight, menustartx);
    if (info_win == NULL)
        CURSES_ERR("Could not create help window\n");
    mwg = menu_wind;
    iwg = info_win;

    /**********************************************
     * Create the CDK Screens for the two windows *
     **********************************************/
    menuscreen = initCDKScreen(menu_wind);
    infoscreen = initCDKScreen(info_win);

    keypad(menu_wind, TRUE);
    keypad(info_win, TRUE);

    /***************************************
     * Draw box outside of any sub-windows *
     ***************************************/
    mvhline(1, 3, '-', 74);
    mvaddch(1, 2, '+');
    mvaddch(1, 76, '+');
    mvhline(23, 3, '-', 74);
    mvaddch(23, 2, '+');
    mvaddch(23, 76, '+');
    mvvline(2, 2, '|', 21);
    mvvline(2, 76, '|', 21);
    refresh();

    menuinfo.minfo_title = "Main Menu";

    /* Find out what kind to device we are expected to be. Use
       Device.xml rather than depending on the driver to tell us what we
       really are */
    hardware_profile = get_hardware_profile();
    switch (hardware_profile) {
    case HARDWARE_PROFILE_DMS:
    case HARDWARE_PROFILE_DMS25:
    case HARDWARE_PROFILE_DMS100:
    case HARDWARE_PROFILE_DMS150:
    case HARDWARE_PROFILE_DMS200:
    case HARDWARE_PROFILE_DMS120:
    case HARDWARE_PROFILE_DMS1000:
    case HARDWARE_PROFILE_DMS1200:
    case HARDWARE_PROFILE_DMS1500:
    case HARDWARE_PROFILE_DMS1550:
    case HARDWARE_PROFILE_DMS1600:
    case HARDWARE_PROFILE_DMS1700:
    case HARDWARE_PROFILE_GENERIC:
        current_device = MFLAG_DEVICE_DMS;
        break;
    case HARDWARE_PROFILE_V2D_TX:
        current_device = MFLAG_DEVICE_V2D_TX;
        break;
    case HARDWARE_PROFILE_V2D_RX:
        current_device = MFLAG_DEVICE_V2D_RX;
        break;
    case HARDWARE_PROFILE_MS:
    case HARDWARE_PROFILE_MS25:
    case HARDWARE_PROFILE_MS150:
        current_device = MFLAG_DEVICE_MS;
        break;
    case HARDWARE_PROFILE_XP200:
    case HARDWARE_PROFILE_XP200S:
    case HARDWARE_PROFILE_XP220:
    case HARDWARE_PROFILE_XP220S:
        current_device = MFLAG_DEVICE_XP200;
        break;
    case HARDWARE_PROFILE_XP100:
        current_device = MFLAG_DEVICE_XP100;
        break;
    case HARDWARE_PROFILE_XPI: 
        {
            HAL *hp = hal_acquire(0);
            current_device = MFLAG_DEVICE_XPI_TX;
            if (hp != NULL && hal_get_board_type(hp) == BOARDTYPE_RX) 
                current_device = MFLAG_DEVICE_XPI_RX;
        } 
        break;
    default:
        /* Fall back to DMS menu layout */
        current_device = MFLAG_DEVICE_DMS;
        break;
    }

    menuinfo.minfo_itemdefs = mainmenuitems;

    menuinfo.minfo_menuscreen = menuscreen;
    menuinfo.minfo_infoscreen = infoscreen;
    menuinfo.minfo_info_label =
        newCDKLabel(infoscreen, 0, 0, emptymesg, 3, FALSE, FALSE);
    menuinfo.minfo_title_label =
        newCDKLabel(menuscreen, 0, 0, emptymesg, 3, FALSE, FALSE);
    menuinfo.minfo_userptr = NULL;

    refreshCDKScreen(menuscreen);

    /*******************************************************
     * Call the entrypoint to the main menuhandler routine *
     *******************************************************/
    rc = menuhandler(&menuinfo);

    show_info(&menuinfo, NULL); /* Erases all memory in show_info */
    werase(menu_wind);
    werase(info_win);
    wrefresh(menu_wind);
    wrefresh(info_win);

  QUIT_LABEL:
    if (menuinfo.minfo_info_label != NULL)
        destroyCDKLabel(menuinfo.minfo_info_label);
    if (menuinfo.minfo_title_label != NULL)
        destroyCDKLabel(menuinfo.minfo_title_label);
    if (menuinfo.minfo_infoscreen != NULL)
        destroyCDKScreen(menuinfo.minfo_infoscreen);
    if (menuinfo.minfo_menuscreen != NULL)
        destroyCDKScreen(menuinfo.minfo_menuscreen);
    if (menu_wind != NULL)
        delwin(menu_wind);
    if (info_win != NULL)
        delwin(info_win);

    endwin();

    CURSES_INFO("Quitting menu ...");

    system("/usr/bin/clear");

    if (error_message != NULL)
        printf("%s", error_message);
    return 0;
}

static int menuhandler(MENUINFO * minfo)
{
    int c;
    MENUINFO menuinfo;
    MENUITEMDEF *itemdefs = NULL;
    MENUITEMDEF *itemdef_array = NULL;
    CDKSCREEN *menuscreen;
    CDKSCREEN *infoscreen;
    ITEM **menu_items = NULL;
    ITEM *curr_item = NULL;
    MENU *curr_menu = NULL;
    WINDOW *menu_wind = NULL;
    WINDOW *info_win = NULL;
    WINDOW *submenu_wind = NULL;
    DIALOG *dialog;
    int (*func_ptr) (MENUINFO *);
    char *item_str = NULL;
    int menuwidth, menuheight;
    int i, rc = RC_NORMAL, done;
    int n_items;
    int helpshown = FALSE;
    int retval = RC_NORMAL;
    int j;

    /*************************
     * Create items for menu *
     *************************/
    itemdefs = minfo->minfo_itemdefs;
    i = 0;
    while (1) {
        if (itemdefs[i].menuitem_name == NULL)
            break;
        i++;
    }
    n_items = i + 1;

    /* Filter out menu items that are not applicable */
    itemdef_array = (MENUITEMDEF *) calloc(n_items, sizeof(MENUITEMDEF));
    j = 0;
    for (i = 0; i < n_items; i++) {
        if (((minfo->minfo_itemdefs[i].menuitem_flags & current_device) ==
             current_device) ||
            (minfo->minfo_itemdefs[i].menuitem_name == NULL)) {

            if ((minfo->minfo_itemdefs[i].menuitem_flags & MFLAG_SYSTEM) &&
                (system_mode == 0))
                continue;

            memcpy(&itemdef_array[j], &minfo->minfo_itemdefs[i],
                   sizeof(MENUITEMDEF));
            j++;
        }
    }
    n_items = j;

    menu_items = (ITEM **) calloc(n_items, sizeof(ITEM *));
    if (menu_items == NULL)
        CURSES_ERR("Could not allocate memory for menu items\n");


    item_str = (char *) calloc(n_items, 4);
    if (item_str == NULL)
        CURSES_ERR("Could not allocate memory for item strings\n");



    for (i = 0; i < n_items; i++) {
        menu_items[i] = NULL;
    }
    for (i = 0; i < n_items - 1; i++) {
        sprintf(item_str + (i * 4), "%d.", i + 1);
        menu_items[i] = new_item(item_str + (i * 4),
                                 itemdef_array[i].menuitem_name);
        if (menu_items[i] == NULL)
            CURSES_ERR("Could not create menu item %d\n", i);
        else
            set_item_userptr(menu_items[i], (void *) &itemdef_array[i]);
    }


    /***************
     * Create menu *
     ***************/
    if ((curr_menu = new_menu((ITEM **) menu_items)) == NULL)
        CURSES_ERR("Could not create menu from items\n");

    /****************************************************
     * Create the window to be associated with the menu *
     ****************************************************/
    menuscreen = minfo->minfo_menuscreen;
    infoscreen = minfo->minfo_infoscreen;
    menu_wind = menuscreen->window;
    info_win = infoscreen->window;

    /***************************************
     * Set menu main window and sub window *
     ***************************************/
    if ((rc = set_menu_win(curr_menu, menu_wind)) != E_OK)
        CURSES_ERR("Could not set menu window %d \n", rc);

    getmaxyx(menu_wind, menuheight, menuwidth);
    submenu_wind = derwin(menu_wind,
                          menuheight - TITLE_HEIGHT,
                          menuwidth - SUBWIN_PAD * 2,
                          TITLE_HEIGHT, SUBWIN_PAD);
    if (submenu_wind == NULL)
        CURSES_ERR("Could not create submenu_wind\n");

    if ((rc = set_menu_sub(curr_menu, submenu_wind)) != E_OK)
        CURSES_ERR("Could not set sub-menu window %d \n", rc);

    /******************************
     * Set menu format attributes *
     ******************************/
    if ((rc = set_menu_format(curr_menu, 14, 1)) != E_OK)
        CURSES_ERR("Could not set menu format %d \n", rc);

    if ((rc = set_menu_mark(curr_menu, "")) != E_OK)
        CURSES_ERR("Could not set menu mark  %d \n", rc);

    menu_opts_off(curr_menu, O_IGNORECASE);

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    /*****************
     * Post the menu *
     *****************/
    if (post_menu(curr_menu) != E_OK)
        CURSES_ERR("Unable to post menu\n");


    /**********************************
     * Main Character processing loop *
     **********************************/
    (void) current_item(curr_menu);
    while (1) {
        done = FALSE;

        /************************************
         * Print help for current menu item *
         ************************************/
        curr_item = current_item(curr_menu);
        if (helpshown == FALSE) {
            show_info(minfo, helpmesg);
            helpshown = TRUE;
        }

        /**************************************
         * Get the cursur to the current item *
         **************************************/
        pos_menu_cursor(curr_menu);

        /***********************
         * Wait for user input *
         ***********************/
        c = wgetch(menu_wind);
        switch (c) {
        case KEY_DOWN:
            menu_driver(curr_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(curr_menu, REQ_UP_ITEM);
            break;
        case KEY_NPAGE:
            menu_driver(curr_menu, REQ_SCR_DPAGE);
            break;
        case KEY_PPAGE:
            menu_driver(curr_menu, REQ_SCR_UPAGE);
            break;

            /**************************
             * Context sensitive help *
             **************************/
        case KEY_F2:
            if (curr_item == NULL ||
                (itemdefs = item_userptr(curr_item)) == NULL)
                break;
            werase(menu_wind);
            werase(info_win);
            unpost_menu(curr_menu);
            show_info(minfo, " ");
            strcpy(tempbuf, "Help for \"");
            strcat(tempbuf, itemdefs->menuitem_name);
            strcat(tempbuf, "\":\n\n\n");
            strcat(tempbuf, itemdefs->menuitem_help);
            strcat(tempbuf, "\n\n\nPress any key to continue...");
            dialog = show_dialog(minfo, 0, 0, tempbuf);
            get_anykey(minfo);

            remove_dialog(dialog);
            werase(menu_wind);
            werase(info_win);
            show_title(minfo, NULL);
            post_menu(curr_menu);
            helpshown = FALSE;
            break;

            /******************
             * Navigaton help *
             ******************/
        case KEY_F3:
            werase(menu_wind);
            werase(info_win);
            unpost_menu(curr_menu);
            show_info(minfo, " ");
            dialog = show_dialog(minfo, 0, 0, menu_help);
            get_anykey(minfo);

            remove_dialog(dialog);
            werase(menu_wind);
            werase(info_win);
            show_title(minfo, NULL);
            post_menu(curr_menu);
            helpshown = FALSE;
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
                int ii = c - 49;
                int morechars = 0;
                wtimeout(menu_wind, 1);
                while (wgetch(menu_wind) != ERR) {
                    if (morechars++ > 10)
                        break;
                }
                if (morechars)
                    break;
                wtimeout(menu_wind, -1);

                if (n_items > ii)
                    set_current_item(curr_menu, menu_items[ii]);
            }

        case KEY_ENTER:
        case KEY_RETURN:
            curr_item = current_item(curr_menu);
            if (curr_item == NULL ||
                (itemdefs = item_userptr(curr_item)) == NULL)
                break;

            switch (itemdefs->menuitem_type) {
            case MTYPE_EXIT:
                done = TRUE;
                retval = RC_NORMAL;
                break;
            case MTYPE_FUNCTION:
            case MTYPE_SUBMENU:
                menuinfo.minfo_menuscreen = menuscreen;
                menuinfo.minfo_infoscreen = infoscreen;
                menuinfo.minfo_info_label = minfo->minfo_info_label;
                menuinfo.minfo_title_label = minfo->minfo_title_label;
                menuinfo.minfo_userptr = minfo->minfo_userptr;

                if ((itemdefs->menuitem_flags & MFLAG_PORTTYPE_MASK) &&
                    (menuinfo.minfo_userptr != NULL)) {
                    CURSES_ERR
                        ("Bug: Parent menu has already set HAL pointer for us!! : %s",
                         itemdefs->menuitem_name);

                }

                if (itemdefs->menuitem_type == MTYPE_SUBMENU) {
                    CURSES_DEBUG("Handling submenu %s.",
                                 itemdefs->menuitem_title);
                    menuinfo.minfo_title = itemdefs->menuitem_title;
                    menuinfo.minfo_itemdefs = itemdefs->menuitem_submenu;

                    /* Check if portselector needs to be invoked for
                     * the whole submenu */
                    if (itemdefs->menuitem_flags & MFLAG_PORTTYPE_MASK) {
                        menuinfo.minfo_userptr = itemdefs;
                        itemdefs->menuitem_func = menuhandler;
                        func_ptr = portSelector;
                    }
                    else {
                        func_ptr = menuhandler; /* recursive */
                    }
                }
                else {          /* MTYPE_FUNCTION */
                    CURSES_DEBUG("Handling function %s.",
                                 itemdefs->menuitem_name);
                    menuinfo.minfo_title = itemdefs->menuitem_name;

                    /* If per port flag is set on current function, invoke
                       the portselector, passing on original function
                       ptr. */
                    if (itemdefs->menuitem_flags & MFLAG_PORTTYPE_MASK) {
                        menuinfo.minfo_userptr = itemdefs;
                        func_ptr = portSelector;
                    }
                    else {
                        func_ptr = itemdefs->menuitem_func;
                    }
                }

                /*************************
                 * Clear current display *
                 *************************/
                werase(menu_wind);
                werase(info_win);
                unpost_menu(curr_menu);

                /*********************
                 * Call the function *
                 *********************/
                if (func_ptr != NULL) {
                    rc = (*func_ptr) (&menuinfo);
                    CURSES_DEBUG("Returning from  %s.",
                                 menuinfo.minfo_title);
                    if (rc == RC_EXIT) {        /* CTRL-X from submenu/subfucntion ? */
                        done = TRUE;
                        retval = RC_EXIT;
                        break;
                    }
                }

                /***************************
                 * Restore current display *
                 ***************************/
                werase(menu_wind);
                werase(info_win);
                show_title(minfo, NULL);
                post_menu(curr_menu);
                helpshown = FALSE;
                break;
            }

            break;

        case 'Q':
        case 'q':
        case '':
        case KEY_F1:
            done = TRUE;
            retval = RC_NORMAL;
            break;

        case '':
            done = TRUE;
            retval = RC_EXIT;
            break;

            /**************************************
             * CTRL-L refreshes the entire screen *
             **************************************/
        case '':
            refresh_screen();
            break;

        default:
            if ((rc = menu_driver(curr_menu, c)) == E_NO_MATCH) {
                /* Clear pattern and try again */
                set_menu_pattern(curr_menu, unctrl(c));
            }
            break;
        }
        if (done == TRUE)
            break;
    }

    /***************
     * Unpost Menu *
     ***************/
    unpost_menu(curr_menu);

  QUIT_LABEL:

    if (curr_menu != NULL)
        free_menu(curr_menu);
    if (menu_items != NULL) {
        for (i = 0; i < n_items; ++i) {
            if (menu_items[i] != NULL)
                free_item(menu_items[i]);
        }
        free(menu_items);
    }
    if (item_str != NULL)
        free(item_str);

    if (submenu_wind != NULL)
        delwin(submenu_wind);

    if (itemdef_array != NULL)
        free(itemdef_array);

    return retval;
}

static void post_func(MENU * menu)
{

    redrawwin(menu_sub(menu));
    wrefresh(menu_sub(menu));
}

/**********************************************************
 * Create a generic item selector for the given number of *
 * items. Uses menu library to create this widget.        *
 **********************************************************/
MENU *create_selector(int numb_items, char *(*selector_cb) (int ident))
{

    MENU *menu = NULL;
    ITEM **items = NULL;
    char **item_str = NULL;
    char *buf, *str;
    int i;

    item_str = (char **) calloc(numb_items + 1, sizeof(char *));
    items = (ITEM **) calloc(numb_items + 1, sizeof(ITEM *));
    for (i = 0; i < numb_items; i++) {
        buf = selector_cb(i);
        str = (char *) malloc(strlen(buf) + 8);
        sprintf(str, "%d.", i + 1);
        sprintf(str + 6, "%s", buf);
        item_str[i] = str;
        items[i] = new_item(str, str + 6);
    }
    items[numb_items] = NULL;
    item_str[numb_items] = NULL;

    menu = new_menu(items);
    set_menu_userptr(menu, item_str);   /* To be able to free later */

    set_menu_format(menu, 14, 1);       /* 14 rows at a time */

    set_menu_mark(menu, "");

    set_menu_init(menu, post_func);

    menu_opts_off(menu, O_IGNORECASE);

    return menu;
}

void clear_selector_init(MENU * menu)
{
    set_menu_init(menu, NULL);
}

void set_selector_init(MENU * menu)
{
    set_menu_init(menu, post_func);
}

/***************************************************
 * Throw away old items and create fresh item list *
 ***************************************************/
int refresh_selector(MENU * menu, int numb_items,
                     char *(*selector_cb) (int ident))
{

    ITEM **items = NULL, **old_items = NULL;
    char **item_str = NULL;
    char *buf, *str;
    int i;
    int itemcount;
    int rc = RC_NORMAL;

    /****************************************************
     * First free all data associated with current menu *
     ****************************************************/
    if (menu == NULL)
        return RC_ERROR;

    old_items = menu_items(menu);
    if (old_items == NULL)
        return RC_ERROR;

    itemcount = item_count(menu);

    if ((item_str = menu_userptr(menu)) != NULL) {
        for (i = 0; i < itemcount; i++) {
            if (item_str[i] != NULL)
                free(item_str[i]);
        }
        free(item_str);
    }

    /*************************************************
     * Allocate fresh data and set the menu pointers *
     *************************************************/
    item_str = (char **) calloc(numb_items + 1, sizeof(char *));
    items = (ITEM **) calloc(numb_items + 1, sizeof(ITEM *));
    for (i = 0; i < numb_items; i++) {
        buf = selector_cb(i);
        str = (char *) malloc(strlen(buf) + 8);
        sprintf(str, "%d.", i + 1);
        sprintf(str + 6, "%s", buf);
        item_str[i] = str;
        items[i] = new_item(str, str + 6);
    }
    items[numb_items] = NULL;
    item_str[numb_items] = NULL;

    rc = set_menu_items(menu, items);
    set_menu_userptr(menu, item_str);   /* To be able to free later */

    /********************************************************
     * The old item list should be freed only after we have *
     * associated a new item list to the menu.              *
     ********************************************************/
    if (rc == E_OK) {
        for (i = 0; i < itemcount; i++)
            free_item(old_items[i]);
        free(old_items);
    }
    return rc;
}

/***************************************************************
 * Free all resources allocated for creating the selector menu *
 ***************************************************************/
void delete_selector(MENU * menu)
{

    ITEM **items;
    char **item_str = NULL;
    int itemcount, i;

    if (menu == NULL)
        return;

    items = menu_items(menu);
    if (items == NULL)
        return;

    itemcount = item_count(menu);

    if ((item_str = menu_userptr(menu)) != NULL) {
        for (i = 0; i < itemcount; i++) {
            if (item_str[i] != NULL)
                free(item_str[i]);
        }
        free(item_str);
    }

    free_menu(menu);

    for (i = 0; i < itemcount; i++)
        free_item(items[i]);
    free(items);
}

/**************************
 * Form processor routine *
 **************************/
int process_selector(MENUINFO * minfo, MENU * menu, char *keylist)
{

    ITEM *curr_item;
    int rc;
    int ch;
    int done = FALSE;
    char *keys;
    int retval = RC_NORMAL;

    if (keylist != NULL && keylist[0] == 'T') {
        wtimeout(minfo->minfo_menuscreen->window, 250);
    }
    else {
        wtimeout(minfo->minfo_menuscreen->window, -1);
    }

    /*************************************
     * Loop through to get user requests *
     *************************************/
    while (1) {
        ch = wgetch(minfo->minfo_menuscreen->window);
        switch (ch) {

        case KEY_DOWN:
            menu_driver(menu, REQ_DOWN_ITEM);
            break;

        case KEY_UP:
            menu_driver(menu, REQ_UP_ITEM);
            break;

        case 'n':
        case 'N':
            menu_driver(menu, REQ_SCR_DPAGE);
            break;

        case 'p':
        case 'P':
            menu_driver(menu, REQ_SCR_UPAGE);
            break;

        case KEY_ENTER:
        case KEY_RETURN:
            curr_item = current_item(menu);
            if (curr_item == NULL)
                break;
            done = TRUE;
            retval = item_index(curr_item);
            keylist[0] = '\0';
            break;

        case ERR:
            // Timedout
            curr_item = current_item(menu);
            if (curr_item == NULL)
                break;
            done = TRUE;
            retval = item_index(curr_item);
            keylist[0] = 'T';
            break;

        case 'Q':
        case 'q':
        case '':
        case KEY_F1:
            done = TRUE;
            retval = RC_QUIT;
            break;

            /*************************************************
             * CTRL-X attempts to quit the whole menu system *
             *************************************************/
        case '':
            done = TRUE;
            retval = RC_EXIT;
            break;

            /**************************************
             * CTRL-L refreshes the entire screen *
             **************************************/
        case '':
            refresh_screen();
            break;

        default:
            /* Look for special keys */
            if (keylist != NULL) {
                keys = keylist;
                while (*keys != '\0') {
                    if (*keys == ch) {
                        curr_item = current_item(menu);
                        done = TRUE;
                        retval = item_index(curr_item);
                        keylist[0] = ch;
                        break;
                    }
                    keys++;
                }
                if (done == TRUE)
                    break;
            }
            if ((rc = menu_driver(menu, ch)) == E_NO_MATCH) {
                /* Clear pattern and try again */
                set_menu_pattern(menu, unctrl(ch));
            }
            break;
        }
        if (done == TRUE)
            break;
    }

    return retval;
}

void set_selector_item(MENU * menu, int idx)
{
    ITEM **items;

    if (menu == NULL)
        return;

    items = menu_items(menu);
    if (items == NULL)
        return;

    if (items[idx] != NULL)
        set_current_item(menu, items[idx]);

}

/**********************************
 * Set the windows for given menu *
 **********************************/
int set_menu_windows(MENUINFO * minfo, MENU * menu)
{
    WINDOW *menu_wind = NULL;
    WINDOW *submenu_wind = NULL;
    int menuwidth, menuheight;
    int rc = RC_NORMAL;

    menu_wind = minfo->minfo_menuscreen->window;

    /********************
     * Set menu windows *
     ********************/
    getmaxyx(menu_wind, menuheight, menuwidth);
    submenu_wind = derwin(menu_wind,
                          menuheight - TITLE_HEIGHT,
                          menuwidth - SUBWIN_PAD * 2,
                          TITLE_HEIGHT, SUBWIN_PAD);
    if (submenu_wind == NULL)
        CURSES_ERR("Could not create submenu_wind\n");

    if ((rc = set_menu_win(menu, menu_wind)) != E_OK)
        CURSES_ERR("Could not set menu window %d \n", rc);

    if ((rc = set_menu_sub(menu, submenu_wind)) != E_OK)
        CURSES_ERR("Could not set sub-menu window %d \n", rc);

  QUIT_LABEL:
    return rc;

}

/*****************************
 * Free sub-window resources *
 *****************************/
void unset_menu_windows(MENU * menu)
{
    WINDOW *submenu_wind = NULL;

    submenu_wind = menu_sub(menu);
    if (submenu_wind != NULL)
        delwin(submenu_wind);
}

#define ROW_WIDTH 58
/******************************************************************
 * Create a generic name-value form entry using FORMITEMDEF array *
 ******************************************************************/
FORM *create_form(FORMITEMDEF * fitems, int starty, int startx,
                  void *userptr)
{
    FORM *form = NULL;
    FIELD **fields;
    int i, len, pad = 2;
    int idx = 0;
    int fieldcount = 0;
    int maxlabelsize = 0;
    int maxlabelsize2 = 0;
    int field_length;
    int field_height;
    int max_field_length;
    int row_width;

    /***************************************************************
     * Find number of fields needed and the maximum size of labels *
     ***************************************************************/
    while (fitems[idx].formitem_name != NULL) {

        if (fitems[idx].formitem_flags & FFLAG_HIDE) {
            idx++;
            continue;
        }

        if (!(fitems[idx].formitem_flags & FFLAG_FOLLOW) &&
            (len = strlen(fitems[idx].formitem_name)) > maxlabelsize) {
            maxlabelsize = len;
            pad = 1;
        }
        else if ((fitems[idx].formitem_flags & FFLAG_FOLLOW) &&
                 (len =
                  strlen(fitems[idx].formitem_name)) > maxlabelsize2) {
            maxlabelsize2 = len;
        }

        idx++;
        fieldcount++;
    }

    fieldcount = (fieldcount * 2);
    maxlabelsize += 1;
    maxlabelsize2 += 1;

    fields = (FIELD **) malloc(sizeof(FIELD *) * (fieldcount + 1));

    if (fields == NULL)
        goto QUIT_LABEL;

    /********************************
     * Create the labels and fields *
     ********************************/
    starty--;
    for (idx = i = 0; i < fieldcount; idx++) {
        // Note: "idx" jumps by one, "i" jumps by two in this loop

        max_field_length = 0;
        field_height = 1;

        if (fitems[idx].formitem_flags & FFLAG_HIDE) {
            continue;
        }

        if (!(fitems[idx].formitem_flags & FFLAG_FOLLOW)) {
            starty++;
            fields[i] = new_field(1, maxlabelsize, starty, startx, 0, 0);

            field_length = fitems[idx].formitem_len;
            field_height = 1;
            row_width = ROW_WIDTH;
            if (idx < (fieldcount - 1) &&
                (fitems[idx + 1].formitem_flags & FFLAG_FOLLOW)) {
                row_width = 29;
            }
            /* For fields that do not fit in a row, make them
             dynamic. If the form is intended for display only, show
             two rows */
            if ((fitems[idx].formitem_len +
                 startx + maxlabelsize + pad) > row_width) {
                field_length = row_width - (startx + maxlabelsize + pad);
                max_field_length = fitems[idx].formitem_len;
                if ((fitems[idx].formitem_flags & FFLAG_DISPLAYONLY) &&
                    !(fitems[idx + 1].formitem_flags & FFLAG_FOLLOW)) {
                    field_height = 2;
                }

            }

            fields[i + 1] = new_field(field_height, field_length,
                                      starty, startx + maxlabelsize + pad,
                                      0, 1);

            // Adjust offset for start of next row
            if (field_height > 1)
                starty += (field_height - 1);

        }
        else {
            fields[i] =
                new_field(1, maxlabelsize, starty, startx + 30, 0, 0);

            field_length = fitems[idx].formitem_len;
            if ((fitems[idx].formitem_len +
                 startx + 30 + maxlabelsize2 + pad) > ROW_WIDTH) {
                field_length = ROW_WIDTH - (startx + 30 + maxlabelsize2 + pad);
                max_field_length = fitems[idx].formitem_len;

            }
            fields[i + 1] =
                new_field(1, field_length, starty,
                          startx + 30 + maxlabelsize2 + pad, 0, 1);
        }
        set_field_buffer(fields[i], 0, fitems[idx].formitem_name);      /* set the label */
        field_opts_off(fields[i], O_ACTIVE);    /* label is
                                                   non-traversible */

        /* For fields that do not fit in a row, make them dynamic */
        if (max_field_length > 0) {
            field_opts_off(fields[i + 1], O_STATIC);
            set_max_field(fields[i + 1], max_field_length);
        }

        if (!(fitems[idx].formitem_options & O_EDIT))
            set_field_back(fields[i + 1], A_UNDERLINE); /* Print a line for
                                                           the option */
        else
            set_field_fore(fields[i + 1], A_BOLD);

        field_opts_off(fields[i + 1], O_AUTOSKIP);      /* Don't go to next field
                                                           when this field is
                                                           filled up */

        if (fitems[idx].formitem_options)       /* Apply custom options */
            field_opts_off(fields[i + 1], fitems[idx].formitem_options);

        set_field_userptr(fields[i + 1], (void *) &fitems[idx]);


        if (fitems[idx].formitem_initval != NULL) {     /* Initialize field */
            int ident = fitems[idx].formitem_ident;
            set_field_buffer(fields[i + 1], 0,
                             (*fitems[idx].formitem_initval) (tempbuf,
                                                              userptr,
                                                              ident));
            set_field_buffer(fields[i + 1], 1, tempbuf);
        }

        /****************************
         * Set validation if needed *
         ****************************/
        switch (fitems[idx].formitem_type) {
        case FTYPE_ALNUM:
            set_field_type(fields[i + 1], TYPE_ALNUM,
                           fitems[idx].formitem_min);
            break;
        case FTYPE_ALPHA:
            set_field_type(fields[i + 1], TYPE_ALPHA,
                           fitems[idx].formitem_min);
            break;
        case FTYPE_ENUM:
            set_field_type(fields[i + 1], TYPE_ENUM,
                           (char **) (fitems[idx].formitem_min), TRUE,
                           FALSE);
            break;
        case FTYPE_ENUM_RANGE:
        case FTYPE_ENUM_IMMEDIATE:
            set_field_type(fields[i + 1], TYPE_ENUM,
                           (char **) (fitems[idx].formitem_min), FALSE,
                           FALSE);
            break;
        case FTYPE_INTEGER:
        case FTYPE_INTEGER_RANGE:
        case FTYPE_INTEGER_IMMEDIATE:
            set_field_type(fields[i + 1], TYPE_INTEGER, 1,
                           (long) fitems[idx].formitem_min,
                           (long) fitems[idx].formitem_max);
            break;
        case FTYPE_NUMERIC:
        case FTYPE_NUMERIC_IMMEDIATE:
            set_field_type(fields[i + 1], TYPE_NUMERIC, 3,
                           (double) fitems[idx].formitem_min,
                           (double) fitems[idx].formitem_max);

        case FTYPE_NUMERIC_LOW:
        case FTYPE_NUMERIC_LOW_IMMEDIATE:
            set_field_type(fields[i + 1], TYPE_NUMERIC, 2,      /* Lower precision */
                           (double) fitems[idx].formitem_min,
                           (double) fitems[idx].formitem_max);
            break;
        case FTYPE_IPV4:
            set_field_type(fields[i + 1], TYPE_IPV4);
            break;

        case FTYPE_REGEXP:
        case FTYPE_REGEXP_IMMEDIATE:
        case FTYPE_EVALUATE:
        case FTYPE_EVALUATE_IMMEDIATE:
            set_field_type(fields[i + 1], TYPE_REGEXP,
                           (char *) (fitems[idx].formitem_min));

        default:
            break;
        }

        /**************************************************
         * Initially all fields are marked as not changed *
         **************************************************/
        set_field_status(fields[i + 1], FALSE);

        i += 2;
    }
    fields[fieldcount] = NULL;

    /*******************
     * Create the form *
     *******************/
    form = new_form(fields);
    form_opts_off(form, O_BS_OVERLOAD);
    set_form_userptr(form, userptr);

  QUIT_LABEL:

    return form;
}

/********************************************************************
 * Free resources allocated to create the form with the create_form *
 * routine.                                                         *
 ********************************************************************/
void delete_form(FORM * form)
{

    FIELD **fields;
    int fieldcount, i;

    if (form == NULL)
        return;

    fields = form_fields(form);
    if (fields == NULL)
        return;

    fieldcount = field_count(form);

    free_form(form);

    for (i = 0; i < fieldcount; i++)
        free_field(fields[i]);
    free(fields);
}

/******************************************************************
 * Refresh all values in the form by re-initilizing all fields to *
 * their current values.                                          *
 ******************************************************************/
static void refresh_form_fields(FORM * form)
{

    FIELD **fields;
    FORMITEMDEF *fitem;
    int fieldcount, i;

    if (form == NULL)
        return;

    fields = form_fields(form);
    if (fields == NULL)
        return;

    fieldcount = field_count(form);


    for (i = 0; i < fieldcount; i++) {
        fitem = field_userptr(fields[i]);
        if ((fitem != NULL) && (fitem->formitem_initval != NULL)) {     /* Initialize field */
            int ident = fitem->formitem_ident;
            set_field_buffer(fields[i], 0,
                             (*fitem->formitem_initval) (tempbuf,
                                                         form_userptr
                                                         (form), ident));
            set_field_buffer(fields[i], 1, tempbuf);

#ifdef EXPERIMENTAL
            if (fitem->formitem_flags & FFLAG_DISABLE)
                field_opts_off(fields[i], O_ACTIVE);
            else
                field_opts_on(fields[i], O_ACTIVE);
#endif
        }
    }

}

/******************************************************************
 * Refresh enable or disable of fields in the form.               *
 ******************************************************************/
void refresh_form_disable(FORM * form)
{

    FIELD **fields;
    FORMITEMDEF *fitem;
    int fieldcount, i;

    if (form == NULL)
        return;

    fields = form_fields(form);
    if (fields == NULL)
        return;

    fieldcount = field_count(form);


    for (i = 0; i < fieldcount; i++) {
        fitem = field_userptr(fields[i]);
        if ((fitem != NULL) && (fitem->formitem_initval != NULL)) {
            if (fitem->formitem_flags & FFLAG_DISABLE) {
                field_opts_off(fields[i], O_ACTIVE);
                set_field_back(fields[i], A_DIM);
            }
            else {
                field_opts_on(fields[i], O_ACTIVE);
                set_field_back(fields[i], A_UNDERLINE);
            }
        }
    }
}

/**********************************************************
 * Refresh all values in the form and re-display the form *
 **********************************************************/
void refresh_form(FORM * form)
{
    WINDOW *form_wind = NULL;

    refresh_form_fields(form);
    form_wind = form_win(form);
    wrefresh(form_wind);
}

/*********************************
 * Restore curses input settings *
 *********************************/
void restore_curs_inopts(void)
{
    struct termios change_termios;
    int fd = 0;                 /* stdin */

    cbreak();
    noecho();
    keypad(curscr, TRUE);
    if (tcgetattr(fd, &change_termios) < 0) {
        CURSES_INFO("Cannot get terminal attributes.");
        return;
    }
    change_termios.c_iflag &= ~IXON;

    tcsetattr(fd, TCSANOW, &change_termios);
}

int map_numpad(int ch)
{
// Loop to get the number pad keys to work in MENU curses
    switch (ch) {

    case 351:
        ch = 48;
        break;

    case 348:
        ch = 49;
        break;

    case 350:
        ch = 50;
        break;

    case 349:
        ch = 51;
        break;

    case 269:
        ch = 52;
        break;

    case 270:
        ch = 53;
        break;

    case 271:
        ch = 54;
        break;

    case 273:
        ch = 55;
        break;

    case 274:
        ch = 56;
        break;

    case 264:
        ch = 57;
        break;

    case 352:
        ch = 46;
        break;

    }
    return ch;
}

/**************************
 * Form processor routine *
 **************************/
int process_form(MENUINFO * minfo, FORM * form,
                 char *default_mesg, int form_flags)
{

    FORMITEMDEF *fitem;
    FIELD *first_field, *last_field, *last_row_field;
    FIELD *curr_field, **fields;
    WINDOW *form_wind = NULL;
    DIALOG *dialog;
    char *buf;
    int fieldcount;
    int ch;
    int done = FALSE;
    int rc = 0;
    int info_changed = FALSE;
    int retval = RC_NORMAL;
    unsigned char do_refresh = 1;
    int do_edit = 0;
    static int refresh_rate = 250;
    int val;

    /*************************************
     * Loop through to get user requests *
     *************************************/
    fields = form_fields(form);
    fieldcount = field_count(form);

    /******************************
     * Find first and last fields *
     ******************************/
    first_field = fields[1];
    last_field = fields[fieldcount - 1];
    fitem = field_userptr(last_field);

    if (fitem->formitem_flags & FFLAG_FOLLOW)
        /* PSCW - seems to me wrong to have this line here */
        /* last_field = fileds[fieldcount-3];              */
        /* So I replaced with the following 3 lines        */
        last_row_field = fields[fieldcount - 2];
    else
        last_row_field = last_field;

    form_wind = form_win(form);

    wtimeout(form_wind, -1);

    /***********************************
     * Check if we are displaying only *
     ***********************************/
    if (form_flags & FORM_DISPLAY_ONLY) {
        wrefresh(form_wind);
        leaveok(form_wind, TRUE);
        show_info(minfo, default_mesg);
        while (done == FALSE) {
            wmove(curscr, 0, 0);
            if (form_flags & FORM_AUTO_REFRESH) {
                if (do_refresh)
                    wtimeout(form_wind, refresh_rate);
                else
                    wtimeout(form_wind, -1);
            }

            ch = wgetch(form_wind);
            switch (ch) {
                /********************************
                 * For timeout, do auto refresh *
                 ********************************/
            case ERR:
                refresh_form_fields(form);
                break;

                /***************************************************
                 * CTRL-Q uncoditionally quits the forms processor *
                 ***************************************************/
            case '':
            case 'Q':
            case 'q':
            case KEY_F1:
                done = TRUE;
                retval = RC_QUIT;
                break;

                /*************************************************
                 * CTRL-X attempts to quit the whole menu system *
                 *************************************************/
            case '':
                done = TRUE;
                retval = RC_EXIT;
                break;

                /*******************************
                 * CTRL-R refreshes all values *
                 *******************************/
            case '':
                refresh_form_fields(form);
                break;

                /**************************************
                 * CTRL-L refreshes the entire screen *
                 **************************************/
            case '':
                refresh_screen();
                break;

            case KEY_RIGHT:
                refresh_rate += 250;
                if (refresh_rate > 5000)
                    refresh_rate = 5000;
                break;

            case KEY_LEFT:
                refresh_rate -= 250;
                if (refresh_rate < 250)
                    refresh_rate = 250;
                break;

                /****************************************
                 * Implement Stop/Start of Auto refresh *
                 ****************************************/
            case 'S':
            case 's':
                if (do_edit == 0) {
                    do_refresh = 1 - do_refresh;
                    if (do_refresh == 0)
                        show_info(minfo,
                                  "Auto Refresh halted. "
                                  "Press </U>S<!U> again to resume...");
                    else
                        show_info(minfo, default_mesg);
                }
                break;

                /**********************************
                 * Implement Single field editing *
                 **********************************/
            case 'E':
            case 'e':
                if (form_flags & FORM_EDITABLE) {
                    do_edit = 1;
                    do_refresh = 0;
                    leaveok(form_wind, FALSE);
                    show_info(minfo,
                              "Entered Edit Mode. "
                              "Press </U>Up/Down<!U> arrow keys to select\n"
                              "field to edit. Press </U>Enter<!U> when done.");
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

                /**************************************************
                 * Honor inter-field navigation only in edit mode *
                 **************************************************/
            case KEY_DOWN:
                if (do_edit == 1) {
                    rc = form_driver(form, REQ_NEXT_FIELD);     /* go down */
                }
                else if (do_edit == 2) {
                    leaveok(form_wind, TRUE);
                    do_refresh = 1;
                    do_edit = 0;
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

            case KEY_UP:
                if (do_edit == 1) {
                    rc = form_driver(form, REQ_PREV_FIELD);     /* go down */
                }
                else if (do_edit == 2) {
                    leaveok(form_wind, TRUE);
                    do_refresh = 1;
                    do_edit = 0;
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

            case KEY_ENTER:
            case KEY_RETURN:
                if (do_edit == 2) {
                    pos_form_cursor(form);
                    curr_field = current_field(form);
                    fitem = field_userptr(curr_field);
                    rc = form_driver(form, REQ_VALIDATION);
                    /************************************************
                     * Use custom validation function pointer which *
                     * is overloaded to set current value           *
                     ************************************************/
                    if (fitem->formitem_val != NULL) {
                        buf = field_buffer(curr_field, 0);
                        stripWhiteSpace(vBOTH, buf);
                        rc = (*(fitem->formitem_val)) (buf);
                        if (rc == RC_ERROR) {
                            show_info(minfo,
                                      "Invalid Value!! Rejected...\n");
                            sleep(2);
                        }
                    }
                    leaveok(form_wind, TRUE);
                    do_refresh = 1;
                    do_edit = 0;
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

            case KEY_BACKSPACE:
            case 0x7f:
            case '':
            case KEY_DC:
                if (do_edit == 2) {
                    form_driver(form, REQ_DEL_PREV);
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

            case '':
                if (do_edit == 2) {
                    form_driver(form, REQ_DEL_CHAR);
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
                break;

            default:
                if (do_edit != 0) {
                    do_edit = 2;
                    rc = form_driver(form, ch);
                    if (rc != E_OK) {
                        leaveok(form_wind, TRUE);
                        do_refresh = 1;
                        do_edit = 0;
                    }
                }
                else {
                    done = TRUE;
                    retval = ch;
                }
            }
        }
        leaveok(form_wind, FALSE);
        nodelay(form_wind, FALSE);      /* reset delay flag */

        goto QUIT_LABEL;        /* Done with FORM_DISPLAY_ONLY */
    }

    /**********************************
     * Regular processing for editing *
     **********************************/
    show_info(minfo, default_mesg);
    while (done == FALSE) {
        pos_form_cursor(form);
        curr_field = current_field(form);
        fitem = field_userptr(curr_field);
        ch = wgetch(form_wind);

        ch = map_numpad(ch);
        /*******************************************************
         * Disable all editing input from non-root user unless *
         * form is "non-root editable" (e.g. change password)  *
         *******************************************************/
        if (!(form_flags & FORM_NONROOT_EDITABLE) && ruid != 0 &&
            ch != 'Q' && ch != '' && ch != '' && ch != '' && ch != '' && ch != '' && ch != KEY_F1 && ch != KEY_F2 && ch != KEY_F3) {
            continue;
        }

        switch (ch) {
            /**************************
             * Handle Down movements *
             **************************/
        case KEY_ENTER:
        case KEY_RETURN:
        case KEY_DOWN:
        case 011:              /* tab */
        case '':              /* Accept all right away */

            if (fitem->formitem_type == FTYPE_INTEGER_IMMEDIATE || fitem->formitem_type == FTYPE_REGEXP_IMMEDIATE || fitem->formitem_type == FTYPE_NUMERIC_IMMEDIATE || fitem->formitem_type == FTYPE_NUMERIC_LOW_IMMEDIATE || fitem->formitem_type == FTYPE_EVALUATE_IMMEDIATE || fitem->formitem_type == FTYPE_ENUM_IMMEDIATE) {      /* Special handling */
                rc = form_driver(form, REQ_VALIDATION);
                if (rc != E_INVALID_FIELD) {
                    if (fitem->formitem_val != NULL) {  /* Check custom validation */
                        buf = field_buffer(curr_field, 0);
                        stripWhiteSpace(vBOTH, buf);
                        rc = (*(fitem->formitem_val)) (buf);
                    }
                    else
                        rc = RC_NORMAL;
                }
                else
                    rc = RC_ERROR;

                if (rc != RC_NORMAL) {
                    sprintf(tempbuf, "Invalid value for %s.",
                            fitem->formitem_name);
                    show_info(minfo, tempbuf);
                    info_changed = TRUE;
                    break;
                }

                if ((ch == KEY_RETURN) || (ch == KEY_ENTER)) {
                    done = TRUE;
                    retval = RC_EXCEPT_FIELD;
#ifdef EXPERIMENTAL
                    form_driver(form, REQ_NEXT_FIELD);
                    form_driver(form, REQ_PREV_FIELD);
#endif
                }
                else {

                    if (ch == '' || curr_field == last_field ||
                        (ch == KEY_DOWN && curr_field == last_row_field)) {
                        done = TRUE;
                        retval = RC_NORMAL;
                    }
                    else if (ch == KEY_DOWN)
                        rc = form_driver(form, REQ_DOWN_FIELD); /* go down */
                    else
                        rc = form_driver(form, REQ_NEXT_FIELD); /* go to next field */
                    if (info_changed == TRUE) {
                        show_info(minfo, default_mesg);
                        info_changed = FALSE;
                    }
                }
                break;
            }

            rc = form_driver(form, REQ_VALIDATION);
            if (rc != E_INVALID_FIELD) {
                if (fitem->formitem_val != NULL) {      /* Check custom validation */
                    buf = field_buffer(curr_field, 0);
                    stripWhiteSpace(vBOTH, buf);
                    rc = (*(fitem->formitem_val)) (buf);
                }
                else
                    rc = RC_NORMAL;
            }
            else
                rc = RC_ERROR;

            if (rc != RC_NORMAL) {
                sprintf(tempbuf, "Invalid value for %s.",
                        fitem->formitem_name);
                show_info(minfo, tempbuf);
                info_changed = TRUE;
            }
            else if (ch == '') {
                done = TRUE;
                retval = RC_NORMAL;
            }
            else if ((curr_field != last_field) ||
                     ((ch != KEY_ENTER) && (ch != KEY_RETURN) && (ch != 011))) {
                if (ch == KEY_DOWN)
                    rc = form_driver(form, REQ_DOWN_FIELD);     /* go down */
                else
                    rc = form_driver(form, REQ_NEXT_FIELD);     /* go to next field */
                set_field_buffer(curr_field, 1, field_buffer(curr_field, 0));   /* save buffer for CTRL-U */
                if (info_changed == TRUE) {
                    show_info(minfo, default_mesg);
                    info_changed = FALSE;
                }
            }
            else {
                done = TRUE;
                retval = RC_NORMAL;
            }
            break;

            /***********************
             * Handle Up movements *
             ***********************/
        case KEY_UP:
            rc = form_driver(form, REQ_VALIDATION);
            if (rc != E_INVALID_FIELD) {
                if (fitem->formitem_val != NULL) {      /* Check custom validation */
                    buf = field_buffer(curr_field, 0);
                    stripWhiteSpace(vBOTH, buf);
                    rc = (*(fitem->formitem_val)) (buf);
                }
                else
                    rc = RC_NORMAL;
            }
            else
                rc = RC_ERROR;

            if (rc != RC_NORMAL) {
                sprintf(tempbuf, "Invalid value for %s.",
                        fitem->formitem_name);
                show_info(minfo, tempbuf);
                info_changed = TRUE;
            }
            else if (curr_field != first_field) {
                rc = form_driver(form, REQ_UP_FIELD);   /* go up */
                set_field_buffer(curr_field, 1, field_buffer(curr_field, 0));   /* save buffer for CTRL-U */
                if (info_changed == TRUE) {
                    show_info(minfo, default_mesg);
                    info_changed = FALSE;
                }
            }
            break;

            /***********************************
             * Handle field editing commands.  *
             ***********************************/
        case KEY_BACKSPACE:
        case '':
        case 0x7f:
        case KEY_DC:
            form_driver(form, REQ_DEL_PREV);    /* Delete previous character */
            break;

        case '':
            form_driver(form, REQ_DEL_CHAR);    /* delete current char */
            break;

        case KEY_LEFT:
        case '':
            if (fitem->formitem_type == FTYPE_INTEGER_RANGE || fitem->formitem_type == FTYPE_INTEGER_IMMEDIATE) {       /* decrement field */
                buf = field_buffer(curr_field, 0);
                val = atoi(buf);
                if (val > fitem->formitem_max)
                    val = fitem->formitem_max;
                else if (val > fitem->formitem_min)
                    val--;

                sprintf(tempbuf, "%d", val);
                set_field_buffer(curr_field, 0, tempbuf);
            }
            else if (fitem->formitem_type == FTYPE_EVALUATE ||
                     fitem->formitem_type == FTYPE_EVALUATE_IMMEDIATE) {
                buf = field_buffer(curr_field, 0);
                sprintf(tempbuf, "%s",
                        ((evalfun *) fitem->formitem_max) (buf, 0));
                set_field_buffer(curr_field, 0, tempbuf);
            }
            else if (fitem->formitem_type == FTYPE_ENUM_IMMEDIATE ||
                     fitem->formitem_type == FTYPE_ENUM_RANGE) {
                /* Go to next value in list */
                char **sl;
                char **firstitem;
                buf = field_buffer(curr_field, 0);
                stripWhiteSpace(vBOTH, buf);
                sl = (char **) (fitem->formitem_min);
                firstitem = sl;
                while (*sl != NULL) {
                    if (strcasecmp(*sl, buf) == 0) {
                        break;
                    }
                    sl++;
                }
                if (*sl != NULL && sl != firstitem && *(sl - 1) != NULL) {
                    sprintf(tempbuf, "%s", *(sl - 1));
                }
                else {
                    /* rolled over -- go to the last item */
                    sl = (char **) (fitem->formitem_min);
                    while (*sl != NULL) {
                        sl++;
                    }
                    sl--;
                    sprintf(tempbuf, "%s", *sl);
                }

                set_field_buffer(curr_field, 0, tempbuf);
            }
            else
                rc = form_driver(form, REQ_LEFT_CHAR);  /* move left one char */
#ifdef EXPERIMENTAL
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_PREV_FIELD);
#endif
            break;
        case KEY_RIGHT:
        case '':
            if (fitem->formitem_type == FTYPE_INTEGER_RANGE ||
                fitem->formitem_type == FTYPE_INTEGER_IMMEDIATE) {
                /* increment field */
                buf = field_buffer(curr_field, 0);
                val = atoi(buf);
                if (val < fitem->formitem_min)
                    val = fitem->formitem_min;
                else if (val < fitem->formitem_max)
                    val++;

                sprintf(tempbuf, "%d", val);
                set_field_buffer(curr_field, 0, tempbuf);
            }
            else if (fitem->formitem_type == FTYPE_EVALUATE ||
                     fitem->formitem_type == FTYPE_EVALUATE_IMMEDIATE) {
                buf = field_buffer(curr_field, 0);
                sprintf(tempbuf, "%s",
                        ((evalfun *) fitem->formitem_max) (buf, 1));
                set_field_buffer(curr_field, 0, tempbuf);
            }
            else if (fitem->formitem_type == FTYPE_ENUM_IMMEDIATE ||
                     fitem->formitem_type == FTYPE_ENUM_RANGE) {
                /* Go to next value in list */
                char **sl;
                buf = field_buffer(curr_field, 0);
                stripWhiteSpace(vBOTH, buf);
                sl = (char **) (fitem->formitem_min);
                while (*sl != NULL) {
                    if (strcasecmp(*sl, buf) == 0) {
                        if (*(sl + 1) != NULL
                            && strcasecmp(*(sl + 1), buf) == 0) {
                            /* The next entry is identical to this one
                               possibly differing only in case (e.g. "yes", Yes") */
                        }
                        else {
                            break;
                        }
                    }
                    sl++;
                }
                if (*sl != NULL && *(sl + 1) != NULL)
                    sprintf(tempbuf, "%s", *(sl + 1));
                else
                    sprintf(tempbuf, "%s",
                            *(char **) (fitem->formitem_min));

                set_field_buffer(curr_field, 0, tempbuf);

            }
            else
                rc = form_driver(form, REQ_RIGHT_CHAR); /* move right one char */
#ifdef EXPERIMENTAL
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_PREV_FIELD);
#endif
            break;

        case '':
            rc = form_driver(form, REQ_BEG_FIELD);      /* Go to beginning of field */
            break;

        case '':
            rc = form_driver(form, REQ_END_FIELD);      /* Go to end of field */
            break;

        case '':
            rc = form_driver(form, REQ_CLR_EOL);        /* Clear to end of line */
            break;

            /*****************************************************
             * CTRL-U (undo) throws away current contents of the *
             * buffer and reverts to previous value.             *
             *****************************************************/
        case '':
            set_field_buffer(curr_field, 0, field_buffer(curr_field, 1));
            break;

            /***************************************************
             * CTRL-Q uncoditionally quits the forms processor *
             ***************************************************/
        case '':
        case '':
        case KEY_F1:
            done = TRUE;
            retval = RC_QUIT;
            break;

            /*************************************************
             * CTRL-X attempts to quit the whole menu system *
             *************************************************/
        case '':
            done = TRUE;
            retval = RC_EXIT;
            break;

            /****************************
             * CTRL-R resets all values *
             ****************************/
        case '':
            refresh_form_fields(form);
            break;

            /**************************************
             * CTRL-L refreshes the entire screen *
             **************************************/
        case '':
            refresh_screen();
            break;

            /************************
             * Context Sensitive Help *
             ************************/
        case KEY_F2:
            werase(form_wind);
            unpost_form(form);
            show_info(minfo, " ");
            strcpy(tempbuf, "Help for \"");
            strcat(tempbuf, fitem->formitem_name);
            strcat(tempbuf, "\":\n\n\n");
            strcat(tempbuf, fitem->formitem_help);
            strcat(tempbuf, "\n\n\nPress any key to continue...");
            dialog = show_dialog(minfo, 0, 0, tempbuf);
            ch = wgetch(form_wind);
            remove_dialog(dialog);
            show_title(minfo, NULL);
            post_form(form);
            show_info(minfo, default_mesg);
            info_changed = FALSE;
            break;

            /************************
             * Show Navigation help *
             ************************/
        case KEY_F3:
            werase(form_wind);
            unpost_form(form);
            show_info(minfo, " ");
            dialog = show_dialog(minfo, 0, 0, form_help);
            ch = wgetch(form_wind);
            if (ch == KEY_F2) {
                remove_dialog(dialog);
                dialog = show_dialog(minfo, 0, 0, form_help2);
                ch = wgetch(form_wind);
            }
            remove_dialog(dialog);
            show_title(minfo, NULL);
            post_form(form);
            show_info(minfo, default_mesg);
            info_changed = FALSE;
            break;

        default:
            /**************************************************
             * If this is a normal character, it gets printed *
             **************************************************/
/*          if (fitem->formitem_type == FTYPE_INTEGER_RANGE) { */
/*              if (ch == 'q' || ch == 'Q') { */
/*                  done = TRUE; */
/*                  retval = RC_QUIT; */
/*                  break; */
/*              } */
/*          } */
            rc = form_driver(form, ch);
            if (rc == E_UNKNOWN_COMMAND) {
                if (ch == 'q' || ch == 'Q') {
                    done = TRUE;
                    retval = RC_QUIT;
                    break;
                }
                sprintf(tempbuf, "Invalid input character for %s.",
                        fitem->formitem_name);
                show_info(minfo, tempbuf);
                info_changed = TRUE;
                break;
            }
            else if (fitem->formitem_type == FTYPE_REGEXP ||
                     fitem->formitem_type == FTYPE_REGEXP_IMMEDIATE ||
                     fitem->formitem_type == FTYPE_ENUM_IMMEDIATE ||
                     fitem->formitem_type == FTYPE_EVALUATE_IMMEDIATE) {
                rc = form_driver(form, REQ_VALIDATION);
                if (rc == E_INVALID_FIELD) {
                    if (ch == 'q' || ch == 'Q') {
                        done = TRUE;
                        retval = RC_QUIT;
                        break;
                    }
                    sprintf(tempbuf, "Invalid input character for %s.",
                            fitem->formitem_name);
                    show_info(minfo, tempbuf);
                    info_changed = TRUE;
                    form_driver(form, REQ_DEL_PREV);
                    break;
                }
            }

            if (info_changed == TRUE) {
                show_info(minfo, default_mesg);
                info_changed = FALSE;
            }
            break;
        }
    }

  QUIT_LABEL:
    return retval;
}

/***********************************************************
 * Returns true if any field in the given form has changed *
 ***********************************************************/
int get_form_status(FORM * form)
{
    FIELD **fields;
    int fieldcount;
    FORMITEMDEF *fitem;
    int i;
    int retval = FALSE;

    fields = form_fields(form);
    fieldcount = field_count(form);
    for (i = 0; i < fieldcount; i++) {
        fitem = field_userptr(fields[i]);
        if ((fitem != NULL) && (field_status(fields[i]) == TRUE)) {
            retval = TRUE;
        }
    }

    return retval;
}

/*************************************************************
 * Retrieve the current text buffer for the given form field *
 *************************************************************/
char *get_field_buffer(FORM * form, int identifier, char *buf)
{
    FIELD **fields;
    int fieldcount;
    FORMITEMDEF *fitem;
    int i;

    /****************************************************************
     * Extract field using identifier. Copy the buffer in the field *
     * to the given buffer and trim trailing/leading white space    *
     * before returning.                                            *
     ****************************************************************/
    buf[0] = '\0';
    fields = form_fields(form);
    fieldcount = field_count(form);
    for (i = 0; i < fieldcount; i++) {
        fitem = field_userptr(fields[i]);
        if ((fitem != NULL) && (fitem->formitem_ident == identifier)) {
            strcpy(buf, field_buffer(fields[i], 0));
            stripWhiteSpace(vBOTH, buf);
        }
    }
    return buf;
}

/**********************************
 * Set the windows for given form *
 **********************************/
int set_form_windows(MENUINFO * minfo, FORM * form)
{
    WINDOW *menu_wind = NULL;
    WINDOW *submenu_wind = NULL;
    int menuwidth, menuheight;
    int rc = 0;

    menu_wind = minfo->minfo_menuscreen->window;

    /********************
     * Set form windows *
     ********************/
    getmaxyx(menu_wind, menuheight, menuwidth);
    submenu_wind = derwin(menu_wind,
                          menuheight - TITLE_HEIGHT,
                          menuwidth - SUBWIN_PAD * 2,
                          TITLE_HEIGHT, SUBWIN_PAD);
    if (submenu_wind == NULL)
        CURSES_ERR("Could not create submenu_wind\n");

    if ((rc = set_form_win(form, menu_wind)) != E_OK)
        CURSES_ERR("Could not set menu window %d \n", rc);

    if ((rc = set_form_sub(form, submenu_wind)) != E_OK)
        CURSES_ERR("Could not set sub-menu window %d \n", rc);

  QUIT_LABEL:
    return rc;

}

/***************************************
 * Delete the subwindow for given form *
 ***************************************/
void unset_form_windows(FORM * form)
{
    WINDOW *submenu_wind = NULL;

    submenu_wind = form_sub(form);
    if (submenu_wind != NULL)
        delwin(submenu_wind);
}

/*********************************
 * General purpose dialog widget *
 *********************************/
DIALOG *show_dialog(MENUINFO * minfo, int starty, int startx,
                    char *dialogstring)
{

    DIALOG *dialog;
    int s_count;

    if (dialogstring == NULL)
        return (DIALOG *) NULL;

    dialog = (DIALOG *) calloc(1, sizeof(DIALOG));

    if (dialog == NULL)
        return dialog;

    s_count = splitString(dialogstring, dialog->message, '\n');

    dialog->label =
        newCDKLabel(minfo->minfo_menuscreen, startx, starty,
                    dialog->message, s_count, FALSE, FALSE);

    wnoutrefresh(minfo->minfo_menuscreen->window);
    drawCDKLabel(dialog->label, FALSE);

    return dialog;
}

int print_dialog(MENUINFO * minfo, DIALOG * dialog, char *dialogstring)
{

    int s_count;

    if (dialogstring == NULL)
        return -1;

    s_count = splitString(dialogstring, dialog->message, '\n');

    setCDKLabelMessage(dialog->label, dialog->message, s_count);

    return 0;
}

/*******************************
 * Delete resources for dialog *
 *******************************/
void remove_dialog(DIALOG * dialog)
{
    int i;
    if (dialog != NULL) {
        if (dialog->label != NULL)
            destroyCDKLabel(dialog->label);
        for (i = 0; i < _DIALOG_LINES_; i++)
            if (dialog->message[i] != NULL)
                free(dialog->message[i]);
        free(dialog);
    }
}

/***********************************
 * Show message on the info screen *
 ***********************************/
void show_info(MENUINFO * minfo, char *infostring)
{
    static char *mesg[3] = { NULL, NULL, NULL };
    static char *oldmesg[3] = { NULL, NULL, NULL };
    int i, s_count;

    for (i = 0; i < 3; i++) {
        if (mesg[i] != NULL) {
            free(mesg[i]);
            mesg[i] = NULL;
        }
    }
    /* Split the string */
    s_count = splitString(infostring, mesg, '\n');
    for (i = s_count; i < 3; i++) {
        mesg[i] = strdup("");
    }
    /********************************************************
     * Optimization, if previous message is the same as the *
     * current one, dont update.                            *
     ********************************************************/
    for (i = 0; i < 3; i++) {
        if ((oldmesg[i] == NULL) || (strcmp(mesg[i], oldmesg[i]) != 0)) {
            setCDKLabelMessage(minfo->minfo_info_label, mesg, 3);
            break;
        }
    }
    if (i == 3) {               /* old == new */
        if (s_count == 0) {
            setCDKLabelMessage(minfo->minfo_info_label, mesg, 3);
            for (i = 0; i < 3; i++) {
                if (oldmesg[i] != NULL) {
                    free(oldmesg[i]);
                    oldmesg[i] = NULL;
                }
            }
        }
    }
    else {
        for (i = 0; i < 3; i++) {
            if (oldmesg[i] != NULL)
                free(oldmesg[i]);
            oldmesg[i] = mesg[i];
            mesg[i] = NULL;
        }
    }
}

int get_title(MENUINFO *minfo, char *titlebuffer) {
    HAL *hp = minfo->minfo_userptr;

    if (hp != NULL) {
        int hardware_type = hal_get_hardware_type(hp);
        int board_type = hal_get_board_type(hp);
        int board_number = hal_get_board_number(hp);
        if (hardware_type == PLX_DEVICE) {
            sprintf(titlebuffer, "%s: Channel %d [%s]",
                    minfo->minfo_title, board_number + 1,
                    hal_utils_port_name(board_type));
        }
        else if (hardware_type == V2DI50_DEVICE ||
		hardware_type == XPI_DEVICE) {
            /* No need of channel number here */
            sprintf(titlebuffer, "%s: [%s]", minfo->minfo_title,
                    hal_utils_port_name(board_type));
        }
    }
    else {
        sprintf(titlebuffer, "%s", minfo->minfo_title);
    }
    return 0;
}

/*****************************************
 * Print title on the main window screen *
 *****************************************/
int show_title(MENUINFO * minfo, char *titlestring)
{
    static char *mesg[3] = { NULL, NULL, NULL };
    int i, s_count;
    char buf[128];

    for (i = 0; i < 3; i++) {
        if (mesg[i] != NULL)
            free(mesg[i]);
    }
    if (titlestring == NULL) {
        s_count = 1;
        mesg[0] = strdup(minfo->minfo_title);
    }
    else {
        s_count = splitString(titlestring, mesg, '\n');
    }
    for (i = s_count; i < 3; i++) {
        mesg[i] = strdup("");
    }

    /* If HAL pointer is set in minfo, show the port info, along with
     * the original title */
    if (minfo->minfo_userptr != NULL &&
        titlestring == NULL && mesg[0] != NULL) {
        char hostname[255];
        char newbuff[255];
        get_title(minfo, buf);
        if (gethostname(hostname, 255) == 0) {
            int titlelen = strlen(buf);
            int hostnamelen = strlen(hostname);
            if ((titlelen + hostnamelen) < 48) {
                snprintf(newbuff, 54, "%s %*s",
                         buf, (50 - titlelen), hostname);
            }
            else {
                snprintf(newbuff, 54, "%s %*s",
                         buf, (50 - titlelen), "");
            }
        }
        else {
            snprintf(newbuff, 54, "%s", buf);
        }
        if (is_standalone_enabled()) {
            snprintf (newbuff + 51, 5, " [S]");
        } else {
            snprintf (newbuff + 51, 5, " [M]");
        }
        free(mesg[0]);
        mesg[0] = strdup(newbuff);
    }

    /* For other menu items (HAL pointer is not set) show the hardware
     * description string */
    if (minfo->minfo_userptr == NULL &&
        titlestring == NULL &&
        mesg[0] != NULL) {
        char hostname[255];
        char hardware_desc[128];
        HAL *hp = hal_acquire(0); // OK to be null

        hal_get_hardware_description(hp, hardware_desc, 128);

        // Show hostname in the title (right justified)
        if (gethostname(hostname, 255) == 0) {
            int titlelen = strlen(mesg[0]) + strlen(hardware_desc);
            int hostnamelen = strlen(hostname);
            if ((titlelen + hostnamelen) < 48) {
                snprintf(buf, 52, "%s: %s %*s",
                         mesg[0], hardware_desc,
                         (48 - titlelen),
                         hostname);
            }
            else {
                snprintf(buf, 52, "%s: %s", mesg[0], hardware_desc);
            }
        }
        else {
            snprintf(buf, 52, "%s: %s", mesg[0], hardware_desc);
        }
        if (is_standalone_enabled()) {
            snprintf (buf+51,5," [S]");
        } else {
            snprintf (buf+51,5," [M]");
        }

        free(mesg[0]);
        mesg[0] = strdup(buf);

        if (hp != NULL)
            hal_release(hp);

    }
    wnoutrefresh(minfo->minfo_menuscreen->window);
    setCDKLabelMessage(minfo->minfo_title_label, mesg, 3);
    return 0;
}

/*******************************************
 * Loop till you get a yes no confirmation *
 *******************************************/
int get_yesno(MENUINFO * minfo)
{
    int ch;

    while (1) {
        //ch = waitCDKLabel(minfo->minfo_info_label, (char) NULL);
        ch = waitCDKLabel(minfo->minfo_info_label, (char) 0);
        if (ch == '') {
            refresh_screen();
        }
        else if (ch == 'Y' || ch == 'y') {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
}

/*******************************************
 * Loop till you get a p(rev, n(ext) or q(uit) *
 *******************************************/
int get_prevnextquit(MENUINFO * minfo)
{
    int ch;

    while (1) {
        //ch = waitCDKLabel(minfo->minfo_info_label, (char) NULL);
        ch = waitCDKLabel(minfo->minfo_info_label, (char) 0);
        if (ch == '') {
            refresh_screen();
        }
        else if (ch == 'P' || ch == 'p') {
            return 'p';
        }
        else if (ch == 'N' || ch == 'n') {
            return 'n';
        }
        else { /* if (ch == '^Q' || ch == KEY_F1 || ch == 'q' || ch == 'Q') */
            return 0;
        }
    }
}

/************************
 * Wait for a key press *
 ************************/
int get_anykey(MENUINFO * minfo)
{
    int ch;
    while (1) {
        //ch = waitCDKLabel(minfo->minfo_info_label, (char) NULL);
        ch = waitCDKLabel(minfo->minfo_info_label, (char) 0);
        if (ch == '')
            refresh_screen();
        else
            return ch;
    }
}

static void custmenu_signal(int sig)
{
    switch (sig) {
    case SIGALRM:
    case SIGHUP:
    case SIGUSR1:
    case SIGUSR2:
    case SIGQUIT:
    case SIGILL:
    case SIGABRT:
    case SIGBUS:
    case SIGFPE:
    case SIGSEGV:
    case SIGPIPE:
    case SIGTERM:
    case SIGINT:
        VLOG_CRITICAL("Ending Program on %s signal.", strsignal(sig));
        tcsetattr(0, TCSANOW, &save_termios);


        if (mwg != NULL && iwg != NULL) {
            werase(mwg);
            werase(iwg);
            wrefresh(mwg);
            wrefresh(iwg);

            delwin(mwg);
            delwin(iwg);
        }

        endwin();

        exit(0);
        break;
    }
}

/*************************************************************
 * Set flag setting for a field in a formitemdef array       *
 *************************************************************/
void set_formitem_flags(FORMITEMDEF fitemdefs[],
                        int size, int identifier, int flag)
{
    FORMITEMDEF *fitem;
    int i;

    for (i = 0; i < size; i++) {
        fitem = &fitemdefs[i];
        if ((fitem != NULL) && (fitem->formitem_ident == identifier)) {
            fitem->formitem_flags = flag;
        }
    }
}

/*************************************************************
 * Set option setting for a field in a formitemdef array       *
 *************************************************************/
void set_formitem_options(FORM * form,
                        int identifier, bool flag)
{
    FORMITEMDEF *fitem;
    FIELD  **fields;

    fields = form_fields(form);
    int fieldcount, i;

    fieldcount = field_count(form);
    for (i = 0; i < fieldcount; i++) {
        fitem = field_userptr(fields[i]);
        if ((fitem != NULL) && (fitem->formitem_ident == identifier)) {
            if(flag)
                fitem->formitem_flags = fitem->formitem_flags | FFLAG_DISABLE;
            else
                fitem->formitem_flags = fitem->formitem_flags & ~FFLAG_DISABLE;
        }
    }
}


int show_fault_dialog(MENUINFO * minfo, char *message)
{
    DIALOG *dialog;
    dialog = show_dialog(minfo, 7, 0, message);
    show_info(minfo, PRESSANYKEY_STR);
    get_anykey(minfo);
    remove_dialog(dialog);
    return RC_NORMAL;
}
