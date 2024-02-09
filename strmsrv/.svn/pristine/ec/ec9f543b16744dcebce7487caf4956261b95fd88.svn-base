#include "strmsrv_compatibility.h"
#include <math.h>

#include "appthread.hh"
#include "squeue.hh"
#include "common.hh"
#include "mutex.hh"

#define NUMB_MESSAGES 5000000
#define MESSAGE_WINDOW 1024

int AFFINITY = 0;
int special_messenger = 1;


class ProducerThread : public CAppThread
{
private:
    int m_id;
    unsigned long m_nMessageCount;
public:
    ProducerThread(int id);
    int Run();
    unsigned long GetNumberOfMessages() {return m_nMessageCount;};
};

ProducerThread::ProducerThread(int id)
{
    m_id = id;
    m_priority = 0;
    m_affinity = AFFINITY;
    m_nMessageCount = 0;
    if (m_pThreadName != NULL)
        sprintf(m_pThreadName, "Producer %d", id);
}

int
ProducerThread::Run()
{
    SYSLOG_INFO("Starting...");

    CSQueueSink *sink = CSQueueManager::Instance()->GetQueueSink("q1");

    char message[256];
    strcpy(message, "hello");

    for (int i = 0; i < NUMB_MESSAGES; i++) {
        CSFrame *frame =
            new CSFrame(message, sizeof(message), m_id, i);
        sink->InsertFrame(frame);
        m_nMessageCount++;

        if (i%MESSAGE_WINDOW == 0) {
            // Producer 2 and 3 behavior: Release and re-acquire the queue periodically
            if (m_id > 1 && special_messenger) {
                CSQueueManager::Instance()->ReleaseQueueSink(sink);
                if (m_id == 2)
                    YieldThread();
                else 
                    LightSleep();
                sink = CSQueueManager::Instance()->GetQueueSink("q1");
            }

            // Wait till the consumers have consumed enough 
            // to drop the queue size to a reasonable number
            if (sink->Size() > MESSAGE_WINDOW * 3) {
                while(sink->Size() > MESSAGE_WINDOW * 2) {
                    //LightSleep();
                    YieldThread();
                    if (m_bExit)
                        break;
                }
            }
        }
        if (m_bExit)
            break;
    }

    CSQueueManager::Instance()->ReleaseQueueSink(sink);

    SYSLOG_INFO("Done");
    return 0;
}

class ConsumerThread : public CAppThread
{
private:
    int m_id;
    unsigned long m_nMessageCount;
    int m_nNextSeq[4];

public:
    ConsumerThread(int id);
    int Run();
    unsigned long GetNumberOfMessages() {return m_nMessageCount;};
    bool m_bStarted;
    bool m_bFault;
};

ConsumerThread::ConsumerThread(int id)
{
    m_id = id;
    m_priority = 0;
    m_affinity = AFFINITY;
    m_nMessageCount = 0;
    m_bStarted = false;
    m_bFault = false;
    m_nNextSeq[1] = 0;
    m_nNextSeq[2] = 0;
    m_nNextSeq[3] = 0;

    if (m_pThreadName != NULL)
        sprintf(m_pThreadName, "Consumer %d", id);
}

int
ConsumerThread::Run()
{
    SYSLOG_INFO("Starting...");

    CSQueueSource *source = CSQueueManager::Instance()->GetQueueSource("q1");


    source->SetMaxSize(500000);
    source->SetMaxBytes(0xFFFFFFFF);
    
    CSFrame * frame;
    
    while (!m_bExit) {
        frame = source->GetFrame();
        if (frame == NULL) {
            //LightSleep();
            YieldThread();
            m_bStarted = true;
            continue;
        }

        m_nMessageCount++;


        int id = frame->GetMediaFlag();
        int seq = frame->GetTimeStamp();
        if (m_nNextSeq[id] != 0) {
            if (m_nNextSeq[id] != seq) {
                SYSLOG_INFO("Unexpected Sequence Number");
                m_bFault = true;
                break;
            }

        }
        m_nNextSeq[id] = seq + 1;
        source->ReleaseFrame(frame);

        if (m_nMessageCount%MESSAGE_WINDOW == 0) {
            // Consumer 2 and 3 behavior: Release and re-acquire the queue periodically
            if (m_id > 1 && special_messenger) {
                CSQueueManager::Instance()->ReleaseQueueSource(source);
                m_nNextSeq[1] = 0;
                m_nNextSeq[2] = 0;
                m_nNextSeq[3] = 0;
                if (m_id == 2)
                    YieldThread();
                else 
                    LightSleep();
                source = CSQueueManager::Instance()->GetQueueSource("q1");
            }

        }


    }

    if (source->DropCount() > 0 && m_id == 1)
        m_bFault = true;

    
    SYSLOG_INFO("Messages = %lu Drops = %d Total = %lu Max = %d", 
                m_nMessageCount, source->DropCount(), 
                m_nMessageCount + source->DropCount(),
                source->MaxLength());

    CSQueueManager::Instance()->ReleaseQueueSource(source);

    SYSLOG_INFO("Done");
    return 0;
}


