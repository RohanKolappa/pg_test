package com.ipvs.mediaroomservice.impl;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.stream.DestSM;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;


public class DestStreamAgent implements ServiceAgentI {
	
    private XMPPF xf;
    private MRLog log;
    @SuppressWarnings("unused")
    private MRDB db;
    private String userJID;
            
    /* List of Transitions */
    public static final String MANAGED = "managed";
    public static final String UNMANAGED = "unmanaged";
    public static final String INTERNAL = "internal";

    public static final String SETUPSTREAMREQUEST = "SetupStreamRequest";
    public static final String SETUPSTREAMERROR = "SetupStreamError";
    public static final String TEARDOWNSTREAMREQUEST = "TeardownStreamRequest";
    public static final String STARTSTREAMRESPONSE = "StartStreamResponse";
    public static final String STARTSTREAMERROR = "StartStreamError";
    public static final String STOPSTREAMRESPONSE = "StopStreamResponse";
    public static final String STOPSTREAMERROR = "StopStreamError";
    public static final String SOURCEACTIONRESPONSE = "SourceActionResponse";
    public static final String SOURCEACTIONERROR = "SourceActionError";
    public static final String SOURCEAVAILABLE = "SourceAvailable";
    public static final String SOURCEINFOEVENT = "SourceInfoEvent";
    public static final String RELAYINFOEVENT = "RelayInfoEvent";
    public static final String STREAMSTARTEDEVENT = "StreamStartedEvent";
    public static final String RELAYSOURCESTARTEDEVENT = "RelaySourceStartedEvent";
    public static final String ENDOFMEDIAREACHEDEVENT = "EndOfMediaReachedEvent";
    public static final String SOURCEDISCONNECTEDEVENT = "SourceDisconnectedEvent";
    public static final String DESTDISCONNECTEDEVENT = "DestDisconnectedEvent";
    public static final String SOURCECONNECTEDEVENT = "SourceConnectedEvent";
    public static final String SOURCEERROREVENT = "SourceErrorEvent";
    public static final String DESTINFOEVENT = "DestInfoEvent";
    public static final String DESTERROREVENT = "DestErrorEvent";
    public static final String SOURCEUNAVAILABLE = "SourceUnavailable";
    public static final String ROOMUNAVAILABLE = "RoomUnavailable";
    public static final String DESTSTOPPEDEVENT ="DestStoppedEvent";
    
    private StreamNotifier streamNotifier = null;
    

