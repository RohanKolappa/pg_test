package com.ipvs.mediaroomservice.impl;

import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.stream.SourceSM;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;


public abstract class SourceStreamAgent implements ServiceAgentI, ConfigListenerI {
    public static final String CREATERELAYCONNECTION_REQUEST_XML = "CreateRelayConnectionRequest.xml";
    public static final String CREATERELAYCONNECTION_REQUEST = "CreateRelayConnectionRequest";
    public static final String CLEANMONITORAGENTSTREAMS_REQUEST_XML = "CleanMonitorAgentStreamsRequest.xml";
    public static final String ADDSTREAMACTION_REQUEST = "AddStreamActionRequest";
    public static final String ADDSTREAMACTION_REQUEST_XML = "AddStreamActionRequest.xml";
    public static final String DELETEMEDIAROOM_REQUEST_XML = "DeleteMediaRoomRequest.xml";
    public static final int LIVE_TO_PLAYBACK = 1;
    public static final int PLAYBACK_TO_LIVE = 2;
    public static final int PLAYBACK_TO_PLAYBACK = 3;
    protected XMPPF xf;
    protected MRDB db;
    protected MRLog log;
    //@SuppressWarnings("unused")
    protected MRResourceFactoryI resourceFactory;
   // @SuppressWarnings("unused")
    protected String userJID;
    protected String deviceType = null;
    protected String deviceNID = null;
    protected StreamingServerI streamingServer = null;
    StreamUtils streamUtils;
    SourceSM sourceSM;
    ArrayList<String> roomNIDList = new ArrayList<String>();
    
