package com.ipvs.mediaroomservice.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
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
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.streamingserver.UDPSSAPI;
import com.ipvs.mediaroomservice.streamingserver.V2DSSAPI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class AddMediaDestRequestHandler implements MRRequestHandlerI {
	
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

	private String getDirStorePortNID(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String destNID, String parentDirNID) throws Exception {
	    String dirStorePortNIDKey = parentDirNID + "." + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME;
	    MRRequestCache cache = request.getCache();
	    if(!cache.containsKey(dirStorePortNIDKey)) {
		    String storePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDir(destNID, request, db, log);
		    cache.put(dirStorePortNIDKey, storePortNID);
		    // System.out.println("AddMediaDestRequestHandler:getDirStorePortNID added key=" + dirStorePortNIDKey + " request#=" + request.hashCode() + " cache#=" + cache.hashCode());

	    }
	    return (String)cache.get(dirStorePortNIDKey);
	}

	private String getAssetRelayPortNID(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String assetResourceNID) throws Exception {
	    String relayPortNIDKey = assetResourceNID + "." + MediaStoreServiceConst.MEDIA_RELAY_PORT_RESOURCE_NID_EL_NAME;
	    MRRequestCache  cache = request.getCache();
	    if(!cache.containsKey(relayPortNIDKey)) {
	    	String relayPortNID = ServiceAgentUtils.getMediaStreamRelayPortResourceNID(assetResourceNID, request.getUserJID(), db, log,xmpp);
		    cache.put(relayPortNIDKey, relayPortNID);
		    // System.out.println("AddMediaDestRequestHandler:getAssetRelayPortNID added key=" + relayPortNIDKey + " request#=" + request.hashCode() + " cache#=" + cache.hashCode());
	    }
	    return (String)cache.get(relayPortNIDKey);
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
// com.ipvs.common.XMPPTransaction.setDEBUG(true);
com.ipvs.common.XMPPTransaction.startBlock("AddMediaDestRequest");

      Document requestDoc = request.getMessage().getDocument();
      String id = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaRoomServiceConst.ID, "");
      String destNID = MRXMLUtils.getValueXpath(requestDoc, "//destNID", "");
      String roomNID = MRXMLUtils.getValueXpath(requestDoc, "//roomNID", "");
      //String description = MRXMLUtils.getValueXpath(requestDoc, "//description", "");
      String callbackPort = "";
      String destIP = MRXMLUtils.getValueXpath(requestDoc, "//destIP", "");
      String destUDPStartPort = MRXMLUtils.getValueXpath(requestDoc, "//destUDPStartPort", "");
	  log.addInfo("AddStreamRequestHandler requestDoc destIP:port=" + destIP + ":" + destUDPStartPort, MRLog.OK, MRLog.NOTIFY);				        

      String destAgentJID = request.getUserJID();
      String destType = MediaRoomServiceConst.OUTPUT_STREAM;
      String trackName = MediaRoomServiceConst.STREAMTRACK;      
      String destPortNID = destNID;
      String destFileUUID = "";
      String destFileParentDirNID = "";
      String destFileRelayNID = "";
      String destPortElementID = "";
      String destAssetType = AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE;
      String destStreamURL = "";
      String trimLength = "";
      String trickPlay = SessionServiceConst.TRUE;
      
      String owneruserJID = request.getUserJID();
      String assetResourceNID = "";

      String title = "";
      Element fileInfoELement = null;
  	
      if(!"".equals(destNID)) {
    	  Document destDoc = MRXMLDBUtils.readElement(request, log, db, destNID);
          assetResourceNID = MRXMLUtils.getValueXpath(destDoc, "//AssetResourceNID", "");
    	  
          // Check that this is not a sourcePort (for IO ports only due to Relay ports also being set to be StreamSrc)
    	  String objectType = destDoc.getRootElement().getName();
          String mediaType = MRXMLUtils.getValueXpath(destDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
          if(MediaRoomServiceConst.STREAMSRC.equals(mediaType) &&
        		  MediaRoomServiceConst.MediaStreamIOPortResource.equals(objectType)) {
        	  throw new MRException(MediaRoomServiceExceptionCodes.CANNOT_USE_SRCPORT_AS_DEST, "Cannot use SrcPort as Dest");    	  
          }
          
    	  Document destPortDoc = destDoc;
    	  title = MRXMLUtils.getValueXpath(destDoc, "//Title", "");
    	  String destObjectName = destDoc.getRootElement().getName();
    	  String fileType = MRXMLUtils.getValueXpath(destDoc, "//Info/MediaStoreFileInfo/Type", "");
    	  if (MediaRoomServiceConst.MediaStreamDstPortResource.equals(destObjectName)||
    		  MediaRoomServiceConst.MediaStreamIOPortResource.equals(destObjectName) ) {
    		  destType = MediaRoomServiceConst.OUTPUT_DECODER;
              destPortElementID = MRXMLUtils.getValueXpath(destPortDoc, "//DeviceElementID", MediaRoomServiceConst.DEFAULT_RXPORTELEMENTID);
              trickPlay = MRXMLUtils.getValueXpath(destDoc, "//Feature[@Name='TrickPlay']", SessionServiceConst.FALSE);
    	  } else if (!"".equals(fileType)) {
    	      if (MediaRoomServiceConst.MEDIACLIP.equals(fileType)) {
    	          destType = MediaRoomServiceConst.OUTPUT_RECORD;
    	          trackName = MediaRoomServiceConst.RECORDTRACK;
    	      } else if (MediaRoomServiceConst.PVRMEDIACLIP.equals(fileType)) {
    	          destType = MediaRoomServiceConst.OUTPUT_PVR;
    	          trackName = MediaRoomServiceConst.PVRTRACK;
    	          trimLength = MRXMLUtils.getValueXpath(requestDoc, "//" + SessionServiceConst.TRIMLENGTH, MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
    	      }
    	      String parentDirNID = MRXMLUtils.getValueXpath(destDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
              destPortNID = this.getDirStorePortNID(request, xmpp, log, db, destNID, parentDirNID);
              destPortDoc = MRXMLDBUtils.readElement(request, log, db, destPortNID);
              destFileUUID = MRXMLUtils.getValueXpath(destDoc, "//"+MediaStoreServiceConst.UUID, "");
              destFileParentDirNID = MRXMLUtils.getValueXpath(destDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
              assetResourceNID = MRXMLUtils.getValueXpath(destPortDoc, "//AssetResourceNID", "");
              destFileRelayNID = this.getAssetRelayPortNID(request, xmpp, log, db, assetResourceNID);
              fileInfoELement = MRXMLUtils.getElementXpath(requestDoc, "//"+MediaRoomServiceConst.FILE_INFO_ELEMENT);
          }
		  destAgentJID = MediaRoomServiceUtils.getAgentJID(request, destPortDoc);
		  // System.out.println("AddMediaDestRequestHandler: destAgentJID=" + destAgentJID +
		  //	  MRXMLUtils.documentToString(destPortDoc));
		  
	      destStreamURL = MRXMLUtils.getValueXpath(destDoc, "//MediaStreamURL", "");
		  if(!"".equals(destStreamURL)) {
			  destAgentJID = owneruserJID; // Set the operator as the destAgentJID
			  boolean validURL = false;
			  try{
				  Document doc = V2DSSAPI.parseV2DURL(destStreamURL, log);
				  // callbackIP = MRXMLUtils.getValueXpath(doc, "//callbackIP", "");
				  callbackPort = MRXMLUtils.getValueXpath(doc, "//callbackPort", "");
				  validURL = true;
			  }
			  catch(Exception e) {
				  // e.printStackTrace();
				  // log.addInfo("AddStreamRequestHandler invalid destStreamURL=" + destStreamURL, MRLog.OK, MRLog.NOTIFY);
			  }
			  try {
				  Document doc = UDPSSAPI.parseUDPURL(destStreamURL, log);
				  String pdestIP = MRXMLUtils.getValueXpath(doc, "//destIP", "");
				  String pdestUDPStartPort = MRXMLUtils.getValueXpath(doc, "//destPort", "");
				  if("".equals(destIP))
					  destIP = pdestIP;
				  if("".equals(destUDPStartPort))
					  destUDPStartPort = pdestUDPStartPort;
				  validURL = true;
				  log.addInfo("AddStreamRequestHandler destStreamURL  destIP:port=" + pdestIP + ":" + pdestUDPStartPort, MRLog.OK, MRLog.NOTIFY);				        
			  }
			  catch(Exception e) {
				  // e.printStackTrace();
				  // log.addInfo("AddStreamRequestHandler invalid destStreamURL=" + destStreamURL, MRLog.OK, MRLog.NOTIFY);
			  }
			  if(!validURL) {
				  log.addInfo("AddStreamRequestHandler invalid destStreamURL=" + destStreamURL, MRLog.OK, MRLog.NOTIFY);				  
			  }
		  }
		  assetResourceNID = MRXMLUtils.getValueXpath(destPortDoc, "//" + AMConst.ASSER_RESOURCE_NID, "");
		  Document assetResourceDoc = MRXMLDBUtils.readElement(request, log, db, assetResourceNID);
		  destAssetType = MRXMLUtils.getValueXpath(assetResourceDoc, "//Type", "");
      } else {
    	  destAgentJID = owneruserJID;    	  
    	  log.addInfo("AddMediaDestRequestHandler.getResponse::destAgentJID =" + destAgentJID, MRLog.OK, MRLog.NOTIFY);
      }

      String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
      
      RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, destAgentJID, assetResourceNID);

      String state = MediaRoomServiceConst.UNAVAILABLE;
      if(!"".equals(destAgentJID))
    	  if (RoomDB.getRoomDB().isReady(xmpp, roomNID, destAgentJID, request.getUserJID(), request.getAppServerJID()))
    		  state = MediaRoomServiceConst.AVAILABLE; 
            
	  log.addInfo("AddStreamRequestHandler createMediaDest destIP:port=" + destIP + ":" + destUDPStartPort, MRLog.OK, MRLog.NOTIFY);			
	  
		if (!destIP.isEmpty() && !destUDPStartPort.isEmpty()
				&& (AMConst.UDP_RX_DEVICE_OBJECTYPE.equals(destAssetType) || AMConst.WEB_CLIENT_DEVICE.equals(destAssetType)) ) {
			destPortElementID = destIP + ":" + destUDPStartPort;
			destStreamURL = destPortElementID;
			log.addInfo("AddStreamRequestHandler createMediaDest destPortElementID:= "+ destPortElementID + " destStreamURL:= "	+ destStreamURL, MRLog.OK, MRLog.NOTIFY);
		}

      String mediaDestNID = RoomDB.getRoomDB().createMediaDestElement(roomNID, id, state,
    		  destNID, destType, destPortNID, destAgentJID, title, destIP, destUDPStartPort, callbackPort, trackNID,
    		  destFileUUID, destFileParentDirNID, destFileRelayNID, destPortElementID, destAssetType, destStreamURL,
    		  trimLength, trickPlay,fileInfoELement, request, xmpp, log, db);
	
	com.ipvs.common.XMPPTransaction.endBlock("AddMediaDestRequest");
	// com.ipvs.common.XMPPTransaction.setDEBUG(false);

		return request.createResponseData("mediaDestNID", mediaDestNID);
	}


}
