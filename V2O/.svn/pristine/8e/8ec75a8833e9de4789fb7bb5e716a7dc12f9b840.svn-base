#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "vlog.h"
#include "strmsrv_comm.h"
#include "strmsrvAccess.h"

#define ST_NOT_CONNECTED        0
#define ST_CONNECTED            1
#define ST_RECIEVING            2
#define ST_DONE                 3

#define READ_LEN        2048

#ifndef TRUE
#define TRUE      1
#endif

#ifndef FALSE
#define FALSE     0
#endif

//#define DPRINTF printf
#define DPRINTF(...) do {} while (0)

static struct strmsvr_result *ss_local_result = NULL;

static int get_fpga_local_info(int ioport, int isTx, int isRevAudio)
{
    char *xmlstr;
    if ((isTx == TRUE && isRevAudio == FALSE) || (isTx == FALSE && isRevAudio == TRUE))
        ss_local_result = strmsvr_command("action=xlist srctype=AnyNetworkPort");
    else
        ss_local_result = strmsvr_command("action=xlist dsttype=AnyDecoderPort");

    if (ss_local_result == NULL)
        return -1;

    if ((xmlstr = strmsvr_get_result_xml(ss_local_result)) == NULL)
        return -1;

    strmsrv_xml_create(xmlstr);

    // Select object based on ioport
    if (isRevAudio == FALSE)
        strmsrv_xml_select_object_by_port(ioport);
    else
        strmsrv_xml_select_rev_audio_object_by_port(ioport);

    return 0;
}

static void release_local_info(void)
{
    strmsrv_xml_destroy();
    if (ss_local_result != NULL)
        strmsvr_release_result(ss_local_result);
}

// This function is meant to be called after after calling
// get_fpga_local_info() once in the same function
static int get_network_local_info(int isTx, int isRevAudio)
{
    char *xmlstr;
    // Get the queue this object is connected to
    int queueid;
    int rc;

    if (ss_local_result == NULL)
        return -1;

    rc = strmsrv_xml_get_integer_parameter("QueueId", &queueid);

    if (rc == -1)
        return -1;

    release_local_info();

    // Use the retrieved queueid to get corresponding network info
    if ((isTx == TRUE && isRevAudio == FALSE) || (isTx == FALSE && isRevAudio == TRUE))
        ss_local_result = strmsvr_command("action=xlist dsttype=AnyDecoderPort");
    else
        ss_local_result = strmsvr_command("action=xlist srctype=AnyNetworkPort");

    if (ss_local_result == NULL)
        return -1;

    if ((xmlstr = strmsvr_get_result_xml(ss_local_result)) == NULL)
        return -1;

    strmsrv_xml_create(xmlstr);


    rc = strmsrv_xml_select_object_by_queueid(queueid);
    int id;
    rc = strmsrv_xml_get_integer_parameter("Id", &id);

    if (rc == -1)
        return -1;

    return id;
}

