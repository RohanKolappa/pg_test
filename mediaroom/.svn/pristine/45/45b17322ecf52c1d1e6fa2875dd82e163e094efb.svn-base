package com.ipvs.mediaroomservice.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediaroomservice.impl.StreamURLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

public class AddMediaSourceRequestHandler implements MRRequestHandlerI {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addTraceBegin(request);
		int authType = MediaRoomObject.MEDIA_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL,
					authType, role, roomPolicyNID, streamPolicyNID,
					token, authority);
		log.addTraceEnd(request);
		return auth;
	}
	
	public Element getObserverJIDElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
    		Document mediaStreamSrcPortResourceDoc) throws Exception {
		Element observerEl = MRXMLUtils.getElementXpath(mediaStreamSrcPortResourceDoc, "//Observer");
		String userNID = MRXMLUtils.getValueXpath(observerEl, "./UserNID", "");
		String resourceNID = MRXMLUtils.getValueXpath(observerEl, "./ResourceNID", "");
		if(userNID.isEmpty() && resourceNID.isEmpty()) {
			return null;
		}
		
		boolean xauth = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(observerEl, "xAuth",""), false);
		boolean monitor = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(observerEl, "monitor",""), false);
		if(!xauth  && !monitor) {
			return null;
		}
		String observerJID = "";
		if(!userNID.isEmpty()) {
			Document userDoc = MRXMLDBUtils.readElement(request, log, db, userNID);
			String username = MRXMLUtils.getValueXpath(userDoc, "//Username", "");
			
			Document resourceDoc = null;
			if(!resourceNID.isEmpty()) {
			    resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID); 
			} else {
				String[] whereXpath = {"matches(//Contact/OwnerUserJID, '"+username+"')"};
				MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE,AMConst.AssetResource , log);
	            String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));            
	        	Element resourceEL = MRXMLDBUtils.readElement(1, null, whereXpath, rootNID, 1, null, null, request, db, log);
	        	if(resourceEL != null) {
	        		resourceDoc = MRXMLUtils.elementToDocument(resourceEL);	        		
	        	}
	     	}
			String resourceTitle = MRXMLUtils.getValueXpath(resourceDoc, "//Title", "");
			if(!resourceTitle.isEmpty()) {
				String state = MRXMLUtils.getValueXpath(resourceDoc, "//StateFlag", "");
				String ownerUserJID = MRXMLUtils.getValueXpath(resourceDoc, "//OwnerUserJID", "");
				if(state.equals(SessionServiceConst.READY)) {
					observerJID = username + "@localhost/" +resourceTitle;
					if(!observerJID.equals(ownerUserJID))
						observerJID = "";
				}
				else
					observerJID = "";
			}
		} else if(!resourceNID.isEmpty()) {			
			Document resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
			observerJID = MRXMLUtils.getValueXpath(resourceDoc, "//OwnerUserJID", "");
		} 
		Element observerJIDEl = MRXMLUtils.addElement("ObserverJID", observerJID);
		MRXMLUtils.addAttribute(observerJIDEl, "xAuth", String.valueOf(xauth));
		MRXMLUtils.addAttribute(observerJIDEl, "monitor", String.valueOf(monitor));
		return observerJIDEl;
	}
	
	private void updateSourceStreamInfo(Document sourceStreamInfoDoc, Document sourceDoc, String sourceType, 
			MRLog log, MRDB db) throws Exception {
	  // System.out.println(MRXMLUtils.documentToString(sourceDoc));
	  String streamType = MRXMLUtils.getValueXpath(sourceDoc, "//StreamType", MediaRoomServiceConst.V2D);
	  if("".equals(streamType))
		  streamType = MediaRoomServiceConst.V2D;
	  String streamProfileInfoXpath = "//MediaStreamProfileInfo/" + streamType + "MediaStreamProfileInfo";
	  String streamProfileName = streamType + "MediaStreamProfile";
	  
	  // TBR : Temp fix since to address 2 problems
	  // 1. UI is not adding SDP-Template-Src
	  // 2. UI not putting in new line in the SDP in streamURL
	  if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType) &&
	     MediaRoomServiceConst.RTP.equals(streamType) &&
		 (MRXMLUtils.getElementXpath(sourceDoc, "//" + streamProfileName) == null) ) {
		  
		  // Patch in the SDP-Template-Src element
		  Element rtpStreamProfileElement = MRXMLUtils.stringToElement(
		  "<RTPMediaStreamProfileInfo><SDP-Template-Src/><RTPRecoderConfig><ThumbnailConfig><Enable>true</Enable></ThumbnailConfig></RTPRecoderConfig></RTPMediaStreamProfileInfo>"
		  	);
		  MRXMLUtils.addElementXpath(sourceDoc, "//MediaStreamProfileInfo",rtpStreamProfileElement);
		  
		  /*// Patch in the new lines into the SDP
		  String portStreamURL = MRXMLUtils.getValueXpath(sourceDoc, "//MediaStreamURL", "");		  
		  // System.out.println("AddMediaSourceRequestHandler:updateSourceStreamInfo before streamURL=" + portStreamURL);
		  portStreamURL = SDPUtils.fixsdp(portStreamURL);
		  // System.out.println("AddMediaSourceRequestHandler:updateSourceStreamInfo after streamURL=" + portStreamURL);		  
		  
		  MRXMLUtils.getElementXpath(sourceDoc, "//MediaStreamURL").setText(portStreamURL);*/
	  }
	  
	  String streamProfile = "";
	  String streamURL = "";
	  Element streamProfileInfoElement = null;
	  if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType)) {
		  // Find the streamURL
		  streamURL = MRXMLUtils.getValueXpath(sourceDoc, "//MediaStreamURL", "");
		  String stateFlag = MRXMLUtils.getValueXpath(sourceDoc, "//StateFlag", "");
		  //Unamnaged source must have the MediaStreamURL
		  if (stateFlag.equals(MRClientUtils.ASSET_STATE_UNMANAGED) && "".equals(streamURL)) {
			  throw new MRException(MediaRoomServiceExceptionCodes.STREAMURL_NOT_CONFIGURED, "StreamURL not configured for Unmanaged source");
		  }
		  if(!"".equals(streamURL)) {
			  // Validate the streamURL
			  if(MediaRoomServiceConst.RTP.equals(streamType)) {
				  StreamURLUtils.validateRTPStreamURL(streamURL, log);
			  }
			  else if(MediaRoomServiceConst.UDP.equals(streamType) ||
					  MediaRoomServiceConst.MPEGTS.equals(streamType)) {
				  StreamURLUtils.validateUDPStreamURL(streamURL, log);
			  }  
			  // Special processing for RTP
			  if(MediaRoomServiceConst.RTP.equals(streamType)) { // If RTP then derive the profile from the streamURL
		    	  String sdptemplate = SDPUtils.getSDPTemplateFromSDP(streamURL, log);
				  MRXMLUtils.getElementXpath(sourceDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE).setText(sdptemplate);
			  }
			  // Find the streamProfile
			  streamProfileInfoElement = MRXMLUtils.getElementXpath(sourceDoc, streamProfileInfoXpath);
			  // System.out.println("AddMediaSourceRequestHandler: sourceDoc=" + MRXMLUtils.documentToString(sourceDoc));
			  // System.out.println("AddMediaSourceRequestHandler: streamProfile=" + streamProfile);
		  }
	  }
	  else if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
		  streamProfileInfoElement = MRXMLUtils.getElementXpath(sourceDoc, streamProfileInfoXpath);
		  if(MediaRoomServiceConst.RTP.equals(streamType)) { // Work around, have to decode the template and put it back in
			  String sdptemplate = MRXMLUtils.getValueXpath(sourceDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
			  sdptemplate = MediaRoomServiceUtils.decodeFromFileSDPTemplate(sdptemplate);
			  MRXMLUtils.getElementXpath(sourceDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE).setText(sdptemplate);
		  }
	  }
	  if(streamProfileInfoElement != null) {
		  Element streamProfileElement = (Element)streamProfileInfoElement.clone();
		  streamProfileElement.setName(streamProfileName);
		  streamProfile = MRXMLUtils.elementToString(streamProfileElement);
		  // System.out.println("AddMediaSourceRequestHandler: streamProfile=" + streamProfile);
	  }
	  
	  MRXMLUtils.getElementXpath(sourceStreamInfoDoc, "//streamType").setText(streamType);
	  MRXMLUtils.getElementXpath(sourceStreamInfoDoc, "//streamProfile").setText(streamProfile);
	  MRXMLUtils.getElementXpath(sourceStreamInfoDoc, "//streamURL").setText(streamURL);
	}

	/**
	 * startTimecode : The base timecode to be used in any timecode computation for the source
	 * startOffset : The offset to be used when starting the source
	 * The SS may require timecode or offset when starting the source
	 * The timecode is computed using: timecode = startTimecode + offset
	 * The startTimecode is inherited from the Track
	 */
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
	  com.ipvs.common.XMPPTransaction.startBlock("AddMediaSourceRequest");

	  // Check the data
		//perf-logs log.addInfo("AddMediaSourceRequestHandler::=" + request.getUserJID()) + " getResponse - Start", MRLog.OK, MRLog.NOTIFY);	
      Document requestDoc = request.getMessage().getDocument();      

	  // Element data = MediaRoomServiceUtils.cloneDataElement(request);
      String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(requestDoc, "//userRoomMediaSourceNID", "");
      String sourceNID = MRXMLUtils.getValueXpath(requestDoc, "//sourceNID", "");
      String id = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaRoomServiceConst.ID, "");
      String roomNID = MRXMLUtils.getValueXpath(requestDoc, "//roomNID", "");
      String startOffset = MRXMLUtils.getValueXpath(requestDoc, "//startOffset", "");      
	  
	  long startTimecode = 0;
	  try {
	      String startTimecodeStr = MRXMLUtils.getValueXpath(requestDoc, "//startTimeCode", "0");
		  startTimecode = Long.parseLong(startTimecodeStr);
	  }
	  catch(Exception e) {
		  // not specified
	  }
		
	  // Change the track to be the streamTrack
	  String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, MediaRoomServiceConst.STREAMTRACK);
      // data.removeChild("roomNID");
	  /*
		if(!MRXMLDBUtils.objectExists(request, db, log, sourceNID))
			throw new MRException(MediaRoomExceptionCodes.SOURCENOTFOUND, sourceNID);
	  */
	  if(!MediaRoomServiceUtils.objectExists(roomNID, request, db, log, trackNID)){
		throw new MRException(MediaRoomServiceExceptionCodes.TRACKNOTFOUND, MRXMLUtils.generateErrorXML("trackNID", trackNID));
	  }
	  
	  
	  // Get sourceType and contactJID
      String sourceType = MediaRoomServiceConst.INPUT_ENCODER;
      String sourcePortNID = sourceNID;       
      Document sourceDoc = MRXMLDBUtils.readElement(request, log, db, sourceNID);        
      String sourceAgentJID = MediaRoomServiceUtils.getAgentJID(request, sourceDoc);
      String assetResourceNID = MRXMLUtils.getValueXpath(sourceDoc, "//AssetResourceNID", "");

      String title = MRXMLUtils.getValueXpath(sourceDoc, "//Title", "");
      String sourcePortElementID = MRXMLUtils.getValueXpath(sourceDoc, "//DeviceElementID", MediaRoomServiceConst.DEFAULT_TXPORTELEMENTID);
      String storePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDirRead(sourceDoc, request, db, log); 
    	  MRXMLUtils.getValueXpath(sourceDoc, "//MediaStorePortResourceNID", "");
      Document mediaStorePortDoc = null;
      if (!"".equals(storePortNID)) {
          sourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
          sourcePortNID = storePortNID;
          mediaStorePortDoc = MRXMLDBUtils.readElement(request, log, db, storePortNID);  
          assetResourceNID = MRXMLUtils.getValueXpath(mediaStorePortDoc, "//AssetResourceNID", "");
          sourceAgentJID = MediaRoomServiceUtils.getAgentJID(request, mediaStorePortDoc);
      }

      // Check if the source is busy except for PVR files
      String sourceState = MRXMLUtils.getValueXpath(sourceDoc, "//StateFlag", "");
      String mediaType = MRXMLUtils.getValueXpath(sourceDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
      if(MediaStoreServiceConst.BUSY.equals(sourceState) &&
    		  (!(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType) || MediaRoomServiceConst.MEDIACLIP.equals(mediaType)))){
    	  throw new MRException(MediaRoomServiceExceptionCodes.MEDIASOURCEBUSY, "Media Source Busy");
      }

      // Check that this is not a destPort
      if(MediaRoomServiceConst.STREAMDST.equals(mediaType)) {
    	  throw new MRException(MediaRoomServiceExceptionCodes.CANNOT_USE_DSTPORT_AS_SOURCE, "Cannot use DstPort as Source");    	  
      }
      
	  com.ipvs.common.XMPPTransaction.startBlock("AddMediaSourceNode");
	  Element sourceNIDElement = MRXMLUtils.getElementXpath(requestDoc, "//sourceNID");
	  String lookbackSourceNID = sourceNIDElement.attributeValue("lookbackSourceNID", "");
	  String lookbackSourceAgentJID = sourceNIDElement.attributeValue("lookbackSourceAgentJID", "");
	  String lookbackSourceFileUUID = sourceNIDElement.attributeValue("lookbackSourceFileUUID", "");
	  String lookbackSourceparentDirNID = sourceNIDElement.attributeValue("lookbackSourceparentDirNID", "");
	  String lookbackPlayLength = sourceNIDElement.attributeValue("lookbackSourceFilePlayLength", "");
	  String lookbackStartTC = sourceNIDElement.attributeValue("lookbackSourceFileStartTC", "");
      if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
          lookbackSourceNID = sourceNID;
          lookbackSourceAgentJID = sourceAgentJID;
          lookbackSourceFileUUID = MRXMLUtils.getValueXpath(sourceDoc, "//UUID", "");
          lookbackSourceparentDirNID = MRXMLUtils.getValueXpath(sourceDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
          lookbackPlayLength = MRXMLUtils.getValueXpath(sourceDoc, "//PlayLength", "");
      }
      else if (MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType)) { 
    	  // Set the mediaSource startTimecode in case of LIVE
    	  // This will be used to set the startTimecode in the PVR source
    	  // The PVR source will be started by computing the tc = startTimecode + offset
          // Document trackdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, trackNID);
          // String trackStartWallClock = MRXMLUtils.getValueXpath(trackdoc, "//startWallClock", "");
    	  // startTimecode = Long.parseLong(trackStartWallClock);
    	  
          Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
          MediaTimeline timeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);
          startTimecode = timeline.getStartWallclock();
      }
      
      Document sourceStreamInfoDoc = MRXMLUtils.stringToDocument("<SourceStreamInfo><streamType/><streamURL/><streamProfile/></SourceStreamInfo>");
      this.updateSourceStreamInfo(sourceStreamInfoDoc, sourceDoc, sourceType, log, db);

	  String streamType = MRXMLUtils.getValueXpath(sourceStreamInfoDoc, "//streamType", MediaRoomServiceConst.V2D);
	  String streamURL = MRXMLUtils.getValueXpath(sourceStreamInfoDoc, "//streamURL", "");
	  String streamProfile = MRXMLUtils.getValueXpath(sourceStreamInfoDoc, "//streamProfile", "");
     
	  RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, sourceAgentJID, assetResourceNID);
	  
	  String state = MediaRoomServiceConst.UNAVAILABLE;
      if (RoomDB.getRoomDB().isReady(xmpp, roomNID, sourceAgentJID,  request.getUserJID(), request.getAppServerJID()))
    	  state = MediaRoomServiceConst.AVAILABLE; 
      
      Element observerEl = getObserverJIDElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sourceDoc);
      boolean xAuth = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(observerEl, "xAuth", ""), false);
      boolean monitor = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(observerEl, "monitor", ""), false);
      String observerAgentJID = MRXMLUtils.getValue2(observerEl,"");
      String xAuthAgentJID = "";
      String monitorAgentJID = "";
   
      if(monitor && observerAgentJID.isEmpty() ) {
    	  throw new MRException(MediaRoomServiceExceptionCodes.MONITOR_AGENT_JID_NOT_FOUND, "Monitor Agent JID not found");
      } else if(monitor && !observerAgentJID.isEmpty()) {
    	  monitorAgentJID = observerAgentJID;
      }
      
      if(xAuth && observerAgentJID.isEmpty()) {
    	  throw new MRException(MediaRoomServiceExceptionCodes.AUTH_AGENT_JID_NOT_FOUND, "Auth Agent JID not found");
      }else if(xAuth && !observerAgentJID.isEmpty()) {
    	  xAuthAgentJID = observerAgentJID;
      }     
         
	  String mediaSourceNID = RoomDB.getRoomDB().createMediaSourceElement(roomNID, id, state,
			  trackNID, sourceNID, sourcePortNID, sourcePortElementID, title, sourceType, sourceAgentJID, startOffset, startTimecode, 
			  lookbackSourceNID, lookbackSourceAgentJID, lookbackSourceFileUUID,lookbackSourceparentDirNID, lookbackPlayLength,lookbackStartTC, streamType, streamURL, streamProfile,
			  userRoomMediaSourceNID, "", xAuthAgentJID,monitorAgentJID, request, xmpp, log, db);

	  // System.out.println("AddMediaSourceRequest:" + MRXMLUtils.elementToString(msElement));
	  
	  com.ipvs.common.XMPPTransaction.endBlock("AddMediaSourceNode");
	  //perf-logs 
	  //perf-logs log.addInfo("AddMediaSourceRequestHandler::=" + request.getUserJID()) + " MRXMLDBUtils.addSourceElement - End", MRLog.OK, MRLog.NOTIFY);	  
	  //perf-logs log.addInfo("AddMediaSourceRequestHandler::=" + request.getUserJID()) + " xmpp.sendInvite - Start", MRLog.OK, MRLog.NOTIFY);
	  if(!"".equals(monitorAgentJID))
		  RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, monitorAgentJID, "");
	  
	  //perf-logs log.addInfo("AddMediaSourceRequestHandler::=" + request.getUserJID()) + " xmpp.sendInvite - End", MRLog.OK, MRLog.NOTIFY);
	  com.ipvs.common.XMPPTransaction.endBlock("AddMediaSourceRequest");
	  return request.createResponseData("mediaSourceNID", mediaSourceNID);
	}

	/*
	public static void main(String[] args) throws Exception {
		String s = "v=0  o=- 4216386752513 4216386752523 IN IP4 10.1.5.151  s=Media Presentation  e=NONE  b=AS:8016  t=0 0  a=control:rtsp://10.1.5.151:554/mpeg4/1/media.amp  a=range:npt=now-  a=mpeg4-iod: 'data:application/mpeg4-iod;base64,AoEAAE8BAQ/1AQOAkwABQHRkYXRhOmFwcGxpY2F0aW9uL21wZWc0LW9kLWF1O2Jhc2U2NCxBVGdCR3dVZkF4Y0F5U1FBWlFRTklCRUVrK0FBZWhJQUFIb1NBQVlCQkFFWkFwOERGUUJsQlFRTlFCVUFDN2dBQUQ2QUFBQStnQVlCQXc9PQQNAQUABAAAAAAAAAAAAAYJAQAAAAAAAAAAA0IAAkA+ZGF0YTphcHBsaWNhdGlvbi9tcGVnNC1iaWZzLWF1O2Jhc2U2NCx3QkFTZ1RBcUJYSmhCSWhRUlFVL0FBPT0EEgINAAACAAAAAAAAAAAFAwAAQAYJAQAAAAAAAAAA'  a=isma-compliance:1,1.0,1  m=video 4510 RTP/AVP 96  c=IN IP4 224.1.151.1/5  b=AS:8000  a=framerate:30.0  a=control:trackID=1  a=rtpmap:96 MP4V-ES/90000  a=fmtp:96 profile-level-id=245; config=000001B0F5000001B5891300000100000001200086C40FA28A021E0A21  a=mpeg4-esid:201  m=audio 4512 RTP/AVP 97  c=IN IP4 224.1.151.1/5  b=AS:16  a=control:trackID=2  a=rtpmap:97 mpeg4-generic/8000/1  a=fmtp:97 streamtype=5; profile-level-id=15; mode=AAC-hbr; config=1588; sizeLength=13; indexLength=3; indexDeltaLength=3; profile=1; bitrate=16000  a=mpeg4-esid:101";
		System.out.println(fixsdp(s));
		
		
	}
	*/

}
