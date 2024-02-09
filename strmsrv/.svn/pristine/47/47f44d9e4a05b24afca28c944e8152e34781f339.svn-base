

#include "strmsrv_compatibility.h"

#include "command.hh"
#include <gtest/gtest.h>

using namespace std;

// The fixture for testing class CQueue.
class CCommandTest : public testing::Test {
protected:
    
    // Initialize and teardown the Command lib so that it can be reset
    // after each test and we can start afresh
    virtual void SetUp() {
        CCommand::Init();
    }
    
    virtual void TearDown() {
        CCommand::Finish();
    }


private:


};


TEST_F(CCommandTest, Create) {

    CCommand *command = new CCommand();

    EXPECT_FALSE(command->Initialized());

    delete command;
}


TEST_F(CCommandTest, EmptyResponse) {


    string expected_response;
    expected_response.append("RESPONSE i:0 r:0 s:0 t:0 cid:0 result:");
    expected_response.append("\"\"");

    CCommand *command = new CCommand();

    command->Init("action=xstatus");

    EXPECT_EQ(expected_response, command->GetResponse());

    delete command;
}


TEST_F(CCommandTest, XMLBasic) {

    string expected_response;
    expected_response.append("RESPONSE i:0 r:0 s:0 t:0 cid:0 result:");
    expected_response.append("\"");
    expected_response.append("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");

    expected_response.append("<result>");
    expected_response.append("<result_code>0</result_code><objects/>");
    expected_response.append("<property name=\"RootInt\" type=\"integer\">12345</property>");  
    expected_response.append("<command_id>0</command_id>");
    expected_response.append("</result>");
    expected_response.append("\"");

    CCommand *command = new CCommand();

    command->Init("action=xstatus");

    command->SetRootParam("RootInt", 12345);
    
    EXPECT_EQ(expected_response, command->GetResponse());


    delete command;
}

TEST_F(CCommandTest, XMLFull) {


    string expected_response;
    expected_response.append("RESPONSE i:0 r:0 s:0 t:0 cid:0 result:");
    expected_response.append("\"");
    expected_response.append("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");

    expected_response.append("<result>");
    expected_response.append("<result_code>0</result_code><objects/>");
    expected_response.append("<property name=\"RootInteger\" type=\"integer\">12345</property>");  
    expected_response.append("<property name=\"RootUnsignedInteger\" type=\"unsignedInteger\">12345</property>");  
    expected_response.append("<property name=\"RootUnsignedLong\" type=\"unsignedLong\">123451234512345</property>");  
    expected_response.append("<property name=\"RootDouble\" type=\"float\">1.234500</property>");  


    expected_response.append("<object>");
    expected_response.append("<property kind=\"constant\" name=\"Id\" type=\"integer\">10</property>");  
    expected_response.append("<property name=\"Integer\" type=\"integer\">12345</property>");  
    expected_response.append("<property name=\"UnsignedInteger\" type=\"unsignedInteger\">12345</property>");  
    expected_response.append("<property name=\"UnsignedLong\" type=\"unsignedLong\">123451234512345</property>");  
    expected_response.append("<property name=\"Boolean\" type=\"boolean\">True</property>");  
    expected_response.append("<property name=\"String\" type=\"string\">12345</property>");  
    expected_response.append("<property name=\"Double\" type=\"float\">1.234500</property>");  
    expected_response.append("<property name=\"Long\" type=\"long\">123451234512345</property>");  
    expected_response.append("<property kind=\"constant\" name=\"ConstantInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"counter\" name=\"CounterInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"bytecounter\" name=\"ByteCounterInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"errorcounter\" name=\"ErrorCounterInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"guage\" name=\"GuageInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"bitrateguage\" name=\"BitrateGuageInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("<property kind=\"byterateguage\" name=\"ByterateGuageInteger\" type=\"unsignedInteger\">12345</property>");
    expected_response.append("</object>");


    expected_response.append("<command_id>0</command_id>");
    expected_response.append("</result>");
    expected_response.append("\"");


    for (int i = 0; i < 1; i++) { // Increase loop count for quick performance test
        CCommand *command = new CCommand();

        command->Init("action=xstatus");

        command->SetRootParam("RootInteger", 12345);
        command->SetRootParam("RootUnsignedInteger", (unsigned long) 12345);
        command->SetRootParam("RootUnsignedLong", (unsigned long long) 123451234512345ULL);

        command->SetRootParam("RootDouble", (double) 1.2345);

        command->SetCurrentObject(10);

        command->SetObjectParam("Integer", 12345);
        command->SetObjectParam("UnsignedInteger", (unsigned long) 12345);
        command->SetObjectParam("UnsignedLong", (unsigned long long) 123451234512345ULL);
        command->SetObjectParam("Boolean", (bool)true);
        command->SetObjectParam("String", "12345");
        command->SetObjectParam("Double", (double) 1.2345);
        command->SetObjectParam("Long", (long long) 123451234512345LL);
        command->SetObjectConstantParam("ConstantInteger", (unsigned long)12345);
        command->SetObjectCounterParam("CounterInteger", (unsigned long)12345);
        command->SetObjectByteCounterParam("ByteCounterInteger", (unsigned long)12345);
        command->SetObjectErrorCounterParam("ErrorCounterInteger", (unsigned long)12345);
        command->SetObjectGuageParam("GuageInteger", (unsigned long)12345);
        command->SetObjectBitrateGuageParam("BitrateGuageInteger", (unsigned long)12345);
        command->SetObjectByterateGuageParam("ByterateGuageInteger", (unsigned long)12345);


        if (i == 0) {
            EXPECT_EQ(expected_response, command->GetResponse());
        }

        delete command;

    }

}