/* Send a command to StreamingServer and get its output */
/* On any error (eg. unable to contact StreamingServer), returns -1) */
static char *send_strmsvr_command(char *command)
{

    int s = 0;
    int status = 0;
    int len = strlen(command) + 2;
    int result_size = 1024;
    int write_len = 0;

    char *output = NULL;
    char *result = NULL;

    char *buffer = malloc(len + READ_LEN);
    if (buffer == NULL)
        goto EXIT_LABEL;

    result = malloc(result_size);
    if (result == NULL)
        goto EXIT_LABEL;

    strcpy(buffer, command);
    strcat(buffer, "\n");

    /* Create socket */
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s <= 0)
        goto EXIT_LABEL;

    /* Make socket non blocking */
    int rc = fcntl(s, F_SETFL, O_NONBLOCK);
    if (rc == -1) {
        goto EXIT_LABEL;
    }

    /* Create destination address */
    struct sockaddr_in dest;
    memset(&dest, 0x0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");
    dest.sin_port = ntohs(STREAMING_SERVER_PORT);

    /* Connect. Non-blocking so could return an error */
    rc = connect(s, (struct sockaddr *) &dest, sizeof(dest));
    if (rc == -1) {
        if (errno != EINPROGRESS)
            goto EXIT_LABEL;
    }

    /* Select on socket. While loop waits till connected, and
     * then sends data */
    int send_count = 0;
    int time_out = 100000;      /* 100 msec limit on all activity till connected */
    while (1) {
        fd_set write_fdset;
        fd_set read_fdset;
        FD_ZERO(&write_fdset);
        FD_ZERO(&read_fdset);
        if (status == ST_RECIEVING)
            FD_SET(s, &read_fdset);
        else
            FD_SET(s, &write_fdset);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = time_out;

        rc = select(s + 1, &read_fdset, &write_fdset, NULL, &timeout);
        if (rc <= 0) {
            /* select error or timeout */
            DPRINTF("Timed out.\n");
            goto EXIT_LABEL;
        }
        /* Socket is ready */
        if (status == ST_NOT_CONNECTED && FD_ISSET(s, &write_fdset)) {
            /* Not yet connected */
            int sockoptval = 0;
            int sockoptlen = sizeof(sockoptval);
            rc = getsockopt(s, SOL_SOCKET, SO_ERROR,
                            &sockoptval, (socklen_t *) & sockoptlen);
            if (rc == -1 || sockoptval != 0) {
                /* Connect failed */
                DPRINTF("Connect failed\n");
                goto EXIT_LABEL;
            }
            status = ST_CONNECTED;
            time_out = 3000000; // Increase timeout to 3 sec after connected
            DPRINTF("Connected\n");
        }
        else if (status == ST_CONNECTED && FD_ISSET(s, &write_fdset)) {
            /* We are connected */
            rc = write(s, buffer + send_count, len);
            if (rc < 0) {
                DPRINTF("Send error\n");
                goto EXIT_LABEL;
            }
            DPRINTF("Sent %d\n", rc);
            send_count += rc;
            len -= rc;
            if (len <= 0) {
                DPRINTF("Done Sending %d\n", send_count);
                status = ST_RECIEVING;
            }
        }
        else if (status == ST_RECIEVING && FD_ISSET(s, &read_fdset)) {
            /* We are waiting for response */
            DPRINTF("Waiting for response %d\n", write_len);
            rc = read(s, buffer, READ_LEN);
            if (rc <= 0) {
                DPRINTF("Remote closed: %d\n", rc);
                break;
            }
            else {
                if (write_len + rc > result_size) {
                    DPRINTF("Growing memory from: %d (%d)",
                            result_size, rc);
                    result_size += write_len + rc;
                    result = (char *) realloc(result, result_size);
                    DPRINTF(" to: %d\n", result_size);
                }

                /* look for newline in output */
                char *cp = memchr(buffer, '\n', rc);
                if (cp != NULL) {
                    DPRINTF("Seem to be done: %d %d\n", rc, cp - buffer);
                    *cp = '\0';
                    rc = cp - buffer + 1;
                    status = ST_DONE;
                }
                memcpy(result + write_len, buffer, rc);
                write_len += rc;
                DPRINTF("Read: %d now: %d\n", rc, write_len);
                if (status == ST_DONE) {
                    output = result;
                    break;
                }
            }

        }
        else {
            DPRINTF("Should not happen!!\n");
            break;
        }
    }


  EXIT_LABEL:
    if (s > 0)
        close(s);
    if (buffer != NULL)
        free(buffer);

    if (output == NULL && result != NULL)
        free(result);

    return output;
}


struct strmsvr_result *strmsvr_command(char *command)
{

    char * result = send_strmsvr_command(command);
    if (result == NULL)
        return NULL;

    struct strmsvr_result *ss_result;
    ss_result = malloc(sizeof(struct strmsvr_result));
    if (ss_result == NULL) {
        free(result);
        return NULL;
    }

    ss_result->result = result;
    return ss_result;
}

int strmsvr_get_status(struct strmsvr_result *ss_result)
{
    if (strncmp(ss_result->result, "200 OK", 6) == 0)
        return 1;
    return 0;
}

char *strmsvr_get_result_xml(struct strmsvr_result *ss_result)
{
    char *result = ss_result->result;
    char *xmlstr;

    if (!strmsvr_get_status(ss_result))
        return NULL;

    if ((xmlstr = strstr(result, "result:\"")) != NULL) {
        xmlstr = xmlstr + 8;
        if (*xmlstr != '\0' && *(xmlstr + 1) != '\0')
            return xmlstr;
    }
    return NULL;
}

void strmsvr_release_result(struct strmsvr_result *ss_result)
{

    if (ss_result != NULL) {
        if (ss_result->result != NULL)
            free(ss_result->result);
        free(ss_result);
        ss_result = NULL;
    }
}

