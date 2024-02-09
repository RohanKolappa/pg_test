package com.barco.device.handler.Media.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRRequest;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.SessionServiceConst;
import com.barco.utils.mr.Utils;
import com.barco.utils.mr.XMPPI;
import com.barco.utils.mr.XMPPTransaction;

public class DMSMediaAgentImpl extends BaseMediaAgentImpl {
	
	public DMSMediaAgentImpl(String userJID, String deviceNID, String deviceType,
			StreamingServerI streamingServer, MRLog log, XMPPI xmpp,
			Document deviceDoc) {
		super(userJID, deviceNID, deviceType, streamingServer, log, xmpp,deviceDoc);
	}


	public void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception {
		if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
	        String mediaUUID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");
	        String mediaParentDirNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
	        MRXMLUtils.addAttribute(inputElement,"mediaUUID", mediaUUID);
	        MRXMLUtils.addAttribute(inputElement,"mediaParentDirNID", mediaParentDirNID);
	        String dirPath = getDirPath(mediaParentDirNID);	  
	    	MRXMLUtils.addAttribute(inputElement,"dirPath", dirPath);		
	        
	        //add ffTracks supported or not
	        String ffTracks = MRXMLUtils.getValueXpath(streamDataDoc, "//FFTracks", "");
	        if(ffTracks.isEmpty()){
	        	MRXMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.FALSE));
	        } else {
	        	MRXMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.TRUE));
	        }
		}
	}

	public void setupExternalInput(String inputNID, Element inputElement, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc,
			Document sourceTableDoc) throws Exception {
	    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
        String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID", "");
        if("".equals(userRoomMediaSourceNID))
        	userRoomMediaSourceNID = mediaSourceNID;
	    
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        XMPPTransaction.startBlock("SourceTable.createRelay");
	        // createRelayRoom(sourceNID, offset, startTimecode, syncTC, streamDataDoc, inputNID, userRoomMediaSourceNID);
	        XMPPTransaction.endBlock("SourceTable.createRelay");
	        MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE));
	    }
	}
	
	 public void teardownExternalInput(Element inputElement) throws Exception {
		 String inputType = inputElement.attributeValue("inputType");
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        String roomNID =MRXMLUtils.getAttributeValue(inputElement,"roomNID");
	        this.log.addInfo(this.userJID + ":SourceTable:deleteInput Started shutdown of Relay roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
	        // this.deleteRelayRoom(roomNID);
	    }
	 }

	 public void updateOutputElement(Element outputElement, String outputType, Document streamDataDoc) throws Exception { 
	    if(outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD) || 
		    		outputType.equals(MediaRoomServiceConst.OUTPUT_PVR)) {
	    	String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID","");
	    	String destFileUUID  = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileUUID","");
	    	String destFileParentDirNID  = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID","");
	    	MRXMLUtils.addAttribute(outputElement,"destFileUUID", destFileUUID);
	    	MRXMLUtils.addAttribute(outputElement,"destFileParentDirNID", destFileParentDirNID);
	    	String dirPath = this.getDirPath(destFileParentDirNID);
	    	MRXMLUtils.addAttribute(outputElement,"dirPath", dirPath);		
	    	initRecordOrPVRFile(streamDataDoc,destNID,destFileUUID,destFileParentDirNID);
	    }
	}

	private String getDirPath(String parentDirNID) {
		String dirPath = null;
		/* TBD Record
		MediaStoreAdminServerAgent mediaStoreAdminServerAgent = MediaStoreAdminServerAgent.getMediaStoreAgent();
    	String dirPath = mediaStoreAdminServerAgent.getMediaStoreRootPathBasedOnDirNID(parentDirNID);
    	*/
    	return dirPath;
	}
		
    private void initRecordOrPVRFile(Document streamDataDoc,String destNID, String destFileUUID, String destFileParentDirNID) throws MRException,Exception {    	
    	/* TBD Record
    	MediaStoreAdminServerAgent storeAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
    	MediaStoreServerI mediaStoreServer = storeAgent.getMediaStoreServer();
    	String rootDir = storeAgent.getMediaStoreRootPathBasedOnDirNID(destFileParentDirNID);
    	Element actionDataElement = MRXMLUtils.stringToElement("<UpdateProfileRequestData><streamType>"+MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "")+"</streamType></UpdateProfileRequestData>");
    	Element profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");    	
    	MRXMLUtils.addElement(actionDataElement,(Element)profileXML.clone()); 
    	
    	if(mediaStoreServer.isMediaStoreExist(rootDir, destFileUUID)) {
      		log.addInfo("Media Store File Exist mediaStoreFile:="+destFileUUID +" mediaStreamFile:="+destNID,MRLog.INFO,MRLog.NOTIFY);
      		return;
      	}
    	
    	log.addInfo("Media Store File Does Not Exist mediaStoreFile:="+destFileUUID +" mediaStreamFile:="+destNID,MRLog.INFO,MRLog.NOTIFY);      	    	
    	
    	mediaStoreServer.initRecordOrPVRFile(destFileUUID, destNID, MRXMLUtils.elementToDocument(actionDataElement), rootDir, null, null);    
    	*/
	}

    /*
     * TBD Record
     * This is a hack to be able to receive tc values on completion of recording
     * Delete the recorder instance when the stream is stopped will prevent tc value updates from streamingServer
     */
    public void deleteRecordPortOnProgressStoppedEventFromStreamingServer(String ssInstanceId) throws MRException,Exception {
		try {
			this.sourceTable.streamingServer.deleteRecordPort(ssInstanceId);
		} catch (Exception e) {
			//ignore the exception
		}
    }

	public void stopSSOutputOnDiskFull(Element streamElement, int eventCode) throws Exception {
		Element mediaSourceEl = streamElement.getParent();
		Element instanceEl = mediaSourceEl.getParent();
		Element inputEl = instanceEl.element("Input");
		long offset = Utils.getLongValue(MRXMLUtils.getAttributeValue(inputEl,"offset"), 0);
		streamElement.addAttribute(SessionServiceConst.ERROR_CODE, String.valueOf(eventCode));
		this.sourceTable.stopSSOutputAt(instanceEl, streamElement, offset);
	}

