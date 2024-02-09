package com.ipvs.mediaroomservice.stream;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.mediastoreservice.handler.UpdateMediaPlayLengthRequestHandler;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class StreamUtils {
    private static final Hashtable<String, StreamUtils> streamUtilsList = new Hashtable<String, StreamUtils>();
    protected XMPPF xf;
    protected MRDB db;
    protected MRLog log;
    private MRResourceFactoryI resourceFactory = null;
    protected String userJID;
    boolean useDB = false;

    // These are the set of docs which will grow/trim over time
    
    private StreamTable streamTable;
    
    // This needs to go away
    private Hashtable<String, Element> recordingFileNIDList;

    public StreamUtils(XMPPF xf, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, String userJID, 
    		String deviceNID, String deviceType, StreamingServerI streamingServer) {
        this.xf = xf;
        this.log = log;
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.userJID = userJID;
        this.streamTable = new StreamTable(userJID, deviceNID, deviceType, streamingServer, log, xf);
        this.recordingFileNIDList = new Hashtable<String, Element>();
        
        StreamUtils.streamUtilsList.put(ServiceAgentUtils.getUserFromJID(userJID), this);
    }

    public static StreamUtils getStreamUtils(String agentJID) {
    	return StreamUtils.streamUtilsList.get(ServiceAgentUtils.getUserFromJID(agentJID));
    }
    
    public StreamTable getStreamTable() {
    	return this.streamTable;
    }

	public void createRxInstance(MRClient client, Document doc, String action, 
			String streamNID, String streamURL, Document streamDataDoc, Document mediaSourceDataDoc, String actionNID) throws Exception {
		this.streamTable.createRxInstance(client, doc, action, streamNID, streamURL, streamDataDoc, mediaSourceDataDoc, actionNID);
	}
	
	public void deleteRxInstance(String streamNID) throws Exception {
		this.streamTable.deleteRxInstance(streamNID);
	}

	public void handleRelayInputStreamResponse(MRClient client, Document doc, String action, 
			String streamNID, String streamURL, Document streamDataDoc)throws Exception {
		this.streamTable.handleRelayInputStreamResponse(client, doc, action, streamNID, streamURL, streamDataDoc);
	}
	
	public void releaseUDPStartPort(String startPort, String streamType) throws Exception {
		this.streamTable.streamingServer.getNetworkResource().releaseUDPStartPort(startPort, streamType);
	}
	
	public String getNextUDPStartPort(String streamType) throws Exception {
		return this.streamTable.streamingServer.getNetworkResource().getNextUDPStartPort(streamType);
	}
	
	public String getIPAddress() throws Exception {
		return this.streamTable.streamingServer.getIPAddress();
	}

    public boolean isRelayInput(String streamNID) {
    	Element inputElement = null;
    	try {
    		inputElement = MRXMLUtils.getElementXpath(this.streamTable.sourceTableDoc, "//Input[@streamNID='" + streamNID + "']");
    		if(inputElement == null)
    			return false;
    		String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
    		return MediaRoomServiceConst.INPUT_RELAY.equals(inputType);
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    	}
	    return false;
    }
    

    public void getStatus(Element rootElement) throws Exception {
    	this.streamTable.getStatus(rootElement);
    	ServiceAgentI destAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.DESTSTREAMAGENT);
	    DestStreamAgent destStreamAgent = (DestStreamAgent)destAgent;
	    if(destStreamAgent != null)
	    	rootElement.add((Element)destStreamAgent.getDestTableDoc().getRootElement().clone());
    }

    /**
     * @return the sourceTableDoc
     */
    public Document getSourceTableDoc() {
        return this.streamTable.sourceTableDoc;
    }

    public void propogateSourceEventToDestinations(MRClient client, Document doc, String action, String actionData) {
        //perf-logs this.log.addInfo(this.userJID + ": SourceTable: handleSourceEvents - Start", MRLog.OK, MRLog.NOTIFY);
        try {
            this.streamTable.sendMessageToAllDestinations(client, doc, action, actionData);
        } catch (Exception e) {
            e.printStackTrace();
        }

        //perf-logs this.log.addInfo(this.userJID + ": SourceTable: handleSourceEvents - end", MRLog.OK, MRLog.NOTIFY);
    }



    public void handleMediaStoreEvents(MRClient client, Document eventDoc, String action) throws MRException,Exception {
    	
  	  String mediaStreamFileNID = MRXMLUtils.getValueXpath(eventDoc, "//"+MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
  	  
  	  if(mediaStreamFileNID.isEmpty()){
  		  log.addInfo("HandleMediaStoreEvent :  No Media Stream File NID Found ",MRLog.INFO,MRLog.NOTIFY);
  		  return;
  	  }
  	  
  	  log.addInfo("SourceTable > Handle Media Store Event for MediaStreamFileNID:="+mediaStreamFileNID,MRLog.INFO,MRLog.NOTIFY);
  	  
  	  String strEventCode = MRXMLUtils.getValueXpath(eventDoc, "//eventCode", "");    	
  	  int eventCode = Utils.getIntValue(strEventCode, MediaRoomServiceConst.UNKNOWN_EVENT);
  	  if(eventCode == MediaRoomServiceConst.UNKNOWN_EVENT ){
  		  log.addInfo("HandleMediaStoreEvent :  Unknown Event Found for Media Stream File:="+mediaStreamFileNID+ " EventCode:="+eventCode,MRLog.INFO,MRLog.NOTIFY);
  		  return;
  	  }
    	
  	  String xPath = "//Instance[Input/@inputType='"+MediaRoomServiceConst.INPUT_MEDIAFILE+"' and Input/@sourceNID='"+mediaStreamFileNID+"']";    	  
  	  List<?> instanceElementList = MRXMLUtils.getListXpath(this.streamTable.sourceTableDoc, xPath);    	  
  	  if(instanceElementList.isEmpty()){
  		  log.addInfo("HandleMediaStoreEvent : No Streams found for MediaStreamFileNID:="+mediaStreamFileNID +". EventCode:="+eventCode,MRLog.INFO,MRLog.NOTIFY);    	    	
  		  return;
  	  }
  	  
  	  Element actionDataEL = null;
	  	  actionDataEL = MRXMLUtils.getElementXpath(eventDoc, "//"+MediaStoreServiceConst.MEDIA_STORE_EVENT+"ActionData/*");
		  if(actionDataEL == null){
			  log.addInfo("HandleMediaStoreEvent : No Action Data Found For MediaStreamFileNID:="+mediaStreamFileNID +". EventCode:="+eventCode,MRLog.INFO,MRLog.NOTIFY); 	    	
		    	
			  return;
		  }
    	  StringBuffer actionData = new StringBuffer();
  	  actionData.append(MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(eventDoc, "//"+MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID)));
  	  actionData.append(MRXMLUtils.elementToString(actionDataEL));    	  
  	  
  	  Iterator<?> instanceElementListIterator = instanceElementList.iterator();    	
  	  
  	  Element streamElement = null;    	 
  	  Element instanceElement = null;
  	  String streamXpath = "MediaSource/Stream";
  	  String streamNID = null;
  	  String destUserJID = null;
  	  
  	  while(instanceElementListIterator.hasNext()){
  		  instanceElement = (Element) instanceElementListIterator.next();
  		  streamElement = MRXMLUtils.getElementXpath(instanceElement, streamXpath);
  		  if(streamElement == null) {
  			  continue;
  		  }
  		  streamNID = MRXMLUtils.getAttributeValue(streamElement, "NID", "");   		  
  		  destUserJID = MRXMLUtils.getAttributeValue(streamElement, "ownerUser", "");  
  		  if(streamNID.isEmpty() || destUserJID.isEmpty() ){
  			  continue;
  		  }
  		Document eventDataDoc = StreamAgentUtils.createSourceEventMessage(streamNID, MediaRoomServiceConst.SOURCEEVENT, eventCode, "", 0, actionData.toString(), MRLog.OK, null, log);
        //  Document eventDataDoc = MRXMLUtils.stringToDocument(eventXML);
          this.propogateSourceEventToDestinations(client, eventDataDoc, MediaRoomServiceConst.SOURCEEVENT, actionData.toString());  
  	  }
	}
    
    public void handleDestinationAgentMediaStoreResponses(MRClient client, Document responseDoc) throws MRException, Exception {  


    	String mediaStreamFileNID = MRXMLUtils.getValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
    	if(mediaStreamFileNID == null ) {
    		return;
    	}
    	int state = MRLog.OK;
    	String logData = null;
    	try{
    		MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, MediaStoreServiceExceptionCodes.MEDIA_STORE_ADMIN_RESPONSE_FAILED);
    	} catch (MRException exp) {
            state = MRLog.ERROR;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());           
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRLog.ERROR;                       
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.MEDIA_STORE_ADMIN_RESPONSE_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        } 
        
        if(state != MRRequest.STATE_EXCEPTION){
        	return;
        }
        log.addInfo("Media Store Response Failed. MediaStreamFileNID:="+mediaStreamFileNID +" LogData:"+logData,MRLog.INFO,MRLog.NOTIFY);
        String outputXPath = "//Stream[Output/@destNID='"+mediaStreamFileNID+"']";
        Element streamElement = MRXMLUtils.getElementXpath(this.streamTable.sourceTableDoc,outputXPath);
        if(streamElement == null) {
        	log.addInfo("No Streams Found for to Generate Destination Event for Failure Response of MediaStore Server Request",MRLog.INFO,MRLog.NOTIFY);
        }
        String streamNID = MRXMLUtils.getAttributeValue(streamElement, "NID", "");
        if(streamNID.isEmpty()){
        	return;
        }
       	sendDestEventMessage(streamNID, MediaRoomServiceConst.MEDIA_STORE_ERROR, "", 0, 0, "", state, logData); 
    }

    
    public void handleDestinationAgentSSEvents(MRClient client, Document doc, String action, String actiondata)
    throws MRException, Exception {

    	Document actionDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(doc, "//SSEventActionData"));

    	String ssInstanceID = MRXMLUtils.getValueXpath(actionDoc, "//Id", "");

    	if (ssInstanceID.isEmpty()) {
    		return;
    	}

    	String event = getSSEventType(actionDoc);

    	if (event == null) {
    		return;
    	}

    	String type = MRXMLUtils.getValueXpath(actionDoc, "//Type", "");

    	// TBD : In future we may going to be implement for all the instance types 
    	if (type.isEmpty()) {
    		return;
    	}    	

    	if (event.equals(MediaRoomServiceConst.SSINSTANCE_STARTED) || event.equals(MediaRoomServiceConst.SSINSTANCE_CONNECTED_STARTED)) {
    		Element outputElement = getElementFromSourceTableDoc("//Output[@ssInstanceID='" + ssInstanceID + "']");
    		this.recordingFileNIDList.put(ssInstanceID, (Element)outputElement.clone());

    		String startTC = MRXMLUtils.getValueXpath(actionDoc, "//StartTimeStamp", "");
    		if(startTC.isEmpty()){
    			return;
    		}
    		String wallClock = MRXMLUtils.getValueXpath(actionDoc, "//StartWallClock", "");
    		handleRecordOutputInstanceEvents(client, actionDoc, event, MediaRoomServiceConst.RECORDING_STARTED, actiondata, ssInstanceID, Utils.getLongValue(startTC, -1), Utils.getLongValue(wallClock, -1), MRLog.OK, "");
    		// System.out.println("Added RecordingOutput size=" + this.recordingFileNIDList.size());
    	} else if (event.equals(MediaRoomServiceConst.SSINSTANCE_STOPPED) || 
    			event.equals(MediaRoomServiceConst.SSINSTANCE_CONNECTED_STOPPED)) {
    		Element outputElement = this.recordingFileNIDList.get(ssInstanceID);
    		this.recordingFileNIDList.remove(ssInstanceID);

    		if(outputElement == null) // This will happen if there are no bits comming in no SSINSTANCE_STARTED event
    			return;
    		
    		String fileNID = MRXMLUtils.getAttributeValue(outputElement,"destNID");
    		// System.out.println("Removed RecordingOutput size=" + this.recordingFileNIDList.size());     		  
    		if(fileNID == null || fileNID.isEmpty()){
    			return ;
    		}

    		String endTC = MRXMLUtils.getValueXpath(actionDoc, "//StopTimeStamp", "");
    		if(endTC.isEmpty()){
    			return;
    		}
    		
    		String wallClock = MRXMLUtils.getValueXpath(actionDoc, "//StopWallClock", "");
    		if(wallClock.isEmpty()){
    			return;
    		}
    	
    		//MediaStoreServiceUtils.sendInternalRequestToMediaStoreAgent(this.userJID, storeFileNID, MediaStoreServiceConst.UPDATE_MEDIA_PLAYLENGTH_REQUEST, fileNID, fileDirNID, actionDoc, userJID, xmpp, log);    	
     		handleRecordOutputInstanceEvents(client, actionDoc, event, MediaRoomServiceConst.RECORDING_STOPPED, actiondata, ssInstanceID, Utils.getLongValue(endTC, -1), Utils.getLongValue(wallClock, -1), MRLog.OK, "");
        	
     	} else if(MediaRoomServiceConst.SSINSTANCE_ERRORS.equals(event)) {     	  		
     		Element outputElement = getElementFromSourceTableDoc("//Output[@ssInstanceID='" + ssInstanceID + "']");
     	
     		if(outputElement == null) {
     			return;
     		}
    		String fileNID = MRXMLUtils.getAttributeValue(outputElement,"destNID");
    		
    		if(fileNID == null) {
    			return;
    		}
    		String wallClock = MRXMLUtils.getValueXpath(actionDoc, "//WallClock", "");  
    		String  errorDetails = MRXMLUtils.getValueXpath(actionDoc, "//ErrorDetail", "");
    		
     		handleRecordOutputInstanceEvents(client, actionDoc, "" , MediaRoomServiceConst.RECORDING_ERROR, fileNID, ssInstanceID,0, Utils.getLongValue(wallClock, 0), MRLog.ERROR, errorDetails);     	   
     	}
      
    }
    
    //Handle Record Output Events. And Create dest event from ssevent and send it to destination.
    private void handleRecordOutputInstanceEvents(MRClient client, Document doc, String event, int eventCode, String actiondata, String ssInstanceID, long timeStamp,long wallClock,int status,String logData)
    throws MRException, Exception {
    	Element streamElement = getElementFromSourceTableDoc("//Stream[Output/@ssInstanceID='" + ssInstanceID + "']");

    	if (streamElement == null) {
    		return;
    	}

    	String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");

    	if (streamNID == null) {
    		return;
    	}
    	
    	sendDestEventMessage(streamNID, eventCode, event, timeStamp, wallClock, actiondata, status, logData);    	
	}
    
    private void sendDestEventMessage(String streamNID,int eventCode,String event,long timeStamp,long wallClock,String actionData,int status, String logData) throws MRException,Exception{
    	Document destEventDoc = StreamAgentUtils.createDestinationEventMessage(streamNID, MediaRoomServiceConst.DESTINATIONEVENT, eventCode, event, timeStamp, wallClock, actionData, status, logData,log);
		Document streamDataDoc = this.streamTable.getStreamData(streamNID);    		
	    String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");	    
        String info = "DestStreamAgent:=" + this.userJID + "/" + streamNID + " > DestinationEvent Message action=" + MediaRoomServiceConst.DESTINATIONEVENT + " to " + ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.DESTSTREAMAGENT);
	    String agentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");	    //WILDCARD
       
        
        long eventWallclock = this.xf.xmpp(userJID).getCurrentTimeMillis();
        ServiceAgentUtils.sendAgentEvent(xf.xmpp(userJID), log, roomNID, "", agentJID, MediaRoomServiceConst.DESTSTREAMAGENT, MRLog.OK, this.userJID, eventWallclock, MediaRoomServiceConst.DESTINATIONEVENT, destEventDoc.getRootElement(), logData , info);
		//ServiceAgentUtils.sendAgentEventMessage(this.xmpp, this.log, roomNID, MediaRoomServiceConst.WILDCARD, MediaRoomServiceConst.DESTSTREAMAGENT, MediaRoomServiceConst.SOURCEEVENT, destEventXML, info);	   	
   
    }

	private void sendSourceEventMessage(Element streamElement, MRClient client, long eventWallclock, int eventLevel, 
			Document sourceEventDoc) throws Exception {
        String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
        String roomNID = MRXMLUtils.getAttributeValue(streamElement,"roomNID");
	    String serviceName = MRConst.LAYOUT_SERVICE;
	    String eventCode = MRXMLUtils.getValueXpath(sourceEventDoc, "//eventCode", "");
        String agentJID = MRXMLUtils.getAttributeValue(streamElement,"destAgentJID");
	    String info = "SourceStreamAgent:=" + client.getUserJID() + "/" + streamNID + " > SourceEvent Message to " +
	        ServiceAgentUtils.getAgentShortName(serviceName) + " EventCode: "+eventCode;
	    ServiceAgentUtils.sendAgentEvent(client.getXmpp(),client.getLog(), roomNID, MediaRoomServiceConst.SOURCESTREAMAGENT, 
	    	agentJID, serviceName, eventLevel, client.getUserJID(), 
	    		eventWallclock, MediaRoomServiceConst.SOURCEEVENT, sourceEventDoc.getRootElement(), "", info);
	}

    
    private String getSSEventType(Document ssEventDoc) throws MRException, Exception {
        String event = MRXMLUtils.getValueXpath(ssEventDoc, "//EventType", "");

        if (event.isEmpty()) {
            return null;
        }

        /*if (event.equals(MediaRoomServiceConst.SSINSTANCE_ERRORS)) {
            //TBD : Once Rahul include the Error codes then i have to implements error handling also
            return null;
        }*/

        return event;
    }

    public void handleSourceAgentSSEvents(MRClient client, Document doc, String action, String actiondata)
        throws MRException, Exception {
        Document actionDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(doc, "//SSEventActionData"));
 
        String event = getSSEventType(actionDoc);

        if (event == null) {
            return;
        }

        // System.out.println("StreamUtils:handleSourceAgentSSEvents doc=" + MRXMLUtils.documentToString(actionDoc));
        String ssInstanceID = MRXMLUtils.getValueXpath(actionDoc, "//Id", "");

        if (ssInstanceID.isEmpty() && !event.equals(MediaRoomServiceConst.DISK_QUOTA_EXCEEDED_EVENT)) {
            return;
        }

        
        String type = MRXMLUtils.getValueXpath(actionDoc, "//Type", "");

        // TBD : In future we may going to be implement for all the instance types 
        if (type.isEmpty() || type.equals(MediaRoomServiceConst.V2D_ENCODER_PORT)) {
            return;
        }
        
        log.addInfo("HandleSSEvents Type:="+type +". Event:="+event, MRLog.OK, MRLog.NOTIFY);

        if (type.equals(MediaRoomServiceConst.DMS_MEDIA_PORT)) {
            if (event.equals(MediaRoomServiceConst.SSINSTANCE_END_OF_MEDIA)) {
            	handleStreamingServerInputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.END_OF_MEDIA_REACHED, "", ssInstanceID,
                    MediaRoomServiceExceptionCodes.END_OF_THE_MEDIA, "End Of the Media File");
            } else if (event.equals(MediaRoomServiceConst.SSINSTANCE_ERRORS)) {
            	
            	String  errorDetails = MRXMLUtils.getValueXpath(actionDoc, "//ErrorDetail", "");        		
            	handleStreamingServerInputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.MEDIA_ERROR, "", ssInstanceID,
                    MediaRoomServiceExceptionCodes.MEDIA_ERROR, errorDetails);
            }
        } else if (type.equals(MediaRoomServiceConst.V2D_NETWORK_PORT)) {
            if (event.equals(MediaRoomServiceConst.SSINSTANCE_DISCONNECTED)) {
            	handleStreamingServerInputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.SOURCE_DISCONNECTED, "", ssInstanceID,
            		MediaRoomServiceExceptionCodes.RELAY_DEVICE_DISCONNECTED, "Source Disconnected");
            }
            if (event.equals(MediaRoomServiceConst.SSINSTANCE_CONNECTED)) {
            	handleStreamingServerInputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.SOURCE_CONNECTED, "", ssInstanceID,
            		MRLog.OK, "Source Connected");
            }
        } else if (event.equals(MediaRoomServiceConst.SSEVENT_PROGRESS) && type.equals(MediaRoomServiceConst.DMS_RECORDER_PORT)) {
        	handleProgressStoppedEvent(doc,  ssInstanceID);        	
        } else if (event.equals(MediaRoomServiceConst.DISK_QUOTA_EXCEEDED_EVENT)) {
    		handleDiskQuotaExceededEvent(client, doc);
    	} else {// if (type.equals(MediaRoomServiceConst.V2D_DECODER_PORT)) {
            if (event.equals(MediaRoomServiceConst.SSINSTANCE_STARTED) || event.equals(MediaRoomServiceConst.SSINSTANCE_CONNECTED_STARTED)) {
                String startTC = MRXMLUtils.getValueXpath(actionDoc, "//StartTimeStamp", "");
                String startWC 
                = MRXMLUtils.getValueXpath(actionDoc, "//StartWallClock", "");

                if ((startTC != null) && (startTC.trim().length() != 0)) {
                	// System.out.println("StreamUtils:handleSourceAgentSSEvents tc=" + startTC);
                    handleStreamingServerOutputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.STREAM_START_TC, "", ssInstanceID, Long.parseLong(startTC), Long.parseLong(startWC), MRLog.OK,"");
                }
            }
	        if (event.equals(MediaRoomServiceConst.SSINSTANCE_DISCONNECTED) ) {
	        	 handleStreamingServerOutputInstanceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.DEST_DISCONNECTED, "", ssInstanceID,0,0, MediaRoomServiceExceptionCodes.DEST_DISCONNECTED,"Destination Disconnected");
	        }
	      }
        //}      
    }

    private void handleProgressStoppedEvent(Document doc, String ssInstanceID) throws MRException, Exception {
    	
    	//delete record port and update media playlength only if there is no stream running. If stream is running update playlength and size in stream instance
    	Element outputElement = getElementFromSourceTableDoc("//Output[@ssInstanceID='" + ssInstanceID + "']");
    	if (outputElement != null) {
    		MRXMLUtils.setAttributeValue(outputElement, "size", MRXMLUtils.getValueXpath(doc, "//BytesWritten","0"));
    		MRXMLUtils.setAttributeValue(outputElement, "playLength", MRXMLUtils.getValueXpath(doc, "//PlayLength","0"));
    		return;
    	}
    	
    	//delete record port instance
    	streamTable.deleteRecordPortOnProgressStoppedEventFromStreamingServer(ssInstanceID);    	
    	
    	//update media playlength
    	UpdateMediaPlayLengthRequestHandler updatePlaylength = new UpdateMediaPlayLengthRequestHandler();
    	String storeFileNID = MRXMLUtils.getValueXpath(doc, "//MediaName", "");
    	String dirPath = MRXMLUtils.getValueXpath(doc, "//MediaDirectory", "");
    	String dirNID = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID).getMediaStoreDirNIDBasedOnRootPath(dirPath);    	
    	Document requestDoc = MediaStoreServiceUtils.generateRequestDocument(MRRequest.getRequestName(UpdateMediaPlayLengthRequestHandler.class.getName()), storeFileNID, "", dirNID, MRXMLUtils.documentToStringOmitDeclaration(doc), "", log);
    	MRRequest request  = MRRequest.createRequest(this.userJID, requestDoc); 
    	try {
			updatePlaylength.getResponse(request, xf.xmpp(userJID), log, db, this.resourceFactory, this.xf.sf(userJID));
		} catch (Exception e) {
			String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
			String childXpath = "//" + AMConst.STATEFLAG;
			String whereXpath[] = { "//"+MediaStoreServiceConst.UUID+"='"+storeFileNID+"'" ,"//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+"='"+dirNID+"'"};
			String statusXML = MRXMLUtils.generateXML(AMConst.STATEFLAG, MediaStoreServiceConst.ERROR);
			MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType, "", childXpath, whereXpath, MRXMLUtils.stringToElement(statusXML));
		}
    	   
	}
 
 	private void handleDiskQuotaExceededEvent(MRClient client, Document doc) throws MRException, Exception {
 		log.addInfo("StreamUtils:Stop the streams and mark them as error", MRLog.OK, MRLog.NOTIFY);
 		int eventCode = MediaRoomServiceConst.DISK_QUOTA_EXCEEDED;
 		String dirPath = MRXMLUtils.getValueXpath(doc, "//MediaDirectory", ""); 		
 		Document sourceTableDoc = getSourceTableDoc(); 		
		
        List<?> streamList = MRXMLUtils.getListXpath(sourceTableDoc, "//Stream[Output/@dirPath='" + dirPath + "']");
 		if ((streamList == null) || (streamList.size() == 0)) {
 		   return;
 		}        
        
        String info = MediaRoomServiceUtils.generateErrorXML(MediaStoreServiceExceptionCodes.DISK_QUOTA_EXCEEDED, "Disk quota exceeded");
        String actiondata = info;
        
        for (Object o : streamList) {
        	Element streamEl = (Element) o;    	
        	String streamNID = MRXMLUtils.getAttributeValue(streamEl, AMConst.NID);        	        	        	
        	Document sourceEventDoc = StreamAgentUtils.createSourceEventMessage(streamNID, MediaRoomServiceConst.SOURCEEVENT, eventCode, "", 0, 
        			actiondata, MRLog.ERROR, info, log);
        	long eventWallclock = this.xf.xmpp(userJID).getCurrentTimeMillis();        	        	        	
			this.streamTable.stopSSOutputOnDiskFull(streamEl, eventCode);			       
        	this.sendSourceEventMessage(streamEl, client, eventWallclock, MRLog.ERROR, sourceEventDoc);
        }	                	       
	}

	private void handleStreamingServerInputInstanceEvents(MRClient client, Document doc, String action, int eventCode, String actiondata, String ssInstanceID, int code, String message)
        throws MRException, Exception {
        
    	Element inputElement = getElementFromSourceTableDoc("//Input[@ssInstanceID='" + ssInstanceID + "']");
        if (inputElement == null) {
            return;
        }
        
        Element instanceElement = MRXMLUtils.getParentElement(inputElement);     
        if (instanceElement == null) {
        	return;
        }
                
        List<?> streamList = MRXMLUtils.getListXpath(instanceElement, "MediaSource/Stream");
        if ((streamList == null) || (streamList.size() == 0)) {
		    return;
		}        
        
        int status = MRLog.OK;
        String state = String.valueOf(StreamTable.INPUT_OK);
        String info = "";
        if(MRLog.OK != code) {
        	status = MRLog.ERROR;
        	state = String.valueOf(StreamTable.INPUT_RELAY_ERROR);
        	info = MediaRoomServiceUtils.generateErrorXML(code, message);
        	actiondata = info;
        }
        MRXMLUtils.setAttributeValue(inputElement, "state", state);
		this.streamTable.sendMessagToAllDestinationsInternal(streamList, client, action, eventCode, actiondata, status, info, "", 0);

    }
    
    private void handleStreamingServerOutputInstanceEvents(MRClient client, Document doc, String action, int eventCode, String actiondata, String ssInstanceID, 
    		long startTC, long startWC, int code, String message)
    throws MRException, Exception {
    	Element streamElement = getElementFromSourceTableDoc("//Stream[Output/@ssInstanceID='" + ssInstanceID + "']");

    	if (streamElement == null) {
    		return;
    	}

    	String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
    	int status = MRLog.OK;
        String info = "";
        if(MRLog.OK != code) {
        	status = MRLog.ERROR;
        	info = MediaRoomServiceUtils.generateErrorXML(code, message);
        	actiondata = info;
        }
    	if (streamNID != null) {
        	if(eventCode == MediaRoomServiceConst.STREAM_START_TC) {
        		streamElement.addAttribute("ssTC", String.valueOf(startTC));
        		streamElement.addAttribute("ssWC", String.valueOf(startWC));
        		MediaTimeline.adjustStreamMediaTimeline(streamElement.element("MediaTimeline"), startTC, startWC, this.log);
        	}
        	
        	Element actiondataElement = MRXMLUtils.stringToElement("<actiondata/>");
        	actiondataElement.add((Element)streamElement.element("MediaTimeline").element("Last").clone());
        	Document sourceEventDoc = StreamAgentUtils.createSourceEventMessageWithActionDataElement(streamNID, action, eventCode, "", startTC, 
        			actiondataElement, status, info, log);
        	
        	// System.out.println("StreamUtils:handleStreamingServerOutputInstanceEvents sourceEventDoc=" + MRXMLUtils.documentToString(sourceEventDoc));
        	
            long eventWallclock = this.xf.xmpp(userJID).getCurrentTimeMillis();
            this.sendSourceEventMessage(streamElement, client, eventWallclock, MRLog.OK, sourceEventDoc);
            
        	// System.out.println("StreamUtils:handleStreamingServerOutputInstanceEvents actionData=" + actiondata + 
        	//		" eventDoc=" + MRXMLUtils.documentToString(sourceEventDoc));
            
            
    	    // this.streamTable.sendMediaSourceEvent(client, instanceEL, MRLog.INFO, MediaRoomServiceConst.RELAYEVENT, relayData);
    		// this.propogateSourceEventToDestinations(client, eventDataDoc, action, actiondata);
    	}
    }

    private Element getElementFromSourceTableDoc(String xPath) throws MRException, Exception {
        return MRXMLUtils.getElementXpath(getSourceTableDoc(), xPath);
    }    

    //put callback argument to get the callback dest
    public void sendDestMessage(Document streamDataDoc, MRClient client, 
    		String requestNID, String streamNID, String userAction, String request, String actiondata, 
    		String streamURL, String relayRoomNID, String relayStreamNID, String actionNID, int status, String logdata,

    		Element mediaTimeline, String clientData, String clientCallback) throws Exception {
        // String name = MediaRoomServiceConst.DESTSTREAMAGENT;
        String info = "SourceStreamAgent:=" + this.userJID + "/" + streamNID + " > Response action =" + request + " to " + ServiceAgentUtils.getAgentShortName(clientCallback);

        String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
        String response = StreamAgentUtils.createSourceStreamAgentResponseMessage(roomNID, streamNID, userAction, request, actiondata, streamURL, relayRoomNID, relayStreamNID, actionNID, status, logdata, log, mediaTimeline);
        String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");

        //perf-logs log.addInfo(this.userJID + ": SourceStreamAgentUtils: sendAgentMessage - Start" , MRLog.OK, MRLog.NOTIFY);        
        /*
        int requestState = MRLog.OK;

        if (status != MRLog.OK) {
            requestState = MRRequest.STATE_EXCEPTION;
        }
        */
        int requestState = status;

        String responseName = MRRequest.getResponseName(request);
        ServiceAgentUtils.sendAgentResponseMessage(xf.xmpp(userJID), log,roomNID, destAgentJID, 
        		clientCallback, MRConst.LAYOUT_SERVICE, responseName, response, info, requestState, logdata, requestNID, this.userJID, clientData);
       

        //perf-logs log.addInfo(this.userJID + ": SourceStreamAgentUtils: sendAgentMessage - End" , MRLog.OK, MRLog.NOTIFY);
    }

    // Stream Info Methods
	public List<?> getStreamList(String streamXPath) throws Exception {
		return this.streamTable.getStreamList(streamXPath);
	}
	
	public long getCurrentStreamTimestamp(String streamNID) throws Exception {
   	   	String streamXPath = "//Stream[@NID='" + streamNID + "']";
    	Element streamElement = this.streamTable.getStream(streamXPath);
    	long timestamp = 0;
    	if (streamElement != null) {
    	    timestamp = this.streamTable.getCurrentTimeStamp(streamElement);
    	}
    	return timestamp;
	}
    public long getOffset(String mediaUUID,String timecodeStr, String srcDirPath) throws MRException,Exception {
    	return this.streamTable.getOffset(mediaUUID, timecodeStr, srcDirPath);
    }
    public long getSyncTC(String sourceUUID, long offset, String rootDir) throws Exception {
    	return this.streamTable.getSyncTC(sourceUUID, offset, rootDir);
    }

    
}
