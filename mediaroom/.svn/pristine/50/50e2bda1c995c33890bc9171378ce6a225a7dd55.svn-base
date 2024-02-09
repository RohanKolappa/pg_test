package com.ipvs.mediaroomservice.stream;

import java.net.URLDecoder;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.AddStreamRequestHandler;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.notification.BaseNotifier;
import com.ipvs.utils.Utils;

public class DestSM {
	
	private XMPPF xf;
    private MRLog log;
    private String userJID;
    Document destTableDoc;
    Document streamDocTemplate = null;


    public DestSM(XMPPF xf, MRLog log, String userJID) {
        this.xf = xf;
        this.log = log;
        this.userJID = userJID;
        try {
            this.destTableDoc = MRXMLUtils.stringToDocument("<DestTable userJID='" + this.userJID + "'/>");
            this.streamDocTemplate = RoomDB.getStreamInstanceXML(log);
        } catch (Exception e) {
        	e.printStackTrace();
        }
    }
    
    private StreamUtils getStreamUtils() throws Exception {
    	return StreamUtils.getStreamUtils(this.userJID);
    }


    public int getStreamState(Element instanceElement) {
        return Integer.parseInt(MRXMLUtils.getValue(MRXMLUtils.getChild(instanceElement,"Status"),"state",""));
    }

    public Document getMediaSourceDataDoc(String streamNID) throws Exception {
    	Element instanceElement = MRXMLUtils.getElementXpath(destTableDoc, "//Instance[@streamNID='" + streamNID + "']");
    	return this.getMediaSourceDataDoc(instanceElement);
    }
    
    public Document getStreamDataDoc(String streamNID) throws Exception {
    	Element instanceElement = MRXMLUtils.getElementXpath(destTableDoc, "//Instance[@streamNID='" + streamNID + "']");
    	return this.getStreamDataDoc(instanceElement);
    }

    public Document getMediaSourceDataDoc(Element instanceElement) throws Exception {
    	if(instanceElement == null)
    		return null;
    	Element mediaSourceData = instanceElement.element("MediaSourceData").element("data");
    	return MRXMLUtils.elementToDocument((Element)mediaSourceData.clone());
    }
    
    public Document getDestTableDoc() {
    	return this.destTableDoc;
    }

    @SuppressWarnings("unchecked")
    private void printDestTable() throws Exception {
    	Document tmp = (Document)this.destTableDoc.clone();
    	List<?> list = MRXMLUtils.getListXpath(tmp, "//Instance");
    	for(Iterator<?> i = list.iterator(); i.hasNext();) {
    		Element instanceElement = (Element)i.next();
    		//instanceElement.removeChildren("MediaSourceData");
            //instanceElement.removeChildren("StreamData");
            List<Element> childlist = (List<Element>)instanceElement.elements("MediaSourceData");
            for (Element object : childlist) {
                object.detach();
            }
            childlist = (List<Element>)instanceElement.elements("StreamData");
            for (Element object : childlist) {
                object.detach();
            }

    	}	
    	MediaRoomServiceUtils.printlog(MRXMLUtils.documentToString(tmp));
    }