// Issue streaming server command and release resources
// Returns the result of the command: 1 if status is OK, 0 otherwise
int strmsvr_execute_command(char *command)
{
    struct strmsvr_result *ss_result;
    int result;

    ss_result = strmsvr_command(command);
    if (ss_result == NULL)
        return 0;

    result = strmsvr_get_status(ss_result);

    strmsvr_release_result(ss_result);

    return result;
}

// Finds the fpgarx or fpgatx instance that matches the given board number
static int strmsrv_get_id_for_port(int ioport, int isTx, int isWriter) {

    char *xmlstr;
    struct strmsvr_result *ss_result = NULL;

    if (isWriter)
        ss_result = strmsvr_command("action=xlist dsttype=AnyRecorderPort");
    else if (isTx)
        ss_result = strmsvr_command("action=xlist srctype=AnyNetworkPort");
    else
        ss_result = strmsvr_command("action=xlist dsttype=AnyDecoderPort");

    if (ss_result == NULL)
        return -1;

    if ((xmlstr = strmsvr_get_result_xml(ss_result)) == NULL) {
        strmsvr_release_result(ss_result);
        return -1;
    }

    strmsrv_xml_create(xmlstr);

    strmsrv_xml_select_object_by_port(ioport);

    int id;
    int rc = strmsrv_xml_get_integer_parameter("Id", &id);

    strmsrv_xml_destroy();
    strmsvr_release_result(ss_result);

    if (rc == -1)
        return -1;

    return id;
}

int strmsrv_set_tx_kbm_params(int ioport, struct strmsrv_tx_kbm_params *params)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        int i;
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", id);

        if (params->kbtype_set == 1) {
            sprintf(buff + strlen(buff), "kbtype=%d ", params->kbtype);
        }

        if (params->mstype_set == 1) {
            sprintf(buff + strlen(buff), "mstype=%d ", params->mstype);
        }
        for (i = 0; i < 4; i++) {
            if (params->console_icon.esc_char_set[i] == 1) {
                sprintf(buff + strlen(buff), "consoleesc%d=%d ", i+1,
                       params->console_icon.esc_char[i]);
            }
        }
        for (i = 0; i < 4; i++) {
            if (params->local_icon.esc_char_set[i] == 1) {
                sprintf(buff + strlen(buff), "localesc%d=%d ", i+1,
                        params->local_icon.esc_char[i]);
            }
        }

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_set_tx_kb_layout_params(int ioport, struct strmsrv_tx_kb_layout_params *params) 
{
    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 0);

    if (id != -1) {
        char buff[256];
        int i;
        
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", id);

        sprintf(buff + strlen(buff), "kblayout=%d ", params->kblayout_set);
        sprintf(buff + strlen(buff), "kblanguage=%s ", params->kblanguage);
        
        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0; 

}


int strmsrv_set_rx_kb_layout_params(int ioport, struct strmsrv_rx_kb_layout_params *params) 
{
    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 0, 0);
    if (id != -1) {
        char buff[256];
        int i;
        
        sprintf(buff, "action=update id=%d dsttype=V2DHardWarePort ", id);

        sprintf(buff + strlen(buff), "kblayout=%d ", params->kblayout_set);
        sprintf(buff + strlen(buff), "kblanguage=%s ", params->kblanguage);
        
        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0; 
}

int strmsrv_set_rx_mouse_input_params(int ioport, struct strmsrv_rx_mouse_input_params *params)
{
    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 0, 0);
    if (id != -1) {
      char buff[256];

      sprintf(buff, "action=update id=%d dsttype=V2DHardWarePort ", id);
      sprintf(buff + strlen(buff), "MousePointer=%d ", params->mouse_input_enable);
      sprintf(buff + strlen(buff), "Timeout=%d ", params->timeout_value);
    
     
      ss_result = strmsvr_command(buff);
      if (ss_result != NULL)
        strmsvr_release_result(ss_result);
    }

   return 0; 
}

