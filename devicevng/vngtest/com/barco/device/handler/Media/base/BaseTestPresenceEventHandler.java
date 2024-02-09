package com.barco.device.handler.Media.base;

import static org.junit.Assert.fail;

import org.dom4j.Document;
import org.dom4j.Element;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.agent.AgentImpl;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.MediaPresenceEventHandler;
import com.barco.utils.XMLUtils;

public class BaseTestPresenceEventHandler extends BaseMediaHandlerTestSetup  {

	protected Document presenceEventHandlerReady;
	
	@BeforeClass
	public static void setUpBeforeClass(){
		BaseMediaHandlerTestSetup.setUpBeforeClass();
		
		
	}
	@Test
	public void testPresenceEventHandlerNotReady() {
					
		headerElement.element("servicename").setText(AgentImpl.Media);
		headerElement.element("eventname").setText("PresenceEvent");
		headerElement.element("eventWallclock").setText(String.valueOf(System.currentTimeMillis()));
		headerElement.element("eventLevel").setText(Message.INFOTYPE);

		Element dataElement;
		try {			
			dataElement = XMLUtils.getElementXpath(eventDoc, "//data");
			Element presenceElement = (Element)presenceDoc.getRootElement().clone();
			//Element presenceElement = (Element)presenceDoc.getRootElement().detach();
			dataElement.add(presenceElement);
			
			// First send unavailable
			presenceElement.addAttribute("value", "Unavailable");
			Element agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");
			Message presenceEventMessage = Message.parseMessage(agentMessage.asXML());
			MediaPresenceEventHandler presenceEventHandler = new MediaPresenceEventHandler();
			presenceEventHandler.handleMessage(presenceEventMessage, device);
		} catch (Exception e) {
			//fail("The test: testPresenceEventHandlerNotReady has failed with following exception: "+e.getStackTrace());
			e.printStackTrace();
		}		
		
	}
	@Test
	public void testPresenceEventHandlerReady() {	
		System.out.println("Enetered: testPresenceEventHandlerReady");
		headerElement.element("servicename").setText(AgentImpl.Media);
		headerElement.element("eventname").setText("PresenceEvent");
		headerElement.element("eventWallclock").setText(String.valueOf(System.currentTimeMillis()));
		headerElement.element("eventLevel").setText(Message.INFOTYPE);
		Element dataElement;
		try {
			// Then send ready
			dataElement = XMLUtils.getElementXpath(eventDoc, "//data");
			Element presenceElement = (Element)presenceDoc.getRootElement().clone();
			dataElement.add(presenceElement);
			presenceElement.addAttribute("value", "Ready");
			Element agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");			
			Message presenceEventMessage = Message.parseMessage(agentMessage.asXML());
			presenceEventMessage = Message.parseMessage(agentMessage.asXML());
			MediaPresenceEventHandler presenceEventHandler = new MediaPresenceEventHandler();			
			presenceEventHandler.handleMessage(presenceEventMessage, device);
			presenceEventHandlerReady = presenceEventMessage.getResult();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			fail("The test: testPresenceEventHandlerReady has failed with following exception: ");
			e.printStackTrace();			
		}
		
	}

}
