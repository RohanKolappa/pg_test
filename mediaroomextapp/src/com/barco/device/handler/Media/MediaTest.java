package com.barco.device.handler.Media;



import java.io.BufferedReader;
import java.io.InputStreamReader;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.DeviceImpl;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.impl.DMSMediaAgentImpl;
import com.barco.device.handler.Media.impl.StreamingServerI;
import com.barco.device.handler.Media.impl.streamingserver.DummyStreamingServerImpl;
import com.barco.utils.FilePath;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.XMPPI;

public class MediaTest {
	
	public static void testStartStopStream(String userJID, String deviceNID, MRLog log, XMPPI xmpp,
			DeviceI device) throws Exception {

		String startRequestXML = FileUtils.readFile(FilePath.startRequestPath);
		Message startStreamRequestMessage = Message.parseMessage(startRequestXML);
		StartStreamRequestHandler startStreamRequestHandler = new StartStreamRequestHandler();
		startStreamRequestHandler.handleMessage(startStreamRequestMessage, device);
		// System.out.println("MediaTest:main startResponse=" + startStreamRequestMessage.getResult().asXML());
		
		String stopRequestXML = FileUtils.readFile(FilePath.stopRequestPath);
		Message stopStreamRequestMessage = Message.parseMessage(stopRequestXML);
		StopStreamRequestHandler stopStreamRequestHandler = new StopStreamRequestHandler();
		stopStreamRequestHandler.handleMessage(stopStreamRequestMessage, device);
		// System.out.println("MediaTest:main stopResponse=" + stopStreamRequestMessage.getResult().asXML());
		
	}
	
	
	public static void testSetupStream(String userJID, String deviceNID, MRLog log, XMPPI xmpp,
			DeviceI device) throws Exception {
		
		String startRequestXML = FileUtils.readFile(FilePath.startRequestPath);
		String setupRequestXML = startRequestXML.replace("StartStreamRequest", "SetupStreamRequest");
		Document setupDoc = MRXMLUtils.stringToDocument(setupRequestXML);
		Element agentMessage = MRXMLUtils.getElementXpath(setupDoc, "//AgentMessage");
		Message setupStreamRequestMessage = Message.parseMessage(agentMessage.asXML());
		SetupStreamRequestHandler setupStreamRequestHandler = new SetupStreamRequestHandler();
		setupStreamRequestHandler.handleMessage(setupStreamRequestMessage, device);
		System.out.println("MediaTest:main setupResponse=" + setupStreamRequestMessage.getResult().asXML());
	}

	public static void testTeardownStream(String userJID, String deviceNID, MRLog log, XMPPI xmpp,
				DeviceI device) throws Exception {
		String stopRequestXML = FileUtils.readFile(FilePath.stopRequestPath);
		String teardownRequestXML = stopRequestXML.replace("StopStreamRequest", "TeardownStreamRequest");
		Document teardownDoc = MRXMLUtils.stringToDocument(teardownRequestXML);
		Element agentMessage = MRXMLUtils.getElementXpath(teardownDoc, "//AgentMessage");
		Message teardownStreamRequestMessage = Message.parseMessage(agentMessage.asXML());
		TeardownStreamRequestHandler teardownStreamRequestHandler = new TeardownStreamRequestHandler();
		teardownStreamRequestHandler.handleMessage(teardownStreamRequestMessage, device);
		System.out.println("MediaTest:main teardownResponse=" + teardownStreamRequestMessage.getResult().asXML());

	}

	public static void main(String []args) throws Exception {

		String userJID = "userJID";
		String deviceNID = "deviceNID";		
		MRLog log = MRLog.getInstance(MRLog.CONSOLE);
		XMPPI xmpp = null;
		
		Document deviceDoc = XMLUtils.fileToDocument(FilePath.deviceXMLPath);
		String deviceType = deviceDoc.getRootElement().getName();
		StreamingServerI streamingServer = new DummyStreamingServerImpl(userJID, log, xmpp, null);
		streamingServer.getNetworkResource().updateNetworkResource(deviceDoc);

		DMSMediaAgentImpl mediaAgentImpl = new DMSMediaAgentImpl(userJID, deviceNID,deviceType,
				streamingServer, log,xmpp,deviceDoc);
		
		DeviceImpl device = new DeviceImpl(null, null, mediaAgentImpl, null, 
				DeviceImpl.DefaultHTTPPort, DeviceImpl.DefaultHandlerPackage);
		// device.login("10.51.48.190","xpi","Barco$123","xpi","managementserver@localhost/managementserver");
		
		// <message to='xpi@localhost/xpi'><x xmlns='com.ipvs.session'><Test/></x></message>
		// Waiting
		
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
	    String request;
	    while (true) {
	    	// prompt
    		System.out.print(">");
    		
    		// Read the request
    		request = in.readLine();
	    	if("bye".equals(request))
	    		break;
	    	
	    	if("setup".equals(request))
				MediaTest.testSetupStream(userJID, deviceNID, log, xmpp, device);

	    	if("teardown".equals(request))
				MediaTest.testTeardownStream(userJID, deviceNID, log, xmpp, device);

	    }
	    
	    //<login ip='10.51.48.190' user='xpi' password='Barco$123' resource='xpi' managementserver='managementserver@localhost/managementserver'/>

		//MediaTest.testStartStopStream(userJID, deviceNID, log, xmpp, device);
	}
}
