#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "vutils.h"
#include "commonutils.h"
#include "text_utils.h"
#include "device_detection.h"
#include "clientutils.h"
#include "menuutils.h"
#include "standalone.h"
#include "stand_alone_mode_menu_gen.h"
#include "tx_control_ports_menu_gen.h"
#include "multicast_params_menu_gen.h"
#include "tx_rfb_params_menu_gen.h"
#include "tx_connection_params_menu_gen.h"
#include "rx_av_ports_menu_gen.h"
#include "reverse_audio_menu_gen.h"
#include "protocol_version_menu_gen.h"
#include "strmsrv_comm.h"
#include "tx_thumbnail_params_menu_gen.h"


#define VLOG_CATEGORY_NAME "standalone"
#include "vlog.h"

#define IPADDRESS_LEN 32
#define PASSWORD_LEN 32

#define TX_NETWORK_PORT_ID  1000
#define RX_NETWORK_PORT_ID  1002
#define ENCODER_PORT_ID  1001
#define DECODER_PORT_ID  1003

static inline int get_encoder_port_id(int board_number)
{
    return ENCODER_PORT_ID + board_number * 1000;
}

static inline int get_encoder_reverse_audio_port_id(int board_number)
{
    return get_encoder_port_id(board_number) + 10;
}

static inline int get_decoder_port_id(int board_number)
{
    return DECODER_PORT_ID + board_number * 1000;
}

static inline int get_decoder_reverse_audio_port_id(int board_number)
{
    return get_decoder_port_id(board_number) + 10;
}

static inline int get_rx_network_port_id(int board_number)
{
    return RX_NETWORK_PORT_ID + board_number * 1000;
}

static inline int get_rx_network_reverse_audio_port_id(int board_number)
{
    return get_rx_network_port_id(board_number) + 10;
}

static inline int get_tx_network_port_id(int board_number)
{
    return TX_NETWORK_PORT_ID + board_number * 1000;
}

static inline int get_tx_network_reverse_audio_port_id(int board_number)
{
    return get_tx_network_port_id(board_number) + 10;
}

static int get_server_port(int board_number)
{
    struct tx_control_ports_parm_params params;
    retrieveTxControlPortsValues(&params);
    return params.port_from_value + (board_number * 2);
}

static int get_reverse_audio_port(int board_number)
{
    return get_server_port(board_number) + 1;
}

static int is_reverse_audio_enabled(int board_number) {
    HAL * hp;
    struct reverse_audio_params reverse_audio_params;
    hp = hal_acquire(board_number);
    if (hp == NULL)
        return 0;
    reverse_audio_params.userptr = hp;
    retrieveReverseAudioParametersValues(&reverse_audio_params);
    hal_release(hp);
    if (reverse_audio_params.enable_value == 1)
        return 1;
    return 0;
}

static char * get_protocol_version(int board_number) {
    HAL * hp;
    struct protocol_version_params protocol_version_params;
    hp = hal_acquire(board_number);
    if (hp == NULL)
        return 0;
    protocol_version_params.userptr = hp;
    retrieveProtocolVersionParametersValues(&protocol_version_params);
    hal_release(hp);
    switch (protocol_version_params.v2d_protocol_version_value) {
    case PROTOCOL_VERSION_V2D_PROTOCOL_VERSION_DEFAULT:
        return NULL;
    case PROTOCOL_VERSION_V2D_PROTOCOL_VERSION_1_DOT_1:
        return "1.1";
    case PROTOCOL_VERSION_V2D_PROTOCOL_VERSION_1_DOT_0:
        return "1.0";
    }
    return 0;
}

static void get_multicast_params(int board_number,
                                 char * multicast_ip,
                                 int * multicast_ttl)
{
    struct multicast_parm_params params;
    HAL *hp = hal_acquire(board_number);

    params.userptr = hp;
    retrieveMulticastParametersValues(&params);
    strcpy(multicast_ip, params.multicast_from_value);
    increment_ip(multicast_ip, board_number);
    *multicast_ttl = params.multicast_ttl_value;
    if (hp)
        hal_release(hp);
}