int strmsrv_set_tx_params(int ioport, struct strmsrv_tx_params *params)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", id);

        if (params->bandwidth_set == 1) {
            sprintf(buff + strlen(buff), "bandwidth=%d ", params->bandwidth * 8);
        }

        if (params->burst_ratio_set == 1) {
            sprintf(buff + strlen(buff), "burstratio=%f ", params->burst_ratio);
        }

        if (params->burst_duration_set == 1) {
            sprintf(buff + strlen(buff), "burstduration=%d ", params->burst_duration);
        }

        if (params->kbm_switch_set == 1) {
            sprintf(buff + strlen(buff), "allowkbmswitch=%d ", params->kbm_switch);
        }

        if (params->kbm_idle_limit_set == 1) {
            sprintf(buff + strlen(buff), "kbmidletime=%d ", params->kbm_idle_limit);
        }

        if (params->av_option_set == 1) {
            sprintf(buff + strlen(buff), "avoption=%d ", params->av_option);
        }

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_get_tx_params(int ioport, struct strmsrv_tx_params *params)
{

    char *xmlstr;
    struct strmsvr_result *ss_result;

    ss_result = strmsvr_command("action=xlist srctype=AnyNetworkPort");
    if (ss_result == NULL)
        return -1;

    if ((xmlstr = strmsvr_get_result_xml(ss_result)) == NULL) {
        strmsvr_release_result(ss_result);
        return -1;
    }

    strmsrv_xml_create(xmlstr);

    strmsrv_xml_select_object_by_port(ioport);

    int rc;
    rc = strmsrv_xml_get_integer_parameter("RequestBandwidth",
                                       &params->bandwidth);
    if (rc != -1)
        params->bandwidth_set = 1;

    rc = strmsrv_xml_get_float_parameter("BurstRatio",
                                         &params->burst_ratio);
    if (rc != -1)
        params->burst_ratio_set = 1;

    rc = strmsrv_xml_get_integer_parameter("BurstDuration",
                                       &params->burst_duration);
    if (rc != -1)
        params->burst_duration_set = 1;

    strmsrv_xml_destroy();

    return 0;
}

int strmsrv_get_tx_rfb_connection_params(int ioport,
        struct strmsrv_tx_rfb_connection_params *params)
{

    char *xmlstr;
    struct strmsvr_result *ss_result;

    params->rfb_server_set = params->rfb_ready_set = 0;

    ss_result = strmsvr_command("action=xlist srctype=AnyNetworkPort");
    if (ss_result == NULL)
        return -1;

    if ((xmlstr = strmsvr_get_result_xml(ss_result)) == NULL) {
        strmsvr_release_result(ss_result);
        return -1;
    }

    strmsrv_xml_create(xmlstr);

    strmsrv_xml_select_object_by_port(ioport);

    int rc;
    rc = strmsrv_xml_get_text_parameter("RFBServer", params->rfb_server, 32);
    if (rc != -1)
        params->rfb_server_set = 1;

    rc = strmsrv_xml_get_boolean_parameter("RFBReady",
                                       &params->rfb_ready);
    if (rc != -1) {
        params->rfb_ready_set = 1;

        strmsrv_xml_get_unsigned_long_parameter("RFBKeyboardSent",
                                        &params->rfb_keyboard_out);

        strmsrv_xml_get_unsigned_long_parameter("RFBMouseSent",
                                        &params->rfb_mouse_out);
    }

    strmsrv_xml_destroy();

    return 0;
}

int strmsrv_get_tx_connection_params(int ioport,
                                  struct strmsrv_tx_connection_params *params)
{

    int rc;
    char buffer[32];
    rc = get_fpga_local_info(ioport, TRUE, FALSE);

    if (rc == -1) 
        goto ERROR_LABEL;

    // Get FPGA side parameters
    rc = strmsrv_xml_get_integer_parameter("RequestBandwidth",
                                       &params->bandwidth);
    if (rc == -1) 
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_long_parameter("VideoBytes",
                                                 &params->video_bytes);

    if (rc == -1) 
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_long_parameter("AudioBytes",
                                                 &params->audio_bytes);

    if (rc == -1)
        goto ERROR_LABEL;

    rc = get_network_local_info(TRUE, FALSE);

    if(rc == -1) 
        goto ERROR_LABEL;

    // Get Network side parameters

    rc = strmsrv_xml_get_text_parameter("Client", params->client, 32);

    rc = strmsrv_xml_get_text_parameter("VideoName", params->lut, 40);

    rc = strmsrv_xml_get_boolean_parameter("Connected", &params->connected);

    rc = strmsrv_xml_get_boolean_parameter("Multicast", &params->multicast);

