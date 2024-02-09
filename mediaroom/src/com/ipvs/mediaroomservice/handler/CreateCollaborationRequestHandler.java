package com.ipvs.mediaroomservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.CollaborationManager;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;


public class CreateCollaborationRequestHandler implements MRRequestHandlerI {

	public MRAuthorization authorizeRequest(MRRequest request,XMPPI xmpp, MRLog log, MRDB schema, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addTraceBegin(request);
		int authType = MediaRoomObject.ROOM_TYPEID;
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


	public static Element getEntityElement(Document roomDoc, String entityID, String entityType) throws Exception {
		Element entityIDElement = MRXMLUtils.getElementXpath(roomDoc, "//Entity[@id = '" + entityID + "' and @role = '" + entityType+ "']");
		return entityIDElement;
	}

	public static String getMediaSourceNID(Document roomDoc, String entityID) throws Exception {
		Element mediaSource = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@id = '" + entityID  + "']");
		return MRXMLUtils.getAttributeValue(mediaSource,"NID");
	}
	
	public static String getMediaDestNID(Document roomDoc, String entityID) throws Exception {
		Element mediaDest = MRXMLUtils.getElementXpath(roomDoc, "//MediaDest[@id = '" + entityID  + "']");
		return MRXMLUtils.getAttributeValue(mediaDest,"NID");
	}