/*
 * TBD Record
		/////////////////////////////////////////////////////////
		// Relay input methods
		//////////////////////////////////////////////////////////
		private void createRelayRoom(String sourceNID, long offset, long startTimecode, long syncTC,
				Document streamDataDoc, String inputNID,
				String userRoomMediaSourceNID)     throws Exception {
		    String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.CREATERELAYCONNECTION_REQUEST_XML;
		    Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		    MRXMLUtils.setValueXpath(requestDoc, "//clientdata",inputNID);
		
		    MRXMLUtils.setValueXpath(requestDoc, "//sourceNID",sourceNID);
		    MRXMLUtils.setValueXpath(requestDoc, "//name",this.userJID);
		    MRXMLUtils.setValueXpath(requestDoc, "//description",MediaRoomServiceConst.RELAY_ROOM);
		    
		    //If the connection is live then change the start time code to start timecode + offset.
		    if(startTimecode > 0) {
		         startTimecode += offset;
		         offset=0;
		    }
		    
			MRXMLUtils.setValueXpath(requestDoc, "//startOffset",String.valueOf(offset));
		    MRXMLUtils.setValueXpath(requestDoc, "//startTimecode",String.valueOf(startTimecode));
		
		    String profileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//profileNID","");
		    MRXMLUtils.getElementXpath(requestDoc, "//profileNID").setText(profileNID);
		    
		    Element profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");
		    MRXMLUtils.copyContent(requestDoc, "//profileXML" , profileXML);
		    
		    String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//relayNID","");
		    MRXMLUtils.getElementXpath(requestDoc, "//destNID").setText(destNID);
		    MRXMLUtils.getElementXpath(requestDoc, "//relayNID").setText(MediaRoomServiceConst.COMPUTERELAYUSINGPOLICY);
		    MRXMLUtils.getElementXpath(requestDoc, "//userRoomMediaSourceNID").setText(userRoomMediaSourceNID);
		
		    String roomNID = null;
		    String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
		    String agentClassName = AppServerAgent.class.getName();
		    String fromAgentClassName = SourceStreamAgent.class.getName();
		    String info = "SourceTable::createRelayRoom > Sending Request CreateRelayConnection";
		
		    String requestNID = "none";
		    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, roomNID, agentJID, agentClassName, requestDoc, info, requestNID);
		}
		private void deleteRelayRoom(String roomNID) throws Exception {
		    if ((roomNID == null) || (roomNID.length() == 0)) {
		        return;
		    }
		
		    log.addInfo("SourceTable:=" + this.userJID + ":deleteRelayRoom roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
		
		    String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.DELETEMEDIAROOM_REQUEST_XML;
		    Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		    MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
		
		    String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
		    String agentClassName = AppServerAgent.class.getName();
		    String fromAgentClassName = SourceStreamAgent.class.getName();
		    String info = "SourceTable::deleteRelayRoom > Sending Request DeletMediaRoom";
		
		    String requestNID = "none";
		    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, null, agentJID, agentClassName, requestDoc, info, requestNID);
		}

		public void handleRelayInputStreamStarted(Element inputElement, String streamNID, String streamURL, String action, Document streamDataDoc) throws Exception {
		    log.addInfo("SourceTable:=" + this.userJID + " processing relayInpuStreamStarted streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
		    MRXMLUtils.addAttribute(inputElement,"streamURL", streamURL);
		    this.streamingServer.updateSSNetworkInput(inputElement, streamDataDoc, action);
		}
		

		private boolean checkResponseOK(Document responseDoc,MRClient client, Element inputElement) {
			try {
				String stateStr = MRXMLUtils.getValueXpath(responseDoc, "//header/state", "");
				int state = 0;
				try {
					state = Integer.parseInt(stateStr);
				} catch(Exception e){}
				if(state == MRRequest.STATE_DONE)
					return true;
				
				Element exceptionEL = MRXMLUtils.getElementXpath(responseDoc, "//Exception");
				String streamNID = MRXMLUtils.getAttributeValue(inputElement, "streamNID", null);    			  
				if(streamNID != null) { 
					String actionData = MRXMLUtils.elementToString(exceptionEL);
					Document doc = StreamAgentUtils.createSourceEventMessage(streamNID, 
							MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.RELAY_SOURCE_STARTED,
							StreamTable.getStreamNIDXML(streamNID), 0, actionData, MRLog.ERROR, "", log);
					
					
					//Document doc = MRXMLUtils.stringToDocument(sourceEventXML);
					// String actionData = MRXMLUtils.getValueXpath(doc, "//sourceEventActionData", "");
					// this.handleSourceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, actionData);
					Element instanceEL = inputElement.getParent();
				    List<?> streamsList = MRXMLUtils.getListXpath(instanceEL, "MediaSource/Stream");
					this.sendMessageToStreamList(client, doc, MediaRoomServiceConst.SOURCEEVENT, actionData, streamsList);
					for(Object streamElement: streamsList) {
						teardownStreamElement(MRXMLUtils.getAttributeValue((Element)streamElement, "NID"));	
					}
					
				}
			} catch(MRException exp){
				exp.printStackTrace();
			}catch(Exception exp){
				exp.printStackTrace();
			}
			return false;
		}

		private void handleAddStreamActionResponse(String fromJID, Document responseDoc,MRClient client) throws Exception {
			String inputNID = MRXMLUtils.getValueXpath(responseDoc, "//clientdata", "");
	    	Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@NID='" + inputNID + "']");
	    	if(inputElement == null)
	    		return;
		   	if(!checkResponseOK(responseDoc, client, inputElement)) {	   		
		        return;
		   	}
		   	MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_OK));	    
		}
		
		private void handleCreateRelayConnectionResponse(String fromJID, Document responseDoc,MRClient client) throws Exception {
			String inputNID = MRXMLUtils.getValueXpath(responseDoc, "//clientdata", "");
	    	Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@NID='" + inputNID + "']");
			String roomNID = MRXMLUtils.getValueXpath(responseDoc, "//roomNID", "");
	    	if(inputElement == null) {
				this.deleteRelayRoom(roomNID);
	    		return;
	    	}
		   	if(!checkResponseOK(responseDoc, client, inputElement)) {	   	    	   			   	    
		        return;
		   	}
		   	
		    String streamNID = MRXMLUtils.getValueXpath(responseDoc, "//streamNID", "");		 
		   	int inputElementState = Utils.getIntValue(MRXMLUtils.getAttributeValue(inputElement, "state"),0);
		   	
		   	if(inputElementState == StreamTable.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE){
		   		updateRealyRoomInfoInInputElement(inputElement,roomNID, streamNID);
		   	}
		    
		    MediaRoomServiceUtils.printlog("StreamTable:handleCreateRelayConnectionResponse roomNID=" + roomNID + " streamNID=" + streamNID);
			Element relayData = MRXMLUtils.stringToElement(
					"<data><mediaSourceNID/><RelayData roomNID='" + roomNID + "' streamNID='" + streamNID + "'/></data>");

			Element instanceEL = (Element)inputElement.getParent().clone();
		    this.sendMediaSourceEvent(client, instanceEL, MRLog.INFO, 
		    		MediaRoomServiceConst.RELAYEVENT, relayData);

		    XMPPTransaction.endBlock("SourceTable.startRelayStream");
		}
		
		private void updateRealyRoomInfoInInputElement(Element inputElement, String roomNID, String streamNID) {
			MRXMLUtils.addAttribute(inputElement,"roomNID", roomNID);
			MRXMLUtils.addAttribute(inputElement,"streamNID", streamNID);
			MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_OK));
			log.addInfo("updateRealyRoomInfoInInputElement: Update Input element with relay room information",MRLog.INFO,MRLog.NOTIFY);
		}

		public void handleResponse(String fromJID,Document responseDoc,MRClient client) {
			try {
				String requestName = MRXMLUtils.getValueXpath(responseDoc, "//requestname", "");
				this.log.addInfo("SourceTable::handleResponse:=" + this.userJID + " > Received Response " + requestName, MRLog.OK, MRLog.NOTIFY);
				if(requestName.equals(SourceStreamAgent.CREATERELAYCONNECTION_REQUEST)){
					handleCreateRelayConnectionResponse(fromJID, responseDoc, client);
				} else if(requestName.equals(SourceStreamAgent.ADDSTREAMACTION_REQUEST)){
					handleAddStreamActionResponse(fromJID, responseDoc, client);
				}
			} catch(MRException exp){
				exp.printStackTrace();
			}catch(Exception exp){
				exp.printStackTrace();
			}
		}

		public void handleRelayInputStreamResponse(MRClient client, Document doc, String action, String streamNID, String streamURL, Document streamDataDoc)throws Exception {
			Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@streamNID='" + streamNID + "']");
			// System.out.println("StreamTable:handleRelayInputStreamResponse inputElement=" + MRXMLUtils.elementToString(inputElement));
			if (inputElement == null) {
				inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@sourceNID='" + MRXMLUtils.getValueXpath(streamDataDoc, "//sourceNID", "") + "']");
				if(inputElement == null || !MediaRoomServiceConst.START.equals(action)) {
					return;
				}
				
				log.addInfo("handleRelayInputStreamResponse: Did not find input stream. Found input stream based on SourceNID",MRLog.INFO,MRLog.NOTIFY);
				
			}
			
			// Check if this is a relay input
	        if (!MediaRoomServiceConst.INPUT_RELAY.equals(MRXMLUtils.getAttributeValue(inputElement,"inputType"))) {
	            return;
	        }
	        
			int  state  = Utils.getIntValue(MRXMLUtils.getAttributeValue(inputElement, "state",""),0);
			if(state==StreamTable.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE){
				String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
				updateRealyRoomInfoInInputElement(inputElement, roomNID, streamNID);
			}
		
			// Check if this is a response from the RelaySource and hup the relayInput if required
			if (MediaRoomServiceConst.START.equals(action) || MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(action) || MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(action) ||
			        MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(action) || MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(action)) {
			    int status = MRLog.OK;
			    String logData = null;
			    int eventCode = MediaRoomServiceConst.RELAY_SOURCE_STARTED;
			    
			    String nowStr = String.valueOf(xmpp.getCurrentTimeMillis());
			    String actionData = this.userJID + "/" + action + "/" + nowStr;
			    int inputState = INPUT_OK;
			    try {
			        status = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//status", ""));
			        if(status == MRLog.ERROR){
			        	//eventCode = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//log/Code", String.valueOf(eventCode)));
			        	logData = MRXMLUtils.getValueXpath(doc, "//log", "");
			        	actionData = StreamTable.childrenToString(MRXMLUtils.getElementXpath(doc, "//actiondata"));
			        }
			    } catch (Exception e) {
			        status = MRLog.ERROR;
			        inputState = INPUT_RELAY_ERROR;
			    }
			
			    if (status == MRLog.OK) {
			        try {
			            this.handleRelayInputStreamStarted(inputElement, streamNID, streamURL, action, streamDataDoc);
			        } catch (Exception exp) {
			        	exp.printStackTrace();
			            logData = exp.toString();
			            status = MRLog.ERROR;
			        }
			    }
			    
			    MRXMLUtils.setAttributeValue(inputElement,"state", String.valueOf(inputState));			 

			    doc = StreamAgentUtils.createSourceEventMessage(streamNID, MediaRoomServiceConst.SOURCEEVENT, eventCode,
			            StreamTable.getStreamNIDXML(streamNID), 0, actionData, status, logData, log);
			   // doc = MRXMLUtils.stringToDocument(sourceEventXML);
			    action = MediaRoomServiceConst.SOURCEEVENT;
			}
			
			// Check if this is a SourceEvent that needs to be propogated
			if (MediaRoomServiceConst.SOURCEEVENT.equals(action)) {
			    String actionData = MRXMLUtils.elementChildrenToString(MRXMLUtils.getElementXpath(doc, "//sourceEventActionData"));
			    this.sendMessageToAllDestinations(client, doc, action, actionData);
			}
		}
		

		*/



}