    strcat(buffer, "");
    rc = strmsrv_xml_get_text_parameter("ControllingClient", buffer, 32);
    if (rc != -1) {
        if (strlen(buffer) > 0 && strcmp(buffer, "0.0.0.0") != 0) {
            params->connected = 1;
            params->multicast = 1;
            strcpy(params->client, buffer);
        }
    }

    release_local_info();

    // Get reverse audio from FPGA side of Tx
    rc = get_fpga_local_info(ioport, TRUE, TRUE);
    if (rc != -1) {
        rc = strmsrv_xml_get_unsigned_long_long_parameter("AudioBytes",
                                       &params->reverse_audio_bytes);
    }
    else {
        params->reverse_audio_bytes = 0;
    }

    release_local_info();
    return 0;

ERROR_LABEL:
    release_local_info();

    return -1;

}

int strmsrv_get_rx_connection_params(int ioport,
                                     struct strmsrv_rx_connection_params *params)
{

    int rc;
    memset(params, 0x00, sizeof(struct strmsrv_rx_connection_params));

    rc = get_fpga_local_info(ioport, FALSE, FALSE);
    if (rc == -1) {
        goto ERROR_LABEL;
    }
    // Get local parameters
    rc = strmsrv_xml_get_unsigned_long_parameter("QueueDrops",
                                                 &params->queue_drops);

    // Get network side parameters
    rc = get_network_local_info(FALSE, FALSE);

    if (rc == -1) {
        params->client_unavailable = 1;
        goto EXIT_LABEL;
    }
    params->client_unavailable = 0;

    // Is the upstream object of type VNCNetworkPort ??
    int is_vnc_connection = 0;
    char typebuffer[33];
    strmsrv_xml_get_text_parameter("Type", typebuffer, 32);
    if (strcmp(typebuffer, "VNCNetworkPort") == 0)
        is_vnc_connection = 1;

    if (!is_vnc_connection) {
        rc = strmsrv_xml_get_text_parameter("Server", params->server, 32);
        rc = strmsrv_xml_get_text_parameter("ClientStatus", params->status, 64);

        rc = strmsrv_xml_get_boolean_parameter("Connected", &params->connected);

        rc = strmsrv_xml_get_boolean_parameter("Multicast", &params->multicast);

        rc = strmsrv_xml_get_unsigned_long_parameter("BytesTransfered",
                &params->total_bytes);

        rc = strmsrv_xml_get_unsigned_long_parameter("QueueDrops",
                &params->queue_drops);

        rc = strmsrv_xml_get_unsigned_long_parameter("VideoBytes",
                &params->video_bytes);

        rc = strmsrv_xml_get_unsigned_long_parameter("VideoPackets",
                &params->video_packets);

        rc = strmsrv_xml_get_unsigned_long_parameter("VideoLoss",
                &params->video_loss);

        rc = strmsrv_xml_get_unsigned_long_parameter("VideoOutOfOrder",
                &params->video_outoforder);

        rc = strmsrv_xml_get_unsigned_long_parameter("AudioBytes",
                &params->audio_bytes);

        rc = strmsrv_xml_get_unsigned_long_parameter("AudioPackets",
                &params->audio_packets);

        rc = strmsrv_xml_get_unsigned_long_parameter("AudioLoss",
                &params->audio_loss);

        rc = strmsrv_xml_get_unsigned_long_parameter("AudioOutOfOrder",
                &params->audio_outoforder);

        unsigned long network_side_drops;
        rc = strmsrv_xml_get_unsigned_long_parameter("QueueDrops",
                &network_side_drops);
        params->queue_drops += network_side_drops;
    }
    else {
        rc = strmsrv_xml_get_text_parameter("RFBServer", params->server, 32);
        strcat(params->server, " (VNC)");
        rc = strmsrv_xml_get_text_parameter("RFBConnectionStatus", params->status, 64);
        rc = strmsrv_xml_get_boolean_parameter("Connected", &params->connected);
        rc = strmsrv_xml_get_unsigned_long_parameter("BytesTransfered",
                &params->total_bytes);

        rc = strmsrv_xml_get_unsigned_long_parameter("RFBBytesRead",
                &params->video_bytes);
        rc = strmsrv_xml_get_unsigned_long_parameter("RFBPacketsRead",
                &params->video_packets);
    }

