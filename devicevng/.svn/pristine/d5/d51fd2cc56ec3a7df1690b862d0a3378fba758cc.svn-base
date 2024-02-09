package com.barco.device.vng.Media;

import java.io.File;
import java.util.Hashtable;
import java.util.List;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.common.StreamEntry;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.XMPPTransaction;

abstract public class RelayMediaAgentImpl extends BaseMediaAgentImpl implements ResponseHandlerI {
	
	SAPListener sapListener;
	ZCListener zcListener;
	Thread sapListenerThread;
	String SAPDirectory = "/data/demos/sap";
	String ZCDirectory = "/data/demos/zc";
	
	public void initAgent(DeviceFrameworkI deviceFrameWork) {
		super.initAgent(deviceFrameWork);
		this.startSAPListener(SAPDirectory);
		this.startZCListener(ZCDirectory);
	}
	
	boolean isEnabled(String dir) {
		try {
			File file = new File(dir);
        	return file.isDirectory();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		return false;
	}
	
	/*
	 * Checks to see if the demo/sap directory exists and based on that starts the SAPListener
	 */
	void startSAPListener(String dir) {
    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startSAPListener checking dir=" + dir); 
		int port = 9875;
		String bcastIP = "224.2.127.254";
		long expiry = 3600 * 1000;
		if(!isEnabled(dir))
			return;
		try {
			sapListener = new SAPListener(bcastIP, port, expiry, dir);
			sapListenerThread = new Thread(sapListener);
			sapListenerThread.start();
	    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startSAPListener started with dir=" + dir); 
		}
		catch(Exception e) {
	    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startSAPListener failed to start exception=" + e.toString()); 			
		}
	}
	
	void startZCListener(String dir) {
    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startZCListener checking dir=" + dir); 
		if(!isEnabled(dir))
			return;
    	try {
    		zcListener = new ZCListener(dir);
	    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startZCListener started with dir=" + dir); 
		}
		catch(Exception e) {
	    	this.deviceFrameWork.getLogger().info("RelayMediaAgentImpl:startZCListener failed to start exception=" + e.toString()); 			
		}
	}

	
	public void startPlaying(DeviceFrameworkI deviceFramework, String streamNID, String destPortElementID,	String streamURL)
	throws Exception {
		// TBD : Instead of this pass in the streamDataDoc
		streamNID = streamNID.substring("Layout.".length());
		StreamEntry entry = deviceFramework.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		//String s = entry.streamDataDoc.asXML();
		try {
			String roomId = XMLUtils.getElementXpath(entry.streamDataDoc, "//context").attributeValue("roomId");
		    Element inputElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Input[@NID='" + roomId + "']");
		    if(inputElement == null) { // This must be a recording stream in which case the input setup includes the output
		    	return;
		    }
			XMLUtils.addAttribute(inputElement,"streamURL", streamURL);
			String roomNID = XMLUtils.getValueXpath(entry.streamDataDoc, "//roomNID", "");
			XMLUtils.addAttribute(inputElement,"roomNID", roomNID);
			XMLUtils.addAttribute(inputElement,"streamNID", streamNID);
			XMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_OK));

			String streamType = XMLUtils.getValueXpath(entry.streamDataDoc, "//streamType", "");
			Document streamDataDoc = entry.streamDataDoc;
			if(MediaRoomServiceConst.V2D.equals(streamType)) { // We need to rebuild the streamDataDoc
				Hashtable<String, String> table = this.parseV2DStreamURL(streamURL);
				if(table == null)
					throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "Cannot play URL=" + streamURL);
				streamDataDoc = XMLUtils.stringToDocument("<data/>");
				addElement(streamDataDoc, "mediaDestNID", table.get("authdata"));
				streamDataDoc.getRootElement().add(XMLUtils.stringToElement(table.get("profileXML")));
				addElement(streamDataDoc, "destPortElementID", destPortElementID);
				String destType = XMLUtils.getValueXpath(entry.streamDataDoc, "//destType", "");
				addElement(streamDataDoc, "destType", destType);
				addElement(streamDataDoc, "streamType", MediaRoomServiceConst.V2D);
			}

