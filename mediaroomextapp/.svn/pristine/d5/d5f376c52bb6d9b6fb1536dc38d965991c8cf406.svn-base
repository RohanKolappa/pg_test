package com.barco.device.handler.Layout;



import java.io.BufferedReader;
import java.io.InputStreamReader;

import org.dom4j.Document;

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
import com.barco.utils.mr.XMPPI;

public class LayoutTest {
	
	public static void testSetDisplayConfig(String userJID, String deviceNID, MRLog log, XMPPI xmpp,
			DeviceI device) throws Exception {

		String setDisplayConfigRequestXML = FileUtils.readFile(FilePath.setDisplayConfigRequestPath);
		Message setDisplayConfigStreamRequestMessage = Message.parseMessage(setDisplayConfigRequestXML);
		SetDisplayConfigRequestHandler setDisplayConfigStreamRequestHandler = new SetDisplayConfigRequestHandler();
		setDisplayConfigStreamRequestHandler.handleMessage(setDisplayConfigStreamRequestMessage, device);
		// System.out.println("MediaTest:main setDisplayConfigResponse=" + setDisplayConfigStreamRequestMessage.getResult().asXML());		
		
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
		device.login("10.51.48.190","xpi","Barco$123","xpi","managementserver@localhost/managementserver");
		
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
	    	
	    	if("setdisplay".equals(request))
				LayoutTest.testSetDisplayConfig(userJID, deviceNID, log, xmpp, device);

	    }
	    
	    //<login ip='10.51.48.190' user='xpi' password='Barco$123' resource='xpi' managementserver='managementserver@localhost/managementserver'/>

		//MediaTest.testStartStopStream(userJID, deviceNID, log, xmpp, device);
	}
}