	public static String getStreamNID(Document roomDoc, String connectionID) throws Exception {
		Element stream = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id = '" + connectionID  + "']");
		return MRXMLUtils.getAttributeValue(stream,"NID");
	}


	private String handleConnectionRequest(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Element connectionElement, Document roomDoc,
			String roomNID, String id, String requestName) throws Exception {
		String connectionID = MRXMLUtils.getAttributeValue(connectionElement,MediaRoomServiceConst.ID);
		String sourceEntityID = MRXMLUtils.getValueXpath(connectionElement, "sourceEntityID", "");
		if("".equals(sourceEntityID))
			throw new MRException(MediaRoomServiceExceptionCodes.MEDIASOURCENOTFOUND, connectionID);
		String destEntityID = MRXMLUtils.getValueXpath(connectionElement, "destinationEntityID", "");
		String profileNID = MRXMLUtils.getValueXpath(connectionElement, "profileNID", "");
		String description = MRXMLUtils.getValueXpath(connectionElement, "description", "");
		String relayNID = MRXMLUtils.getValueXpath(connectionElement, "relayNID", "");
		Element profileXML = MRXMLUtils.getElementXpath(connectionElement, "profileXML");
		String mediaSourceNID = CreateCollaborationRequestHandler.getMediaSourceNID(roomDoc, sourceEntityID);
		String mediaDestNID = CreateCollaborationRequestHandler.getMediaDestNID(roomDoc, destEntityID);
// System.out.println("CreateCollaborationRequestHandler:handleConnectionRequest request#=" + request.hashCode());

		Element responseElement = ConnectionUtils.createConnectionInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, 
				id, mediaSourceNID, mediaDestNID, roomNID, description,
				profileNID, profileXML, relayNID,"","");
		String streamNID = MRXMLUtils.getValueXpath(responseElement, "streamNID", "");
		return streamNID;
	}

	public Document getResponse(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		// Element mroot = ((MRMessage)request.getMessage()).getDocument().getRootElement();
// com.ipvs.common.XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
com.ipvs.common.XMPPTransaction.startBlock("CreateCollaborationRequest");

		String requestName = "CreateCollaborationRequest";
		
		request.setCache(new MRRequestCache());
		
        Document requestDoc = request.getMessage().getDocument();

		String collaborationID = MRXMLUtils.getValueXpath(requestDoc, "//collaborationID", "");
		
		// The collaborationID is used for all session api calls
		if("".equals(collaborationID.trim())) {
			collaborationID = "default." + System.currentTimeMillis();
			Element collaborationIDElement = MRXMLUtils.getElementXpath(requestDoc, "//collaborationID");
			if(collaborationIDElement == null) {
				collaborationIDElement = MRXMLUtils.stringToElement("<collaborationID/>");
				Element requestData = MRXMLUtils.getElementXpath(requestDoc, "//CreateCollaborationRequestData");
				MRXMLUtils.addElement(requestData,collaborationIDElement);
			}
			collaborationIDElement.setText(collaborationID);
		}
		
		String collaborationTitle = MRXMLUtils.getValueXpath(requestDoc, "//CreateCollaborationRequestData/Info/Title", "");
		String collaborationDescription = MRXMLUtils.getValueXpath(requestDoc, "//CreateCollaborationRequestData/Info/Description", "");
		Element collaborationGroups = MRXMLUtils.getElementXpath(requestDoc, "//CreateCollaborationRequestData/Info/Groups");		
		String syncSourceEntityID = MRXMLUtils.getValueXpath(requestDoc, "//syncSourceEntityID","");
		//String startTime = String.valueOf(xmpp.getCurrentTimeMillis());
		String syncData = "";
		String roomType = MediaRoomServiceConst.COLLABORATION;
		
		// System.out.println("CreateCollaborationRequestHandler: collaborationData=" + MRXMLUtils.elementToString(collaborationData));
		Document response = ConnectionUtils.addMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, 
				collaborationID, collaborationTitle, collaborationDescription, syncData, roomType, collaborationGroups);
		String roomNID = MRXMLUtils.getValueXpath(response, "//roomNID","");
		
		Document roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		Element collaborationData = MRXMLUtils.getElementXpath(requestDoc, "//CreateCollaborationRequestData");
		CollaborationManager collaborationManager = CollaborationManager.getCollaborationManager();
		collaborationManager.addCollaboration(request, log, db, roomNID, collaborationData, roomDoc);

		// Create all the MediaSources and MediaDestinations and update the collaboration
		List<?>entityElementList = MRXMLUtils.getElementXpath(requestDoc, "//EntityList").elements();
		for(int i=0; i< entityElementList.size();i++) {
			Element entity = (Element)entityElementList.get(i);
			String entityID = MRXMLUtils.getAttributeValue(entity,MediaRoomServiceConst.ID);
			String role = MRXMLUtils.getAttributeValue(entity,"role");
			String resourceNID = MRXMLUtils.getValueXpath(entity, ".//resourceNID","");
			try {
				if(MediaRoomServiceConst.MEDIASOURCE.equals(role)) {
					String userRoomMediaSourceNID = "";
					String description = MRXMLUtils.getValueXpath(entity, ".//resourceNID","");
					String startOffset = MRXMLUtils.getValueXpath(entity, ".//startOffset", "");
					String startTimecode = MRXMLUtils.getValueXpath(entity, ".//startTimecode", "");
					response = ConnectionUtils.addMediaSource(request,xmpp, log, db,resourceFactory, serviceAgentFactory, requestName,  roomNID,  
							resourceNID,  userRoomMediaSourceNID,  description,  startOffset,  startTimecode, entityID);
					String	mediaSourceNID = MRXMLUtils.getValueXpath(response,"//mediaSourceNID", "");
					if(entityID.equals(syncSourceEntityID)) {
				    	MediaRoomServiceUtils.updateTrackSyncData(roomNID, mediaSourceNID, request, db, log);
					}
					Document mediaSourceDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, mediaSourceNID);
					collaborationManager.handlePresenceChanged(xmpp, request, log, db, roomNID, mediaSourceDoc.getRootElement());
					// System.out.println("Added MediaSource id=" + entityID + " NID=" + mediaSourceNID);
				}
				if(MediaRoomServiceConst.MEDIADEST.equals(role)) {
					String description = MRXMLUtils.getValueXpath(entity, ".//description","");
					String destIP = MRXMLUtils.getValueXpath(entity, ".//destIP","");
					String destUDPStartPort = MRXMLUtils.getValueXpath(entity, ".//destUDPStartPort","");
					response = ConnectionUtils.addMediaDest(request,xmpp, log, db,resourceFactory, serviceAgentFactory, roomNID,  
							resourceNID,  description, entityID, destIP, destUDPStartPort, "", null);
					String	mediaDestNID = MRXMLUtils.getValueXpath(response,"//mediaDestNID", "");
					// System.out.println("Added MediaDest id=" + entityID + " NID=" + mediaDestNID);
					Document mediaDestDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, mediaDestNID);
					collaborationManager.handlePresenceChanged(xmpp, request, log, db, roomNID, mediaDestDoc.getRootElement());
				}
			}
			catch(Exception e) {
				collaborationManager.handleSetupError(xmpp, request, log, db, roomNID, entityID, e);
			}
		}
		

		List<?>connectionElementList = MRXMLUtils.getElementXpath(requestDoc, "//ConnectionList").elements();
		// The roomDoc will now have changed after the addition of the MediaSources and MediaDestinations
		roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		for(int i=0; i< connectionElementList.size();i++) {
			// System.out.println("CreateCollaborationRequestHandler:Processing connection element i=" + i);
			Element connectionElement = (Element)connectionElementList.get(i);
			String connectionID = MRXMLUtils.getAttributeValue(connectionElement,MediaRoomServiceConst.ID);
			try {
				String streamNID = this.handleConnectionRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
						connectionElement, roomDoc, roomNID, connectionID, requestName);
				Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
				String	mediaSourceNID = MRXMLUtils.getValueXpath(response,"//mediaSourceNID", "");
				Document mediaSourceDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, mediaSourceNID);
				collaborationManager.handleStreamStatusChanged(xmpp, request, log, db, roomNID, streamDoc.getRootElement(), mediaSourceDoc.getRootElement());
			}catch(Exception e) {
				e.printStackTrace();
				collaborationManager.handleSetupError(xmpp, request, log, db, roomNID, connectionID, e);
			}
		}

		// Refresh the roomDoc
		roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		Element data = collaborationManager.getData(request, xmpp, log, db, roomNID);
		roomDoc.getRootElement().add(data);
		response = request.createResponseData((Element)(roomDoc.getRootElement().detach()));
		
com.ipvs.common.XMPPTransaction.endBlock("CreateCollaborationRequest");
// com.ipvs.common.XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());

        return response;
	}
}