static void apply_connection_params(HAL *hp)
{
    struct tx_connection_parm_params params, orig_params;

    params.userptr = hp;
    retrieveConfiguredTxConnectionParametersValues(&params);
    memcpy(&orig_params, &params, sizeof(params));

    unmaskTxConnectionParametersValues(&params);
    params.framerate_divider_set = 1;
    params.min_refresh_set = 1;
    params.max_refresh_set = 1;
    params.min_slice_set = 1;
    params.max_slice_set = 1;
    params.block_threshold_set = 1;
    params.color_sampling_set = 1;

    applyTxConnectionParameters(&params, &orig_params);
}

static void get_av_ports(int board_number, int *videoport, int *audioport)
{
    struct rx_av_ports_parm_params params;
    retrieveRxAVPortsValues(&params);
    *videoport = params.port_from_value + (board_number * 2);
    *audioport = *videoport + 1;
}

static int execute_strmsvr_command(char *command) {
    if (strmsvr_execute_command(command) == 0) {
        VLOG_WARNING("StandAlone: Failed to issue command %s", command);
        return 0;
    }
    return 1;
}

static void resetStreamingServer(void)
{
    char command[256];
    // Create the reset command
    sprintf(command, "action=reset");
    // Execute the reset command
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_tx_tn(int board_number)
{
    /* Thumbnails not yet available in standalone mode */
    return;


    char command[256];

    int encoder_port_id = get_encoder_port_id(board_number);
    int queue_id = get_encoder_port_id(board_number);

    //Create ImageWriterPort for live thumbnails.
    sprintf(command, "action=add id=%d ioport=%d dsttype=ImageWriterPort dstdir=\"/data/DSS/data/portal/site/thumbnails/%d/\" streamtype=V2D",
            encoder_port_id + 1, board_number + 1, board_number + 1);
    if (!execute_strmsvr_command(command))
        return;

    //Start ImageWriterPort
    sprintf(command, "action=start id=%d dsttype=ImageWriterPort queueid=%d ioport=%d", encoder_port_id + 1, queue_id, board_number + 1);
    if (!execute_strmsvr_command(command))
        return;

}

static void setup_tx_fpga(int board_number)
{
    HAL *hp;
    char command[256];
    int encoder_port_id;
    int queue_id;

    hp = hal_acquire(board_number);
    if (hp == NULL) {
        VLOG_WARNING("StandAlone: Could not acquire HAL for board %d.", board_number);
        return;
    }

    // Collect all required parameters
    encoder_port_id = get_encoder_port_id(board_number);
    queue_id = get_encoder_port_id(board_number);


    // Apply connection params that are not handled by StreamingServer
    apply_connection_params(hp);

    hal_release(hp);

    // Create
    sprintf(command, "action=add id=%d operation=input srctype=V2DEncoderPort ioport=%d",
            encoder_port_id, board_number + 1);
    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=input srctype=V2DEncoderPort",
            encoder_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_tx_fpga_reverse_audio(int board_number)
{
    HAL *hp;
    char command[256];
    int encoder_reverse_audio_port_id;
    int queue_id, audioport;
    struct client_conn_config cc_cfg;

    hp = hal_acquire(board_number);
    if (hp == NULL) {
        VLOG_WARNING("StandAlone: Could not acquire HAL for board %d.", board_number);
        return;
    }

    // Collect all required parameters
    encoder_reverse_audio_port_id = get_encoder_reverse_audio_port_id(board_number);
    queue_id = get_encoder_reverse_audio_port_id(board_number);
    audioport = get_reverse_audio_port(board_number);
    hal_release(hp);

    // Create
    sprintf(command, "action=add id=%d operation=output dsttype=V2DAudioDecoderPort ioport=%d audioport=%d",
            encoder_reverse_audio_port_id, board_number + 1, audioport);
    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=output dsttype=V2DAudioDecoderPort",
            encoder_reverse_audio_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_tx_network(int board_number)
{
    int server_port;
    char multicast_ip[IPADDRESS_LEN];
    int multicast_ttl = 1;
    char command[256];
    int tx_network_port_id;
    int queue_id;
    char * protocol_version;

    // Collect all required parameters
    tx_network_port_id = get_tx_network_port_id(board_number);
    queue_id = get_encoder_port_id(board_number);

    server_port = get_server_port(board_number);
    get_multicast_params(board_number, multicast_ip, &multicast_ttl);
    protocol_version = get_protocol_version(board_number);

    // Create
    sprintf(command, "action=add id=%d operation=output dsttype=V2DDecoderPort dstport=%d mcastaddr=%s mcastttl=%d  v2dtx=1 listener=1 policycheck=1 ",
            tx_network_port_id, server_port,
            multicast_ip, multicast_ttl);
    if (protocol_version != NULL)
        sprintf(command + strlen(command), "protocolversion=%s ", protocol_version);

    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=output dsttype=V2DDecoderPort",
            tx_network_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_tx_network_reverse_audio(int board_number)
{
    int reverse_audio_port;
    char command[256];
    int tx_network_reverse_audio_port_id;
    int queue_id;


    // Collect all required parameters
    tx_network_reverse_audio_port_id = get_tx_network_reverse_audio_port_id(board_number);
    queue_id = get_encoder_reverse_audio_port_id(board_number);

    reverse_audio_port = get_reverse_audio_port(board_number);

    // Create
    sprintf(command, "action=add id=%d operation=input srctype=RTPNetworkPort srcip=0.0.0.0 videoport=0 audioport=%d",
            tx_network_reverse_audio_port_id, reverse_audio_port);
    if (!execute_strmsvr_command(command))
        return;


    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=input srctype=RTPNetworkPort",
            tx_network_reverse_audio_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;

}


static void setup_tx(int board_number)
{
    setup_tx_fpga(board_number);
    setup_tx_tn(board_number);
    setup_tx_network(board_number);
    if (is_reverse_audio_enabled(board_number)) {
        setup_tx_fpga_reverse_audio(board_number);
        setup_tx_network_reverse_audio(board_number);
    }
}

static void teardown_tx_tn(int board_number)
{
    /* Thumbnails not yet available in standalone mode */
    return;

    if (get_hardware_category() == HARDWARE_CATEGORY_I50)
        return;

    char command[256];

    int encoder_port_id = get_encoder_port_id(board_number);

    sprintf(command, "action=stop id=%d operation=output dsttype=ImageWriterPort", encoder_port_id + 1);
    execute_strmsvr_command(command);

    sprintf(command, "action=delete id=%d operation=output dsttype=ImageWriterPort", encoder_port_id + 1);
    execute_strmsvr_command(command);
}

static void teardown_tx_fpga(int board_number)
{
    int encoder_port_id;
    char command[256];

    encoder_port_id = get_encoder_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=input srctype=V2DEncoderPort",
            encoder_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=input srctype=V2DEncoderPort",
            encoder_port_id);
    execute_strmsvr_command(command);
}

static void teardown_tx_fpga_reverse_audio(int board_number)
{
    int encoder_reverse_audio_port_id;
    char command[256];

    encoder_reverse_audio_port_id = get_encoder_reverse_audio_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=output dsttype=V2DAudioDecoderPort",
            encoder_reverse_audio_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=output dsttype=V2DAudioDecoderPort",
            encoder_reverse_audio_port_id);
    execute_strmsvr_command(command);
}

static void teardown_tx_network(int board_number)
{
    int tx_network_port_id;
    char command[256];

    tx_network_port_id = get_tx_network_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=output dsttype=V2DDecoderPort",
            tx_network_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=output dsttype=V2DDecoderPort",
            tx_network_port_id);
    execute_strmsvr_command(command);
}

static void teardown_tx_network_reverse_audio(int board_number)
{
    int tx_network_reverse_audio_port_id;
    char command[256];

    tx_network_reverse_audio_port_id = get_tx_network_reverse_audio_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=input srctype=RTPNetworkPort",
            tx_network_reverse_audio_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=input srctype=RTPNetworkPort",
            tx_network_reverse_audio_port_id);
    execute_strmsvr_command(command);
}

