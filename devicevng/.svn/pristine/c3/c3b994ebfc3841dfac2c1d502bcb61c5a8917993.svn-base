/*package com.barco.device.handler.Media.base;

import static org.junit.Assert.*;

import org.dom4j.Document;
import org.dom4j.Element;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.common.Message;
import com.barco.device.handler.Layout.TeardownRequestHandler;
import com.barco.test.utils.TestConfigConst;
import com.barco.test.utils.TestUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;

public class BaseTestTearDownStreamRequestHandler extends BaseMediaHandlerTestSetup{

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Test
	public void testTeardownStreamRequestHandler() throws Exception {
		try {
			TestUtils testUtils = new TestUtils();
			String stopRequestXML = testUtils.loadXmlFromResource(TestConfigConst.TEARDOWNSTREAMREQUEST);			
			String teardownRequestXML = stopRequestXML.replace("StopStreamRequest", "TeardownStreamRequest");
			Document teardownDoc = XMLUtils.stringToDocument(teardownRequestXML);
			Element agentMessage = XMLUtils.getElementXpath(teardownDoc, "//AgentMessage");			
			Message teardownStreamRequestMessage = Message.parseMessage(agentMessage.asXML());		
			TeardownRequestHandler teardownStreamRequestHandler = new TeardownRequestHandler();
			teardownStreamRequestHandler.handleMessage(teardownStreamRequestMessage, device);
			//System.out.println("MediaTest:main teardownResponse=" + teardownStreamRequestMessage.getResult().asXML());
		} catch (Exception e) {
			String message = "The test: testTeardownStreamRequestHandler has failed with the exception: "+e.getMessage();
			fail(message);
			e.printStackTrace();
		}

	}

}
*/