    // Get reverse audio from FPGA side of Rx
    // Since we reach here only if client is connected,
    // getting FPGA side info is sufficient
    release_local_info();
    rc = get_fpga_local_info(ioport, FALSE, TRUE);
    if (rc != -1) {
        rc = get_network_local_info(FALSE, TRUE);

        // This should pick up the AudioBytes form RTP TX object
        rc = strmsrv_xml_get_unsigned_long_long_parameter("AudioBytes",
                                       &params->reverse_audio_bytes);
    }
    else {
        params->reverse_audio_bytes = 0;
    }


EXIT_LABEL:
    release_local_info();

    return 0;

ERROR_LABEL:
    release_local_info();
    return -1;

}

int strmsrv_stop_fpga (int ioport) {
    int network_id = -1, fpga_id = -1;
    struct strmsvr_result *ss_result;
    char stopbuff[256];
    char delbuff[256];
    int istx = TRUE;
    // Send parameters to StreamingServer
    if (get_fpga_local_info(ioport, istx, FALSE) == 0) {
        strmsrv_xml_get_integer_parameter("Id", &fpga_id);
    }
    if (fpga_id < 0) {
        release_local_info();
        istx = 0;
        if (get_fpga_local_info(ioport, istx, FALSE) == 0) {
            strmsrv_xml_get_integer_parameter("Id", &fpga_id);
        }
    }
    if (fpga_id < 0) {
        VLOG_INFO("Could not find fpga object for port %d\n", ioport);
    }
    else {
        network_id = get_network_local_info(istx, FALSE);
    }
    if (network_id >= 0) {
        switch (istx) {
            case 0:
                sprintf(stopbuff, "action=stop id=%d srctype=V2DNetworkPort ",
                        network_id);
                sprintf(delbuff, "action=delete id=%d srctype=V2DNetworkPort ",
                        network_id);
                break;
            case 1:
                sprintf(stopbuff, "action=stop id=%d dsttype=V2DDecoderPort ",
                        network_id);
                sprintf(delbuff, "action=delete id=%d dsttype=V2DDecoderPort ",
                        network_id);
                break;
        }
        ss_result = strmsvr_command(stopbuff);
        if (ss_result != NULL) {
            strmsvr_release_result(ss_result);
        }
        ss_result = strmsvr_command(delbuff);
        if (ss_result != NULL) {
            strmsvr_release_result(ss_result);
        }
    }
    if (fpga_id >= 0) {
        switch (istx) {
            case 0:
                sprintf(stopbuff,
                        "action=stop id=%d dsttype=V2DHardWarePort ", fpga_id);
                sprintf(delbuff,
                        "action=delete id=%d dsttype=V2DHardWarePort ", fpga_id);
                break;
            case 1:
                sprintf(stopbuff,
                        "action=stop id=%d srctype=V2DEncoderPort ", fpga_id);
                sprintf(delbuff,
                        "action=delete id=%d srctype=V2DEncoderPort ", fpga_id);
                break;
        }
        ss_result = strmsvr_command(stopbuff);
        if (ss_result != NULL) {
            strmsvr_release_result(ss_result);
        }
        ss_result = strmsvr_command(delbuff);
        if (ss_result != NULL) {
            strmsvr_release_result(ss_result);
        }
    }
    release_local_info();
    return fpga_id;
}

int strmsrv_set_tx_rfb_params(int ioport, struct strmsrv_tx_rfb_params *params)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", id);

        sprintf(buff + strlen(buff), "rfbenable=%d ", params->enable);
        sprintf(buff + strlen(buff), "rfbip=%s ", params->ipaddress);
        sprintf(buff + strlen(buff), "rfbpasswd=%s ", params->password);
        sprintf(buff + strlen(buff), "rfbport=%d ", params->port);
        sprintf(buff + strlen(buff), "rfboffsetx=%d ", params->mouse_offset_x);
        sprintf(buff + strlen(buff), "rfboffsety=%d ", params->mouse_offset_y);

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_set_tx_splash_params(int ioport)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", id);

        sprintf(buff + strlen(buff), "updatesplash=1 ");
        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_set_rx_splash_params(int ioport)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 0, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d dsttype=V2DHardWarePort ", id);

        sprintf(buff + strlen(buff), "updatesplash=1 ");

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_get_tx_io_params(int ioport,
                             struct strmsrv_tx_io_params *params) {
    int rc;

    memset(params, 0x0, sizeof(struct strmsrv_tx_io_params));

    rc = get_fpga_local_info(ioport, TRUE, FALSE);

    if (rc == -1)
        goto ERROR_LABEL;

    // Get FPGA side parameters
    rc = strmsrv_xml_get_unsigned_long_parameter("KeyBoardPackets",
                                                 &params->keyboard_in);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("MousePackets",
                                                 &params->mouse_in);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("SerialInPackets",
                                                 &params->serial_in);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("SerialOutPackets",
                                                 &params->serial_out);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("UsbKbInBytes",
                                                 &params->usbkb_in_bytes);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("UsbMsInBytes",
                                                 &params->usbms_in_bytes);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_integer_parameter("KeyboardType",
                                            &params->kb_type);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_integer_parameter("MouseType",
                                            &params->ms_type);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_text_parameter("KeyboardMode",
                                        params->kb_mode, 32);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = get_network_local_info(TRUE, FALSE);
    if(rc == -1)
        goto ERROR_LABEL;

    // Get Network side parameters

    rc = strmsrv_xml_get_unsigned_long_parameter("CtrlPacketsRcvd",
                                                 &params->control_in);
    rc = strmsrv_xml_get_unsigned_long_parameter("CtrlPacketsSent",
                                                 &params->control_out);

    release_local_info();

    return 0;

ERROR_LABEL:
    release_local_info();
    return -1;
}