static void teardown_tx(int board_number)
{
    teardown_tx_tn(board_number);
    teardown_tx_network(board_number);
    teardown_tx_network_reverse_audio(board_number);
    teardown_tx_fpga(board_number);
    teardown_tx_fpga_reverse_audio(board_number);
}

static void setup_rx_fpga(int board_number)
{
    char command[256];
    int decoder_port_id;
    int queue_id;

    decoder_port_id = get_decoder_port_id(board_number);
    queue_id = get_rx_network_port_id(board_number);

    // Create
    sprintf(command, "action=add id=%d operation=output dsttype=V2DHardWarePort ioport=%d enablekbm=1",
            decoder_port_id, board_number + 1);
    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=output dsttype=V2DHardWarePort ",
            decoder_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_rx_fpga_reverse_audio(int board_number)
{
    char command[256];
    int decoder_reverse_audio_port_id;
    int queue_id;

    decoder_reverse_audio_port_id = get_decoder_reverse_audio_port_id(board_number);
    queue_id = get_rx_network_reverse_audio_port_id(board_number);

    // Create
    sprintf(command, "action=add id=%d operation=input srctype=V2DAudioEncoderPort ioport=%d",
            decoder_reverse_audio_port_id, board_number + 1);
    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=input srctype=V2DAudioEncoderPort ",
            decoder_reverse_audio_port_id, queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_rx_network(int board_number)
{
    char command[256];
    int rx_network_port_id;
    int queue_id;
    struct client_conn_config cc_cfg;
    int videoport, audioport;

    rx_network_port_id = get_rx_network_port_id(board_number);
    queue_id = get_rx_network_port_id(board_number);

    // Collect needed parameters
    get_client_conn_config(board_number, &cc_cfg);
    if (!cc_cfg.connect_enable) {
        return; // No need to make a connection
    }
    get_av_ports(board_number, &videoport, &audioport);

    // Create
    if (cc_cfg.rfb_connection == 0) {
        sprintf(command, "action=add id=%d operation=input srctype=V2DNetworkPort srcip=%s srcmulticast=%d srcport=%d srcbitrate=%lu avoption=%d videoport=%d audioport=%d",
                rx_network_port_id,
                cc_cfg.server_address,
                cc_cfg.multicast,
                cc_cfg.server_port,
                cc_cfg.av_max_bw * 8,
                cc_cfg.avoption,
                videoport,
                audioport);
    }
    else {
        sprintf(command, "action=add id=%d operation=input srctype=VNCNetworkPort rfbip=%s rfbport=%d rfbpasswd=%s",
                rx_network_port_id,
                cc_cfg.server_address,
                cc_cfg.rfb_port,
                cc_cfg.rfb_password);
    }
    if (!execute_strmsvr_command(command))
        return;

    // Start
    if (cc_cfg.rfb_connection == 0) {
        sprintf(command, "action=start id=%d queueid=%d operation=input srctype=V2DNetworkPort",
                rx_network_port_id,
                queue_id);
    }
    else {
        sprintf(command, "action=start id=%d queueid=%d operation=input srctype=VNCNetworkPort",
                rx_network_port_id,
                queue_id);
    }
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_rx_network_reverse_audio(int board_number)
{
    char command[256];
    int rx_network_reverse_audio_port_id;
    int queue_id;
    struct client_conn_config cc_cfg;
    //int audioport, dstport;
    int audioport;

    rx_network_reverse_audio_port_id = get_rx_network_reverse_audio_port_id(board_number);
    queue_id = get_rx_network_reverse_audio_port_id(board_number);

    // Collect needed parameters
    get_client_conn_config(board_number, &cc_cfg);
    if (!is_reverse_audio_enabled(board_number)) {
        return; // Reverse audio not configured for this board
    }
    if (!cc_cfg.connect_enable) {
        return; // No need to make a connection
    }
    if (cc_cfg.bidir == 0) {
        return; // Reverse audio not enabled for this connection entry
    }
    if (cc_cfg.multicast != 0) {
        return; // No reverse audio for multicast connections
    }
    if (cc_cfg.rfb_connection != 0) {
        return; // No reverse audio for VNC connections
    }

    // Server expects audio port to be one more than the server port
    audioport = cc_cfg.server_port + 1;

    // RTP sender only allows "dstport" configuration which is used for video port.
    // It always sends audio at videoport + 2
    //dstport = audioport - 2; // rtp sender always sends audio at videoport + 2

    // Create
    sprintf(command, "action=add id=%d operation=output dsttype=RTPDecoderPort dstaddr=%s dstport=%d audioonly=1",
                rx_network_reverse_audio_port_id,
                cc_cfg.server_address, audioport);
    if (!execute_strmsvr_command(command))
        return;

    // Start
    sprintf(command, "action=start id=%d queueid=%d operation=output dsttype=RTPDecoderPort",
            rx_network_reverse_audio_port_id,
            queue_id);
    if (!execute_strmsvr_command(command))
        return;
}

static void setup_rx(int board_number)
{
    setup_rx_fpga(board_number);
    setup_rx_network(board_number);
    if (is_reverse_audio_enabled(board_number)) {
        setup_rx_fpga_reverse_audio(board_number);
        setup_rx_network_reverse_audio(board_number);
    }
}

static void teardown_rx_fpga(int board_number)
{
    char command[256];
    int decoder_port_id;

    decoder_port_id = get_decoder_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=output dsttype=V2DHardWarePort ",
            decoder_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=output dsttype=V2DHardWarePort ",
            decoder_port_id);
    execute_strmsvr_command(command);
}

static void teardown_rx_fpga_reverse_audio(int board_number)
{
    char command[256];
    int decoder_reverse_audio_port_id;

    decoder_reverse_audio_port_id = get_decoder_reverse_audio_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=input srctype=V2DAudioEncoderPort ",
            decoder_reverse_audio_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=input srctype=V2DAudioEncoderPort ",
            decoder_reverse_audio_port_id);
    execute_strmsvr_command(command);
}

static void teardown_rx_network(int board_number)
{
    char command[256];
    int rx_network_port_id;

    rx_network_port_id = get_rx_network_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=input srctype=V2DNetworkPort",
            rx_network_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=input srctype=V2DNetworkPort",
            rx_network_port_id);
    execute_strmsvr_command(command);
}

