package com.ipvs.mediaroomservice.handler;

import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
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
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

public class AddStreamRequestHandler implements MRRequestHandlerI,AsynchDBRequestResponseListenerI {

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.STREAM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;

		// If destNID is empty then use the request.getResourceGroup
		// as the destGroup to find the matching policy
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);

		return auth;
	}


	private void updateMediaSourceWithLookbackInfo(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID, String mediaSourceNID, 
			Element mediaSourceElement, 
			String destNID, String destAgentJID, String lookbackSourceFileUUID,  String lookbackParentDirNID, String trimLength,
			Document roomdoc) throws Exception {
		String lookbackSourceNID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//lookbackSourceNID","");

		if (!"".equals(lookbackSourceNID)) { // Check if there is already a PVR
			throw new MRException(MediaRoomServiceExceptionCodes.DUPLICATEPVR, MRXMLUtils.generateErrorXML("mediaSourceNID", mediaSourceNID));
		}

		Element mediaSourceDataElement = mediaSourceElement.element("data");
		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//pvrEnabled").setText("true");
		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//lookbackSourceNID").setText(destNID);
		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//lookbackSourceAgentJID").setText(destAgentJID);

		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//lookbackSourceFileUUID").setText(lookbackSourceFileUUID);
		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//lookbackSourceFileParentDirNID").setText(lookbackParentDirNID);
		MRXMLUtils.getElementXpath(mediaSourceDataElement, ".//lookbackSourceFilePlayLength").setText(trimLength);

		log.addInfo("AddStreamRequestHandler:=" + request.getUserJID() + " set lookbackSourceNID=" + destNID, MRLog.OK, MRLog.NOTIFY);
		XMPPTransaction.startBlock("setMediaSourceObjectData");
		MediaRoomServiceUtils.setObjectData(roomNID, request, db, log, mediaSourceNID, mediaSourceDataElement);
		XMPPTransaction.endBlock("setMediaSourceObjectData");


		// Used for legacy code where the PVR is setup after the stream is started
		List<?> streamList = MRXMLUtils.getListXpath(roomdoc, "//Stream[data/mediaSourceNID='" + mediaSourceNID + 
				"' and Status/state > " + MediaRoomServiceConst.WAITING_FOR_DEST +"]");
		for(Object o:streamList) {
			Element streamElement = (Element)o;
			String streamNID = streamElement.attributeValue("NID");
			String sourceAgentJID = MRXMLUtils.getValueXpath(streamElement, ".//sourceAgentJID", "");
			Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
			String actionMessage = StreamAgentUtils.createSourceStreamAgentActionRequestMessage(
					streamNID, "", "", 
					actiondata, 0, "", 
					MRXMLUtils.stringToDocument("<streamdatadoc/>"), 
					MRXMLUtils.elementToDocument(mediaSourceDataElement), 
					log);
			String info = "AddStreamRequestHandler:updateLookbackInfo send mediaSourceUpdate to " +
					ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT);
			ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, roomNID,"", sourceAgentJID, 
					MediaRoomServiceConst.SOURCESTREAMAGENT, MediaRoomServiceConst.MEDIASOURCEUPDATEREQUEST, actionMessage, info, "none","",MRConst.MEDIA_SERVICE,"");
		}
	
	}


	public String getTrackMediaSourceNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String roomNID,
			String streamTrackMediaSourceNID, String trackNID, String sourceNID, String sourcePortNID, String sourceDeviceElementIDID, String sourceType, String sourceAgentJID, 
			String streamType, String streamURL, String streamProfile, String xAuthAgentJID, String monitorAgetnJID, Document roomdoc)
					throws Exception {

		Element mediaSourceDataElement = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource/data[streamTrackMediaSourceNID='" + streamTrackMediaSourceNID + 
				"' and trackNID='" + trackNID + "']");
		if (mediaSourceDataElement != null) {
			Element mediaSourceElement = mediaSourceDataElement.getParent();
			return MRXMLUtils.getAttributeValue(mediaSourceElement,"NID");
		}
		String id = "";
		String state = MediaRoomServiceConst.UNAVAILABLE;

		Element srcEl = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource");
		String title = MRXMLUtils.getAttributeValue(srcEl, "title");

		if (RoomDB.getRoomDB().isReady(xmpp, roomNID, sourceAgentJID,  request.getUserJID(), request.getAppServerJID()))
			state = MediaRoomServiceConst.AVAILABLE; 
		String trackMediaSourceNID = RoomDB.getRoomDB().createMediaSourceElement(roomNID, id, state, trackNID, 
				sourceNID, sourcePortNID, sourceDeviceElementIDID, title, sourceType, sourceAgentJID, 
				"0", 0, "", "", "", "","0","",
				streamType, streamURL, streamProfile, "", streamTrackMediaSourceNID, xAuthAgentJID, monitorAgetnJID,request, xmpp, log, db);

		return trackMediaSourceNID;
	}    

	public static boolean isUnManagedSource(String relayNID, String srcStreamURL) {
		return ("".equals(relayNID) && !"".equals(srcStreamURL)); 
	}

	public static boolean isUnManagedDest(String destStreamURL,String destAssetType) {
		return (!"".equals(destStreamURL) || destAssetType.equals(AMConst.WEB_CLIENT_DEVICE)); 
	}


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws Exception {

		MRRequestCache cache = request.getCache();
		if(cache == null)
			request.setCache(new MRRequestCache());

		//com.ipvs.common.XMPPTransaction.setDEBUG(true);
		com.ipvs.common.XMPPTransaction.startBlock("AddStreamRequest");

		com.ipvs.common.XMPPTransaction.startBlock("ReadRoomDB");

		long startTime = System.currentTimeMillis();
		log.addInfo(request.getUserJID() + ":AddStreamRequest action=Start Begin", MRLog.OK, MRLog.NOTIFY);

		// Check the data
		//perf-logs log.addInfo("AddStreamRequestHandler:=" + request.getUserJID() + " getResponse - Start", MRLog.OK, MRLog.NOTIFY);
		Element data = MediaRoomServiceUtils.cloneDataElement(request);

		Document requestDoc = request.getMessage().getDocument();

		String id = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaRoomServiceConst.ID, "");
		String mediaSourceNID = MRXMLUtils.getValueXpath(requestDoc, "//mediaSourceNID", "");        
		String description = MRXMLUtils.getValueXpath(requestDoc, "//description", "");
		String mediaDestNID = MRXMLUtils.getValueXpath(requestDoc, "//mediaDestNID", "");
		String windowId = MRXMLUtils.getValueXpath(requestDoc, "//"+SessionServiceConst.WINDOWID, "");
		String canvasId = MRXMLUtils.getValueXpath(requestDoc, "//"+SessionServiceConst.CANVASID, "");
		String xAuthAgentJID = MRXMLUtils.getValueXpath(requestDoc, "//xAuthAgentJID", "");
		String trimLength = "";

		String roomNID = MRXMLUtils.getValueXpath(requestDoc, "//roomNID", "");
		if("".equals(roomNID))
			roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, mediaSourceNID);

		Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);

		com.ipvs.common.XMPPTransaction.startBlock("mediaSourceDataDoc");
		/*
        String mediaSourceDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID);

        if (mediaSourceDataDocStr == null) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIASOURCENOTFOUND,MRXMLUtils.generateErrorXML("mediasourceNID", mediaSourceNID));
        }

        Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(mediaSourceDataDocStr);
		 */
		Element mediaSourceElement = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource[@NID='" + mediaSourceNID + "']");
		if (mediaSourceElement == null) {
			throw new MRException(MediaRoomServiceExceptionCodes.MEDIASOURCENOTFOUND,MRXMLUtils.generateErrorXML("mediasourceNID", mediaSourceNID));
		}
		com.ipvs.common.XMPPTransaction.endBlock("mediaSourceDataDoc");
		String sourceNID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourceNID", "");
		String sourcePortNID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourcePortNID", "");
		String sourcePortElementID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourcePortElementID", "");
		String sourceType = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourceType", "");
		String sourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourceAgentJID", "");
		String srcStreamType = MRXMLUtils.getValueXpath(mediaSourceElement, ".//streamType", "");
		String srcStreamURL = MRXMLUtils.getValueXpath(mediaSourceElement, ".//streamURL", "");
		String srcStreamProfile = MRXMLUtils.getValueXpath(mediaSourceElement, ".//streamProfile", "");
		String monitorAgentJID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//monitorAgentJID", "");

		if("".equals(mediaDestNID)) {  // Backwards compatibility code
			String destNID = MRXMLUtils.getValueXpath(requestDoc, "//destNID", "");
			String destIP = MRXMLUtils.getValueXpath(requestDoc, "//destIP", "");
			String destUDPStartPort = MRXMLUtils.getValueXpath(requestDoc, "//destUDPStartPort", "");
			Document addMediaDestResponse = ConnectionUtils.addMediaDest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					roomNID, destNID, description, "", destIP, destUDPStartPort, trimLength,null);
			mediaDestNID = MRXMLUtils.getValueXpath(addMediaDestResponse,"//mediaDestNID", "");
			roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID); // Refresh the roomdoc
		}

		com.ipvs.common.XMPPTransaction.startBlock("mediaDestDataDoc");
		/*
        String mediaDestDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaDestNID);
        Document mediaDestDataDoc = MRXMLUtils.stringToDocument(mediaDestDataDocStr);
		 */
		Element mediaDestElement = MRXMLUtils.getElementXpath(roomdoc, "//MediaDest[@NID='" + mediaDestNID + "']");
		//System.out.println(MRXMLUtils.elementToString(mediaDestElement));
		if (mediaDestElement == null) {
			throw new MRException(MediaRoomServiceExceptionCodes.MEDIADESTNOTFOUND,MRXMLUtils.generateErrorXML("mediaDestNID", mediaDestNID));
		}
		com.ipvs.common.XMPPTransaction.endBlock("mediaDestDataDoc");
		// System.out.println("AddStreamRequestHandler: " + MRXMLUtils.documentToString(mediaDestDataDoc));
		String destNID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destNID", "");
		String destPortNID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destPortNID", "");
		String destType = MRXMLUtils.getValueXpath(mediaDestElement, ".//destType", "");
		String destAgentJID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destAgentJID", "");
		String destIP = MRXMLUtils.getValueXpath(mediaDestElement, ".//destIP", "");
		String destUDPStartPort = MRXMLUtils.getValueXpath(mediaDestElement, ".//destUDPStartPort", "");
		String callbackPort = MRXMLUtils.getValueXpath(mediaDestElement, ".//callbackPort", "");
		String destFileUUID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destFileUUID", "");
		String destFileParentDirNID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destFileParentDirNID", "");
		String destFileRelayNID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destFileRelayNID", "");
		String destAssetType = MRXMLUtils.getValueXpath(mediaDestElement, ".//destAssetType", "");
		String destStreamURL = MRXMLUtils.getValueXpath(mediaDestElement, ".//destStreamURL", "");
		String destPortElementID = MRXMLUtils.getValueXpath(mediaDestElement, ".//destPortElementID", "");
		trimLength = MRXMLUtils.getValueXpath(mediaDestElement, ".//data/trimLength",trimLength);
		String trickPlay = MRXMLUtils.getValueXpath(mediaDestElement, ".//data/trickPlay", SessionServiceConst.FALSE);
		String relayNID = MRXMLUtils.getValueXpath(requestDoc, "//relayNID", "").trim();
		String profileNID = MRXMLUtils.getValueXpath(requestDoc, "//profileNID", "").trim();
		String profileXML = MRXMLUtils.elementToString(MRXMLUtils.getFirstChild(data.element("profileXML")));
		Element fileInfoElement = null;

		String owneruserJID = request.getUserJID();

		Document relayPortDoc = null;
		String trackName = MediaRoomServiceConst.STREAMTRACK;
		String relayAgentJID = "";
		String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);

		com.ipvs.common.XMPPTransaction.endBlock("ReadRoomDB");
		com.ipvs.common.XMPPTransaction.startBlock("SetRelay");

		// DVITest: Testing DVI: No relays possible for DVI
		// id = "FORCEDVI_1";
		if((id != null) && id.startsWith("_FORCEDVI_")) {
			log.addInfo("AddStreamRequestHandler:getResponse DVITest setting streamType=" + srcStreamType, MRLog.OK, MRLog.NOTIFY);
			srcStreamType = MediaRoomServiceConst.DVI;
		}
		if(MediaRoomServiceConst.DVI.equals(srcStreamType)) {
			log.addInfo("AddStreamRequestHandler:getResponse removing relay", MRLog.OK, MRLog.NOTIFY);
			relayNID = "";
			if (	(MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) || 
					(MediaRoomServiceConst.OUTPUT_RECORD.equals(destType))) {
				throw new MRException(MediaRoomServiceExceptionCodes.DEST_NOT_SUPPORTED_FOR_STREAMTYPE, "Dest " + destType + " not supported for streamType=" + srcStreamType);
			}
		}



		if(sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)&& destType.equals(MediaRoomServiceConst.OUTPUT_RECORD)){
			throw new MRException(MediaRoomServiceExceptionCodes.RECORD_ERROR, "Cannot record from playback file"); 

		}
		String mediaRelayNID = "";
		if (	(MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) || 
				(MediaRoomServiceConst.OUTPUT_RECORD.equals(destType))) {
			fileInfoElement = MRXMLUtils.getElementXpath(mediaDestElement, ".//"+MediaRoomServiceConst.FILE_INFO_ELEMENT);
			com.ipvs.common.XMPPTransaction.startBlock("getTrackMediaSourceNID");
			relayNID = destFileRelayNID;        	
			// update PVR info in the streamTrack mediaSource for pvr, note using updated destAgentJID
			if (MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) {
				com.ipvs.common.XMPPTransaction.startBlock("updateMediaSourceWithLookbackInfo");
				this.updateMediaSourceWithLookbackInfo(request, xmpp, log, db, roomNID, mediaSourceNID, mediaSourceElement, 
						destNID, destAgentJID, destFileUUID, destFileParentDirNID, trimLength, roomdoc);

				trackName = MediaRoomServiceConst.PVRTRACK;
				com.ipvs.common.XMPPTransaction.endBlock("updateMediaSourceWithLookbackInfo");
			} else {
				trackName = MediaRoomServiceConst.RECORDTRACK;
			}
			// create the Track mediaSource for pvr and recording, note updates the mediaSourceNID
			trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
			// Create the mediaSource under the track if required
			mediaSourceNID = this.getTrackMediaSourceNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID, 
					mediaSourceNID, trackNID, sourceNID, sourcePortNID, sourcePortElementID, sourceType,
					sourceAgentJID, srcStreamType, srcStreamURL, srcStreamProfile,xAuthAgentJID, monitorAgentJID, roomdoc);
			com.ipvs.common.XMPPTransaction.endBlock("getTrackMediaSourceNID");
		} else if (MediaRoomServiceConst.COMPUTERELAYUSINGPOLICY.equals(relayNID)) { // Compute the relay
			//com.ipvs.common.XMPPTransaction.setDEBUG(true);
			com.ipvs.common.XMPPTransaction.startBlock("ComputeRelay");
			log.addInfo(request.hashCode() + " AddStreamRequestHandler:getRelayProfileResult Begin", MRLog.OK, MRLog.NOTIFY);	

			Document relayProfileResultDoc = StreamPolicyUtils.getRelayProfileResult(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					sourcePortNID, destPortNID, profileNID, profileXML,"","");
			Element relayProfile = MRXMLUtils.getElementXpath(relayProfileResultDoc, "//RelayProfile");
			if(relayProfile != null) {
				relayNID = MRXMLUtils.getAttributeValue(relayProfile,"relayNID","");
				profileNID = MRXMLUtils.getAttributeValue(relayProfile,"profileNID");
				Element profileXMLElement = MRXMLUtils.getFirstChild(relayProfile);
				StreamPolicyUtils.updateRecorderConfigInProfile(request, xmpp, log ,db, sourceNID, profileXMLElement);
				profileXML = MRXMLUtils.elementToString(profileXMLElement);
			} else {
				relayNID = "";
			}
			com.ipvs.common.XMPPTransaction.endBlock("ComputeRelay");
			log.addInfo(request.hashCode() + " AddStreamRequestHandler:getRelayProfileResult End", MRLog.OK, MRLog.NOTIFY);	
		}
		
		
		
		/**
		 *  case a: validate no of streams connected to v2d tx encoder. Maximum it allows one stream
		 *  case b: replace client profiles parameters for kbm if stream is auto started 
		 *  
       	 */
		String streamType = srcStreamType;
		Document profileDoc = MRXMLUtils.stringToDocument(profileXML);
		// System.out.println("relayNID:="+relayNID +" \nstreamType:="+streamType+" \nsrcStreamURL:="+srcStreamURL);
		if(relayNID.isEmpty() && streamType.equals(MediaRoomServiceConst.V2D)) {
			
			//v2d://10.51.50.68/port=6060?bandwidth=10240000?avoption=Both?ismulticast=true?enablekbm=false?authscheme=TOKEN?authdata=f16aa06e-8619-417a-819b-6ddad5d2232e

			
			String clientMulticastParam = MRXMLUtils.getValueXpath(profileDoc, "//IsMulticast", "");

			//case a			
			if(clientMulticastParam.equalsIgnoreCase(MediaRoomServiceConst.FALSE) && srcStreamURL.isEmpty()) {
				String xpath = "//Stream/data[sourceNID='"+sourceNID+"' and destNID!='' and relayNID='' and .//IsMulticast='false']";
				Document mediaroomListDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, xpath, false);
				List<?> mediaroomList = MRXMLUtils.getListXpath(mediaroomListDoc, "//MediaRoom");
				if(mediaroomList!=null && !mediaroomList.isEmpty()) {
					throw new MRException(MediaRoomServiceExceptionCodes.SOURCE_BUSY, "Connection error.  The  source device has reached its maximum allowable streams");			
				}
 			}
			
			if(!srcStreamURL.isEmpty() ) {
				log.addInfo("AddStreamRequestHandler: Before Update srcstreamURL :="+srcStreamURL,MRLog.INFO,MRLog.NOTIFY);
				
				//case b: update client profile information in the  source URL
				String regex =  "enablekbm=(?i)[A-Z]+";
				Matcher matcher = Pattern.compile(regex).matcher(srcStreamURL);
				if(matcher.find()) {
					srcStreamURL= matcher.replaceAll("enablekbm="+MRXMLUtils.getValueXpath(profileDoc,"//EnableKbm","false"));
				}
				
				log.addInfo("AddStreamRequestHandler:Updated srcstreamURL :="+srcStreamURL,MRLog.INFO,MRLog.NOTIFY);
			}
			
		}
		
		
		com.ipvs.common.XMPPTransaction.endBlock("SetRelay");
		
		if(!"".equals(relayNID)) {
			com.ipvs.common.XMPPTransaction.startBlock("IfRelaySetSourceAgentJID");
			com.ipvs.common.XMPPTransaction.startBlock("readRelayPortDoc");
			relayPortDoc = MRXMLDBUtils.readElement(request, log, db, relayNID);
			String title = MRXMLUtils.getValueXpath(relayPortDoc, "//Title", "");
			com.ipvs.common.XMPPTransaction.endBlock("readRelayPortDoc");
			com.ipvs.common.XMPPTransaction.startBlock("getRelayAgentJID");
			relayAgentJID = MediaRoomServiceUtils.getAgentJID(request, relayPortDoc);
			com.ipvs.common.XMPPTransaction.endBlock("getRelayAgentJID");

			if (sourceAgentJID.equals(relayAgentJID)) { // Make sure we are not relaying to ourselves
				relayNID = "";
				relayAgentJID = "";
				relayPortDoc = null;
			} else {
				sourceAgentJID = relayAgentJID;
				Element mediaRelayElement = MRXMLUtils.getElementXpath(roomdoc, 
						"//MediaRelay[data/relayNID='" + relayNID + "']");
				if(mediaRelayElement != null) {
					mediaRelayNID = MRXMLUtils.getAttributeValue(mediaRelayElement,"NID");
					// System.out.println("AddStreamRequestHandler:getResponse reusing mediaRelay NID=" + mediaRelayNID);
				} else {
					String state = MediaRoomServiceConst.UNAVAILABLE;
					if (RoomDB.getRoomDB().isReady(xmpp, roomNID, relayAgentJID,  request.getUserJID(), request.getAppServerJID())) {
						state = MediaRoomServiceConst.AVAILABLE; 
					}
					XMPPTransaction.startBlock("createMediaRelayElement");

					mediaRelayNID = RoomDB.getRoomDB().createMediaRelayElement(roomNID, state, relayNID, relayAgentJID, 
							trackNID, title, request, log, db);

					XMPPTransaction.endBlock("createMediaRelayElement");

					com.ipvs.common.XMPPTransaction.startBlock("sendRelayInvite");
					String assetResourceNID = MRXMLUtils.getValueXpath(relayPortDoc, "//AssetResourceNID", "");
					RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, relayAgentJID, assetResourceNID);
					com.ipvs.common.XMPPTransaction.endBlock("sendRelayInvite");
				}
			}
			com.ipvs.common.XMPPTransaction.endBlock("IfRelaySetSourceAgentJID");
		}

		//com.ipvs.common.XMPPTransaction.setDEBUG(true);
		com.ipvs.common.XMPPTransaction.startBlock("AdjustStreamProfileAndURLType");

		
		// If no SDP Template then set the profile to be empty
		if(MediaRoomServiceConst.RTP.equals(srcStreamType) ) {
			String sdpTemplate =  MRXMLUtils.getValueXpath(profileDoc, "//SDP-Template-Src", "");
			if("".equals(sdpTemplate))
				profileXML = "";
		}
		// If empty stream profile then use the src stream profile
		if("".equals(profileXML)) {
			profileXML = srcStreamProfile;
			profileDoc = MRXMLUtils.stringToDocument(profileXML);					
		}

		String streamURLType = "";
		if(	MediaRoomServiceConst.RTP.equals(streamType) &&
				(AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE.equals(destAssetType) ||
						AddStreamRequestHandler.isUnManagedDest(destStreamURL,destAssetType))
				) {
			streamURLType = MediaRoomServiceConst.HTTP;
			//direct connection is not supported from older ipvs clients make sure we are not strting direct connection from those players
			if(isUnManagedSource(mediaRelayNID, srcStreamURL) && !isUnManagedDest(destStreamURL, destAssetType)){
				throw new MRException(MediaRoomServiceExceptionCodes.DIRECT_CONNECTION_NOT_SUPPORTED_FOR_RTP, "Direct Connection Not supported for RTP Source");
			}
		}

		com.ipvs.common.XMPPTransaction.endBlock("AdjustStreamProfileAndURLType");
		//com.ipvs.common.XMPPTransaction.setDEBUG(false);

		// Set the destPortElementID to be the streamID for IPVClient
		// This will allow IPVSClient reuse in case of switch
		if(AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE.equals(destAssetType)) {
			String roomID = roomdoc.getRootElement().attributeValue(MediaRoomServiceConst.ID);
			String escapedroomId = java.util.regex.Pattern.quote(roomID);
			String escapedId = java.util.regex.Pattern.quote(id);
			destPortElementID = escapedroomId + "." + escapedId;
		}

		// com.ipvs.common.XMPPTransaction.setDEBUG(true);
		com.ipvs.common.XMPPTransaction.startBlock("ResolveNAT");

		String streamURL = "";
		Document natResultDoc = null;
		if(!isUnManagedSource(relayNID, srcStreamURL)) {
			Element sourcePortResource = null;
			if(relayPortDoc != null)
				sourcePortResource = relayPortDoc.getRootElement();
			else {
				Document sourcePortDoc = MRXMLDBUtils.readElement(request, log, db, sourcePortNID);
				sourcePortResource = sourcePortDoc.getRootElement();
			}

			Element destPortResource = null;
			if("".equals(destNID)){
				destPortResource = null;
			}
			else {
				Document destPortDoc = MRXMLDBUtils.readElement(request, log, db, destPortNID);
				destPortResource = destPortDoc.getRootElement();
			}
			natResultDoc = StreamPolicyUtils.getNATResult(request, xmpp, log, db, 
					sourcePortResource, destPortResource, destIP, streamType);
		}
		else {
			// If direct connection to unmanaged source which has a srcStreamURL
			streamURL = srcStreamURL;
			natResultDoc = MRXMLUtils.stringToDocument(
					"<NATResult>" +
							"<sourcerule publishedIP='' resolvedIP=''/>" +
							"<destrule publishedIP='' resolvedIP=''/>" +
					"</NATResult>");			
		}
		Element sourceRule = MRXMLUtils.getElementXpath(natResultDoc, "//sourcerule");
		Element sourceNetworkAddressRule = sourceRule.element("NetworkAddressRule");
		Element destRule = MRXMLUtils.getElementXpath(natResultDoc, "//destrule");
		Element destNetworkAddressRule =null;
		String resolvedDestIP = "";
		String publishedDestIP = "";
		if(destRule != null){        	
			destNetworkAddressRule = destRule.element("NetworkAddressRule");
			resolvedDestIP = MRXMLUtils.getAttributeValue(destRule,"resolvedIP");
			publishedDestIP = MRXMLUtils.getAttributeValue(destRule,"publishedIP");
		}

		String resolvedSourceIP = MRXMLUtils.getAttributeValue(sourceRule,"resolvedIP");
		String publishedSourceIP = MRXMLUtils.getAttributeValue(sourceRule,"publishedIP");




		com.ipvs.common.XMPPTransaction.endBlock("ResolveNAT");
		// com.ipvs.common.XMPPTransaction.setDEBUG(false);

		com.ipvs.common.XMPPTransaction.startBlock("createStreamElement");
		if(AddStreamRequestHandler.isUnManagedDest(destStreamURL,destAssetType)) {
			//if destination is unmanaged set the agent jid as appserveragent (component jid) 
			destAgentJID = request.getAppServerJID();
		}


		//Assign multicastIPAddress 
		
		
		String roomId = MRXMLUtils.getAttributeValue(roomdoc.getRootElement(), MediaRoomServiceConst.ID, "");
		String roomOwnerJID = MRXMLUtils.getValueXpath(roomdoc, "//roomOwnerJID", "");		
		String streamNID = Utils.getUUID();
		String multicastIP= "";
		if(MediaRoomServiceConst.OUTPUT_DECODER.equals(destType) || MediaRoomServiceConst.OUTPUT_STREAM.equals(destType)){ //check streamtype only if LIVE then only assign multicast
		    multicastIP = AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory).getServiceConfigNetworkResources().reserveMulticastIPAddress(streamNID, sourceNID, relayNID, profileDoc);
		}
		RoomDB.getRoomDB().createStreamElement(roomId, id,roomOwnerJID, mediaSourceNID, mediaDestNID,mediaRelayNID, description, profileXML, profileNID, roomNID, owneruserJID, sourceNID, sourcePortNID, sourceAgentJID, 
				destNID, destPortNID, destPortElementID, destType,
				destAgentJID, destFileUUID ,destFileParentDirNID,relayNID, xAuthAgentJID,  streamType, streamURL, destIP, destUDPStartPort, callbackPort, destStreamURL,
				streamURLType, trackNID, MediaRoomServiceConst.TRACK_STARTED_CURRENT, 0, resolvedSourceIP, resolvedDestIP, publishedSourceIP.toString(), publishedDestIP.toString(), 
				sourceNetworkAddressRule, destNetworkAddressRule ,windowId,canvasId, trimLength, trickPlay,streamNID,multicastIP,fileInfoElement, xmpp, request, log, db,serviceAgentFactory);		
		com.ipvs.common.XMPPTransaction.endBlock("createStreamElement");

				// com.ipvs.common.XMPPTransaction.DEBUG = false;
		long duration = System.currentTimeMillis() - startTime;
		log.addInfo(request.getUserJID() + ":AddStreamRequest:" + String.valueOf(duration) + " action=Start End", MRLog.OK, MRLog.NOTIFY);

		

		com.ipvs.common.XMPPTransaction.endBlock("AddStreamRequest");
		// com.ipvs.common.XMPPTransaction.setDEBUG(false);

		return request.createResponseData("streamNID", streamNID);
	}


	@Override
	public void handleAsynchDBRequestResponse(String dbAction,
			Element responseElement, Object callBackDataObject, int status, String logData, ServiceAgentFactoryI serviceAgentFactory)
					throws Exception {
		if(status != MRRequest.STATE_DONE) {
			MRLog.getInstance(MRLog.CONSOLE).addInfo(callBackDataObject+" port Instance Failed LogData:=" +logData);
		}		
	}

}