    public void initInternal(XMPPF xf, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, 
    		String userJID, String deviceNID, String deviceType, StreamingServerI streamingServer) throws Exception {
        this.xf = xf;
        this.log = log;
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.userJID = userJID;
        this.deviceType = deviceType;
        this.streamingServer = streamingServer;
        this.streamUtils = new StreamUtils(xf, log, db, resourceFactory, userJID, deviceNID,
        		deviceType, streamingServer);
        this.sourceSM = new SourceSM(xf, log, userJID, streamUtils);
		ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), MediaRoomServiceConst.SOURCESTREAMAGENT, MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);

    }
    
    public boolean handleEvent(MRClient client, Document doc, Element eventElement)
        throws Exception {
        return false;
    }

    public void handleRelayInputStreamMessage(MRClient client, String fromJID, String message)
        throws Exception {
        // TBR 
        /*
        Document doc = MRXMLUtils.stringToDocument(message);
        Element data = MRXMLUtils.getElementXpath(doc, "//data");
        String action = data.elementText("action");
        String streamNID = data.elementText("streamNID");
        String streamURL = data.elementText("streamURL");
        
        this.sourceTable.handleRelayInputStreamResponse(client, doc, action, streamNID, streamURL);
        */
    }
    
    /*
     * Handle the get current time stamp and get offset at time code requests
     */
    public void handleStreamInfoRequest(MRClient client, Document doc, 
    		Element data, long systemTime, String fromJID, String requestName) throws Exception {
        int status = MRLog.OK;
        String logdata = "";
        String streamNID = null;
        String streamInfo = "";
        streamNID = data.elementText("streamNID");

        try {
           	if(MediaRoomServiceConst.GETCURRENTTIMESTAMPREQUEST.equals(requestName)) {
           		long timestamp = this.streamUtils.getCurrentStreamTimestamp(streamNID);
                streamInfo = "<TimeStamp><StreamCurrentTimeStamp>" + timestamp + "</StreamCurrentTimeStamp>" +
                "<SystemCurrentTimeStamp>" + systemTime + "</SystemCurrentTimeStamp> </TimeStamp>"; 
        	}
        	else if(MediaRoomServiceConst.GETOFFSETATTIMECODEREQUEST.equals(requestName)) {        		
                String timecodeStr = MRXMLUtils.getValue(data,"timecode","0");
                String mediaUUID = MRXMLUtils.getValue(data, "mediaUUID", "");  
                String parentDirNID = MRXMLUtils.getValue(data, "prentDirNID", ""); 
                MediaStoreAdminServerAgent storeAdminAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
                String rootDir = storeAdminAgent.getMediaStoreRootPathBasedOnDirNID(parentDirNID);
                long offset = this.streamUtils.getOffset(mediaUUID, timecodeStr,rootDir);
                this.log.addInfo("SourceStreamAgent:getOffsetAtTimeCode uuid=" + 
                		mediaUUID + " tc=" + timecodeStr + " offset=" + offset, MRLog.OK, MRLog.NOTIFY);
                streamInfo = "<Offset><StreamOffset>" + offset + "</StreamOffset>" +
				"<SystemCurrentTimeStamp>" + systemTime + "</SystemCurrentTimeStamp> </Offset>";
        	} else if (MediaRoomServiceConst.GETTIMECODEATOFFSETREQUEST.equals(requestName)){
        		 String offsetStr = MRXMLUtils.getValue(data,"offset","0");
        		 String sourceUUID = MRXMLUtils.getValue(data, "mediaUUID", "");
        		 MediaStoreAdminServerAgent storeAdminAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
        		 String parentDirNID = MRXMLUtils.getValue(data, "prentDirNID", ""); 
                 String rootDir = storeAdminAgent.getMediaStoreRootPathBasedOnDirNID(parentDirNID);               
        		long timeStamp = this.streamUtils.getSyncTC(sourceUUID, Long.parseLong(offsetStr),rootDir);
        		 streamInfo = "<Offset><StreamCurrentTimeStamp>" + timeStamp + "</StreamCurrentTimeStamp>" +
 				"<SystemCurrentTimeStamp>" + systemTime + "</SystemCurrentTimeStamp> </Offset>";
        	}
        } catch (Exception e) {
            status = MRLog.ERROR;
            logdata = e.toString();
        }

        this.sendStreamInfoResponseMessage(client, doc, streamNID, streamInfo, fromJID, status, logdata);
    }

    /*
     * Send the response of get current  time stamp from source stream agent to source client agent
     */
    public void sendStreamInfoResponseMessage(MRClient client, Document doc, String streamNID, 
    		String streamInfo, String toJID, int status, String logdata)
        throws Exception {
    	String callback = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_CALLBACK, "");
        // System.out.println("SourceTable:sendClientMessage fromAgentName=" + fromAgentName + " doc=" + MRXMLUtils.documentToString(doc));
        String requestNID = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.REQUEST_NID, "");
        String serviceName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, "");
        String requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        String clientData = MRXMLUtils.getValueXpath(doc, "//"+MRConst.CLIENT_DATA, "");

        String info = "SourceStreamAgent:=" + this.userJID + "/" + streamNID + " > streamInfo Response  to " + ServiceAgentUtils.getAgentShortName(callback);
        String response = "<data>" + "<streamNID>" + streamNID + "</streamNID>" + streamInfo +
        					"<requestNID>" + requestNID + "</requestNID><log>" + logdata + "</log>" + "</data>";
        int responseState = MRRequest.STATE_DONE;

        if (status != MRLog.OK) {
            responseState = MRRequest.STATE_EXCEPTION;
        }

        String roomNID = null;
        String fromAgentClassName = MediaRoomServiceConst.SOURCESTREAMAGENT;
        String toAgentJID = toJID;
        String toAgentClassName = callback;
        String data = response;
        ServiceAgentUtils.sendAgentResponseMessage(xf.xmpp(userJID), log, roomNID, fromAgentClassName, 
        		toAgentJID, toAgentClassName, serviceName, requestName, data, info, responseState, logdata, requestNID, userJID, clientData);
    }


    public String handleMessage(MRClient client, String fromJID, String message)
            throws Exception {
            Document doc = MRXMLUtils.stringToDocument(message);
            Element data = ServiceAgentUtils.cloneDataElement(doc);

            String agentMessageType = MRXMLUtils.getAttributeValueXpath(doc, "//AgentMessage","type", "");
    	    if (ServiceClient.RESPONSE.equals(agentMessageType)) {
    	        Element responseElement = MRXMLUtils.getElementXpath(doc, "//" + ServiceClient.RESPONSE);
    	        Document responseDoc = MRXMLUtils.getDocument();
    	        responseDoc.setRootElement((Element) responseElement.detach());            
    	        this.sourceSM.handleResponse(fromJID, responseDoc,client);
    	    } else if (ServiceClient.REQUEST.equals(agentMessageType)) {
    	    	String requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
    	    	String clientCallback = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_CALLBACK, "");
    	    	String clientData = MRXMLUtils.getValueXpath(doc,"//" + MRConst.CLIENT_DATA, "");
    	    	// System.out.println("SourceStreamAgent:handleMessage clientCallback=" + clientCallback + " clientData=" + clientData + " doc=" + doc.asXML());
    	    	if(MediaRoomServiceConst.GETCURRENTTIMESTAMPREQUEST.equals(requestName) ||
    	    			MediaRoomServiceConst.GETOFFSETATTIMECODEREQUEST.equals(requestName)||
    	    			MediaRoomServiceConst.GETTIMECODEATOFFSETREQUEST.equals(requestName)) {        		
    	            long systemTime = client.getXmpp().getCurrentTimeMillis();
    	            this.handleStreamInfoRequest(client, doc, data, systemTime, fromJID, requestName);
    	        }
    	    	else if(MediaRoomServiceConst.STARTREQUEST.equals(requestName) ||
    	    			MediaRoomServiceConst.STOPREQUEST.equals(requestName)) {
    	    		String action = requestName;
    	            // String action = MRXMLUtils.getValueXpath(doc, "//action", "");
    	            String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID", "");
    	            String actionNID = MRXMLUtils.getValueXpath(doc, "//actionNID", "");
    	            Element actiondata = MRXMLUtils.getElementXpath(doc, "//actiondata");
    	            String userAction = MRXMLUtils.getValueXpath(doc, "//useraction", "");
    	            String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
    	            if (MediaRoomServiceConst.STARTREQUEST.equals(action) || 
    	            		MediaRoomServiceConst.STOPREQUEST.equals(action) || 
    	            		MediaRoomServiceConst.ABORTREQUEST.equals(action)) {
    	                Document streamDataDoc = null;
    	                Document mediaSourceDataDoc = null;
    		            if (MediaRoomServiceConst.STARTREQUEST.equals(action)) {
    		                String streamdatadocStr = data.elementText("streamdatadoc");
    		                streamDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(streamdatadocStr, "UTF-8"));
    		                String mediasourcedatadocStr = data.elementText("mediasourcedatadoc");
    		                mediaSourceDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(mediasourcedatadocStr, "UTF-8"));
    		            }
    		            else { // Find it from the cache
    		            	streamDataDoc = this.sourceSM.getStreamDataDoc(streamNID);
    		            	mediaSourceDataDoc = this.sourceSM.getMediaSourceDataDoc(streamNID);
    		            }
    	                this.handleStreamAction(client, requestNID, streamNID, actionNID, userAction, action, actiondata, streamDataDoc, mediaSourceDataDoc, 
    	                		clientData, clientCallback);
    	            }
    	            else 
    	            	this.log.addInfo("SourceStreamAgent::handleMessage:=" + this.userJID + " invalid action=" + action, MRLog.OK, MRLog.NOTIFY);
    	    	}
    	    	else if(MediaRoomServiceConst.TIMELINEUPDATEREQUEST.equals(requestName)) {
                	List<?> timelineUpdateList = MRXMLUtils.getListXpath(doc, "//timelineupdate");
                	for(Object o: timelineUpdateList) {
                		Element timelineUpdate = (Element)o;
    	            	this.handleTimelineUpdate(client, doc, data, timelineUpdate);
                	}	    		
    	    	}
    	    	else if(MediaRoomServiceConst.MEDIASOURCEUPDATEREQUEST.equals(requestName)) { 
    	    		// Used only for legacy code where the mediaSource may not have the lookback info for live
    	            String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
    	            String mediasourcedatadocStr = MRXMLUtils.getValueXpath(doc, "//mediasourcedatadoc", "");
                    Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(mediasourcedatadocStr, "UTF-8"));	  
                    this.sourceSM.updateMediaSourceData(streamNID, mediaSourceDataDoc);
                	this.log.addInfo("SourceStreamAgent::handleMessage:=" + this.userJID + " mediasource update", MRLog.OK, MRLog.NOTIFY);
    	    	}
    	        else {
    	            this.log.addInfo("SourceStreamAgent::handleMessage:=" + this.userJID + " invalid requestName=" + requestName, MRLog.OK, MRLog.NOTIFY);
    	        }
    	    }
        	else if(ServiceClient.EVENT.equals(agentMessageType)) {
        		String eventname = MRXMLUtils.getAttributeValueXpath(doc,"//Event/Header", "eventName","");
        		if (MediaRoomServiceConst.SOURCEEVENT.equals(eventname)) {
        			String actiondata = MRXMLUtils.elementChildrenToString(MRXMLUtils.getElement(doc, "//sourceEventActionData"));
        			this.streamUtils.propogateSourceEventToDestinations(client, doc, eventname, actiondata);
        		} else if (MediaRoomServiceConst.SSEVENT.equals(eventname)) {
    	            String actiondata = MRXMLUtils.getValueXpath(doc, "//actiondata", "");
        			this.streamUtils.handleSourceAgentSSEvents(client, doc, eventname, actiondata);
        		} else if (MediaStoreServiceConst.MEDIA_STORE_EVENT.equals(eventname)) {
    	            String userAction = MRXMLUtils.getValueXpath(doc, "//useraction", "");
        			this.streamUtils.handleMediaStoreEvents(client, doc, userAction);
        		}
        	}
            else {
                this.log.addInfo("SourceStreamAgent::handleMessage:=" + this.userJID + " invalid messagetype=" + agentMessageType, MRLog.OK, MRLog.NOTIFY);
            }

            return message;
        }



    public boolean handleRoomUnavailable(MRClient client, Document doc, String roomNID)
	    throws Exception {
	    //System.out.println("HandleRoomUnavailable");
	    String streamXPath = "//Stream[@roomNID='" + roomNID + "']";
	    List<?> streamElementList = this.streamUtils.getStreamList(streamXPath);
	    client.getLog().addInfo("SourceTable:=" + this.userJID + ":handleRoomUnavailable roomNID=" + roomNID + 
	    		" removing stream#=" +  streamElementList.size()	, MRLog.OK, MRLog.NOTIFY);
	
	    return this.handleAbortStreamList(streamElementList);
	}
	
	// Release the streamElement if the dest is no longer available
	public boolean checkDestUnavailable(MRClient client, Document doc, String value, String destAgentJID)
	    throws Exception {
	    if (destAgentJID == null) {
	        return false;
	    }
	    
	    if(!"Ready".equals(value)) {
	    	   String streamXPath = "//Stream[@destAgentJID='" + destAgentJID + "']";
	   	    List<?> streamElementList = this.streamUtils.getStreamList(streamXPath);
	   	    client.getLog().addInfo("SourceTable:=" + this.userJID + ":handleDestUnavailable destAgentJID=" + destAgentJID + 
	   	    		" removing stream#=" +  streamElementList.size()	, MRLog.OK, MRLog.NOTIFY);	
	   	    return this.handleAbortStreamList(streamElementList);
    	}
    	
	
	 
	    return false;
	}

    public boolean handleMediaPresence(MRClient client, Document doc, String presenceType, String value, String roomUserJID, boolean available, String roomNID)
        throws Exception {
        if("MediaRoom".equals(presenceType)) {
        	if("Ready".equals(value)) {
        		this.log.addInfo("SourceStreamAgent:handleMediaPresence add Room roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
        		this.roomNIDList.add(roomNID);
        		return true;
        	}
        	else {
        		this.log.addInfo("SourceStreamAgent:handleMediaPresence remove Room roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
            	if(this.roomNIDList.contains(roomNID))
            		this.roomNIDList.remove(roomNID);
                return this.handleRoomUnavailable(client, doc, roomNID);
        	}
        }
        else
        	return this.checkDestUnavailable(client, doc,value, roomUserJID);
    }

    /**
     * @return the deviceType
     */
    public String getDeviceType() {
        return deviceType;
    }

    /**
     * @param deviceType the deviceType to set
     */
    public void setDeviceType(String deviceType) {
        this.deviceType = deviceType;
    }

    @Override
    public void handleConfigUpdates(String localConfigFile,String updateParentXPath,String updateXPath, String userJID, ServiceMessageHandlerI messageHandler)
        throws MRException, Exception {
        // TODO Auto-generated method stub
    }
    
    public static Document getStatus(String userJID) throws Exception {
	    Document statusdoc = MRXMLUtils.stringToDocument("<" + MediaRoomServiceConst.SOURCESTREAMAGENT + "Status/>");
	    StreamUtils streamUtils = StreamUtils.getStreamUtils(userJID);
	    if (streamUtils != null) {
		    Element rootElement = statusdoc.getRootElement();
		    streamUtils.getStatus(rootElement);
	    }
	    return statusdoc;
	}
    
    public void handleStreamAction(MRClient client, String requestNID, String streamNID, String actionNID, String userAction, String action, 

    		Element actiondata, Document streamDataDoc,
        Document mediaSourceDataDoc, String clientData, String clientCallback) throws Exception {

        // com.ipvs.common.XMPPTransaction.DEBUG = true;
        com.ipvs.common.XMPPTransaction.startBlock("SourceTable.handleStreamAction");

        String streamURL = "";
        String relayRoomNID = "";
        String relayStreamNID = "";
        int status = MRLog.OK;
        String logdata = "";
        Element mediaTimeline = null;
        String responseActionData = "";
        try {
            if (MediaRoomServiceConst.ABORTREQUEST.equals(action)) {
                this.sourceSM.teardownStreamElement(streamNID);

                return;
            } else if (MediaRoomServiceConst.STARTREQUEST.equals(action)) {            	
            	String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
            	if(!this.roomNIDList.contains(roomNID)) {
            		throw new MRException(MediaRoomServiceExceptionCodes.ROOMIDNOTFOUND, "Room not found roomNID=" + roomNID);

            	}
                // action = MediaRoomServiceConst.START;
                Element streamElement = this.sourceSM.setupStreamElement(streamNID, streamDataDoc, mediaSourceDataDoc, actionNID);
                mediaTimeline = streamElement.element("MediaTimeline");
                Element outputElement = streamElement.element("Output");
                Element instanceElement = streamElement.getParent().getParent();
                Element inputElement = instanceElement.element("Input");
                // TBD Debug line 
                mediaTimeline.addAttribute("ssInstanceID", inputElement.attributeValue("ssInstanceID"));
                streamURL = MRXMLUtils.getAttributeValue(outputElement,"streamURL");
                relayRoomNID = MRXMLUtils.getAttributeValue(inputElement,"roomNID");
                relayStreamNID = MRXMLUtils.getAttributeValue(inputElement,"streamNID");
                // streamURL = this.getStreamURL(streamNID, streamDataDoc, mediaSourceDataDoc);
            } else if (MediaRoomServiceConst.STOPREQUEST.equals(action)) {
                // action = MediaRoomServiceConst.STOP;
        	    if(this.sourceSM.getMediaSourceDataDoc(streamNID) == null) {
        	        this.log.addInfo("StreamTable:teardownStreamElement stream no longer in cache");
        	    	return;
        	    }

                this.sourceSM.teardownStreamElement(streamNID);
            }
        } catch (MRException mre) {
            status = MRLog.ERROR;
            logdata = MRException.getStackTrace(mre);
            responseActionData = mre.formatStringException();
            mre.printStackTrace();

        }
        catch (Exception e) {
            status = MRLog.ERROR;
            logdata = MRException.getStackTrace(e);
 			e.printStackTrace();
 			MRException mr = new MRException(MRExceptionCodes.SYSTEMERROR, e.toString());
 			responseActionData = mr.formatStringException();
		}
        
        

        
      	this.streamUtils.sendDestMessage(streamDataDoc, client, requestNID, streamNID, userAction, action, responseActionData, 
        		streamURL, relayRoomNID, relayStreamNID, actionNID, status, logdata, mediaTimeline, clientData, clientCallback);
        
        com.ipvs.common.XMPPTransaction.endBlock("SourceTable.handleStreamAction");

        // /com.ipvs.common.XMPPTransaction.DEBUG = false;
    }

    private void sendUpdateSyncAction(Element data, Element timelineUpdate, long syncTC) throws Exception {
    	// System.out.println("Before data=" + MRXMLUtils.elementToString(data));
        String roomNID = MRXMLUtils.getAttributeValue(timelineUpdate,"roomNID");
        String streamNID = "";
        String requestNID = "";
        
        // Generate the new actiondata
        MRXMLUtils.addAttribute(timelineUpdate,"syncTC", String.valueOf(syncTC));
        Element actiondata = MediaTimeline.createTimelineActionData(timelineUpdate);
        
        // Now replace the actiondata in the original request
        Element timelineUpdateRequestData = (Element)data.clone();
        Element tmpactiondata = MRXMLUtils.getElementXpath(timelineUpdateRequestData, "./actiondata");
        timelineUpdateRequestData.remove(tmpactiondata);
        timelineUpdateRequestData.add(actiondata);

        String syncSourceAgentJIDList = MRXMLUtils.getAttributeValue(timelineUpdate,"syncSourceAgentJIDList");
        StringTokenizer st = new StringTokenizer(syncSourceAgentJIDList,",");
        while(st.hasMoreTokens()) 
        // System.out.println("After data=" + actionRequestMessage);
        	ServiceAgentUtils.sendAgentRequestMessage(xf.xmpp(userJID), log, roomNID,"", 
        		st.nextToken(), 
        		MediaRoomServiceConst.SOURCESTREAMAGENT, 
        		MediaRoomServiceConst.TIMELINEUPDATEREQUEST,
        		MRXMLUtils.elementToString(timelineUpdateRequestData),
        		"SourceStreamAgent::handleUpdateSyncAction:=" + 
        		this.userJID + "/" + streamNID + 
            		" > syncC=" + syncTC + " Message to roomNID=" + roomNID,
            	requestNID,this.userJID,MRConst.MEDIA_SERVICE,"");
	}


    public void handleTimelineUpdate(MRClient client, Document doc, Element data, Element timelineUpdate) throws Exception {
        String actionNID = MRXMLUtils.getValueXpath(doc, "//actionNID", "");
        String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID", "");
		// Check to see if this is a syncSourceEvent
	    // Element actionElement = MRXMLUtils.getElementXpath(doc, "//action");
		// String syncMessage = MRXMLUtils.elementToString(doc.getRootElement());
	    boolean sync = MRConst.TRUE.equals(MRXMLUtils.getAttributeValue(timelineUpdate,"sync"));
	    String sourceAction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("sourceAction");
	    long syncTC = 0;
	    if(sync && !MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {
	    	syncTC = Long.parseLong(MRXMLUtils.getAttributeValue(timelineUpdate,"syncTC"));
	    	if(syncTC == 0) {
	    		this.log.addInfo("SourceStreamAgent:handleSourceAction skipping sourceAction due to missing syncTC action=" + 
	    				sourceAction, MRLog.OK, MRLog.NOTIFY);
	    		String syncAgentJID = MRXMLUtils.getAttributeValue(timelineUpdate,"syncAgentJID");
	    		if(ServiceAgentUtils.matchesJID(syncAgentJID, this.userJID)) {
		    		String syncSourceUUID = MRXMLUtils.getAttributeValue(timelineUpdate,"syncSourceUUID");
		    		String syncSourceParentDirNID = MRXMLUtils.getAttributeValue(timelineUpdate,"syncSourceParentDirNID");
	    			long offset = MediaTimeline.getOffsetFromTimelineUpdate(timelineUpdate);
	    			MediaStoreAdminServerAgent storeAdminAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);	        		 
	                String rootDir = storeAdminAgent.getMediaStoreRootPathBasedOnDirNID(syncSourceParentDirNID);
	    			syncTC = this.streamUtils.getSyncTC(syncSourceUUID, offset, rootDir);
		    		if(syncTC > 0) {
	    				this.log.addInfo("SourceStreamAgent:handleSourceAction sending updateSyncAction action=" + 
	    						sourceAction + " syncTC=" + syncTC, MRLog.OK, MRLog.NOTIFY);
		    			this.sendUpdateSyncAction(data, timelineUpdate, syncTC);
		    		}
		    		else {
	    				this.log.addInfo("SourceStreamAgent:handleSourceAction failed to find syncTC for mediaUUID=" + 
	    						syncSourceUUID + " offset=" + offset,MRLog.OK, MRLog.NOTIFY);		    			
		    		}
	    		}
				return;
	    	}
		}
	    
	    List<?> streamNIDList = timelineUpdate.element("streamNIDList").elements();
	    if(streamNIDList.isEmpty())
	    	return;
	    
        StringBuffer sb = null;
        
        for(Object o : streamNIDList) {
        	Element streamNIDElement = (Element)o;
        	if(sb == null) {
        		sb = new StringBuffer("Stream[");
        	}
        	else {
        		sb.append(" or ");
        	}
        	sb.append("@NID='" + streamNIDElement.getText() + "'");
        }
        sb.append("]"); 

        String streamXPath = sb.toString();
	    this.sourceSM.handleTimelineUpdate(client, requestNID, actionNID, streamXPath, timelineUpdate, 
	    		sync, syncTC);
    }


    boolean handleAbortStreamList(List<?> streamElementList) throws Exception {
        com.ipvs.common.XMPPTransaction.startBlock("SourceTable.handleAbortStreamList");

        if ((streamElementList == null) || (streamElementList.size() == 0)) {
            return false;
        }

        Iterator<?> streamListIterator = streamElementList.iterator();

        //System.out.println("StreamList ------>"+streamElementList);
        Element streamElement = null;
        String streamNID = null;

        while (streamListIterator.hasNext()) {
            streamElement = (Element) streamListIterator.next();
            streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
            this.sourceSM.teardownStreamElement(streamNID);
        }

        com.ipvs.common.XMPPTransaction.endBlock("SourceTable.handleAbortStreamList");

        return true;
    }


    // This method is used to handle cases where there is no SourceAgent and
    // the streamURL is part of the SourcePort information in the database
    // This method is maintain in the SourceTable file so that it stays in sync
    // with handleStreamAction
    public void handleDummyStreamAction(MRClient client, String requestNID, String streamNID, String actionNID, String userAction, String action, String actiondata, Document streamDataDoc,
            Document mediaSourceDataDoc, String streamURL) throws Exception {
	    int status = MRLog.OK;
	    String logdata = "";
	    if (MediaRoomServiceConst.STARTREQUEST.equals(action)) {
	        action = MediaRoomServiceConst.START;
	    } else if (MediaRoomServiceConst.STOPREQUEST.equals(action)) {
	        action = MediaRoomServiceConst.STOP;
	    }
	   // Element mediaTimeline = MRXMLUtils.stringToElement("<MediaTimeline/>");
	    String trackStateAtStart = MRXMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    long trackOffsetAtStart = Long.parseLong(MRXMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", ""));
	    long mediaOffset = trackOffsetAtStart; 
	    Element mediaTimeline = MediaTimeline.createMediaTimelineElement(log);
	    String offset = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//startOffset","");
	    long now = xf.xmpp(userJID).getCurrentTimeMillis();
	    Element start = mediaTimeline.element("Start");
	    start.addAttribute("WC", String.valueOf(now));		
	    start.addAttribute("Offset", String.valueOf(offset));
	    long tc = now;
	    start.addAttribute("TC", String.valueOf(tc));
	    MediaTimeline.updateMediaTimelineElement(mediaTimeline, 
	    		now, mediaOffset, tc,0,
	    		MediaRoomServiceConst.START, trackStateAtStart, MediaRoomServiceConst.START, MediaRoomServiceConst.START);
	    String clientData = "";
	    String clientCallback = MediaRoomServiceConst.DESTSTREAMAGENT;
	    this.streamUtils.sendDestMessage(streamDataDoc, client, requestNID, streamNID, userAction, action, actiondata, 

	    		streamURL, "", "", actionNID, status, logdata, mediaTimeline, clientData, clientCallback);

    }
    
	@Override
	public void reset() throws Exception {
		this.streamingServer = null;
	}
	public Element getStreamInfoListElement() throws Exception {
		Element streamInfoList = MRXMLUtils.stringToElement("<StreamInfoList/>");		
		List<?> streamList = MRXMLUtils.getListXpath(this.streamUtils.getSourceTableDoc(), "//Stream");
		for(Object o : streamList) {
		    
			Element stream = (Element)o;
			
			String key = MRXMLUtils.getAttributeValue(stream, "NID", "");			
			
			String streamURL = MRXMLUtils.getAttributeValue(stream.element("Output"), "streamURL");			
			Document streamStatusDoc = null;
			if ("".equals(streamURL)){
			    streamStatusDoc = this.generateStreamStatusDoc(stream, MediaRoomServiceConst.START_ERROR);
			}
			else{
			    streamStatusDoc = this.generateStreamStatusDoc(stream, MediaRoomServiceConst.STREAM_STARTED);
			}
			Document streamDataDoc = this.sourceSM.getStreamDataDoc(key);
			Document mediaSourceDataDoc = this.sourceSM.getMediaSourceDataDoc(key);			
			Element streamInfoDoc = MRXMLUtils.stringToElement("<StreamInfo />");			
			MRXMLUtils.addAttribute(streamInfoDoc, "streamNID", key);
			Element streamDataDocInfo = MRXMLUtils.stringToElement("<StreamDataDoc />");
			Element mediaSourceDataDocInInfo = MRXMLUtils.stringToElement("<MediaSourceDataDoc />");
						
			streamDataDocInfo.add((Element)streamDataDoc.getRootElement().clone());			
			streamDataDocInfo.add((Element)streamStatusDoc.getRootElement().clone());
			mediaSourceDataDocInInfo.add((Element)mediaSourceDataDoc.getRootElement().clone());
			streamInfoDoc.add(streamDataDocInfo);
			streamInfoDoc.add(mediaSourceDataDocInInfo);
			streamInfoList.add(streamInfoDoc);
			
		}
		Element mediaServiceInfoDoc = MRXMLUtils.stringToElement("<"+MediaRoomServiceConst.MEDIA_SERVICE_INFO+"/>");
		mediaServiceInfoDoc.add(streamInfoList);

		return mediaServiceInfoDoc;
	}
	private Document generateStreamStatusDoc(Element streamElement, int state) throws Exception {
	    	Document streamDocTemplate = RoomDB.getStreamInstanceXML(log);
		Document streamStatusDoc = MRXMLUtils.getDocumentXpath(streamDocTemplate, "//Status");;
		MRXMLUtils.setValueXpath(streamStatusDoc, "//state", String.valueOf(state));
		Element outputElement = streamElement.element("Output");
		String streamURL = outputElement.attributeValue("streamURL");
		MRXMLUtils.setValueXpath(streamStatusDoc, "//URL", streamURL);
		
		// Replace the MediaTimeline
		Element statusElement = streamStatusDoc.getRootElement();
		statusElement.element("MediaTimeline").detach();
		statusElement.add((Element)streamElement.element("MediaTimeline").clone());
		String streamNID = streamElement.attributeValue("NID");
		String roomNID = streamElement.attributeValue("roomNID");
		statusElement.addAttribute("streamNID", streamNID);
		statusElement.addAttribute("roomNID", roomNID);		
		return streamStatusDoc;
	}
}