static void teardown_rx_network_reverse_audio(int board_number)
{
    char command[256];
    int rx_network_reverse_audio_port_id;

    rx_network_reverse_audio_port_id = get_rx_network_reverse_audio_port_id(board_number);

    // Stop
    sprintf(command, "action=stop id=%d operation=output dsttype=RTPDecoderPort",
            rx_network_reverse_audio_port_id);
    execute_strmsvr_command(command);

    // Delete
    sprintf(command, "action=delete id=%d operation=output dsttype=RTPDecoderPort",
            rx_network_reverse_audio_port_id);
    execute_strmsvr_command(command);
}

static void teardown_rx(int board_number)
{
    teardown_rx_fpga(board_number);
    teardown_rx_network(board_number);
    teardown_rx_fpga_reverse_audio(board_number);
    teardown_rx_network_reverse_audio(board_number);
}

static void print_standalone_help(void) {
    printf("\t-r \tTeardown all streams\n");
    printf("\t-c \tChannel Number [1,2]\n");
    printf("\t-h \tPrint this message\n");
}


int standalone_main(int argc, char **argv)
{
    int opt;
    int do_reset = 0;
    int cno = -1;

    // Grab command line args
    while ((opt = getopt(argc, argv, "rc:")) > 0) {
        switch (opt) {
        case 'r':
            do_reset = 1;
            break;
        case 'c':
            cno = atoi(optarg);
            break;
        case 'h':
        case '?':
            print_standalone_help();
            return -1;
        default:
            break;
        }
    }

    VLOG_INFO("Starting standalone mode...");

    // Abort if not in standalone mode
    if (!is_standalone_enabled()) {
        VLOG_INFO("Not configured as stand alone mode. Aborting...\n");
        printf("Not configured as stand alone mode. Aborting...\n");
        return -1;
    }

    if (do_reset) {
        if (cno < 0) {
            standalone_stopall();
        }
        else {
            standalone_stop(cno);
        }
        return 0;
    }

   if (cno < 0) {
       standalone_startall();
   }
   else {
       standalone_start(cno);
   }

    return 0;
}

