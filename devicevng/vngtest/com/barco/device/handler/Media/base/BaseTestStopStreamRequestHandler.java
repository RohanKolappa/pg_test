package com.barco.device.handler.Media.base;

import org.junit.Test;

import com.barco.device.common.Message;
import com.barco.device.handler.Media.StopRequestHandler;
import com.barco.test.utils.TestConfigConst;
import com.barco.test.utils.TestUtils;

public class BaseTestStopStreamRequestHandler extends BaseMediaHandlerTestSetup {

	@Test
	public void testStopStreamRequestHanlder() throws Exception {
		TestUtils testUtils = new TestUtils();
		String stopRequestXML = testUtils.loadXmlFromResource(TestConfigConst.STOPSTREAMREQUEST);	
		Message stopStreamRequestMessage = Message.parseMessage(stopRequestXML);
		StopRequestHandler stopStreamRequestHandler = new StopRequestHandler();
		stopStreamRequestHandler.handleMessage(stopStreamRequestMessage, device);
		System.out.println("MediaTest:main stopResponse=" + stopStreamRequestMessage.getResult().asXML());
	}

}
