#include "portability.h"
#include <unistd.h>
#include <ctype.h>


#include "rfbclient.h"

// Make this 1 if you connect to a real pc. 
// This will send purely mouse movement commands that shuld be safe
int realserver = 0;

static void usage(char *command) {
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    exit(1);
}

static unsigned long long getmillisec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long) tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

static void Process_RFB(RFBClient_t *client) {

    RFB_Go(client);
    if (RFBState(client) == RFB_CLIENT_STATE_ERROR) {
        printf("Process_RFB Error %d\n", RFBErrorCode(client));
        exit(0);
    }

}

static void SendMyMouseEvent(RFBClient_t *client, int x, int y, int buttonmask)
{
    
    while (SendMouseEvent(client, x, y, buttonmask) != RFB_CLIENT_RC_SUCCESS) {
        printf("Retrying send mouse event\n");
        usleep(1);
        Process_RFB(client);
    }

    Process_RFB(client);
}

static void SendMyKeyboardEvent(RFBClient_t *client, unsigned long key)
{
    if (realserver)
        return;
    // send an up event and a down event for every key
    if (SendKeyboardEvent(client, key, 1) != RFB_CLIENT_RC_SUCCESS) {
        printf("Cannot send keyboard event\n");
        exit(0);
    }

    Process_RFB(client);

    if (SendKeyboardEvent(client, key, 0) != RFB_CLIENT_RC_SUCCESS) {
        printf("Cannot send keyboard event\n");
        exit(0);
    }

    Process_RFB(client);
}

static void millisleep(int ms)
{
    if (ms > 0)
        usleep(ms);
}

static void DoVideoTest(RFBClient_t *client) {

    RFBRect_t *rect;
    int i;
    int rc;
    int gotrect = 0;
    int waitcount = 0;
    int rectanglecount = 0;

    unsigned long long startmillisec = getmillisec();

    RFBSendExpectedPixelFormat(client);
    RFBSendSupportedEncodings(client);
    RFBSendVideoRequest(client, 0, 0, client->m_frameWidth, client->m_frameHeight, 0);
    //RFBSendVideoRequest(client, 0, 0, 200, 150, 0);


    for (i = 0; i < 10000; i++) {
        rc = RFB_Go(client);
        //printf("TS = %"PRIu64"\n", getmillisec() - startmillisec);

        if (RFBState(client) == RFB_CLIENT_STATE_ERROR) {
            printf("RFB protocol errored out...\n");
            break;
        }

        rect = RFBGetRectangle(client);

        if (rect != NULL) {
            rectanglecount++;
            printf("Got a rect [%d %d]: %dX%d@%d:%d TS = %"PRIu64"\n", rectanglecount, i,
                    rect->m_Width, rect->m_Height,
                    rect->m_XPos, rect->m_YPos,
                    getmillisec() - startmillisec);
            if (!LAST_RECTANGLE(rect))
                printf("More rectangles pending %d\n", client->m_nRemainingRectangles);
            RFBDestroyRectangle(rect);
            gotrect = 1;
        }


        if (rectanglecount >= 100)
            break;

        if (gotrect) {
            waitcount++;
        }

        if (waitcount > 10 ) {
            RFBSendVideoRequest(client, 0, 0, client->m_frameWidth, client->m_frameHeight, 1);
            gotrect = 0;
            waitcount = 0;
        }

        //printf("step %d\n", i);
        if (rc == 0) {
            usleep(1000);
        }
        else {
         //   printf("Not waiting %d...\n", i);
        }
    }
}

static void DoKBMTest(RFBClient_t *client)
{
    //int interval = 1000;
    int interval = 1;
    int step = 50;
    int i, j;
    int buttonmask = 0;
    int key = 0x61;

    for (i = 0; i < 5; i++) {

        int x = i * step;
        int y = i * step;
        if (!realserver) {
            if (i%2==1)
                buttonmask = 1;
            else
                buttonmask = 0;
        }
        for (j = 0; j < step; j++) {
            SendMyMouseEvent(client, x, y++, buttonmask);
            RFB_Go(client);
            millisleep(interval);
        }
        SendMyKeyboardEvent(client, key++);
        for (j = 0; j < step; j++) {
            SendMyMouseEvent(client, x++, y, buttonmask);
            RFB_Go(client);
            millisleep(interval);
        }
        SendMyKeyboardEvent(client, key++);
        for (j = 0; j < step; j++) {
            SendMyMouseEvent(client, x, y--, buttonmask);
            RFB_Go(client);
            millisleep(interval);
        }
        SendMyKeyboardEvent(client, key++);
        for (j = 0; j < step; j++) {
            SendMyMouseEvent(client, x--, y, buttonmask);
            RFB_Go(client);
            millisleep(interval);
        }

    }
}

int main(int argc, char *argv[]) {
    
    int port = 5900;
    int ch;
    char *host;
    RFBClient_t *client;
    char *password = NULL;
    char askpassword[128];

    host = strdup("127.0.0.1");

    while( (ch=getopt(argc, argv, "d:p:P:h")) != -1)
    {
        switch(ch)
        {
        case 'd':
            host = strdup(optarg);
            break;

        case 'p':
            port = atoi(optarg);
            break;
            

        case 'P':
            password = strdup(optarg);
            break;
            
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }

    printf("Testing RFB for %s:%d\n", host, port);

#ifdef WINDOWS
    // Windows sockets need a startup method
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
    client = CreateRFBNetworkClient(host, port, password);

    // Start the client protocol
    RFBStart(client);
    int done = 0;
    int rc;
    while(!done) {
        
        rc = RFB_Go(client);

        if (rc == 1) {
            switch(RFBState(client)) {
            case RFB_CLIENT_STATE_READY:
                printf("Ready\n");
                done = 1;
                break;

            case RFB_CLIENT_STATE_ASK_PASSWORD:
                printf("Provide password: ");
                fgets(askpassword, 16, stdin);
                int pos = strlen(askpassword) - 1;
                if (askpassword[pos] == '\n') {
                    askpassword[pos] = '\0';
                }
                RFBSetPassword(client, askpassword);

                break;

            case RFB_CLIENT_STATE_ERROR:
                printf("Error %d\n", RFBErrorCode(client));
                done = 1;
                exit(0);
                break;
            }
        }
        else {
            usleep(1);
        }
    }


    printf("Desktop: \"%s\" Width: %d Height:%d\n", 
           client->m_desktopName, client->m_frameWidth, client->m_frameHeight);


    if (0) {
        DoVideoTest(client);
    }

    if (0) {
        DoKBMTest(client);
    }

    free(host);
    free(password);
    RFBDestroy(client);
    return 0;
}