void standalone_startall()
{
    HAL *hp=NULL;
    int channelnumber;
    int nBoards;
    hp = hal_acquire(0);
    if (hp != NULL) {
        nBoards = hal_get_number_of_boards(hp);
        VLOG_DEBUG("Number of boards: %d", nBoards);
        hal_release(hp);
        resetStreamingServer();
        for (channelnumber = 1; channelnumber <= nBoards; channelnumber++) {
            standalone_start(channelnumber);
        }
    } else {
        VLOG_CRITICAL("Could not find number of boards");
    }
}

void standalone_start(int cno)
{
    int hardware_profile = get_hardware_profile();
    int hardware_category = get_hardware_category();

    // Reset StreamingServer before we do anything
    // resetStreamingServer();
    
    // Handle Tx
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX) {
        setup_tx(0);
    }

    // Handle Rx
    if (hardware_profile == HARDWARE_PROFILE_V2D_RX) {
        setup_rx(0);
    }

    // Handle XP and XPi
    if (hardware_category == HARDWARE_CATEGORY_XP ||
		hardware_category == HARDWARE_CATEGORY_XPI) {
        int board_number; 
        HAL *hp;
        board_number = cno - 1;
        hp = hal_acquire(board_number);
        if (hp != NULL) {
            int board_type = hal_get_board_type(hp);
            if (board_type == BOARDTYPE_TX) {
                setup_tx(board_number);
            } else if (board_type == BOARDTYPE_RX) {
                setup_rx(board_number);
            }
            hal_release(hp);
        }
    }
}