int strmsrv_set_rx_clnt_params(int ioport,
                               struct strmsrv_rx_clnt_params *params)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 0, 0);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        int i;
        sprintf(buff, "action=update id=%d dsttype=V2DHardWarePort ", id);

        if (params->kbmode_set == 1) {
            sprintf(buff + strlen(buff), "kbmode=%d ", params->kbmode);
        }
        for (i = 0; i < 4; i++) {
            if (params->console_icon.esc_char_set[i] == 1) {
                sprintf(buff + strlen(buff), "consoleesc%d=%d ", i+1,
                       params->console_icon.esc_char[i]);
            }
        }
        for (i = 0; i < 4; i++) {
            if (params->local_icon.esc_char_set[i] == 1) {
                sprintf(buff + strlen(buff), "localesc%d=%d ", i+1,
                        params->local_icon.esc_char[i]);
            }
        }
        for (i = 0; i < 4; i++) {
            if (params->remote_icon.esc_char_set[i] == 1) {
                sprintf(buff + strlen(buff), "remoteesc%d=%d ", i+1,
                        params->remote_icon.esc_char[i]);
            }
        }
        if (params->rfb_mousecursor == 1) {
            sprintf(buff + strlen(buff), "rfbmousecursor=1 ");
        }
        if (params->rfb_refresh == 1) {
            sprintf(buff + strlen(buff), "rfbrefresh=1 ");
        }
        if (params->local_kbtype_set == 1) {
            sprintf(buff + strlen(buff), "localkbtype=%d ",
                    params->local_kbtype);
        }
        if (params->local_mstype_set == 1) {
            sprintf(buff + strlen(buff), "localmstype=%d ",
                    params->local_mstype);
        }
        if (params->ov_option_set == 1) {
            sprintf(buff + strlen(buff), "overrideoption=%d ",
                    params->ov_option);
        }
        if (params->ov_color_set == 1) {
            sprintf(buff + strlen(buff), "overridecolor=%d ",
                    params->ov_color);
        }
        if (params->ov_keepaspect_set == 1) {
            sprintf(buff + strlen(buff), "overridekeepaspect=%d ",
                    params->ov_keepaspect);
        }

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

int strmsrv_set_hotplug_params(int ioport,
                               struct strmsrv_hotplug_params *params)
{
    struct strmsvr_result *ss_result;
    int tx_id = strmsrv_get_id_for_port(ioport, 1, 0);
    int rx_id = strmsrv_get_id_for_port(ioport, 0, 0);
    char buff[256];

    // Send parameters to StreamingServer
    if (tx_id != -1) {
        sprintf(buff, "action=update id=%d srctype=V2DEncoderPort ", tx_id);
    }
    else if (rx_id != -1) {
        sprintf(buff, "action=update id=%d dsttype=V2DHardWarePort ", rx_id);
    }
    else {
        return 0;
    }

    if (params->add_event_device_set == 1) {
        sprintf(buff + strlen(buff), "addeventdevice=%d ",
                    params->add_event_device);
    }
    if (params->remove_event_device_set == 1) {
        sprintf(buff + strlen(buff), "removeeventdevice=%d ",
                    params->remove_event_device);
    }
    if (params->add_serial_device_set == 1) {
        sprintf(buff + strlen(buff), "addserialdevice=%d ",
                    params->add_serial_device);
    }
    if (params->remove_serial_device_set == 1) {
            sprintf(buff + strlen(buff), "removeserialdevice=%d ",
                    params->remove_serial_device);
    }

