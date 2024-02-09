
#include "strmsrv_compatibility.h"

#include <gtest/gtest.h>
#include "squeue.hh"


// The fixture for testing class CQueue.
class CQueueTest : public testing::Test {
protected:

    // Acquire the queue manager singleton so that can be destroyed
    // after each test and we can start afresh
    virtual void SetUp() {
        m_queueManager = CSQueueManager::Instance();
    }

    virtual void TearDown() {
        m_queueManager->Destroy();
    }

    char message[32];

    CSQueueManager *m_queueManager;
};

// Acquire and release a sink queue
TEST_F(CQueueTest, AcquireSink) {

    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueSink *sink1 = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink1 != NULL);

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q2"));
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSink(sink1);
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());

}

// Acquire and release more than one sink queue
TEST_F(CQueueTest, AcquireSinkMany) {

    CSQueueSink *sink1 = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink1 != NULL);

    CSQueueSink *sink2 = CSQueueManager::Instance()->GetQueueSink("q2");
    ASSERT_TRUE(sink2 != NULL);

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q2"));
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q3"));
    EXPECT_EQ(2, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSink(sink1);
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSink(sink2);
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q2"));
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());


}

// Get hold of more than one handle to the same sink queue
TEST_F(CQueueTest, AcquireSinkMultiple) {

    CSQueueSink *sink1 = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink1 != NULL);

    CSQueueSink *sink2 = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink2 != NULL);

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSink(sink1);
    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));

    // Only released one handle so queue manager should still have one queue
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSink(sink2);
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());

}


// Acquire and release a source queue
TEST_F(CQueueTest, AcquireSource) {

    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());

    CSQueueSource *source1 = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source1 != NULL);

    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());
    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));

    CSQueueManager::Instance()->ReleaseQueueSource(source1);
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));

}

// Acquire and release more than one source queue
TEST_F(CQueueTest, AcquireSourceMany) {

    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());

    CSQueueSource *source1 = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source1 != NULL);

    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueSource *source2 = CSQueueManager::Instance()->GetQueueSource("q2");
    ASSERT_TRUE(source2 != NULL);

    EXPECT_EQ(2, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(2, CSQueueManager::Instance()->NumberOfSinks());

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q2"));

    CSQueueManager::Instance()->ReleaseQueueSource(source1);
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueManager::Instance()->ReleaseQueueSource(source2);
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());

    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));
    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q2"));

}

// Get hold of more than one handle to the same source queue
TEST_F(CQueueTest, AcquireSourceMultiple) {

    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());

    CSQueueSource *source1 = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source1 != NULL);

    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    CSQueueSource *source2 = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source2 != NULL);

    EXPECT_EQ(2, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));

    CSQueueManager::Instance()->ReleaseQueueSource(source1);
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(1, CSQueueManager::Instance()->NumberOfSinks());

    EXPECT_TRUE(CSQueueManager::Instance()->HasQueueSink("q1"));

    CSQueueManager::Instance()->ReleaseQueueSource(source2);
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());
    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSinks());

    EXPECT_FALSE(CSQueueManager::Instance()->HasQueueSink("q1"));

}

// Test ActiveSink
TEST_F(CQueueTest, ActiveSink) {

    EXPECT_EQ(0, CSQueueManager::Instance()->NumberOfSources());

    CSQueueSource *source1 = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source1 != NULL);

    EXPECT_FALSE(CSQueueManager::Instance()->HasActiveSink(source1));

    CSQueueSink *sink1 = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink1 != NULL);

    EXPECT_TRUE(CSQueueManager::Instance()->HasActiveSink(source1));

    CSQueueManager::Instance()->ReleaseQueueSink(sink1);

    EXPECT_FALSE(CSQueueManager::Instance()->HasActiveSink(source1));

    CSQueueManager::Instance()->ReleaseQueueSource(source1);


}