void standalone_stopall()
{
    HAL *hp=NULL;
    int i;
    int nBoards;
    hp = hal_acquire(0);
    if (hp != NULL) {
        nBoards = hal_get_number_of_boards(hp);
        hal_release(hp);
        for (i = 1; i <= nBoards; i++) {
            standalone_stop(i);
        }
    } else {
        VLOG_CRITICAL("Could not find number of boards");
    }
}

static void toggle_tx_reverse_audio(int board_number) {
    teardown_tx_fpga_reverse_audio(board_number);
    teardown_tx_network_reverse_audio(board_number);
    if (is_reverse_audio_enabled(board_number)) {
        setup_tx_fpga_reverse_audio(board_number);
        setup_tx_network_reverse_audio(board_number);
    }
}

static void toggle_rx_reverse_audio(int board_number) {
    teardown_rx_fpga_reverse_audio(board_number);
    teardown_rx_network_reverse_audio(board_number);
    if (is_reverse_audio_enabled(board_number)) {
        setup_rx_fpga_reverse_audio(board_number);
        setup_rx_network_reverse_audio(board_number);
    }
}

void standalone_toggle_reverse_audio(int channel_number)
{
    int hardware_profile = get_hardware_profile();

    // Abort if not in standalone mode
    if (!is_standalone_enabled())
        return;

    // Handle Tx
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX) {
        toggle_tx_reverse_audio(0);
    }

    // Handle Rx
    if (hardware_profile == HARDWARE_PROFILE_V2D_RX) {
        toggle_rx_reverse_audio(0);
    }

    // Handle XP and XPi
    if (get_hardware_category() == HARDWARE_CATEGORY_XP ||
            get_hardware_category() == HARDWARE_CATEGORY_XPI) {
        int board_number; 
        HAL *hp;
        board_number = channel_number - 1;
        hp = hal_acquire(board_number);
        if (hp != NULL) {
            int board_type = hal_get_board_type(hp);
            if (board_type == BOARDTYPE_TX) {
                toggle_tx_reverse_audio(board_number);
            } else if (board_type == BOARDTYPE_RX) {
                toggle_rx_reverse_audio(board_number);
            }
            hal_release(hp);
        }
    }
}


void standalone_stop(int cno)
{
    int hardware_profile = get_hardware_profile();
    int hardware_category = get_hardware_category();

    // Handle Tx
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX) {
        teardown_tx(0);
    }

    // Handle Rx
    if (hardware_profile == HARDWARE_PROFILE_V2D_RX) {
        teardown_rx(0);
    }

    // Handle XP
    if (hardware_category == HARDWARE_CATEGORY_XP) {
        teardown_rx(cno - 1);
        teardown_tx(cno - 1);
    }

    // Handle XPi
    if (hardware_category == HARDWARE_CATEGORY_XPI) {
        HAL *hp;
        hp = hal_acquire(cno - 1);
        if (hp != NULL) {
            int board_type = hal_get_board_type(hp);
            if (board_type == BOARDTYPE_TX) {
                teardown_tx(cno - 1);
            } 
            else if (board_type == BOARDTYPE_RX) {
                teardown_rx(cno - 1);
            }
            hal_release(hp);
        }
    } 
}

