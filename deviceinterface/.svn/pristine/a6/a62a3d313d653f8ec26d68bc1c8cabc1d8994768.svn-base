/*
 * Device Test, this is used to test the functionality of newly added service provider
 * In the final product this will be part of deviceapp, DeviceTest will extend it for testing purposes
 */
package com.barco.device.test;



import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ServiceLoader;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.AgentImpl;
import com.barco.device.agent.MediaAgentI;
import com.barco.device.app.DeviceApp;
import com.barco.device.base.DeviceI;
import com.barco.device.base.DeviceImpl;
import com.barco.device.common.Message;
import com.barco.device.handler.Layout.SetDisplayConfigRequestHandler;
import com.barco.device.handler.Layout.SetupRequestHandler;
import com.barco.device.handler.Layout.TeardownRequestHandler;
import com.barco.device.handler.Media.MediaPresenceEventHandler;
import com.barco.device.handler.Media.StartRequestHandler;
import com.barco.device.handler.Media.StopRequestHandler;
import com.barco.device.handler.Media.StreamStatusEventHandler;
import com.barco.device.service.ServiceI;
import com.barco.utils.MediaUtils;
import com.barco.utils.XMLUtils;

public class BaseDeviceTest extends DeviceApp{
	
	protected static String configFile = null;
	protected static String statusFile = null;	
	static String supportedDevice = null;
	static ServiceLoader<ServiceI> providers= null;	
	
	public void testStartStream(DeviceI device) throws Exception {
		String startRequestXML = XMLUtils.loadXMLResource(TestConst.StartRequestPath).asXML();
		Message startStreamRequestMessage = Message.parseMessage(startRequestXML);
		StartRequestHandler startStreamRequestHandler = new StartRequestHandler();
		startStreamRequestHandler.handleMessage(startStreamRequestMessage, device);
		System.out.println("MediaTest:main startResponse=" + startStreamRequestMessage.getResult().asXML());		
	}
	
	public static void testStopStream(DeviceI device) throws Exception {
		String stopRequestXML = XMLUtils.loadXMLResource(TestConst.StopRequestPath).asXML();
		Message stopStreamRequestMessage = Message.parseMessage(stopRequestXML);
		StopRequestHandler stopStreamRequestHandler = new StopRequestHandler();
		stopStreamRequestHandler.handleMessage(stopStreamRequestMessage, device);
		System.out.println("MediaTest:main stopResponse=" + stopStreamRequestMessage.getResult().asXML());
	}
	
	public void testSetupStream(DeviceI device) throws Exception {
		
		String startRequestXML = XMLUtils.loadXMLResource(TestConst.StartRequestPath).asXML();
		String setupRequestXML = startRequestXML.replace("StartStreamRequest", "SetupStreamRequest");
		Document setupDoc = XMLUtils.stringToDocument(setupRequestXML);
		Element agentMessage = XMLUtils.getElementXpath(setupDoc, "//AgentMessage");
		Message setupStreamRequestMessage = Message.parseMessage(agentMessage.asXML());
		SetupRequestHandler setupStreamRequestHandler = new SetupRequestHandler();
		setupStreamRequestHandler.handleMessage(setupStreamRequestMessage, device);
		System.out.println("MediaTest:main setupResponse=" + setupStreamRequestMessage.getResult().asXML());
	}

	public void testStreamStatusEvent(DeviceI device) throws Exception {
		String eventXML = XMLUtils.loadXMLResource(TestConst.EventPath).asXML();
		Document eventDoc = XMLUtils.stringToDocument(eventXML);
		Document streamStatusDoc = MediaUtils.createStreamStatusDoc();
		Element headerElement = XMLUtils.getElementXpath(eventDoc, "//Header");
		headerElement.addAttribute("serviceName", AgentImpl.Media);
		headerElement.addAttribute("eventName", "StreamStatusEvent");
		headerElement.addAttribute("eventWallclock", String.valueOf(System.currentTimeMillis()));
		headerElement.addAttribute("eventLevel", Message.ERRORTYPE);
	
		Element dataElement = XMLUtils.getElementXpath(eventDoc, "//Data");
		dataElement.add(streamStatusDoc.getRootElement().detach());
		XMLUtils.setValueXpath(eventDoc, "//state", String.valueOf(MediaAgentI.END_OF_MEDIA)); 
		Element agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");
		Message streamStatusEventMessage = Message.parseMessage(agentMessage.asXML());
		StreamStatusEventHandler streamStatusEventHandler = new StreamStatusEventHandler();
		streamStatusEventHandler.handleMessage(streamStatusEventMessage, device);
	}