// Send and receive a single message
TEST_F(CQueueTest, Message) {


    CSQueueSource *source = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source != NULL);

    CSQueueSink *sink = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink != NULL);

    strcpy(message, "hello");
    CSFrame *frame =
        new CSFrame(message, sizeof(message));

    ASSERT_TRUE(frame != NULL);

    sink->InsertFrame(frame);
    EXPECT_EQ(1, sink->Size());

    // Get the message
    EXPECT_EQ(1, source->Size());
    frame = source->GetFrame();
    ASSERT_TRUE(frame != NULL);
    EXPECT_STREQ("hello", frame->GetData());

    // Can get the same frame again since not released yet
    EXPECT_EQ(1, source->Size());
    EXPECT_EQ(1, sink->Size());
    frame = source->GetFrame();
    EXPECT_TRUE(frame != NULL);
    EXPECT_STREQ("hello", frame->GetData());

    // Now release the frame and ensure cannot get any more frames
    source->ReleaseFrame(frame);
    EXPECT_EQ(0, source->Size());
    EXPECT_EQ(0, sink->Size());
    frame = source->GetFrame();
    EXPECT_TRUE(frame == NULL);

    CSQueueManager::Instance()->ReleaseQueueSink(sink);

    CSQueueManager::Instance()->ReleaseQueueSource(source);

}


// Send and receive many messages
TEST_F(CQueueTest, ManyMessages) {

    int message_count = 50;

    CSQueueSink *sink = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink != NULL);

    CSQueueSource *source = CSQueueManager::Instance()->GetQueueSource("q1");
    ASSERT_TRUE(source != NULL);


    // Send unique messages
    for (int count = 0; count < message_count; count++) {
        sprintf(message, "hello_%d", count);
        CSFrame *frame =
            new CSFrame(message, sizeof(message));
        ASSERT_TRUE(frame != NULL);

        sink->InsertFrame(frame);

    }
    EXPECT_EQ(message_count, sink->Size());

    // The sink may go away even before the messages are
    // retrived by consumer
    CSQueueManager::Instance()->ReleaseQueueSink(sink);


    // See of we can retrieve all the unique messages
    EXPECT_EQ(message_count, source->Size());
    for (int count = 0; count < message_count; count++) {
        CSFrame *frame = source->GetFrame();
        ASSERT_TRUE(frame != NULL);
        sprintf(message, "hello_%d", count);
        EXPECT_STREQ(message, frame->GetData());
        source->ReleaseFrame(frame);
    }
    EXPECT_EQ(0, source->Size());
    EXPECT_EQ(0, sink->Size());


    CSQueueManager::Instance()->ReleaseQueueSource(source);

}


// Multiple  Consumers
TEST_F(CQueueTest, MultipleConsumers) {

    int message_count = 50;
    int consumer_count = 10;

    CSQueueSink *sink = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink != NULL);

    CSQueueSource *source_array[consumer_count];

    // All sources get a handle to the same queue id
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {
        source_array[source_idx] = CSQueueManager::Instance()->GetQueueSource("q1");
        ASSERT_TRUE(source_array[source_idx] != NULL);
    }


    // Send unique messages
    for (int count = 0; count < message_count; count++) {
        sprintf(message, "hello_%d", count);
        CSFrame *frame =
            new CSFrame(message, sizeof(message));
        ASSERT_TRUE(frame != NULL);

        sink->InsertFrame(frame);
    }
    EXPECT_EQ(message_count, sink->Size());


    // Ensure each consumer can independently retrieve all messages
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {

        EXPECT_EQ(message_count, source_array[source_idx]->Size());
        for (int count = 0; count < message_count; count++) {
            CSFrame *frame = source_array[source_idx]->GetFrame();
            ASSERT_TRUE(frame != NULL);
            sprintf(message, "hello_%d", count);
            EXPECT_STREQ(message, frame->GetData());
            source_array[source_idx]->ReleaseFrame(frame);
        }

        EXPECT_EQ(0, source_array[source_idx]->Size());
        CSQueueManager::Instance()->ReleaseQueueSource(source_array[source_idx]);
    }

    CSQueueManager::Instance()->ReleaseQueueSink(sink);

}