void standalone_client_reconnect(int board_number)
{
    int hardware_profile = get_hardware_profile();
    int hardware_category = get_hardware_category();

    // Abort if not in standalone mode
    if (!is_standalone_enabled())
        return;

    // Handle Rx on I50
    if (hardware_profile == HARDWARE_PROFILE_V2D_RX) {
        teardown_rx_network(0);
        teardown_rx_network_reverse_audio(0);
        setup_rx_network(0);
        setup_rx_network_reverse_audio(0);
    }

    // Handle Rx on XP200
    if (hardware_category == HARDWARE_CATEGORY_XP) {
        teardown_rx_network(board_number);
        teardown_rx_network_reverse_audio(board_number);
        setup_rx_network(board_number);
        setup_rx_network_reverse_audio(board_number);
    }

    // Handle Rx on XPi
    if (hardware_category == HARDWARE_CATEGORY_XPI) {
        teardown_rx_network(board_number);
        teardown_rx_network_reverse_audio(board_number);
        if (is_reverse_audio_enabled(board_number))
            standalone_rx_reverse_audio_stop(board_number);
        setup_rx_network(board_number);
        setup_rx_network_reverse_audio(board_number);
        if (is_reverse_audio_enabled(board_number))
            standalone_rx_reverse_audio_start(board_number);
    }
}

void standalone_restart_tx(int channel_number)
{
    int hardware_profile = get_hardware_profile();
    int hardware_category = get_hardware_category();
    // Abort if not in standalone mode
    if (!is_standalone_enabled())
        return;

    if ((hardware_category == HARDWARE_CATEGORY_I50 || 
        hardware_category == HARDWARE_CATEGORY_XPI) && (channel_number == 2))
            return;

    // Handle Tx on I50
    if (hardware_profile == HARDWARE_PROFILE_V2D_TX) {
        teardown_tx_network(0);
        setup_tx_network(0);
    }

    // Handle Tx on XP200 and XPi
    if (hardware_category == HARDWARE_CATEGORY_XP ||
            hardware_category == HARDWARE_CATEGORY_XPI) {
        int board_number; 
        HAL *hp;
        board_number = channel_number - 1;
        hp = hal_acquire(board_number);
        if (hp != NULL) {
            int board_type = hal_get_board_type(hp);
            if (board_type == BOARDTYPE_TX) {
                teardown_tx_network(board_number);
                setup_tx_network(board_number);
            }
            hal_release(hp);
        }
    }
}

void standalone_rx_reverse_audio_start(int board_number)
{
    int hardware_category = get_hardware_category();
    HAL *hp;
    struct client_conn_config cc_cfg;
    int audioport;
    char command[256];
    int decoder_reverse_audio_port_id;

    // Abort if not XPi 
    if (hardware_category != HARDWARE_CATEGORY_XPI)
        return;

    // Abort if reverse audio not enabled 
    if (!is_reverse_audio_enabled(board_number))
        return;

    // Abort if not Rx
    hp = hal_acquire(board_number);
    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX) 
        return;

    // Collect needed parameters
    get_client_conn_config(board_number, &cc_cfg);
    decoder_reverse_audio_port_id = 
        get_decoder_reverse_audio_port_id(board_number);
    hal_release(hp);

    if (!cc_cfg.connect_enable) 
        return; // No need to make a connection

    if (cc_cfg.bidir == 0) 
        return; // Reverse audio not enabled for this connection entry

    // Start transmit audio
    sprintf(command, "action=update id=%d srctype=V2DAudioEncoderPort srcport=%d dstip=%s dstport=%d xmit=1", 
            decoder_reverse_audio_port_id, cc_cfg.server_port,
            cc_cfg.server_address, cc_cfg.server_port + 1);
    if (!execute_strmsvr_command(command))
        return;
}

void standalone_rx_reverse_audio_stop(int board_number)
{
    int hardware_category = get_hardware_category();
    HAL *hp;
    char command[256];
    int decoder_reverse_audio_port_id;

    // Abort if not XPi 
    if (hardware_category != HARDWARE_CATEGORY_XPI)
        return;
    
    // Abort if reverse audio not enabled 
    if (!is_reverse_audio_enabled(board_number))
        return;

    // Abort if not Rx
    hp = hal_acquire(board_number);
    if (hp == NULL || hal_get_board_type(hp) != BOARDTYPE_RX) 
        return;

    // Collect all required parameters
    decoder_reverse_audio_port_id = 
        get_decoder_reverse_audio_port_id(board_number);

    hal_release(hp);

    // Stop transmit audio
    sprintf(command, "action=update id=%d srctype=V2DAudioEncoderPort xmit=0",
            decoder_reverse_audio_port_id);

    if (!execute_strmsvr_command(command))
        return;
}

