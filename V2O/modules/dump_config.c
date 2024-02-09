
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "hal.h"
#include "vutils.h"
#include "configureServerlists.h"
#include "configureSystem.h"
#include "configureVideo.h"
//#include "configureEdid.h"
//#include "configureColor.h"
#include "generatedHeaders.h"
#include "routeList.h"

int dump_config_main(int argc, char **argv) 
{

    if (argc > 1 && (strcmp(argv[1], "--network") == 0)) {
        dumpDNSClient();
        dumpIFList();
        dump_route_table();
        return 0;
    }

    dumpIFList();
    dumpDNSClient();
    dumpTelnetServer();
    dump_route_table();
    dumpTxVideoParameters();
    dumpXpTxVideoParameters();
    dumpTxIOParameters();
    dumpRxIOParameters();
    dumpRxInputVideoParameters();
    dumpLUTList();
    dumpXMPPServer();
    dumpXMPPAgent();
    dumpDHCPServer();
    dumpNTPClient();
    dumpNTPServer();
    dumpAdminAccount();
    dumpBoardInfo();
    dumpDisplayParameters();
    dumpTxRFBParameters();
    //dump_colorgain_offset_parm();
    //dump_edid_parm();
    dumpTxControlPorts();
    dumpRxAVPorts();
    dumpMPEGTSRxAVParameters();
    dumpUDPRxAVParameters();
    dumpRTPRxAVParameters();
    dumpMulticastParameters();
    dumpRxVideoParameters();
    dumpStandAlone();
    dumpTxSplashParameters();
    dumpRxSplashParameters();
    dumpNASServerList();
    dumpDatabaseManagement();
    dumpHostList();
    dumpNATZoneList();
    dumpXPAudioMixerParameters();
    dumpHA();
    dumpBanner();
    dumpMaxMediaClients();

    return 0;
}