	public void testPresenceEvent(DeviceI device) throws Exception {
		String eventXML = XMLUtils.loadXMLResource(TestConst.EventPath).asXML();
		Document eventDoc = XMLUtils.stringToDocument(eventXML);
		String presenceXML = XMLUtils.loadXMLResource(TestConst.PresencePath).asXML();
		Document presenceDoc = XMLUtils.stringToDocument(presenceXML);
		Element headerElement = XMLUtils.getElementXpath(eventDoc, "//Header");
		headerElement.addAttribute("servicename", AgentImpl.Media);
		headerElement.addAttribute("eventname", "StreamStatusEvent");
		headerElement.addAttribute("eventWallclock", String.valueOf(System.currentTimeMillis()));
		headerElement.addAttribute("eventLevel", Message.ERRORTYPE);
	
		Element dataElement = XMLUtils.getElementXpath(eventDoc, "//Data");
		Element presenceElement = (Element)presenceDoc.getRootElement().detach();
		dataElement.add(presenceElement);
		
		// First send unavailable
		presenceElement.addAttribute("value", "Unavailable");
		Element agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");
		Message presenceEventMessage = Message.parseMessage(agentMessage.asXML());
		MediaPresenceEventHandler presenceEventHandler = new MediaPresenceEventHandler();
		presenceEventHandler.handleMessage(presenceEventMessage, device);

		// Then send ready
		presenceElement.addAttribute("value", "Ready");
		agentMessage = XMLUtils.getElementXpath(eventDoc, "//AgentMessage");
		presenceEventMessage = Message.parseMessage(agentMessage.asXML());
		presenceEventHandler = new MediaPresenceEventHandler();
		presenceEventHandler.handleMessage(presenceEventMessage, device);

	}

	

	public void testTeardownStream(DeviceI device) throws Exception {
		String stopRequestXML = XMLUtils.loadXMLResource(TestConst.StopRequestPath).asXML();
		String teardownRequestXML = stopRequestXML.replace("StopStreamRequest", "TeardownStreamRequest");
		Document teardownDoc = XMLUtils.stringToDocument(teardownRequestXML);
		Element agentMessage = XMLUtils.getElementXpath(teardownDoc, "//AgentMessage");
		Message teardownStreamRequestMessage = Message.parseMessage(agentMessage.asXML());
		TeardownRequestHandler teardownStreamRequestHandler = new TeardownRequestHandler();
		teardownStreamRequestHandler.handleMessage(teardownStreamRequestMessage, device);
		System.out.println("MediaTest:main teardownResponse=" + teardownStreamRequestMessage.getResult().asXML());

	}
	
	public void testSetDisplayConfig(DeviceI device) throws Exception {

		String setDisplayConfigRequestXML = XMLUtils.loadXMLResource(TestConst.SetDisplayConfigRequestPath).asXML();
		Message setDisplayConfigStreamRequestMessage = Message.parseMessage(setDisplayConfigRequestXML);
		SetDisplayConfigRequestHandler setDisplayConfigStreamRequestHandler = new SetDisplayConfigRequestHandler();
		setDisplayConfigStreamRequestHandler.handleMessage(setDisplayConfigStreamRequestMessage, device);
		System.out.println("MediaTest:main setDisplayConfigResponse=" + setDisplayConfigStreamRequestMessage.getResult().asXML());		
		
	}

	

	public void run() throws Exception {
	
		AgentImpl agentImpl = getAgentImpl();
		agentImpl.deviceDB.setDeviceConfigLoader(configFile);
		agentImpl.deviceDB.setDeviceStatusLoader(statusFile);
		DeviceImpl device = new DeviceImpl(agentImpl,DeviceImpl.DefaultHTTPPort, DeviceImpl.DefaultHandlerPackage,"","");
	    //<login ip='10.51.48.190' user='xpi' password='Barco$123' resource='xpi' managementserver='managementserver@localhost/managementserver'/>
		// device.login("10.51.48.190","xpi","Barco$123","xpi","managementserver@localhost/managementserver");
		// <message to='xpi@localhost/xpi'><x xmlns='com.ipvs.session'><Test/></x></message>
		// Waiting
		System.out.println("Please enter your option: ");
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));		
	    String request;
	    while (true) {
	    	// prompt
			System.out.print(">");
			
			// Read the request
			request = in.readLine();
	    	if("bye".equals(request))
	    		break;
	
	    	if("start".equals(request))
				testStartStream(device);
	
	    	if("stop".equals(request))
				testStopStream(device);
	
	    	if("setup".equals(request))
				testSetupStream(device);
	
	    	if("teardown".equals(request))
				testTeardownStream(device);
	
	    	if("setdisplay".equals(request))
				testSetDisplayConfig(device);
	    	
	    	if("status".equals(request))
				testStreamStatusEvent(device);
	    	
	    	if("presence".equals(request))
				testPresenceEvent(device);
	    	
	    	if("login".equals(request))
				testPresenceEvent(device);
	
	    }
	}

	
	
	public static void main(String []args) throws Exception {

		if("reload".equalsIgnoreCase(args[0]))
		{
			if(providers != null)
				providers.reload();
			configFile = args[1];
			statusFile = args[2];
		}
		else{
		configFile = args[0];
		statusFile = args[1];
		}		
	
		
		BaseDeviceTest deviceTest = new BaseDeviceTest();
		deviceTest.run();
	}
}
