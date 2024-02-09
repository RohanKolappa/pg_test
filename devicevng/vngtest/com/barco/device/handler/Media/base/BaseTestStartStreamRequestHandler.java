package com.barco.device.handler.Media.base;

import org.dom4j.Document;
import org.junit.Test;

import com.barco.device.common.Message;
import com.barco.device.handler.Media.StartRequestHandler;
import com.barco.test.utils.TestConfigConst;
import com.barco.test.utils.TestUtils;


public class BaseTestStartStreamRequestHandler extends BaseMediaHandlerTestSetup {
	protected Document startStreamRequestResult;
	@Test
	public void testStartStreamRequestHandler(){
		String startRequestXML;
		try {		
			TestUtils testUtils = new TestUtils();			
			startRequestXML= testUtils.loadXmlFromResource(TestConfigConst.STARTSTREAMREQUEST);			
			Message startStreamRequestMessage = Message.parseMessage(startRequestXML);			
			StartRequestHandler startStreamRequestHandler = new StartRequestHandler();
			startStreamRequestHandler.handleMessage(startStreamRequestMessage, device);			
			startStreamRequestResult = startStreamRequestMessage.getResult();
			
		} catch (Exception e) {
			String message = "The test: testStartStreamRequestHandler has failed with following exception: "+e.getMessage();
			//fail(message);
			e.printStackTrace();
		}
		
	}

}
