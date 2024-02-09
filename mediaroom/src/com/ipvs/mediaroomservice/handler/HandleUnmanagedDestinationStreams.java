package com.ipvs.mediaroomservice.handler;

import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Semaphore;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class HandleUnmanagedDestinationStreams  implements AgentResponseHandlerI {
	public static final String SDP_FILE_LOCATION = "/home/onair/modules/mediaroom/public";
	public static final int RESOURCENOTAVAILABLE = 1006; 
    public static Hashtable<String,Set<String>> unManagedDestinationStreamCache = new Hashtable<String,Set<String>>();
    
    
    public static void addStreamNID (String streamNID, String roomNID,MRLog log) {
    	if(!unManagedDestinationStreamCache.containsKey(roomNID)) {
    		unManagedDestinationStreamCache.put(roomNID, new HashSet<String>());
    	}
    	log.addInfo("HandleUnmanagedDestinationStreams: Add streamNID in cache for RTP direct connections streamNID:="+streamNID +" roomNID :="+roomNID);
    	unManagedDestinationStreamCache.get(roomNID).add(streamNID);
    }
    
    public static Set<String> deleteRoomFromCache(String roomNID,MRLog log) {
    	
    	if(unManagedDestinationStreamCache.containsKey(roomNID)) {
    		log.addInfo("HandleUnmanagedDestinationStreams: remove RTP direct connections cache for  roomNID :="+roomNID);
    		return unManagedDestinationStreamCache.remove(roomNID);
    	}
    	return null;
    }
    
	public void sendStartRequest(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,String roomNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc) throws MRException,Exception {
		
		String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
		String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
	
		// we have to do search on media room list to find out any other stream is available for same destportelementid
		String destPortRoomNID=null;
		try {
			destPortRoomNID = MediaRoomServiceUtils.getRoomNIDWithXPath(request, xmpp, log, db, "//MediaRoom[@NID!='"+roomNID+"']//Stream/data[destNID='"+destNID+"' and destPortElementID='"+destPortElementID+"']");
		} catch (Exception e) {
			//ignore exception
			destPortRoomNID = null;
		}
		if(destPortRoomNID != null) {
			throw new MRException(RESOURCENOTAVAILABLE, "port is in use. "+destPortElementID+" not available for streamNID=" + streamNID);
		}
		
		
		//TODO: check stream url is not empty or not
		String streamURL = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURL", "");
	    if(streamURL.isEmpty()) {
	    	sendStartStopRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, mediaSourceDataDoc, MediaRoomServiceConst.START);
	    	MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
              		String.valueOf(MediaRoomServiceConst.WAITING_FOR_STREAMURL));
	    } else {
	    	Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
	    	
	    	String streamType = MRXMLUtils.getValueXpath(streamDoc, "//streamType", "");
	    	if(streamType.equals(MediaRoomServiceConst.RTP)) {
	    		streamURL = SDPUtils.fixsdp(streamURL);
	    		String sdpFileName = streamNID+".sdp";
	    		FileUtils.writeFile(SDP_FILE_LOCATION+"/"+sdpFileName, streamURL);
			
	    		Document destdoc = MRXMLDBUtils.readElement(request, log, db, MRXMLUtils.getValueXpath(streamDoc, "//destNID", "")); //read from cache no extra db hit
	    		Element msIPAddressinfo = AppServerAgentUtils.getMSResourceInfo(log, xmpp, db, serviceAgentFactory, request, AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getAPIKey(), 
	    				MRXMLUtils.getValueXpath(destdoc, "//Title", ""),MRXMLUtils.getValueXpath(destdoc, "//destAgentJID", ""));
	    	    String msIPAddress =  MRXMLUtils.getAttributeValue(msIPAddressinfo, "ipAddress","");
	    	    streamURL=   "http://" + msIPAddress + "/mediaroom/" + sdpFileName;
	    	    addStreamNID(streamNID, roomNID,log);
	    	}
	    	
	    	System.out.println(streamURL);
	    	Element statusEl = MRXMLUtils.getElementXpath(streamDoc, "//Status");
	    	
	    	statusEl.element("state").setText(String.valueOf(MediaRoomServiceConst.STREAM_STARTED));
	    	MRXMLUtils.addElement(statusEl, "URL", streamURL);
	    	MediaRoomServiceUtils.updateStreamStatus(xmpp, request, db, log, roomNID, streamNID, statusEl);
	    }

	}
	
	public void sendStopRequest(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,String roomNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc) throws MRException,Exception {
		
		String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
		if(streamType.equals(MediaRoomServiceConst.RTP)) {
			this.releaseURL(streamNID);
		}
			
		
		sendStartStopRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, mediaSourceDataDoc, MediaRoomServiceConst.STOP);
	}


	public void sendStartStopRequest(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,String roomNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc,String action) throws MRException,Exception {

		String actionNID = roomNID;
		Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
		String toAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID", "");	    
		String requestName = action +"Request";
		String actionMessage = StreamAgentUtils.createSourceStreamAgentActionRequestMessage(streamNID, action, requestName, 
				actiondata, 0, actionNID, streamDataDoc, mediaSourceDataDoc, log);	     
		AppServerAgent.sendAgentRequestForUserRequest(MRRequest.createRequest(request.getAppServerJID()), xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				toAgentJID, requestName, MRConst.MEDIA_SERVICE, actionMessage, this.getClass().getName());

	}
	
	@Override
	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document responseDoc = request.getResult();	

		String action = StringUtils.substringBefore(request.getRequestName(), "Request");
		String streamURL = MRXMLUtils.getValueXpath(responseDoc, "//"+SessionServiceConst.URL, "");		
		String streamNID = MRXMLUtils.getAttributeValueXpath(responseDoc, "//StreamStatus",SessionServiceConst.STREAM_NID , "");		
		String roomNID = MRXMLUtils.getAttributeValueXpath(responseDoc, "//StreamStatus",SessionServiceConst.ROOMNID , "");
		log.addInfo("HandleUnmanagedDestinationStreams:handleAgentResponse::roomNID= "+roomNID+ ": StreamURL"+streamURL, MRLog.OK, MRLog.NOTIFY);


		if(!MediaRoomServiceConst.START.equals(action)){
			return; 
			//no need to handle stop response.
		}
		log.addInfo("HandleUnmanagedDestinationStreams :Handle StartResponse aquire lock on  StreamNID:="+streamNID);
		Semaphore lock = IPVSMonitor.getMonitor().getLock(streamNID, request.getNID());				
		log.addInfo("HandleUnmanagedDestinationStreams :Handle StartResponse aquired lock on  StreamNID:="+streamNID );

		try{

			Element statusEl = null;
			int state =MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
			if(streamURL.isEmpty() || streamURL.equals("")){

				String mediasourcedatadocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID);
				Document streamDataDoc = MRXMLUtils.stringToDocument(mediasourcedatadocStr);
				statusEl = MRXMLUtils.getElementXpath(streamDataDoc, "//Status");
				Element lastEventData = MRXMLUtils.getElementXpath(statusEl, "./lastEventData");

				Element errorElement = MRXMLUtils.getElementXpath(responseDoc, "//BarcoError");
				if(lastEventData != null) {
					lastEventData.clearContent();
				}

				MRXMLUtils.addElement(lastEventData,(Element) errorElement.detach());

				state = MediaRoomServiceConst.START_ERROR;

			} else{
				statusEl =(Element) MRXMLUtils.getElementXpath(responseDoc, "//StreamStatus");
			}
			statusEl.element("state").setText(String.valueOf(state));
			MediaRoomServiceUtils.updateStreamStatus(xmpp, request, db, log, roomNID, streamNID, statusEl);
		}
		catch(Exception e){
			MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
					String.valueOf(MediaRoomServiceConst.START_ERROR));
		}finally  {
			log.addInfo("HandleUnmanagedDestinationStreams:Handle StartResponse release lock  StreamNID:="+streamNID);
			IPVSMonitor.getMonitor().releaseLock(streamNID, lock);
		}

	}

	private void updateLastEventData(Element statusEl, Element eventEl) throws Exception {
		Element lastEventData = MRXMLUtils.getElementXpath(statusEl, "./lastEventData");

		if(lastEventData != null) {
			lastEventData.clearContent();
		}

		MRXMLUtils.addElement(lastEventData,(Element) eventEl.detach());
	}

	public void handleSourceEvent(XMPPI xmpp,  MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,Document eventDoc)  throws Exception {

		String streamNID = MRXMLUtils.getValueXpath(eventDoc,"//streamNID","");
		Element headerEl = MRXMLUtils.getElementXpath(eventDoc, "//Header");   
		int eventLevel = Utils.getIntValue(headerEl.attributeValue("eventLevel"), 0);
		int eventCode = Utils.getIntValue(MRXMLUtils.getValueXpath(eventDoc, "//eventCode", "0"),0);
		String userJID = AppServerAgentUtils.getAppServerAgentJID("", serviceAgentFactory);
		MRRequest request = MRRequest.createRequest(userJID);
		
		
		log.addInfo("HandleUnmanagedDestinationStreams :handleSourceEvent aquire lock on  StreamNID:="+streamNID);
		Semaphore lock = IPVSMonitor.getMonitor().getLock(streamNID, request.getNID());				
		log.addInfo("HandleUnmanagedDestinationStreams :handleSourceEvent aquired lock on  StreamNID:="+streamNID );

		try {

			String roomNID = MediaRoomServiceUtils.getRoomNID(request ,  xmpp,  log,  db,  streamNID);

			Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
			Element statusEl = MRXMLUtils.getElementXpath(streamDoc, "//Status");
			String state = MRXMLUtils.getValueXpath(statusEl, "./state",""); 
			if(eventLevel == MRLog.OK){ 
				if(eventCode == MediaRoomServiceConst.STREAM_START_TC || eventCode == MediaRoomServiceConst.SOURCE_CONNECTED || eventCode == MediaRoomServiceConst.TIMELINEUPDATEEVENT) { // Only handling stated for now
					state = String.valueOf(MediaRoomServiceConst.STREAM_STARTED);
					//update media time line
					if(eventCode == MediaRoomServiceConst.STREAM_START_TC || eventCode == MediaRoomServiceConst.TIMELINEUPDATEEVENT) {

						Element mediaTimeline = MRXMLUtils.getElementXpath(statusEl, "./MediaTimeline");
						if(mediaTimeline == null) {
							statusEl.add(MRXMLUtils.getElementXpath(eventDoc, "//MediaTimeline").detach());	
						} else {
							MRXMLUtils.removeChild(mediaTimeline, "Last");
							statusEl.add(MRXMLUtils.getElementXpath(eventDoc, "//Last").detach());
						}
					}

					//update last event data
					if(eventCode != MediaRoomServiceConst.TIMELINEUPDATEEVENT)
						updateLastEventData(statusEl, MRXMLUtils.getElementXpath(eventDoc, "//Data"));
				}			
			} else if(eventLevel != MRLog.OK) {
				state = String.valueOf(MediaRoomServiceConst.SOURCE_ERROR);
				Element errorElement = MRXMLUtils.getElementXpath(eventDoc, "//Exception");
				updateLastEventData(statusEl, errorElement);
			}

			MRXMLUtils.setValueXpath(statusEl, "./state", state);
			MediaRoomServiceUtils.updateStreamStatus(xmpp, request, db, log, roomNID, streamNID, statusEl);

		} catch (Exception e) {
			e.printStackTrace();
		}finally  {
			log.addInfo("HandleUnmanagedDestinationStreams:handleSourceEvent release lock  StreamNID:="+streamNID);
			IPVSMonitor.getMonitor().releaseLock(streamNID, lock);
		}
	}	


	public void handleRelayEvent(XMPPI xmpp,  MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,Document eventDoc) throws Exception {

		String streamNID = MRXMLUtils.getValueXpath(eventDoc,"//streamNID","");

		String userJID = AppServerAgentUtils.getAppServerAgentJID("", serviceAgentFactory);
		MRRequest request = MRRequest.createRequest(userJID);
	
		log.addInfo("HandleUnmanagedDestinationStreams :handleRelayEvent aquire lock on  StreamNID:="+streamNID);
		Semaphore lock = IPVSMonitor.getMonitor().getLock(streamNID, request.getNID());				
		log.addInfo("HandleUnmanagedDestinationStreams :handleRelayEvent aquired lock on  StreamNID:="+streamNID );

		try {
	    	String roomNID = MediaRoomServiceUtils.getRoomNID(request ,  xmpp,  log,  db,  streamNID);

			Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
			
			Element statusEl = MRXMLUtils.getElementXpath(streamDoc, "//Status");
			
			Element relatDataEl = MRXMLUtils.getElementXpath(streamDoc, "//relayData");
			Element relayDataInEvent = MRXMLUtils.getElementXpath(eventDoc, "//RelayData");
			String relayRoomNID = relayDataInEvent.attributeValue("roomNID");
			String relayStreamNID = relayDataInEvent.attributeValue("streamNID");
			relatDataEl.addAttribute("relayRoomNID", relayRoomNID);
			relatDataEl.addAttribute("relayStreamNID", relayStreamNID);
			
			MediaRoomServiceUtils.updateStreamStatus(xmpp, request, db, log, roomNID, streamNID, statusEl);

		} catch (Exception e) {
			e.printStackTrace();
		}finally  {
			log.addInfo("HandleUnmanagedDestinationStreams:handleRelayEvent release lock  StreamNID:="+streamNID);
			IPVSMonitor.getMonitor().releaseLock(streamNID, lock);
		}
	}

	
	public void handleMediaPresenceEvent(XMPPI xmpp,  MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,Document eventDoc) throws Exception {

		
		Element presence = MRXMLUtils.getElementXpath(eventDoc, "//" + MediaRoomServiceConst.MEDIA_PRESENCE_EVENT+"Data");
		
		String type = presence.attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);
		String value = presence.attributeValue(SessionServiceConst.VALUE_ATTRIBUTE);
		String jid = presence.attributeValue(SessionServiceConst.JID);
		
		if(type.equals("MediaRoom")) {
			
			//release rtp stream url's on room unavailable
			if(MediaRoomServiceConst.UNAVAILABLE.equalsIgnoreCase(value)) {
				String roomNID = presence.attributeValue(SessionServiceConst.ROOMNID);
				
				Set<String> streamNIDSet = deleteRoomFromCache(roomNID,log);
				if(streamNIDSet == null) {
					return;
				}
				
				for(String streamNID : streamNIDSet) {
					this.releaseURL(streamNID);
				}
			}
			
			return;
		}
		
		String userJID = AppServerAgentUtils.getAppServerAgentJID("", serviceAgentFactory);
		MRRequest request = MRRequest.createRequest(userJID);
	
		Document roomListDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, "//Stream[data/destAgentJID='"+userJID+"' and data/sourceAgentJID='"+jid+"']", false);
		List<?> roomNIDList = MRXMLUtils.getListXpath(roomListDoc, "//MediaRoom");
		for(Object object : roomNIDList) {			
			Element roomEl = (Element) object;
			
			
			String xpath = ".//Stream[data/destAgentJID='"+userJID+"']";
			List<?> streamNIDList = MRXMLUtils.getListXpath(roomEl, xpath);
			if(streamNIDList ==null || streamNIDList.isEmpty()) {
				continue;
			}
			
			String roomNID = MRXMLUtils.getAttributeValue(roomEl, MediaRoomServiceConst.NID,"");
			
			for(Object object1:streamNIDList) {
				
				Element streamEl = (Element) object1;
				String streamNID  = MRXMLUtils.getAttributeValue(streamEl, MediaRoomServiceConst.NID,"");
				
				//update stream state as waiting for stream start
				if(MediaRoomServiceConst.UNAVAILABLE.equalsIgnoreCase(value)) {
					try {	
						MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, "", String.valueOf(MediaRoomServiceConst.WAITING_FOR_SOURCE));
						log.addInfo("HandleUnmanagedDestinationStreams: handleMediaPresenceEvent set stream state as waiting for stream start. sourceJID : ="+jid +" streamNID :="+streamNID);
					} catch (Exception e) {
						log.addInfo("HandleUnmanagedDestinationStreams: handleMediaPresenceEvent error while setting stream state streamNID :="+streamNID+ " as waiting for source JDI:="+jid+". Error:= "+MRException.getStackTrace(e));
					}				
				}
				//Resend stream start request to source
				else if(SessionServiceConst.READY.equalsIgnoreCase(value)) {
					try {
						Document streamDataDoc =MRXMLUtils.elementToDocument((Element)MRXMLUtils.getElement(streamEl,"data").clone());
						Document mediaSourceDataDoc =MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "")));						
						sendStartRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, mediaSourceDataDoc);
						log.addInfo("HandleUnmanagedDestinationStreams: handleMediaPresenceEvent sourceJID : ="+jid +" avaiable. Resending start request for streamNID :="+streamNID);
					}
					catch(Exception e) {
						log.addInfo("HandleUnmanagedDestinationStreams: handleMediaPresenceEvent error while recovering the session on source available. "+MRException.getStackTrace(e));
					}
				}
			}
		}	
		
	
	}

	public void releaseURL(String streamNID) {
		try {
			String sdpFileName = streamNID+".sdp";
			if(FileUtils.isFileExist(SDP_FILE_LOCATION, sdpFileName)) {
				FileUtils.deleteFile(SDP_FILE_LOCATION+"/"+sdpFileName);
			}
		} catch (Exception e) {			
		}
	}
}