// Multiple Produces and Consumers
TEST_F(CQueueTest, MultipleProducersConsumers) {

    int message_count = 50;
    int consumer_count = 10;
    int producer_count = 5;

    CSQueueSink *sink_array[producer_count];

    // All producers get a handle to the same queue id
    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        sink_array[sink_idx] = CSQueueManager::Instance()->GetQueueSink("q1");
        ASSERT_TRUE(sink_array[sink_idx] != NULL);
    }

    CSQueueSource *source_array[consumer_count];

    // All consumers get a handle to the same queue id
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {
        source_array[source_idx] = CSQueueManager::Instance()->GetQueueSource("q1");
        ASSERT_TRUE(source_array[source_idx] != NULL);
    }


    // Each producer sends all their messages one after another
    // Each producer creates his own unique message
    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        for (int count = 0; count < message_count; count++) {
            sprintf(message, "hello_%d_%d", count, sink_idx);
            CSFrame *frame =
                new CSFrame(message, sizeof(message));
            ASSERT_TRUE(frame != NULL);

            sink_array[sink_idx]->InsertFrame(frame);
        }
    }


    // Ensure each consumer can independently retrieve all messages
    // from all producers
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {

        EXPECT_EQ(message_count * producer_count, source_array[source_idx]->Size());
        for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
            for (int count = 0; count < message_count; count++) {
                CSFrame *frame = source_array[source_idx]->GetFrame();
                ASSERT_TRUE(frame != NULL);
                sprintf(message, "hello_%d_%d", count, sink_idx);
                EXPECT_STREQ(message, frame->GetData());
                source_array[source_idx]->ReleaseFrame(frame);
            }
        }
        CSQueueManager::Instance()->ReleaseQueueSource(source_array[source_idx]);
    }

    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        CSQueueManager::Instance()->ReleaseQueueSink(sink_array[sink_idx]);
    }

}

// Multiple Produces and Consumers, Spliced
TEST_F(CQueueTest, MultipleProducersConsumersSpliced) {

    int message_count = 50;
    int consumer_count = 5;
    int producer_count = 10;

    CSQueueSink *sink_array[producer_count];

    // All producers get a handle to the same queue id
    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        sink_array[sink_idx] = CSQueueManager::Instance()->GetQueueSink("q1");
        ASSERT_TRUE(sink_array[sink_idx] != NULL);
    }

    CSQueueSource *source_array[consumer_count];

    // All consumers get a handle to the same queue id
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {
        source_array[source_idx] = CSQueueManager::Instance()->GetQueueSource("q1");
        ASSERT_TRUE(source_array[source_idx] != NULL);
    }


    // Each producer creates his own unique message and sends it spliced
    for (int count = 0; count < message_count; count++) {
        for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
            sprintf(message, "hello_%d_%d", count, sink_idx);
            CSFrame *frame =
                new CSFrame(message, sizeof(message));
            ASSERT_TRUE(frame != NULL);

            sink_array[sink_idx]->InsertFrame(frame);
        }
    }


    // Ensure each consumer can independently retrieve all messages
    // from all producers
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {

        EXPECT_EQ(message_count * producer_count, source_array[source_idx]->Size());
        for (int count = 0; count < message_count; count++) {
            for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
                CSFrame *frame = source_array[source_idx]->GetFrame();
                ASSERT_TRUE(frame != NULL);
                sprintf(message, "hello_%d_%d", count, sink_idx);
                EXPECT_STREQ(message, frame->GetData());
                source_array[source_idx]->ReleaseFrame(frame);
            }
        }
        CSQueueManager::Instance()->ReleaseQueueSource(source_array[source_idx]);
    }

    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        CSQueueManager::Instance()->ReleaseQueueSink(sink_array[sink_idx]);
    }

}

TEST_F(CQueueTest, CreatePipe) {

    int pipeid = 1;

    EXPECT_TRUE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));

    EXPECT_TRUE(CSQueueManager::Instance()->DeletePipe(pipeid));

}

TEST_F(CQueueTest, DeletePipe) {

    int pipeid = 1;

    EXPECT_TRUE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));

    EXPECT_TRUE(CSQueueManager::Instance()->DeletePipe(pipeid));

    EXPECT_FALSE(CSQueueManager::Instance()->DeletePipe(pipeid));

}

TEST_F(CQueueTest, FailSelfPipe) {

    int pipeid = 1;

    EXPECT_FALSE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q1"));

}