    private void sendStreamStatusEvent(Element instanceElement) throws Exception {
    	int state = this.getStreamState(instanceElement);
        String roomNID = MRXMLUtils.getAttributeValue(instanceElement,"roomNID");
        String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
        String toAgentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,xf.sf(userJID));
        int eventLevel = MRLog.OK;
        if( (state == MediaRoomServiceConst.START_ERROR) ||
        	(state == MediaRoomServiceConst.SOURCE_ERROR) ) {
        	eventLevel = MRLog.ERROR;
        }
        long now = System.currentTimeMillis();
        String logStr = "";
        String info = "DestStreamAgent:=" + this.userJID + "/" + streamNID + " > StreamEvent eventname=" + DestStreamAgent.STREAMSTATUSEVENT + 
        					" to " + ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.APPSERVERAGENT) +" state:=" + state;
        Element streamStatusElement = instanceElement.element("Status");
        
       //queue stream status events
        String fromAgentClassName= MediaRoomServiceConst.DESTSTREAMAGENT;
        String agentJID=toAgentJID;
        String messageType = ServiceClient.EVENT;
        String data = ServiceAgentUtils.getAgentEventDataString(xf.xmpp(userJID), log, MRConst.PUBSUB_SERVICE, eventLevel,toAgentJID,  now, DestStreamAgent.STREAMSTATUSEVENT, 
        		streamStatusElement, logStr,info);
        String message = ServiceAgentUtils.getAgentMessage(userJID, fromAgentClassName, roomNID, agentJID, messageType, data, info);
        String notificationId = roomNID + "." + streamNID;
        ServiceAgentFactoryI sf = this.xf.sf(userJID);
        if(state==MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED || state==MediaRoomServiceConst.STREAM_STARTED || eventLevel == MRLog.ERROR) {
        	((DestStreamAgent)( sf.getServiceAgent(DestStreamAgent.class.getName()))).getStreamNotifier().expediteNotification(BaseNotifier.ROOM_NOTIFICATION_TYPE,
        			 AppServerAgentUtils.getAppServerAgentJID(userJID,sf), notificationId, message, null);
        } else {
        	((DestStreamAgent)( sf.getServiceAgent(DestStreamAgent.class.getName()))).getStreamNotifier().queueNotification(BaseNotifier.ROOM_NOTIFICATION_TYPE,
        			 AppServerAgentUtils.getAppServerAgentJID(userJID,sf), notificationId, message, null);
        }
        log.addInfo(info + ":" + agentJID, MRLog.OK, MRLog.NOTIFY);        
        this.printDestTable();
    }

    public void setStreamState(MRRequest request, MRClient client, 
    		Element instanceElement, int state) throws Exception {
        instanceElement.element("Status").element("state").setText(String.valueOf(state));
        try {
        	this.sendStreamStatusEvent(instanceElement);
        }
        catch(Exception e) {
        	String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
            log.addInfo("DestStreamAgent:=" + this.userJID + ":sendStreamStatus Error streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
        	e.printStackTrace();
        }
    }

    public void handleTransition(MRClient client, Element instanceElement, String transition, Document doc) throws Exception {
        String requestNID = "requestNID";
        MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, requestNID, "serviceName", "requestName", new MRMessage(doc), "");

        int state = this.getStreamState(instanceElement);
       
        if (DestStreamAgent.TEARDOWNSTREAMREQUEST.equals(transition)) {
            this.handleTeardownStream(request, client, instanceElement);
            state = MediaRoomServiceConst.STREAM_STOPPED;
        }
        
        if (DestStreamAgent.STARTSTREAMRESPONSE.equals(transition)) {
        	this.handleStartStreamResponse(request, client, instanceElement, doc);
        	String sourceAgentType = MRXMLUtils.getAttributeValue(instanceElement,"sourceAgentType");        	
        	if(DestStreamAgent.UNMANAGED.equals(sourceAgentType)){        	  
        	    state = MediaRoomServiceConst.STREAM_STARTED;
        	}
        	else
        	    state = MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
        }

        if (DestStreamAgent.STARTSTREAMERROR.equals(transition)) {
        	this.handleStartStreamError(request, client, instanceElement, doc);        	
        	state = MediaRoomServiceConst.START_ERROR;
        }

        /*
        if (DestStreamAgent.STOPSTREAMRESPONSE.equals(transition)) {        	
        	this.handleStopStreamResponse(request, client, instanceElement, doc);
        }

        if (DestStreamAgent.STOPSTREAMERROR.equals(transition)) {
        	this.handleStopStreamError(request, client, instanceElement, doc);
        }
        */

        if (DestStreamAgent.SOURCEACTIONRESPONSE.equals(transition)) {
        	this.handleSourceActionResponse(request, client, instanceElement, doc);
        	// state = MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
        }
        
        if(DestStreamAgent.SETUPSTREAMREQUEST.equals(transition)) {
        	this.handleSetupStreamRequest(request, client, instanceElement);
        	state = MediaRoomServiceConst.WAITING_FOR_STREAMURL;
        }

        if(DestStreamAgent.SOURCEAVAILABLE.equals(transition)) {
        	this.handleSourceAvailable(request, client, instanceElement);
        	state = MediaRoomServiceConst.WAITING_FOR_STREAMURL;
        }

        if(DestStreamAgent.SOURCEUNAVAILABLE.equals(transition)) {
        	this.handleSourceUnavailable(request, client, instanceElement);
        	state = MediaRoomServiceConst.WAITING_FOR_SOURCE;
        }
        
        if(DestStreamAgent.ROOMUNAVAILABLE.equals(transition)) {
        	this.handleRoomUnavailable(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.STREAM_STOPPED;
        }
        
        if(DestStreamAgent.SOURCEINFOEVENT.equals(transition)) {
        	this.handleSourceInfoEvent(request, client, instanceElement, doc);
        	// Not setting ths state for this transition
        }

        if(DestStreamAgent.ENDOFMEDIAREACHEDEVENT.equals(transition)) {
        	this.handleSourceInfoEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.END_OF_MEDIA;
        }

        if(DestStreamAgent.SOURCEDISCONNECTEDEVENT.equals(transition)) {
        	this.handleSourceInfoEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
        }

        if(DestStreamAgent.DESTDISCONNECTEDEVENT.equals(transition)) {
        	this.handleSourceInfoEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
        }

        if(DestStreamAgent.SOURCECONNECTEDEVENT.equals(transition)) {
        	this.handleSourceInfoEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.STREAM_STARTED;
        }

        if(DestStreamAgent.RELAYINFOEVENT.equals(transition)) {
        	this.handleRelayInfoEvent(request, client, instanceElement, doc);
        	// Not setting ths state for this transition
        }

        if(DestStreamAgent.STREAMSTARTEDEVENT.equals(transition)) {
        	this.handleStreamStartedEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.STREAM_STARTED;
        }

        if(DestStreamAgent.RELAYSOURCESTARTEDEVENT.equals(transition)) {
        	this.handleRelaySourceStartedEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.STREAM_STARTED;
        }

        if(DestStreamAgent.SOURCEERROREVENT.equals(transition)) {
        	this.handleSourceErrorEvent(request, client, instanceElement, doc);
            state = MediaRoomServiceConst.SOURCE_ERROR;
        }
        
        if(DestStreamAgent.SOURCEACTIONERROR.equals(transition)) {
        	this.handleSourceActionError(request, client, instanceElement, doc);        	
        	state = MediaRoomServiceConst.SOURCE_ERROR;
        }

        if(DestStreamAgent.DESTINFOEVENT.equals(transition)) {
        	this.handleDestInfoEvent(request, client, instanceElement, doc);
        	// Not setting ths state for this transition
        }

        if(DestStreamAgent.DESTERROREVENT.equals(transition)) {
        	this.handleDestErrorEvent(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.START_ERROR;
        }
        
        if(DestStreamAgent.DESTSTOPPEDEVENT.equals(transition)) {
            this.handleDestStopped(request,client,instanceElement,doc);
            state = MediaRoomServiceConst.DEST_ERROR;
        }

        if(DestStreamAgent.SETUPSTREAMERROR.equals(transition)) {
        	this.handleSetUpStreamError(request, client, instanceElement, doc);
        	state = MediaRoomServiceConst.SETUP_ERROR;
        }

        if(state != MediaRoomServiceConst.STREAM_STOPPED){           
            this.setStreamState(request, client, instanceElement, state);
        	
        }
    }

    private void handleDiskSpaceUnavailable(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	int code = MediaStoreServiceExceptionCodes.NO_AVAILABLE_DISK_SPACE; 
    	String description = "No Disk space Available on Device To Record Media";
    	String streamdatadocStr = MRXMLUtils.getValueXpath(doc, "//streamdatadoc", "");
        Document streamDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(streamdatadocStr, "UTF-8"));
        String streamNID = MRXMLUtils.getValueXpath(streamDataDoc, "//streamNID", "");
        Element actionData = MRXMLUtils.stringToElement("<Exception><Code>" + String.valueOf(code) + "</Code><Description>" + description + "</Description></Exception>");
		Document destEventDoc = StreamAgentUtils.createDestinationEventDoc(streamNID, MediaRoomServiceConst.DESTINATIONEVENT, 
				MediaRoomServiceConst.START_ERROR, "", 0, 0, actionData, MRLog.ERROR,log);
		this.setStreamLastEvent(request, client, instanceElement, destEventDoc.getRootElement());
    }

    private void handleNoMorePortsAvailable(MRRequest request, MRClient client, Element instanceElement, String streamNID) throws Exception {
    	int code = MediaRoomServiceExceptionCodes.NOAVAILABLEPORTS; 
    	String description = "There are no available ports";
        Element actionData = MRXMLUtils.stringToElement("<Exception><Code>" + String.valueOf(code) + "</Code><Description>" + description + "</Description></Exception>");
		Document destEventDoc = StreamAgentUtils.createDestinationEventDoc(streamNID, MediaRoomServiceConst.DESTINATIONEVENT, 
				MediaRoomServiceConst.SETUP_ERROR, "", 0, 0, actionData, MRLog.ERROR,log);
		this.setStreamLastEvent(request, client, instanceElement, destEventDoc.getRootElement());
    }

    private void handleBusy(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	int code = MediaRoomServiceExceptionCodes.MEDIADESTBUSY; 
    	String description = "Media Destination Busy";
    	String streamdatadocStr = MRXMLUtils.getValueXpath(doc, "//streamdatadoc", "");
        Document streamDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(streamdatadocStr, "UTF-8"));
        String streamNID = MRXMLUtils.getValueXpath(streamDataDoc, "//streamNID", "");
        Element actionData = MRXMLUtils.stringToElement("<Exception><Code>" + String.valueOf(code) + "</Code><Description>" + description + "</Description></Exception>");
		Document destEventDoc = StreamAgentUtils.createDestinationEventDoc(streamNID, MediaRoomServiceConst.DESTINATIONEVENT, 
				MediaRoomServiceConst.START_ERROR, "", 0, 0, actionData, MRLog.ERROR,log);
		this.setStreamLastEvent(request, client, instanceElement, destEventDoc.getRootElement());
    }

	private void handleStreamEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
    	Element eventData = MRXMLUtils.getElementXpath(doc, "//Data");
    	this.setStreamLastEvent(request, client, instanceElement, eventData);
        log.addInfo("DestStreamAgent@handleStreamEvent" + this.userJID + " streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
        this.propogateToRelaySourceStreamAgent(client, doc, MediaRoomServiceConst.SOURCEEVENT, instanceElement, "");
        // System.out.println("DestStreamAgent:handleStreamEvent doc=" + MRXMLUtils.documentToString(doc));
    }


    private void handleStartStreamResponse(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
        String streamURL = MRXMLUtils.getValueXpath(doc, "//URL","");
        String actionNID = MRXMLUtils.getValueXpath(doc, "//actionNID","");
        com.ipvs.common.XMPPTransaction.startBlock("DestStreamAgent.handleActionResponse");        
        Document streamDataDoc = this.getStreamDataDoc(instanceElement);
        String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
        String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
        String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType", "");
        if(!"".equals(destNID)) { // This is a device and we have to hand over to SS
            StreamUtils streamUtils = StreamUtils.getStreamUtils(userJID);
            if(streamUtils.isRelayInput(streamNID)) {
            	this.propogateToRelaySourceStreamAgent(client, doc, MediaRoomServiceConst.START, instanceElement, streamURL);
            } else if(MediaRoomServiceConst.OUTPUT_DECODER.equals(destType) ) { // Is Rx
            	Document mediaSourceDataDoc = this.getMediaSourceDataDoc(instanceElement);
            	String destStreamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
            	if(!AddStreamRequestHandler.isUnManagedDest(destStreamURL,"")) // Is Managed destination
            		this.getStreamUtils().createRxInstance(client, doc, MediaRoomServiceConst.START, streamNID, streamURL, streamDataDoc, mediaSourceDataDoc, actionNID);
            } else {
            	// Recording/PVR we do not do anything
            	// if (MediaRoomServiceConst.START.equals(action)) {
            	//	MediaRoomServiceUtils.updateDestFileStreamProfile(streamDataDoc);
            	// }
            }
        }
        // System.out.println("*** StreamURL:" + streamURL);
        Element relayData = MRXMLUtils.getElementXpath(doc, "//relayData");
        String relayRoomNID = MRXMLUtils.getAttributeValue(relayData,"relayRoomNID");
        String relayStreamNID = MRXMLUtils.getAttributeValue(relayData,"relayStreamNID");
        Element eventData = MRXMLUtils.stringToElement("<startStreamResponse><streamURL>" + streamURL + "</streamURL></startStreamResponse>");
        this.setStreamLastEvent(request, client, instanceElement, eventData);
        this.setStreamRelayData(request, client, instanceElement, relayRoomNID, relayStreamNID);
    	Element newMediaTimeline = MRXMLUtils.getElementXpath(doc, "//MediaTimeline");
    	this.setMediaTimeline(request, client, instanceElement, newMediaTimeline);
        com.ipvs.common.XMPPTransaction.endBlock("DestStreamAgent.handleActionResponse");
    }

	private void handleSourceInfoEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		this.handleStreamEvent(request, client, instanceElement, doc);
	}

	private void handleStreamStartedEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	Element last = MRXMLUtils.getElementXpath(doc, "//Last");
    	// System.out.println("DestSM:handleStreamStartedEvent updating MediaTimeline=" + MRXMLUtils.elementToString(newMediaTimeline));
    	this.updateMediaTimeline(request, client, instanceElement, last);		
		this.handleStreamEvent(request, client, instanceElement, doc);
	}

	private void handleRelayInfoEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		Element relayData = MRXMLUtils.getElementXpath(doc, "//RelayData");
		String relayRoomNID = relayData.attributeValue("roomNID");
		String relayStreamNID = relayData.attributeValue("streamNID");
        this.setStreamRelayData(request, client, instanceElement, relayRoomNID, relayStreamNID);
	}

	private void handleRelaySourceStartedEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
        this.handleStreamEvent(request, client, instanceElement, doc);
        List<?> list = MRXMLUtils.getListXpath(doc, "//streamNIDListItem");
        int count = list.size();
        if(count == 2) {
        	Element relayStreamNIDElement = (Element)list.get(0);
            this.setStreamRelayData(request, client, instanceElement, 
            		null, MRXMLUtils.getValue(relayStreamNIDElement));
        }
	}
	
	private void handleSourceActionError(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		Element eventData = MRXMLUtils.getElementXpath(doc, "//Data");
    	this.setStreamLastEvent(request, client, instanceElement, eventData);   
	}

	private void handleSourceErrorEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		this.handleStreamEvent(request, client, instanceElement, doc);
	}

	private void handleDestInfoEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		this.handleStreamEvent(request, client, instanceElement, doc);
	}

	private void handleDestErrorEvent(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
		this.handleStreamEvent(request, client, instanceElement, doc);
	}

    private void handleTeardownStream(MRRequest request, MRClient client, Element instanceElement)  throws Exception {
	    String requestNID = "";
	    String updateAction = MediaRoomServiceConst.STOP;
        this.sendStreamActionRequest(client, requestNID, updateAction, instanceElement);
        this.deleteInstance(request, client, instanceElement);
    }
    
    private void handleSetUpStreamError(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	//System.out.println("Placeholder method for handling setup stream error");
    }
    
    private void handleStartStreamError(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	Element eventData = MRXMLUtils.getElementXpath(doc, "//Data");
    	this.setStreamLastEvent(request, client, instanceElement, eventData);        
    	this.propogateToRelaySourceStreamAgent(client, doc, MediaRoomServiceConst.START, instanceElement, "");
    }
    
    private void handleSourceActionResponse(MRRequest request, MRClient client, Element instanceElement, Document doc) throws Exception {
    	Element newMediaTimeline = MRXMLUtils.getElementXpath(doc, "//MediaTimeline");
    	this.setMediaTimeline(request, client, instanceElement, newMediaTimeline);
    	//System.out.println("TBD : DestStreamAgent:handleSourceActionResponse");
    }

	private void handleSourceAvailable(MRRequest request, MRClient client, Element instanceElement)
	    throws Exception {		
		String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
		String requestNID = "";
		String actionData= "";
		String sourceAgentJID = MRXMLUtils.getAttributeValue(instanceElement, "sourceAgentJID", "");			
        client.getLog().addInfo("DestStreamAgent:=" + this.userJID + ":handleSourceAvailable streamNID=" + streamNID +" SourceAgentJID:="+sourceAgentJID, MRLog.OK, MRLog.NOTIFY);
        sendSourcePresenceEvent(sourceAgentJID,streamNID,MediaRoomServiceConst.SOURCE_AVAILABLE, MRRequest.createRequest(this.userJID), client, instanceElement, actionData);
        this.sendStreamActionRequest(client, requestNID, MediaRoomServiceConst.START, instanceElement);
	}
	
	
	private void handleSetupStreamRequest(MRRequest request, MRClient client, Element instanceElement)
    throws Exception {		
		String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
		String requestNID = "";
	    client.getLog().addInfo("DestStreamAgent:=" + this.userJID + ":handleSetupStreamRequest streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
	    this.sendStreamActionRequest(client, requestNID, MediaRoomServiceConst.START, instanceElement);
    }

	private void handleRoomUnavailable(MRRequest request, MRClient client, Element instanceElement, Document doc)
	    throws Exception {
		this.deleteInstance(request, client, instanceElement);
		((DestStreamAgent)( this.xf.sf(userJID).getServiceAgent(DestStreamAgent.class.getName()))).getStreamNotifier().flushNotificationsOnRoomDelete(MRXMLUtils.getAttributeValue(instanceElement, "roomNID",""));
	}

	
	private void handleSourceUnavailable(MRRequest request, MRClient client, Element instanceElement)
    throws Exception {			
		String sourceAgentJID = MRXMLUtils.getAttributeValue(instanceElement, "sourceAgentJID", "");
		String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
		log.addInfo("DestStreamAgent:=" + this.userJID + ":handleSourceUnAvailable sourceJID=" + sourceAgentJID + " streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
		String actionData = "<Exception><Code>"+MediaRoomServiceExceptionCodes.SOURCEUNAVAILABLE+"</Code><Description>Source Unavailable</Description></Exception>";		
		sendSourcePresenceEvent(sourceAgentJID,streamNID,MediaRoomServiceConst.SOURCE_UNAVAILABLE, request, client, instanceElement,actionData);
	}
	
	private void sendSourcePresenceEvent(String sourceAgentJID, String streamNID, int eventCode,MRRequest request, MRClient client, Element instanceElement, String actionData) throws MRException, Exception{
		
		String sourceEventData = "<agentJId>"+sourceAgentJID+"</agentJId>"+actionData; 			
		Document sourceEventXML = StreamAgentUtils.createSourceEventMessage(streamNID, MediaRoomServiceConst.SOURCEEVENT,eventCode, streamNID, 0, sourceEventData,
	            MRLog.OK, "",log);
	    Document streamDataDoc = this.getStreamDataDoc(instanceElement);
	    String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
	    String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    String info = "DestStreamAgent:=" + this.userJID + "/" + streamNID + " > SourceEvent Message action=" + MediaRoomServiceConst.SOURCEEVENT + " to " +
	    ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.DESTSTREAMAGENT);
	    long eventWallclock = this.xf.xmpp(userJID).getCurrentTimeMillis();
	    ServiceAgentUtils.sendAgentEvent(this.xf.xmpp(userJID), this.log, roomNID, "", destAgentJID, MRConst.LAYOUT_SERVICE, MRLog.OK, this.userJID, eventWallclock, MediaRoomServiceConst.SOURCEEVENT,sourceEventXML.getRootElement(), "", info);

	}
    private void deleteInstance(MRRequest request, MRClient client, Element instanceElement)  throws Exception {
    	
	    Document streamDataDoc = this.getStreamDataDoc(instanceElement);
        String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType", "");
        
        if(MediaRoomServiceConst.OUTPUT_DECODER.equals(destType)) {
            String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
        	String destStreamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
        	if(!AddStreamRequestHandler.isUnManagedDest(destStreamURL,"")) // Is Managed destination
        		this.getStreamUtils().deleteRxInstance(streamNID);
        } else if (MediaRoomServiceConst.OUTPUT_RECORD.equals(destType)) {
        	String strState = MRXMLUtils.getValueXpath(instanceElement, "./Status/state", "");
        	int state = Utils.getIntValue(strState, 0);
          	
            
        	if(state == MediaRoomServiceConst.SETUP_ERROR){
        		this.log.addInfo("DestSM: deleteInstance. Recording stream status is Error send UpdatePlayLengthRequest to MediaStoreRequest. State:="+state,MRLog.INFO,MRLog.NOTIFY);
        	 	String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
            	String mediaStoreFileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileUUID", "");
            	String destParentDirNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID", "");
                MediaStoreServiceUtils.sendInternalRequestToMediaStoreAgent(this.userJID, mediaStoreFileNID, MediaStoreServiceConst.UPDATE_MEDIA_PLAYLENGTH_REQUEST, destNID, destParentDirNID, null, userJID, xf.xmpp(userJID), log);      	
        	}
        }
        this.releaseDestIPPort(streamDataDoc);
        Element destTableElement = instanceElement.getParent();
        destTableElement.remove(instanceElement);
        this.printDestTable();
    }

    private void setStreamRelayData(MRRequest request, MRClient client, 
    		Element instanceElement, String relayRoomNID, String relayStreamNID) throws Exception {
        Element relayData = instanceElement.element("Status").element("relayData");
        if(relayRoomNID != null) // may not be available
            MRXMLUtils.addAttribute(relayData,"relayRoomNID", relayRoomNID);
        MRXMLUtils.addAttribute(relayData,"relayStreamNID", relayStreamNID);
    }

    private void setMediaTimeline(MRRequest request, MRClient client, 
    		Element instanceElement, Element newMediaTimeline) throws Exception {
        Element status = instanceElement.element("Status");
        Element mediaTimeline = status.element("MediaTimeline");
        if(mediaTimeline != null) // may not be available
        	status.remove(mediaTimeline);
        status.add((Element)newMediaTimeline.clone());
    }

    private void updateMediaTimeline(MRRequest request, MRClient client, 
    		Element instanceElement, Element last) throws Exception {
        Element status = instanceElement.element("Status");
        Element mediaTimeline = status.element("MediaTimeline");
        if(mediaTimeline == null)
        	return;
        Element prevLast = mediaTimeline.element("Last");
        mediaTimeline.remove(prevLast);
        mediaTimeline.add((Element)last.clone());
    }

    private void propogateToRelaySourceStreamAgent(MRClient client, Document doc, String action, 
    		Element instanceElement, String streamURL)
	    throws Exception {
	    Document streamDataDoc = this.getStreamDataDoc(instanceElement);
	    String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
	    this.getStreamUtils().handleRelayInputStreamResponse(client, doc, action, streamNID, streamURL, streamDataDoc);
	}

    public void setStreamLastEvent(MRRequest request, MRClient client, Element instanceElement,
    		Element eventData)
    	throws Exception {
        Element status = instanceElement.element("Status");
        MRXMLUtils.removeChild(status,"lastEventData");
    	Element lastEventData = (Element)eventData.clone();
    	lastEventData.setName("lastEventData");
    	status.add(lastEventData);
    }

    public Document getStreamDataDoc(Element instanceElement) throws Exception {
    	if(instanceElement == null)
    		return null;
    	Element streamData = instanceElement.element("StreamData").element("data");
    	return MRXMLUtils.elementToDocument((Element)streamData.clone());
    }

    /* List of possible state transitions 
     * Watch - add stream to cache
     * NotifyStart - send out the StartRequest
     * StartResponse - mark as started
     * NotifyStop - send out StopRequest
     * StopResponse - mark as stopped
     * SourceUnavailable - mark as source unavailable
     * SourceAvailable - mark as source availabe and send StartRequest
     * RoomUnavailable - delete stream from cache
     * Unwatch - delete stream from cache
     */
    private boolean sendStreamActionRequest(MRClient client, String requestNID, 
    		String updateAction, Element instanceElement)
        throws Exception {
    	
    	String roomNID = MRXMLUtils.getAttributeValue(instanceElement,"roomNID");
    	String actionNID = roomNID;
    	String streamNID = MRXMLUtils.getAttributeValue(instanceElement,"streamNID");
    	String sourceAgentJID = MRXMLUtils.getAttributeValue(instanceElement,"sourceAgentJID");
    	String sourceAgentType = MRXMLUtils.getAttributeValue(instanceElement,"sourceAgentType");
    	Document streamDataDoc = this.getStreamDataDoc(instanceElement);
    	
        // Parse the actiondoc
        String userAction = updateAction;
        String requestAction = null;

        // Check if this is a valid action for the current state
        if (MediaRoomServiceConst.START.equals(updateAction)) {
            requestAction = MediaRoomServiceConst.STARTREQUEST;
        } else if (MediaRoomServiceConst.STOP.equals(updateAction)) {
            requestAction = MediaRoomServiceConst.STOPREQUEST;
        }
        
        Document mediaSourceDataDoc = this.getMediaSourceDataDoc(instanceElement);

        // Find out if the streamURL is already available in the streamDataDoc
        String streamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURL", "");
        if(DestStreamAgent.UNMANAGED.equals(sourceAgentType)) {
        	// Send a response back to ourselves with the streamURL
           // String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
            String actiondata = "";
            ServiceAgentI sourceAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.SOURCESTREAMAGENT);
            SourceStreamAgent sourceStreamAgent = (SourceStreamAgent)sourceAgent;
            sourceStreamAgent.handleDummyStreamAction(client, requestNID, streamNID, actionNID, userAction, requestAction, actiondata, streamDataDoc, mediaSourceDataDoc, streamURL);
            return true;
        }

        // Set the action data
        Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
        String actionMessage = StreamAgentUtils.createSourceStreamAgentActionRequestMessage(streamNID, userAction, requestAction, 
        		actiondata, 0, actionNID, streamDataDoc, mediaSourceDataDoc, log);
        String info = "DestStreamAgent:=" + this.userJID + "/" + streamNID + " > request action=" + requestAction + " to " +
            ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT);


        if(DestStreamAgent.INTERNAL.equals(sourceAgentType)) {
            this.log.addInfo("DestStreamAgent:=" + this.userJID + "/" + streamNID + " > INTERNAL request action=" + requestAction + " to " +
                    ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT), MRLog.OK, MRLog.NOTIFY);
            ServiceAgentI sourceAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.SOURCESTREAMAGENT);
            SourceStreamAgent sourceStreamAgent = (SourceStreamAgent)sourceAgent;

    	    String clientData = "";
    	    String clientCallback = MediaRoomServiceConst.DESTSTREAMAGENT;
            sourceStreamAgent.handleStreamAction(client, requestNID, streamNID, actionNID, userAction, requestAction, actiondata, streamDataDoc, mediaSourceDataDoc,
            		clientData, clientCallback);

            return true;
        }
        
        
        ServiceAgentUtils.sendAgentRequestMessage(this.xf.xmpp(userJID), this.log, roomNID,DestStreamAgent.class.getName(), sourceAgentJID, 
        		MediaRoomServiceConst.SOURCESTREAMAGENT, requestAction, actionMessage, info, "none","",MRConst.MEDIA_SERVICE,"");

        return true;
    }

    private boolean needsUDPDestPort(String streamType,String destType) {
    	return ((!MediaRoomServiceConst.OUTPUT_PVR.equals(destType) 
    			&& !MediaRoomServiceConst.OUTPUT_RECORD.equals(destType) ) 
    			&& (MediaRoomServiceConst.RTP.equals(streamType) ||
    			MediaRoomServiceConst.MPEGTS.equals(streamType) ||
    			MediaRoomServiceConst.UDP.equals(streamType)));
    }
   

    private void getDestIPPort(Document streamDataDoc) throws Exception { // TBD
        // Set the destIP and destPort
        String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
        String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType", "");
        if(!this.needsUDPDestPort(streamType,destType))
        	return;
        
    	String destStreamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
    	if(AddStreamRequestHandler.isUnManagedDest(destStreamURL,"")) // Is Un Managed destination
    		return;

        String destUDPStartPort = MRXMLUtils.getValueXpath(streamDataDoc, "//destUDPStartPort", "");
        if(!"".equals(destUDPStartPort))
           	return; 
    	destUDPStartPort = this.getStreamUtils().getNextUDPStartPort(streamType);
    	MRXMLUtils.setValueXpath(streamDataDoc, "//destUDPStartPort", destUDPStartPort);
    	
    }

    private void releaseDestIPPort(Document streamDataDoc) throws Exception { // TBD
        // Release the destIP and destPort
        String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
        String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType", "");
        
        if(!this.needsUDPDestPort(streamType,destType))
        	return;
        
    	String destStreamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
    	if(AddStreamRequestHandler.isUnManagedDest(destStreamURL,"")) // Is Un Managed destination
    		return;

     	String destUDPStartPort = MRXMLUtils.getValueXpath(streamDataDoc, "//destUDPStartPort", "");
        
     	this.getStreamUtils().releaseUDPStartPort(destUDPStartPort, streamType);
    }

    public boolean isAvailable(XMPPI xmpp, String roomNID, String userJID, String myJID) throws MRException,Exception {
    	/*
        // TBD : Temp fix if myJID is not owner xmpp.isAvailable fails
        XMPPTransaction.startBlock("isAvailable");
    	boolean result = false;
        try {
        	if(myJID.equals(userJID))
        		result = true;
        	String nickname = ServiceAgentUtils.getRoomNickname(userJID);
        	if(nickname != null)
        		result = xmpp.isAvailable(roomNID, nickname);
        } catch (Exception e) {
        	e.printStackTrace();
        	throw new MRException(MRExceptionCodes.CHECKING_AVAILABLE_PRESENCE_FAILED,"Checking Available Presence Failed.UserJID:="+userJID );
            //result = false;
        }
        XMPPTransaction.endBlock("isAvailable");
        return result;
        */
    	return true;
    }

    public String handleSetupStream(MRClient client, Document doc)  throws Exception {
        String requestNID = "requestNID";
        MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, requestNID, "serviceName", "requestName", new MRMessage(doc), "");
        String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
        Element instanceElement = MRXMLUtils.getElementXpath(this.destTableDoc, "//Instance[@streamNID='" + streamNID + "']");
        if(instanceElement != null) {
            throw new MRException(MediaRoomServiceExceptionCodes.STREAMALREADYPRESENT,MRXMLUtils.generateErrorXML("streamNID", streamNID));
        }
        
        String streamdatadocStr = MRXMLUtils.getValueXpath(doc, "//streamdatadoc", "");
        Document streamDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(streamdatadocStr, "UTF-8"));
        String mediasourcedatadocStr = MRXMLUtils.getValueXpath(doc, "//mediasourcedatadoc", "");
        Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(mediasourcedatadocStr, "UTF-8"));
        // Document streamDataDoc = MRXMLUtils.stringToDocument(MRXMLDBUtils.getObjectData(request, db, log, streamNID));
        
        // System.out.println("DestStreamAgent:handleSetupStream streamDataDoc=" + MRXMLUtils.documentToString(streamDataDoc));

        log.addInfo("DestStreamAgent:=" + this.userJID + ":handleWatchStream Adding streamNID to watchList streamNID=" + streamNID);
        String transition = null;
        try {
        	this.getDestIPPort(streamDataDoc);
        }catch(Exception e) {
        	e.printStackTrace();
            log.addInfo("DestStreamAgent:handleSetupStream@" + this.userJID + ":setup error no more ports available");
        	transition = DestStreamAgent.SETUPSTREAMERROR;
        }

        // Document streamDataDoc = MRXMLUtils.stringToDocument(MRXMLDBUtils.getObjectData(request, db, log, streamNID));
        Document instanceElementDoc = MRXMLUtils.stringToDocument("<Instance><MediaSourceData/><StreamData/></Instance>");
        String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
        String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
        String sourceAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID","");
        String streamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURL", "");
        String destFileParentDirNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID","");
        String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID", "");
        String sourceUser = ServiceAgentUtils.getUserFromJID(sourceAgentJID);
        String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID","");
        String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc, "//destPortElementID","");

        MRXMLUtils.getElementXpath(instanceElementDoc, "//MediaSourceData").add(mediaSourceDataDoc.getRootElement().detach());
        MRXMLUtils.getElementXpath(instanceElementDoc, "//StreamData").add(streamDataDoc.getRootElement().detach());

        instanceElement = instanceElementDoc.getRootElement();
        MRXMLUtils.addAttribute(instanceElement,"roomNID", roomNID);
        MRXMLUtils.addAttribute(instanceElement,"streamNID", streamNID);
        MRXMLUtils.addAttribute(instanceElement,"mediaSourceNID", mediaSourceNID);
        MRXMLUtils.addAttribute(instanceElement,"sourceAgentJID", sourceAgentJID);
        MRXMLUtils.addAttribute(instanceElement,"sourceUser", sourceUser);
        Element streamStatusElement = (Element)MRXMLUtils.getElementXpath(this.streamDocTemplate, "//Status").clone();
        MRXMLUtils.addAttribute(streamStatusElement,"streamNID", streamNID);
        MRXMLUtils.addAttribute(streamStatusElement,"roomNID", roomNID);
        instanceElement.add(streamStatusElement);
        MRXMLUtils.getElementXpath(instanceElementDoc, "/Instance/Status/state").setText(String.valueOf(MediaRoomServiceConst.WAITING_FOR_SOURCE));
        MRXMLUtils.getElementXpath(instanceElementDoc, "/Instance/Status/lastEventData").setText("");
        
        MRXMLUtils.addAttribute(instanceElement,"destNID", destNID);
        MRXMLUtils.addAttribute(instanceElement,"destPortElementID", destPortElementID);

        this.destTableDoc.getRootElement().add(instanceElement.detach());
        
        if(DestStreamAgent.SETUPSTREAMERROR.equalsIgnoreCase(transition)) {
        	this.handleNoMorePortsAvailable(request, client, instanceElement, streamNID);
        	return transition;
        }
        // Find out if the streamURL is already available in the streamDataDoc
        String sourceAgentType = DestStreamAgent.MANAGED;
        
        if(!"".equals(streamURL))
        	sourceAgentType = DestStreamAgent.UNMANAGED;
        else if(sourceAgentJID.equals(destAgentJID))
        	sourceAgentType = DestStreamAgent.INTERNAL;
        MRXMLUtils.addAttribute(instanceElement,"sourceAgentType", sourceAgentType);

       transition = DestStreamAgent.SETUPSTREAMREQUEST;
        if (DestStreamAgent.MANAGED.equals(sourceAgentType) &&
        		!this.isAvailable(xf.xmpp(userJID), roomNID, sourceAgentJID, this.userJID)) {
        	transition = DestStreamAgent.SOURCEUNAVAILABLE;
        }
       
        String destType = MRXMLUtils.getValueXpath(instanceElement, "StreamData/data/destType", "");
       
        if (	destType.equals(MediaRoomServiceConst.OUTPUT_PVR) || 
        		destType.equals(MediaRoomServiceConst.OUTPUT_RECORD)) {
        	try {
        		MediaRoomServiceUtils.checkAvailableDiskSpace(sourceAgentJID,destNID, destFileParentDirNID);
        	}
        	catch(MRException e) {
                e.printStackTrace();
                this.handleDiskSpaceUnavailable(request,client,instanceElement,doc);
                transition = DestStreamAgent.SETUPSTREAMERROR;
        	}
        } if (	destType.equals(MediaRoomServiceConst.OUTPUT_DECODER) ) {
        	String stateXpath = "(./Status/state[.!='"+MediaRoomServiceConst.SETUP_ERROR+"'] )";
            List<?> instanceList = MRXMLUtils.getListXpath(destTableDoc, "//Instance[@destNID='" + destNID + "' and @destPortElementID='" + destPortElementID + "' and "+stateXpath+"]");	int maxInstanceCount = 1;
    		if(instanceList.size() > maxInstanceCount) {
                this.handleBusy(request,client,instanceElement,doc);
    			transition = DestStreamAgent.SETUPSTREAMERROR;
    		}
        }


        return transition;
    }

    private void handleDestStopped(MRRequest request, MRClient client,Element instanceElement, Document doc)
    throws MRException,Exception {
	int code = MediaRoomServiceExceptionCodes.MEDIADEST_STOPPED; 
	String description = "Media Destination Stopped";
	String streamNID = MRXMLUtils.getAttributeValueXpath(instanceElement, ".//Stream","NID","");
	Element actionData = MRXMLUtils.stringToElement("<Exception><Code>" + String.valueOf(code) + "</Code><Description>" + description + "</Description></Exception>");
	Document destEventDoc = StreamAgentUtils.createDestinationEventDoc(streamNID, MediaRoomServiceConst.DESTINATIONEVENT, 
		MediaRoomServiceConst.DEST_ERROR, "", 0, 0, actionData, MRLog.ERROR,log);
	this.setStreamLastEvent(request, client, instanceElement, destEventDoc.getRootElement());
    }
}