    public static final String STREAMSTATUSEVENT = "StreamStatusEvent";
    DestSM destSM;
    
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDoaminResponseDoc) throws Exception  {
        this.xf = xf;
        this.log = log;
        this.db = db;
        this.userJID = userJID;
        this.destSM = new DestSM(xf, log, userJID);
    	
        streamNotifier  = new StreamNotifier(db,log, xf, userJID);
    	Thread streamNotifierThread = new Thread(streamNotifier);
    	streamNotifierThread.start();
    
    	
    	ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), MediaRoomServiceConst.DESTSTREAMAGENT , MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);

    }
        
    public Document getMediaSourceDataDoc(String streamNID) throws Exception {
    	return this.destSM.getMediaSourceDataDoc(streamNID);
    }
    
    public Document getStreamDataDoc(String streamNID) throws Exception {
    	return this.destSM.getStreamDataDoc(streamNID);
    }
    
    public Document getDestTableDoc() {
    	return this.destSM.getDestTableDoc();
    }


    //handle media store response messages. If any failure occurs on request then generate the dest event from media store response.
    private void handleMediaStoreServiceMessage(MRClient client,String fromJID, Document doc) throws MRException,Exception {
    	 String messagetype = MRXMLUtils.getAttributeValueXpath(doc, "//AgentMessage","type", "");

         if (ServiceClient.RESPONSE.equals(messagetype)) {       
	    	 StreamUtils streamUtils = StreamUtils.getStreamUtils(this.userJID);
	   	     streamUtils.handleDestinationAgentMediaStoreResponses(client, doc);
         }
   	}

	
	public String handleMessage(MRClient client, String fromJID, String message) throws Exception {
        // com.ipvs.common.XMPPTransaction.DEBUG = true;
        com.ipvs.common.XMPPTransaction.startBlock("DestStreamAgent.handleMessage");
		String retval = this.handleMessageInternal(client, fromJID, message);
        com.ipvs.common.XMPPTransaction.endBlock("DestStreamAgent.handleMessage");
        // com.ipvs.common.XMPPTransaction.DEBUG = false;
		return retval;
	}
	
	
	private String handleMessageInternal(MRClient client, String fromJID, String message)
        throws Exception {

        Document doc = MRXMLUtils.stringToDocument(message);
        String transition = null;
        int responseStatus = MRLog.OK;
        String streamNID = null;

        // Check for new request format
        Element request = MRXMLUtils.getElementXpath(doc, "//Request");
        if(request != null) {
            Element setupRequestData = MRXMLUtils.getElementXpath(doc, "//SetupRequestData");
            Element teardownRequestData = MRXMLUtils.getElementXpath(doc, "//TeardownRequestData");        	
	        if (setupRequestData != null) {
	        	transition = this.destSM.handleSetupStream(client, doc);
	        }
	        if (teardownRequestData != null) {
	        	transition = DestStreamAgent.TEARDOWNSTREAMREQUEST;
	        }
        }

        // Check for the new response format
        Element response = MRXMLUtils.getElementXpath(doc, "//Response");
        if(response != null) {
        	
			Element streamStatus = MRXMLUtils.getElementXpath(doc, "//StreamStatus");
        	streamNID = streamStatus.attributeValue("streamNID");
            try {
            	// int state = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//Response/header/state", ""));
            	int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(doc, "//Header", "state", String.valueOf(MRRequest.STATE_EXCEPTION)));
            	if(state == MRRequest.STATE_EXCEPTION)
            		responseStatus = MRLog.ERROR;
            }
            catch(Exception e) {
            	responseStatus = MRLog.ERROR;
            }

            Element startResponseData = MRXMLUtils.getElementXpath(doc, "//StartResponseData");
			Element stopResponseData = MRXMLUtils.getElementXpath(doc, "//StopResponseData");
			Element timelineUpdateResponseData = MRXMLUtils.getElementXpath(doc, "//TimelineUpdateResponseData");
	        // if (MediaRoomServiceConst.START.equals(action) ) {
	        if (startResponseData != null) {
	        	transition = DestStreamAgent.STARTSTREAMRESPONSE;
	            if(responseStatus == MRRequest.STATE_EXCEPTION) 
	            	transition = DestStreamAgent.STARTSTREAMERROR;
	        }
	        if (stopResponseData != null) {
	        	transition = DestStreamAgent.STOPSTREAMRESPONSE;
	            if(responseStatus == MRRequest.STATE_EXCEPTION) 
	            	transition = DestStreamAgent.STOPSTREAMERROR;
	        }
	        if (timelineUpdateResponseData != null) {
		    	transition = DestStreamAgent.SOURCEACTIONRESPONSE;
	            if(responseStatus == MRRequest.STATE_EXCEPTION) 
		        	transition = DestStreamAgent.SOURCEACTIONERROR;
	        }
        }
        
        // MediaStore Request
        String serviceName = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, "");
        if(ServiceClient.MEDIASTORESERVICE.equals(serviceName)) {
        	this.handleMediaStoreServiceMessage(client, fromJID, doc);    	
        	return message;
        }
        
        // Element data = ServiceAgentUtils.cloneDataElement(doc);
        // String action = data.elementText("action");
        String agentMessageType = MRXMLUtils.getAttributeValueXpath(doc, "//AgentMessage","type", "");
        String actionXpath = "//action";
        String statusXpath = "//status";
        String action = MRXMLUtils.getValueXpath(doc, actionXpath, "");
        if(ServiceClient.EVENT.equals(agentMessageType)) {
        	action = MRXMLUtils.getAttributeValueXpath(doc,"//Event/Header", "eventName", "");        	
        	statusXpath = "//Header/@eventLevel";
        } else {
        	action = MRXMLUtils.getValueXpath(doc, actionXpath, "");
        	statusXpath = "//status";
        }

        
        // SSEvent
        if(action.equals(MediaRoomServiceConst.SSEVENT)){            
            	String actiondata = MRXMLUtils.getValueXpath(doc, "//actiondata", "");          
            	this.handleSSEvents(client, doc, action, actiondata);        	
        	return message;
        }
        
        if(streamNID == null)
        	streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
        
        
        log.addInfo("DestStreamAgent:=" + this.userJID + ":handleMessage streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);

        /*
        if (MediaRoomServiceConst.WATCH.equals(action) ) {
            transition = this.destSM.handleSetupStream(client, doc);
        }
        */

        if  (MediaRoomServiceConst.RELAYEVENT.equals(action)) {
        	transition = DestStreamAgent.RELAYINFOEVENT;
        	String mediaSourceNID = MRXMLUtils.getValueXpath(doc, "//mediaSourceNID", "");
        	List<?> instanceElementList = MRXMLUtils.getListXpath(this.destSM.getDestTableDoc(), 
        			"//Instance[@mediaSourceNID='" + mediaSourceNID + "']");
        	for(Object o:instanceElementList) {
        		Element instanceElement = (Element)o;
        		this.destSM.handleTransition(client, instanceElement, transition, doc);
        	}
            
            return message;
        }


        Element instanceElement = MRXMLUtils.getElementXpath(this.destSM.getDestTableDoc(), "//Instance[@streamNID='" + streamNID + "']");        	

        // Check is this is a response, if so then get the resultStatus
        // TBD This should be a TimelineUpdateResponse
        if (	MediaRoomServiceConst.SOURCEEVENT.equals(action) ||
        		MediaRoomServiceConst.DESTINATIONEVENT.equals(action) ||
        		MediaRoomServiceConst.START.equals(action) || 
        		MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(action) || 
        		MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(action) ||
                MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(action) || 
                MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(action)) {
	        try {
	            responseStatus = Integer.parseInt(DocumentHelper.createXPath(statusXpath).selectSingleNode(doc).getText());
	        } catch (Exception e) {
	        	e.printStackTrace();
	            responseStatus = MRLog.ERROR;
	        }
        }

        if  (MediaRoomServiceConst.SOURCEEVENT.equals(action)) {
        	transition = DestStreamAgent.SOURCEINFOEVENT;
        	int eventCode = MediaRoomServiceConst.UNKNOWN_EVENT; 
        	try {
        		eventCode = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//eventCode", ""));
        	}
        	catch(Exception e) {}
            if(responseStatus != MRLog.OK){ 
            	transition = DestStreamAgent.SOURCEERROREVENT;
            	if(eventCode == MediaRoomServiceConst.END_OF_MEDIA_REACHED)
	        		transition = DestStreamAgent.ENDOFMEDIAREACHEDEVENT;
            }
            else {
	        	
	        	if(eventCode == MediaRoomServiceConst.STREAM_START_TC)
	        		transition = DestStreamAgent.STREAMSTARTEDEVENT;
	        	if(eventCode == MediaRoomServiceConst.RECORDING_STARTED)
	        		transition = DestStreamAgent.STREAMSTARTEDEVENT;
	        	if(eventCode == MediaRoomServiceConst.RELAY_SOURCE_STARTED)
	        		transition = DestStreamAgent.RELAYSOURCESTARTEDEVENT;
	        	
	        	if(eventCode == MediaRoomServiceConst.SOURCE_DISCONNECTED)
	        		transition = DestStreamAgent.SOURCEDISCONNECTEDEVENT;
	        	if(eventCode == MediaRoomServiceConst.DEST_DISCONNECTED)
	        		transition = DestStreamAgent.DESTDISCONNECTEDEVENT;

	        	if(eventCode == MediaRoomServiceConst.SOURCE_CONNECTED)
	        		transition = DestStreamAgent.SOURCECONNECTEDEVENT;
            }
        }
        
        if  (MediaRoomServiceConst.DESTINATIONEVENT.equals(action)) {
        	transition = DestStreamAgent.DESTINFOEVENT;
            if(responseStatus != MRLog.OK) 
            	transition = DestStreamAgent.DESTERROREVENT;
        }

        /*
        if (MediaRoomServiceConst.UNWATCH.equals(action) ) {
        	transition = DestStreamAgent.TEARDOWNSTREAMREQUEST;
        }
        */
        
        if (instanceElement != null)
            this.destSM.handleTransition(client, instanceElement, transition, doc);
        // else
        //	System.out.println("DestStreamAgent:handleTransition not found streamNID=" + streamNID);
        
        return message;
    }
    


    public boolean handleMediaPresence(MRClient client, Document doc, String presenceType, String value, String roomUserJID, boolean available, String roomNID)
        throws Exception {
        if("MediaRoom".equals(presenceType)) {
        	if("Unavailable".equals(value)) {
	            List<?> instanceList = MRXMLUtils.getListXpath(this.destSM.getDestTableDoc(), "//Instance[@roomNID='" + roomNID + "']");
	            Iterator<?> i = instanceList.iterator();
	            while (i.hasNext()) {
	                Element instanceElement = (Element) i.next();
	                try {
	                	String transition = DestStreamAgent.ROOMUNAVAILABLE;
	                	this.destSM.handleTransition(client, instanceElement, transition, doc);
	                } catch (Exception e) {
	                    e.printStackTrace();
	                }
	            }
        	}
            return true;
        } else {
            String roomUser = ServiceAgentUtils.getUserFromJID(roomUserJID);
            List<?> instanceList = MRXMLUtils.getListXpath(this.destSM.getDestTableDoc(), 
            		"//Instance[@sourceUser='" + roomUser + "']");
    		// "//Instance[@sourceUser='" + roomUser + "' and @roomNID='" + roomNID + "']");
            Iterator<?> i = instanceList.iterator();
            while (i.hasNext()) {
                Element instanceElement = (Element) i.next();
                try {
                	String transition = DestStreamAgent.SOURCEAVAILABLE;
                	if(!available)
                		transition = DestStreamAgent.SOURCEUNAVAILABLE;
                	this.destSM.handleTransition(client, instanceElement, transition, doc);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            return true;
        }
    }

    public boolean handleEvent(MRClient client, Document doc, Element eventElement)
        throws Exception {              
        return false;
    }
    
    
    public void handleSSEvents(MRClient client, Document doc, String action,  String actiondata) throws MRException,Exception {
	Document actionDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(doc, "//SSEventActionData"));

	String ssInstanceID = MRXMLUtils.getValueXpath(actionDoc, "//Id", "");

	if (ssInstanceID.isEmpty()) {
	    return;
	}

	String event = MRXMLUtils.getValueXpath(actionDoc, "//EventType", "");

	if (event.isEmpty()) {
	    return;
	}

	String type = MRXMLUtils.getValueXpath(actionDoc, "//Type", "");

	// TBD : In future we may going to be implement for all the instance types 
	if (type.isEmpty()) {
	    return;
	}  

	if (type.equals(MediaRoomServiceConst.V2D_HARDWARE_DECODER_PORT)) {

	    //TBD: for receiving state
	    /* if (event.equals(MediaRoomServiceConst.SSINSTANCE_STARTED)) {
                 handleStreamingServerInputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.SOURCE_DISCONNECTED, "", ssInstanceID,
             		MediaRoomServiceExceptionCodes.RELAY_DEVICE_DISCONNECTED, "Source Disconnected");
             }*/

	    if (event.equals(MediaRoomServiceConst.SSINSTANCE_STOPPED)) { 
		String streamXPath = "//Stream[Output/@ssInstanceID='" + ssInstanceID + "']";
		Element streamEL = MRXMLUtils.getElementXpath(StreamUtils.getStreamUtils(this.userJID).getSourceTableDoc(), streamXPath);
		if(streamEL == null) {
		    return;
		}
		String instanceXpath = "//Instance[@streamNID='"+MRXMLUtils.getAttributeValue(streamEL, "NID","")+"']";
		
		Element instanceElement = MRXMLUtils.getElementXpath(this.destSM.getDestTableDoc(), instanceXpath);
	
		if (instanceElement == null) {
		    return;
		}
		this.destSM.handleTransition(client, instanceElement, DestStreamAgent.DESTSTOPPEDEVENT, actionDoc);
		return;                        
	    }
	}

    }

	@Override
	public void reset() throws Exception {
		this.streamNotifier.setReset(true);
		this.streamNotifier = null;
	}

	/**
	 * @return the streamNotifier
	 */
	public StreamNotifier getStreamNotifier() {
		return streamNotifier;
	}

	public Element getStreamInfoListElement() throws Exception {
		Element streamInfoList = MRXMLUtils.stringToElement("<StreamInfoList/>");
		List<?> instanceList = MRXMLUtils.getListXpath(this.destSM.getDestTableDoc(), "//Instance");
		for(Object o : instanceList) {
			Element instanceElement = (Element)o;
			Document streamDataDoc = this.destSM.getStreamDataDoc(instanceElement);
			Document mediaSourceDataDoc = this.destSM.getMediaSourceDataDoc(instanceElement);
			
			Element streamInfoDoc = MRXMLUtils.stringToElement("<StreamInfo />");
			String key = instanceElement.attributeValue("streamNID");
			MRXMLUtils.addAttribute(streamInfoDoc, "streamNID", key);
			Element streamDataDocInInfo = MRXMLUtils.stringToElement("<StreamDataDoc />");
			Element mediaSourceDataDocInInfo = MRXMLUtils.stringToElement("<MediaSourceDataDoc />");
			Element status = (Element)MRXMLUtils.getElementXpath(instanceElement, ".//Status").clone();
			status.setName("StreamStatus");
			
			streamDataDocInInfo.add((Element)streamDataDoc.getRootElement().clone());
			streamDataDocInInfo.add(status);
			mediaSourceDataDocInInfo.add((Element)mediaSourceDataDoc.getRootElement().clone());
			streamInfoDoc.add(streamDataDocInInfo);
			streamInfoDoc.add(mediaSourceDataDocInInfo);
			streamInfoList.add(streamInfoDoc);			
		}		
		//MIYE TBD Remove after testing
		//String xml = streamInfoList.asXML();
		Element mediaServiceInfoDoc = MRXMLUtils.stringToElement("<"+MediaRoomServiceConst.LAYOUT_SERVICE_INFO+"/>");
		mediaServiceInfoDoc.add(streamInfoList);
		return mediaServiceInfoDoc;
	}

}