TEST_F(CQueueTest, FailDuplicatePipe) {

    int pipeid = 1;

    EXPECT_TRUE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));

    EXPECT_FALSE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));

    EXPECT_TRUE(CSQueueManager::Instance()->DeletePipe(pipeid));

}


TEST_F(CQueueTest, PipeMessage) {

    int pipeid = 1;

    EXPECT_TRUE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));

    // Consumer gets a handle to the source end of pipe
    CSQueueSource *source = CSQueueManager::Instance()->GetQueueSource("q2");
    ASSERT_TRUE(source != NULL);

    // Producer gets a handle to the sink end of pipe
    CSQueueSink *sink = CSQueueManager::Instance()->GetQueueSink("q1");
    ASSERT_TRUE(sink != NULL);

    strcpy(message, "hello");
    CSFrame *frame =
        new CSFrame(message, sizeof(message));

    ASSERT_TRUE(frame != NULL);

    sink->InsertFrame(frame);

    // Get the message
    EXPECT_EQ(1, source->Size());
    frame = source->GetFrame();
    ASSERT_TRUE(frame != NULL);
    EXPECT_STREQ("hello", frame->GetData());

    // Can get the same frame again since not released yet
    EXPECT_EQ(1, source->Size());
    frame = source->GetFrame();
    EXPECT_TRUE(frame != NULL);
    EXPECT_STREQ("hello", frame->GetData());

    // Now release the frame and ensure cannot get any more frames
    source->ReleaseFrame(frame);
    EXPECT_EQ(0, source->Size());
    frame = source->GetFrame();
    EXPECT_TRUE(frame == NULL);

    CSQueueManager::Instance()->ReleaseQueueSink(sink);

    CSQueueManager::Instance()->ReleaseQueueSource(source);

    EXPECT_TRUE(CSQueueManager::Instance()->DeletePipe(pipeid));


}

TEST_F(CQueueTest, PipeMultipleProducersConsumers) {

    int pipeid = 1;

    EXPECT_TRUE(CSQueueManager::Instance()->CreatePipe(pipeid, "q1", "q2"));


    int message_count = 50;
    int consumer_count = 10;
    int producer_count = 5;

    CSQueueSink *sink_array[producer_count];

    // All producers get a handle to the sink end of pipe
    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        sink_array[sink_idx] = CSQueueManager::Instance()->GetQueueSink("q1");
        ASSERT_TRUE(sink_array[sink_idx] != NULL);
    }

    CSQueueSource *source_array[consumer_count];

    // All consumers get a handle to the source end of pipe
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {
        source_array[source_idx] = CSQueueManager::Instance()->GetQueueSource("q2");
        ASSERT_TRUE(source_array[source_idx] != NULL);
    }


    // Each producer sends all their messages one after another
    // Each producer creates his own unique message
    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        for (int count = 0; count < message_count; count++) {
            sprintf(message, "hello_%d_%d", count, sink_idx);
            CSFrame *frame =
                new CSFrame(message, sizeof(message));
            ASSERT_TRUE(frame != NULL);

            sink_array[sink_idx]->InsertFrame(frame);
        }
    }


    // Ensure each consumer can independently retrieve all messages
    // from all producers
    for (int source_idx = 0; source_idx < consumer_count; source_idx++) {

        EXPECT_EQ(message_count * producer_count, source_array[source_idx]->Size());
        for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
            for (int count = 0; count < message_count; count++) {
                CSFrame *frame = source_array[source_idx]->GetFrame();
                ASSERT_TRUE(frame != NULL);
                sprintf(message, "hello_%d_%d", count, sink_idx);
                EXPECT_STREQ(message, frame->GetData());
                source_array[source_idx]->ReleaseFrame(frame);
            }
        }
        CSQueueManager::Instance()->ReleaseQueueSource(source_array[source_idx]);
    }

    for (int sink_idx = 0; sink_idx < producer_count; sink_idx++) {
        CSQueueManager::Instance()->ReleaseQueueSink(sink_array[sink_idx]);
    }

    EXPECT_TRUE(CSQueueManager::Instance()->DeletePipe(pipeid));

}