    ss_result = strmsvr_command(buff);
    if (ss_result != NULL)
        strmsvr_release_result(ss_result);

    return 0;

}


int strmsrv_get_rx_io_params(int ioport,
                             struct strmsrv_rx_io_params *params)
{

    int rc;
    rc = get_fpga_local_info(ioport, FALSE, FALSE);
    if (rc == -1)
        goto ERROR_LABEL;

    // Get FPGA side parameters
    rc = strmsrv_xml_get_unsigned_long_parameter("SerialInPackets",
                                                 &params->serial_in);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("SerialOutPackets",
                                                 &params->serial_out);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("UsbKbInBytes",
                                                 &params->usbkb_in_bytes);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = strmsrv_xml_get_unsigned_long_parameter("UsbMsInBytes",
                                                 &params->usbms_in_bytes);
    if (rc == -1)
        goto ERROR_LABEL;
    rc = strmsrv_xml_get_text_parameter("KeyboardMode",
                                        params->kb_mode, 32);
    if (rc == -1)
        goto ERROR_LABEL;
    rc = strmsrv_xml_get_boolean_parameter("LocalCursor",
                                           &params->local_cursor);
    if (rc == -1)
        goto ERROR_LABEL;

    rc = get_network_local_info(FALSE, FALSE);
    if (rc != -1) {

        // Get Network side parameters

        rc = strmsrv_xml_get_unsigned_long_parameter("CtrlPacketsRcvd",
                                                     &params->control_in);
        rc = strmsrv_xml_get_unsigned_long_parameter("CtrlPacketsSent",
                                                     &params->control_out);
    }
    release_local_info();

    return 0;

ERROR_LABEL:
    release_local_info();
    return -1;
}

int strmsrv_tx_disconnect(int ioport)
{

    struct strmsvr_result *ss_result;

    int id = get_fpga_local_info(ioport, TRUE, FALSE);
    if (id == -1)
       goto ERROR_LABEL;
    id = get_network_local_info(TRUE, FALSE);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d dsttype=V2DDecoderPort ", id);

        sprintf(buff + strlen(buff), "disconnectall=1");

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    release_local_info();
    return 0;

ERROR_LABEL:
    release_local_info();
    return -1;
}

int strmsrv_rx_fullscreen_refresh(int ioport)
{

    struct strmsvr_result *ss_result;
    int id = get_fpga_local_info(ioport, FALSE, FALSE);

    if (id == -1)
       goto ERROR_LABEL;
    id = get_network_local_info(FALSE, FALSE);

    // Send parameters to StreamingServer
    if (id != -1) {
        char buff[256];
        sprintf(buff, "action=update id=%d srctype=V2DNetworkPort ", id);

        sprintf(buff + strlen(buff), "fullscreen_refresh=1");

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    release_local_info();
    return 0;

ERROR_LABEL:
    release_local_info();
    return -1;
}

int strmsrv_set_tx_thumbnail_params(int ioport,
                                    struct strmsrv_tx_thumbnail_params *params)
{

    struct strmsvr_result *ss_result;

    int id = strmsrv_get_id_for_port(ioport, 1, 1); // Searching Writer ports

    // Send parameters to StreamingServer
    if (id != -1) {
    	char buff[256];
    	sprintf(buff, "action=update id=%d ioport=%d dsttype=ImageWriterPort dstdir=\"/data/DSS/data/portal/site/thumbnails/%d/\" streamtype=V2D enabletn=%d" \
    	            " native=%d tnlarge=%d tnsmall=%d tnlargewidth=%d tnlargeheight=%d tnsmallwidth=%d tnsmallheight=%d tntimeinterval=%d",
    	            id, ioport, ioport, params->enable_thumbnails, params->enable_native, params->enable_large_thumbnail, params->enable_small_thumbnail,
    	            params->large_thumbnail_width, params->large_thumbnail_height, params->small_thumbnail_width, params->small_thumbnail_height,
    	            params->thumbnail_interval);

        ss_result = strmsvr_command(buff);
        if (ss_result != NULL)
            strmsvr_release_result(ss_result);
    }

    return 0;
}