static void usage(char *command) {
    printf("Usage: %s [OPTIONS]\n", command);
    printf("\nOptions:\n");
    printf("\t-d duration\t\tDuration of run in seconds (default 5)\n");
    printf("\t-a affinity\t\tCPU affinity. Use -1 for no affinity (default 0)\n");
    printf("\t-n number\t\tNumber of Producers/Consumers (default 3)\n");
    printf("\t-R number\t\tProducers/Consumers are regular (no special behaviour)\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    bool fault = false;
    int ch;
    int affinity = 0;
    unsigned long duration = 5000;
    int number_of_messengers = 3;

    while( (ch=getopt(argc, argv, "d:a:n:R")) != -1) {
        switch(ch)
        {
        case 'd':
            duration = (unsigned long)(atof(optarg) * 1000.0);
            break;
        case 'a':
            affinity = atoi(optarg);
            break;
        case 'n':
            number_of_messengers = atoi(optarg);
            if (number_of_messengers < 1 ||
                number_of_messengers > 3)
                number_of_messengers = 3;
            break;
        case 'R':
            special_messenger = 0;
            break;
        case 'h':
        default:
            usage(argv[0]);
            break;
        }
    }

    if (affinity < -1 || affinity > 3) {
        printf("Affinity should be -1, 0, 1, 2, or 3\n");
        exit(-1);
    }
    AFFINITY = affinity;

    InitSSSyslog(true);
    AddThreadName("Main");

    // Initialize the timekeeper
    CTimeKeeper::Begin();

    ConsumerThread *c1, *c2, *c3;
    ProducerThread *p1, *p2, *p3;

    c1 = new ConsumerThread(1);
    c2 = new ConsumerThread(2);
    c3 = new ConsumerThread(3);
    

    p1 = new ProducerThread(1);
    p2 = new ProducerThread(2);
    p3 = new ProducerThread(3);


    c1->Start();
    if (number_of_messengers > 1)
        c2->Start();
    if (number_of_messengers > 2)
        c3->Start();

    while (!c1->m_bStarted)
        usleep(1);
    if (number_of_messengers > 1) {
        while (!c2->m_bStarted)
            usleep(1);
    }
    if (number_of_messengers > 2) {
        while (!c3->m_bStarted)
            usleep(1);
    }
           
    SYSLOG_INFO("Consumers ready");

    timestamp_t startTime = CTimeKeeper::GetTime();

    p1->Start();
    if (number_of_messengers > 1)
        p2->Start();
    if (number_of_messengers > 2)
        p3->Start();

    timestamp_t endTime = startTime;
    while (1) {
        endTime = CTimeKeeper::GetTime();
        if (endTime - startTime >= duration) {
            SYSLOG_INFO("Terminating Test Run.");
            break;
        }
        usleep(1000);
    }


    SYSLOG_INFO("Stopping test.");

    // Stop all procucers
    p1->Stop();
    if (number_of_messengers > 1)
        p2->Stop();
    if (number_of_messengers > 2)
        p3->Stop();

    // Wait for primary consumer to get all the messages
    unsigned long messagesConsumed = 0;
    unsigned long messagesProduced = 
        p1->GetNumberOfMessages() +
        p2->GetNumberOfMessages() +
        p3->GetNumberOfMessages();
    int count = 0;
    while (1) {
        messagesConsumed = c1->GetNumberOfMessages();
        if (messagesConsumed >= messagesProduced) {
            break;
        }
        usleep(10000);
        if (count++ > 10) { // wait upto 1 sec
            SYSLOG_INFO("Error: not all messages received by primary consumer.");
            fault = true;
            break;
        }
    }

    // Stop the consumers
    c1->Stop();
    if (number_of_messengers > 1)
        c2->Stop();
    if (number_of_messengers > 2)
        c3->Stop();

    if (c1->m_bFault ||
        c2->m_bFault ||
        c3->m_bFault)
        fault = true;

    unsigned long numbMessages = 0;
    if (c1->GetNumberOfMessages() > numbMessages)
        numbMessages = c1->GetNumberOfMessages();
    if (c2->GetNumberOfMessages() > numbMessages)
        numbMessages = c2->GetNumberOfMessages();
    if (c3->GetNumberOfMessages() > numbMessages)
        numbMessages = c3->GetNumberOfMessages();


    SYSLOG_INFO("Messages Produced = %lu, Consumed = %lu",
                messagesProduced, messagesConsumed);

    SYSLOG_INFO("Time taken = %"PRIu64",  Messages = %lu",
                endTime - startTime,
                numbMessages);

    SYSLOG_INFO("Thread Tests: %s",
                fault ? "FAILED" :"PASSED");

}