			this.startInput(streamDataDoc, inputElement);
		}
		catch(Exception e) {
			e.printStackTrace();
		}


	}


	private String getCreateRelayRequestData(String userJID, String id, String sourceNID, String relayNID, long offset, long startTimecode, 
			String userRoomMediaSourceNID)     throws Exception {
		
	    //If the connection is live then change the start time code to start timecode + offset.
	    if(startTimecode > 0) {
	         startTimecode += offset;
	         offset=0;
	    }

	    String data =
	    "<CreateRelayConnectionRequestData>" +
   	    "<id>" + id + "</id>" +
	    "<name>" + userJID + "</name>" +
	    "<sourceNID>" + sourceNID + "</sourceNID>" +
	    "<destNID>" + relayNID + "</destNID>" +
	    "<profileNID/>" +
	    "<profileXML/>" +
	    "<relayNID>" + MediaRoomServiceConst.COMPUTERELAYUSINGPOLICY + "</relayNID>" +
	    "<group/>" +
	    "<description>" + MediaRoomServiceConst.RELAY_ROOM + "</description>" +
	    "<startOffset>" + offset + "</startOffset>" +
	    "<startTimecode>" + startTimecode + "</startTimecode>" +
	    "<userRoomMediaSourceNID>" + userRoomMediaSourceNID + "</userRoomMediaSourceNID>" +
	    "</CreateRelayConnectionRequestData>";

	    /*	
	    String profileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//profileNID","");
	    MRXMLUtils.getElementXpath(requestDoc, "//profileNID").setText(profileNID);
	    
	    Element profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");
	    MRXMLUtils.copyContent(requestDoc, "//profileXML" , profileXML);
	    */

	    return data;

	}

	public void setupExternalInput(String inputNID, Element inputElement, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc,
			Document sourceTableDoc) throws Exception {
	    
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        XMPPTransaction.startBlock("SourceTable.createRelay");
			String requestNID = "";
			String clientdata = inputNID;
			String clientcallback = this.getClass().getName();
		    String mediaSourceNID = XMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
	        String userRoomMediaSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID", "");
	        if("".equals(userRoomMediaSourceNID))
	        	userRoomMediaSourceNID = mediaSourceNID;
		    String relayNID = XMLUtils.getValueXpath(streamDataDoc, "//relayNID","");
			String data = this.getCreateRelayRequestData(this.deviceFrameWork.getMyJID(), inputNID, sourceNID, relayNID, offset, startTimecode, userRoomMediaSourceNID);
			Document requestDocument = XMLUtils.createRequestDocument("mediaroomservice", "CreateRelayConnectionRequest", this.deviceFrameWork.getMyJID(),
					requestNID, clientdata, clientcallback, data);
			Message createRelayRequestMessage = Message.createRequest(requestDocument, this.deviceFrameWork.getAppserverJID(), this.deviceFrameWork.getMyJID());
			this.deviceFrameWork.sendMessage(createRelayRequestMessage);
	        // createRelayRoom(sourceNID, offset, startTimecode, syncTC, streamDataDoc, inputNID, userRoomMediaSourceNID);
	        XMPPTransaction.endBlock("SourceTable.createRelay");
	        XMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE));
	    }
	}

	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		String inputNID = response.getClientData();
		if("".equals(inputNID))
			return; // Nothing to be done just a response to the delete request
		DMSMediaAgentImpl mediaAgentImpl = (DMSMediaAgentImpl)device.getMediaAgentImpl();
	    Element inputElement = XMLUtils.getElementXpath(mediaAgentImpl.sourceTable.getDoc(), "//Input[@NID='" + inputNID + "']");
	    
	    Document responseDoc = response.getMessageDoc();
		String roomNID = XMLUtils.getValueXpath(responseDoc, "//roomNID", "");
		
	    //send delete media room request to SC if we don't find input element
	    if(inputElement == null) {
	    	sendDeleteMediaRoomRequest(roomNID,device);
	    	return;
	    }
	    
	    // String s = responseDoc.asXML();
	    String streamNID = XMLUtils.getValueXpath(responseDoc, "//streamNID", "");
		XMLUtils.addAttribute(inputElement,"roomNID", roomNID);
		XMLUtils.addAttribute(inputElement,"streamNID", streamNID);
        XMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_OK));
        
        // Find all the connected streams and send the RelayEvent
        List<?> streamList = XMLUtils.selectNodesXpath(inputElement.getParent(), ".//Stream");
        for(Object o : streamList) {
        	Element stream = (Element)o;
	        String destAgentJID = stream.attributeValue("destAgentJID");
	        String destStreamNID = stream.attributeValue("NID");
	        String mediaSourceNID = stream.getParent().attributeValue("NID");
			String data = "<RelayEventData>" +
					"<streamNID>" + destStreamNID + "</streamNID>" +
					"<mediaSourceNID>" + mediaSourceNID + "</mediaSourceNID>" + // adding MediaSourceNID for backward compatibility with mediaroom.jar
					"<RelayData roomNID='" + roomNID + "' streamNID='" + streamNID + "'/>" +
						"</RelayEventData>";
			Element eventElement = XMLUtils.stringToElement(data);
			Message m = Message.createEvent(eventElement, destAgentJID, device.getMyJID(), "Layout", "RelayEvent", 
					String.valueOf(MediaAgentI.EVENT_LEVEL_OK), Level.INFO);
			device.sendNotification(streamNID, m);
        }

	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		DMSMediaAgentImpl mediaAgentImpl = (DMSMediaAgentImpl)device.getMediaAgentImpl();		 
		String inputNID = response.getClientData();
		Element inputElement = XMLUtils.getElementXpath(mediaAgentImpl.sourceTable.getDoc(), "//Input[@NID='" + inputNID + "']");
		if(inputElement == null) {
			return;
		}

		XMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_RELAY_ERROR));

		Element exceptionEL = XMLUtils.getElementXpath(response.getMessageDoc(), "//Legacy");
		exceptionEL.setName("Exception");
		String actionData = XMLUtils.elementToString(exceptionEL);
		int eventLevel = 0;
		String sourceEvent =  "<SourceEventData>" +
				"<eventCode>" + String.valueOf(MediaRoomServiceConst.RELAY_SOURCE_STARTED) + "</eventCode>" +
				"<streamNID></streamNID>" +
				"<status>" + String.valueOf(eventLevel) + "</status>" +
				"<startTC>" + String.valueOf(0) + "</startTC>" +  
				"<streamNIDList/>" + 
				"<sourceEventActionData>" + actionData + "</sourceEventActionData>" +
				"</SourceEventData>";
		System.out.println(response.getMessageDoc().asXML());
		Document sourceEventDoc= XMLUtils.stringToDocument(sourceEvent);

		Element instanceEL = inputElement.getParent();
		List<?> streamsList = XMLUtils.selectNodesXpath(instanceEL, "MediaSource/Stream");
		for(Object object: streamsList) {
			Element streamElement = (Element)object;
			XMLUtils.setValueXpath(sourceEventDoc, "//streamNID", XMLUtils.getAttributeValue(streamElement, "NID")); 
			Message message = Message.createEvent(sourceEventDoc.getRootElement(),  XMLUtils.getAttributeValue(streamElement, "destAgentJID"), device.getMyJID(),"Layout",
					"SourceEvent", String.valueOf(eventLevel), Level.ERROR);
			device.sendMessage(message);
		}
	}

	private String getDeleteRelayRequestData(String roomNID) throws Exception {
		String data =   
			"<DeleteMediaRoomRequestData>" +
			"<roomNID>" + roomNID + "</roomNID>" +
			"</DeleteMediaRoomRequestData>";
		return data;
	}
	
	public void teardownExternalInput(Element inputElement) throws Exception {
		 String inputType = inputElement.attributeValue("inputType");
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
			String roomNID =XMLUtils.getAttributeValue(inputElement,"roomNID");
	        sendDeleteMediaRoomRequest(roomNID,this.deviceFrameWork);
	    }
	}
	
	
	private void sendDeleteMediaRoomRequest(String roomNID,DeviceFrameworkI device) throws Exception  {
		device.getLogger().info(device.getMyJID() + ":SourceTable:deleteInput Started shutdown of Relay roomNID=" + roomNID);
		String requestNID = "";
		String clientdata = "";
		String clientcallback = this.getClass().getName();    
		String data = this.getDeleteRelayRequestData(roomNID);
		Document requestDocument = XMLUtils.createRequestDocument("mediaroomservice", "DeleteMediaRoomRequest", device.getMyJID(),
				requestNID, clientdata, clientcallback, data);
		Message createRelayRequestMessage = Message.createRequest(requestDocument, device.getAppserverJID(), device.getMyJID());
		device.sendMessage(createRelayRequestMessage);
	}
	
	protected Document generateStreamStatusDoc(Element streamElement, int state) throws Exception {
		Document streamStatusDoc = super.generateStreamStatusDoc(streamElement, state);
		Element relayData = streamStatusDoc.getRootElement().element("relayData");
		if(streamElement.getParent() ==null || streamElement.getParent().getParent() == null) {
			return streamStatusDoc;
		}
		
		Element inputElement =streamElement.getParent().getParent().element("Input");
		if(inputElement==null) {
			return streamStatusDoc;
		}
		
		String relayRoomNID = inputElement.attributeValue("roomNID");
		String relayStreamNID = inputElement.attributeValue("streamNID");
		relayData.addAttribute("relayRoomNID", relayRoomNID);
		relayData.addAttribute("relayStreamNID", relayStreamNID);
		return streamStatusDoc;
	}

	
}
