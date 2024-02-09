package com.barco.device.handler.Media.base;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.fail;

import org.dom4j.Document;
import org.dom4j.Element;
import org.junit.Test;

import com.barco.device.agent.AgentImpl;
import com.barco.device.agent.MediaAgentI;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.StreamStatusEventHandler;
import com.barco.test.utils.TestConfigConst;
import com.barco.test.utils.TestUtils;
import com.barco.utils.XMLUtils;

public class BaseTestStreamStatusEventHandler extends BaseMediaHandlerTestSetup {	
	@Test
	public void testStreamStatusEventHandler(){
		try{			
		TestUtils testUtils = new TestUtils();
		String streamStatusXML = testUtils.loadXmlFromResource(TestConfigConst.STREAMSTATUSREQUEST);	
		Document streamStatusDoc = XMLUtils.stringToDocument(streamStatusXML);	
		headerElement.element("servicename").setText(AgentImpl.Media);
		headerElement.element("eventname").setText("StreamStatusEvent");
		headerElement.element("eventWallclock").setText(String.valueOf(System.currentTimeMillis()));
		headerElement.element("eventLevel").setText(Message.ERRORTYPE);

		Element dataElement = XMLUtils.getElementXpath(eventDoc, "//data");
		dataElement.add(streamStatusDoc.getRootElement().detach());
		XMLUtils.setValueXpath(eventDoc, "//state", String.valueOf(MediaAgentI.END_OF_MEDIA)); 
		Element agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");
		Message streamStatusEventMessage = Message.parseMessage(agentMessage.asXML());
		StreamStatusEventHandler streamStatusEventHandler = new StreamStatusEventHandler();
		streamStatusEventHandler.handleMessage(streamStatusEventMessage, device);
		assertNotNull("TestStreamStatusEventHandler: testHandleMessage : streamStatusEventMessage Result is null",streamStatusEventMessage.getResult());
		}catch(Exception e){
			String message = "The test: testStreamStatusEventHandler has failed with following exception: "+e.getMessage();
			fail(message);
			e.printStackTrace();
		}
	}

}
