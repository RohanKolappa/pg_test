package com.ipvs.sessionservice.impl;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.notification.ResourceManager;
import com.ipvs.policyadminservice.impl.SessionPolicyUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.utils.Utils;

public class SessionMediaRoomUtils {
	
	public static final String GET_SESSION_REQUEST_XML = "/com/ipvs/sessionservice/session/xml/GetSessionRequest.xml";
	
	static void updateSharedPVRInfo(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
		Element srcEntityElement, String sourceNID, Element pvrElement) {	
	    try {
		//
		String pvrSessionId = sourceNID + "." + SessionServiceConst.PVR + SessionServiceConst.SESSION;

		SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);			
		Document sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, pvrSessionId);			
		Element connectionElement = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@state='"+SessionServiceConst.CONNECTION_STATE_STREAMING+"']");
		if(connectionElement ==null) {
		    return;
		}
		//check access policy and connection policy for pvr, if policy allows then only update the shared pvr info		
		String pvrAccess = SessionPolicyUtils.getPVRAccessXpath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
		if(pvrAccess.equals(PolicyAdminConstants.NULL))
		    return;				
		SessionPolicyUtils.getDirNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sourceNID, MediaRoomServiceConst.PVRMEDIACLIP, "",pvrAccess);
		String connId  = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.ID, "");
		Element srcEntityPVRElement = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.SRCENTITYID)+"']/PVR");
		MRXMLUtils.setAttributeValue(pvrElement, SessionServiceConst.SESSION_ID, pvrSessionId);
		MRXMLUtils.setAttributeValue(pvrElement, SessionServiceConst.CONN_ID, connId);
		MRXMLUtils.setAttributeValue(pvrElement, SessionServiceConst.TRIMLENGTH, MRXMLUtils.getAttributeValue(srcEntityPVRElement, SessionServiceConst.TRIMLENGTH,MediaRoomServiceConst.DEFAULT_TRIMLENGTH));
		MRXMLUtils.setAttributeValue(pvrElement, SessionServiceConst.START_TC_ATTRIBUTE, MRXMLUtils.getAttributeValue(srcEntityPVRElement, SessionServiceConst.START_TC_ATTRIBUTE));
		MRXMLUtils.setAttributeValue(pvrElement, SessionServiceConst.FF_TRACKS, MRXMLUtils.getAttributeValue(srcEntityPVRElement, SessionServiceConst.FF_TRACKS));
	    }
	    catch(Exception e) {
		return;
	    }
	}
	
	static Document getSharedPVRStream(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Element srcEntityElement, String sourceNID) throws Exception {

		String pvrSessionId = sourceNID + "." + SessionServiceConst.PVR + SessionServiceConst.SESSION;
	
		Element pvrElement = srcEntityElement.element(SessionServiceConst.PVR);
		if(pvrElement == null)
			return null;
		String sharedSessionID = pvrElement.attributeValue(SessionServiceConst.SESSION_ID,"");
		if(sharedSessionID.isEmpty())
			return null;
		try {
			SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);		
			String roomNID = sessionManager.getRoomNID(pvrSessionId);
			Document streamDoc = MediaRoomServiceUtils.readElementByXpath(roomNID, request, log, db, "//Stream[@id='"+pvrElement.attributeValue(SessionServiceConst.CONN_ID,"")+"']");
			if(streamDoc == null) {
				return null;
			}		
			String streamSourceNID = MRXMLUtils.getValueXpath(streamDoc, ".//sourceNID", "");
			if(!sourceNID.equals(streamSourceNID))
				return null;
			return streamDoc;
		}
		catch(Exception e) {
			return null;
		}
	}

	public static String createMediaSource(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String srcEntityID, Document sessionDoc, String roomNID) throws Exception {
		Element srcEntityElement = MRXMLUtils.getElementXpath(sessionDoc, 
				"//Entity[@" + SessionServiceConst.ID + "='" + srcEntityID + "']");
		String type = MRXMLUtils.getAttributeValue(srcEntityElement,SessionServiceConst.TYPE_ATTRIBUTE);
		Element resource = null; 
		if(SessionServiceConst.SRCPORT.equals(type))
			resource = MRXMLUtils.getElementXpath(srcEntityElement, ".//" + SessionServiceConst.PORT);
		else if(SessionServiceConst.PLYFILE.equals(type))
			resource = MRXMLUtils.getElementXpath(srcEntityElement, ".//" + SessionServiceConst.FILE);
		
		if(resource == null) {
			throw new MRException(SessionServiceExceptionCodes.SOURCE_RESOURCE_NOT_FOUND, "SOURCE_RESOURCE_NOT_FOUND");
		}
		String sourceNID = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCENID);
		if((sourceNID == null) || "".equals(sourceNID)) {
			throw new MRException(SessionServiceExceptionCodes.SOURCE_RESOURCE_NOT_FOUND, "SOURCE_RESOURCE_NOT_FOUND");
		}
		String userRoomMediaSourceNID = "";
		String description = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCETITLE);
		String startOffset = "";
		String startTimecode = "";
		String requestName = request.getRequestName();
		// System.out.println("StartSessionRequestHandler:createMediaSource sessionDoc=" + MRXMLUtils.documentToString(sessionDoc) + " entity=" + MRXMLUtils.elementToString(srcEntityElement));
		
		
		String lookbackSourceNID = "";
		String lookbackSourceAgentJID = "";
		String lookbackSourceFileUUID = "";
		String lookbackSourceparentDirNID = "";
		String lookbackSourceFilePlayLength = "";
		String lookbackSourceFileStartTC = "";		
		
		Document sharedPVRStreamDoc = SessionMediaRoomUtils.getSharedPVRStream(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcEntityElement, sourceNID);
	   if(sharedPVRStreamDoc != null) {
			lookbackSourceNID = MRXMLUtils.getValueXpath(sharedPVRStreamDoc, "//destNID", "");
			lookbackSourceAgentJID = MRXMLUtils.getValueXpath(sharedPVRStreamDoc, "//destAgentJID", "");
			lookbackSourceFileUUID = MRXMLUtils.getValueXpath(sharedPVRStreamDoc, "//destFileUUID", "");
			lookbackSourceparentDirNID = MRXMLUtils.getValueXpath(sharedPVRStreamDoc, "//destFileParentDirNID", "");
			lookbackSourceFilePlayLength = MRXMLUtils.getValueXpath(sharedPVRStreamDoc, "//trimLength", "");
			lookbackSourceFileStartTC = MRXMLUtils.getAttributeValueXpath(sharedPVRStreamDoc, "//MediaTimeline/Last", "ssTC", "");
		}
		
		Document response = ConnectionUtils.addMediaSourceWithLookbackInfo(request,xmpp, log, db,resourceFactory, serviceAgentFactory, 
				requestName,  roomNID, sourceNID,  userRoomMediaSourceNID,  description,  startOffset,  startTimecode, srcEntityID,
				lookbackSourceNID,lookbackSourceAgentJID,lookbackSourceFileUUID,lookbackSourceparentDirNID, lookbackSourceFilePlayLength,lookbackSourceFileStartTC);
		
		String	mediaSourceNID = MRXMLUtils.getValueXpath(response,"//mediaSourceNID", "");
		return mediaSourceNID;
	}

	public static String createMediaDest(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String dstEntityID, Document sessionDoc, String roomNID) throws Exception {
		Element dstEntityElement = MRXMLUtils.getElementXpath(sessionDoc, 
				"//Entity[@" + SessionServiceConst.ID + "='" + dstEntityID + "']");
		String type = MRXMLUtils.getAttributeValue(dstEntityElement,SessionServiceConst.TYPE_ATTRIBUTE);
		Element resource = null;
		String trimLength = "";
		Element fileInfoElement = null;
		if(SessionServiceConst.DSTPORT.equals(type))
			resource = MRXMLUtils.getElementXpath(dstEntityElement, ".//" + SessionServiceConst.PORT);
		else if(SessionServiceConst.RECFILE.equals(type) || SessionServiceConst.PVRFILE.equals(type)) {
			
			resource = MRXMLUtils.getElementXpath(dstEntityElement, ".//" + SessionServiceConst.FILE);

			//add file info in media destination
			fileInfoElement = MRXMLUtils.newElement(MediaRoomServiceConst.FILE_INFO_ELEMENT);
			MRXMLUtils.addAttribute(fileInfoElement, SessionServiceConst.TITLE, MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCETITLE));
			String fileNID =  MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCENID);
			Document mediaStreamFileDoc = MRXMLDBUtils.readElement(request, log, db, fileNID);
			
			Element fileMetaDataEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc,"//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
			if(fileMetaDataEL!=null) {
				MRXMLUtils.addElement(fileInfoElement, (Element)fileMetaDataEL.clone());			
			}
			
			MRXMLUtils.addAttribute(fileInfoElement, SessionServiceConst.CLIPID, MRXMLUtils.getAttributeValue(resource,SessionServiceConst.CLIPID));
			MRXMLUtils.addAttribute(fileInfoElement, SessionServiceConst.MEDIAGROUPID, MRXMLUtils.getAttributeValue(resource,SessionServiceConst.MEDIAGROUPID));
			
			//add group information if the file is sync clip
			Element groupElement = MRXMLUtils.getElementXpath(sessionDoc, "//GrpFile[@"+SessionServiceConst.CLIPENTITYID+"='"+dstEntityID+"']");
			if(groupElement!=null) {
				Element groupInfoElement = MRXMLUtils.newElement(MediaRoomServiceConst.GROUP_INFO_ELEMENT);
			
				MRXMLUtils.addAttribute(groupInfoElement, SessionServiceConst.TITLE, MRXMLUtils.getAttributeValue(groupElement,SessionServiceConst.RESOURCETITLE));
				String groupNID  = MRXMLUtils.getAttributeValue(groupElement,SessionServiceConst.RESOURCENID);
				MRXMLUtils.addAttribute(groupInfoElement, SessionServiceConst.NID, groupNID);
				mediaStreamFileDoc = MRXMLDBUtils.readElement(request, log, db, groupNID);
				String mediaGroupUUID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.UUID, "");
				Element groupMetaDataEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
				MRXMLUtils.addAttribute(groupInfoElement, MediaStoreServiceConst.UUID, mediaGroupUUID);
				
				if(groupMetaDataEL!=null) {
					MRXMLUtils.addElement(groupInfoElement, (Element)groupMetaDataEL.clone());			
				}
				
				MRXMLUtils.addElement(fileInfoElement,groupInfoElement);
			}
			
		}
		
		if(resource == null) {
			throw new MRException(SessionServiceExceptionCodes.DEST_RESOURCE_NOT_FOUND, "DEST_RESOURCE_NOT_FOUND");
		}
		
		if(SessionServiceConst.PVRFILE.equals(type)) {
			Element connEl = MRXMLUtils.getElementXpath(sessionDoc, 
				"//Connection[@dstEntityId='" + dstEntityID + "']");
			String connID = MRXMLUtils.getAttributeValue(connEl, "id");
			Element pvrElement = MRXMLUtils.getElementXpath(sessionDoc, 
					"//PVR[@connId='" + connID + "']");
			trimLength = MRXMLUtils.getAttributeValue(pvrElement, "trimLength", MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
		}
		
		
		String destNID = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCENID);
		String description = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCETITLE);
		String destIP = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCEIP);
		if(destIP == null)
			destIP = "";
		String destUDPStartPort = MRXMLUtils.getAttributeValue(resource,SessionServiceConst.RESOURCEPORTNUMBER);
		if(destUDPStartPort == null)
			destUDPStartPort = "";
		log.addInfo("SessionMediaRoomUtils:createMediaDest IP:port=" + destIP + ":" + destUDPStartPort, MRLog.OK, MRLog.NOTIFY);
		Document response = ConnectionUtils.addMediaDest(request,xmpp, log, db,resourceFactory, serviceAgentFactory, roomNID,  
				destNID,  description, dstEntityID, destIP, destUDPStartPort, trimLength,fileInfoElement);
		String	mediaDestNID = MRXMLUtils.getValueXpath(response,"//mediaDestNID", "");
		return mediaDestNID;
	}


	public static void startConnectionList(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			List<?> connectionList, String sessionId) throws Exception {
		
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		Element connectionElement = null;
		Element mediaSourceElement = null;
		Element mediaDestElement = null;
		String sourcePortNID  = "";
		String destPortNID = "";
		Hashtable<String, String> streamNIDTable = new Hashtable<String, String>();
		SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		// Refresh the roomdoc
		Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		Document sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		
		// Create the Connections
		//Hashtable<String, Document> srcBasedRelayResults = new Hashtable<String, Document>();
		for(Object o: connectionList) {
			connectionElement = (Element)o;
			String connectionID = MRXMLUtils.getAttributeValue(connectionElement,MediaRoomServiceConst.ID);
			
			//check connection state is error or not
		    String state = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.STATE,"");
		    if(!state.isEmpty()) {
		    	continue;
		    }
		    		    
			try {
				String srcEntityID = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.SRCENTITYID);
				if("".equals(srcEntityID))
					throw new MRException(SessionServiceExceptionCodes.SOURCE_ENTITY_ERROR,
							"srcEntityId not found in connection");
				
				//check source entity has any error 
				Element srcEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityID+"']");
				String errorCode = MRXMLUtils.getAttributeValue(srcEntity, SessionServiceConst.ERROR_CODE,"");
				if(!errorCode.isEmpty()) {
					throw new MRException(Utils.getIntValue(errorCode,SessionServiceExceptionCodes.SOURCE_ENTITY_ERROR),MRXMLUtils.getAttributeValue(srcEntity, SessionServiceConst.ERROR_DATA));
				}
				
				
				mediaSourceElement = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource[@id='" + srcEntityID + "']");				
				if(mediaSourceElement == null) {
						throw new MRException(SessionServiceExceptionCodes.SOURCE_ENTITY_ERROR,
							"MediaSource not found in room srcEntityId=" + srcEntityID);
				}
				
				String mediaSourceNID = mediaSourceElement.attributeValue(SessionServiceConst.NID);
				String dstEntityID = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.DSTENTITYID);
				if("".equals(dstEntityID)) {
					throw	new MRException(SessionServiceExceptionCodes.DEST_ENTITY_ERROR,
							"dstEntityId not found in connection");
				}
				
				
				//check destination entity has any error 
				Element dstEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+dstEntityID+"']");
				errorCode = MRXMLUtils.getAttributeValue(dstEntity, SessionServiceConst.ERROR_CODE,"");
				if(!errorCode.isEmpty()) {
					throw new MRException(Utils.getIntValue(errorCode,SessionServiceExceptionCodes.DEST_ENTITY_ERROR),MRXMLUtils.getAttributeValue(dstEntity, SessionServiceConst.ERROR_DATA));
				}
				
				
				mediaDestElement = MRXMLUtils.getElementXpath(roomdoc, "//MediaDest[@id='" + dstEntityID + "']");
				if(mediaDestElement == null) {
					throw	new MRException(SessionServiceExceptionCodes.DEST_ENTITY_ERROR,
							"MediaDest not foundin room dstEntityId=" + dstEntityID);
				}
				String mediaDestNID = mediaDestElement.attributeValue(SessionServiceConst.NID);
				String description = "ConnectionDescription";						
				
				XMPPTransaction.startBlock("ComputeRelayProfile");
				
				
				// Compute the profile and relay
				sourcePortNID = MRXMLUtils.getValueXpath(roomdoc, 
						"//MediaSource[@NID='" + mediaSourceNID + "']/data/sourcePortNID", "");
				
				destPortNID = MRXMLUtils.getValueXpath(roomdoc, 
						"//MediaDest[@NID='" + mediaDestNID + "']/data/destPortNID", "");
				
				// Cache the src based relay result for this request only
				//Document srcRelayProfileResultDoc = srcBasedRelayResults.get(sourceNID);
				//if(srcRelayProfileResultDoc == null) {
				
				//check requested profile matching with policy or not.
				String requestedProfileNID = "";
				String requestedProfileTitle = "";
				
				String profileId = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.PROFILE_ID,"");
				if(!profileId.isEmpty()) {
					sessionDoc  = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
					Element profileEl = MRXMLUtils.getElementXpath(sessionDoc,"//Profile[@id='"+profileId+"']");
					if(profileEl != null ) {
						requestedProfileNID = MRXMLUtils.getAttributeValue(profileEl, "profileNID","");
						requestedProfileTitle = MRXMLUtils.getAttributeValue(profileEl, "name","");						
					}					
				}
				
				
				
				Document relayProfileResultDoc = StreamPolicyUtils.getRelayProfileResult(
						request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
						sourcePortNID, destPortNID, "", null,requestedProfileNID,requestedProfileTitle);
				
				
				Element relayProfile = MRXMLUtils.getElementXpath(relayProfileResultDoc, "//RelayProfile");	
				
						
				
				String relayNID = MRXMLUtils.getAttributeValue(relayProfile,"relayNID");
				String profileNID = MRXMLUtils.getAttributeValue(relayProfile,"profileNID");
				Element streamProfile = MRXMLUtils.getFirstChild(relayProfile);
				Element profileXML = MRXMLUtils.stringToElement("<profileXML/>");
				profileXML.add(streamProfile.detach());
				
				
				//update profile info in session doc - nice to have this 
				String profileTitle =  MRXMLUtils.getValueXpath(streamProfile, ".//Title", "");
				if(!profileNID.isEmpty() && !profileTitle.isEmpty()) {
					String sourceNID = MRXMLUtils.getValueXpath(roomdoc, 
							"//MediaSource[@NID='" + mediaSourceNID + "']/data/sourceNID", "");
					StreamPolicyUtils.updateRecorderConfigInProfile(request, xmpp, log ,db, sourceNID, profileXML);
					String ffTracks = MRXMLUtils.getValueXpath(profileXML, ".//V2DRecorderConfig/FFTracks", "");
					Element profileEL = MRXMLUtils.getElementXpath(sessionDoc, "//Profile[@profileNID='"+profileNID+"' or @name='"+profileTitle+"']");
					if(profileEL == null) {
						profileEL = MRXMLUtils.addElement(SessionServiceConst.PROFILE);
						MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.PROFILE_NID, profileNID);
						MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.NAME, profileTitle);
						if(profileId.isEmpty()) {
							profileId = Utils.getUUID();
						}
						MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.ID, profileId);
						MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.FF_TRACKS, ffTracks);
						sessionManager.addProfile(roomNID, profileEL);
					} else {
						profileId = MRXMLUtils.getAttributeValue(profileEL, SessionServiceConst.ID,"");
						sessionManager.updateProfile(roomNID, profileId, profileNID, profileTitle, ffTracks);
					}
					String type = MRXMLUtils.getAttributeValueXpath(connectionElement, "./@type", "");
					if(type.equals(SessionServiceConst.PVR)) {
						sessionManager.updatePVRInfo(roomNID,"", srcEntityID,"","","", ffTracks,"");
					}
					sessionManager.updateProfileInfoInConnection(roomNID, connectionID, profileId);
				}	 
				
				XMPPTransaction.endBlock("ComputeRelayProfile");
				
				String windowId = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.WINDOWID,"");
				String canvasId = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.CANVASID,"");
				
				if( (windowId == null) || windowId.isEmpty()) {
					canvasId = sessionId;
					windowId = connectionID;
					sessionManager.updateConnectionWindow(roomNID, connectionID, canvasId, windowId);
				}
				
				//get the xauth agent jid from media source. if auth jid found then create auth entity in session and update auth entity id in connection
				String xAuthAgentJID = SessionAuthUtils.getXAuthAgentJIDFromMediaSource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionElement, mediaSourceElement,mediaDestElement);
				String authEntityId = "";
				if(!xAuthAgentJID.isEmpty()) {
					authEntityId = SessionAuthUtils.addAuthEntityIfNotExists(request, xmpp, log, db, resourceFactory, serviceAgentFactory,xAuthAgentJID,sessionId,roomNID);
					sessionManager.updateConnectionAuthEntityId(roomNID, connectionID, authEntityId);
					MRXMLUtils.setAttributeValue(connectionElement, SessionServiceConst.AUTH_ENTITY_ID, authEntityId);
				}
				
				Document response = ConnectionUtils.addMediaStream(request,xmpp, log, db, resourceFactory, serviceAgentFactory,
						request.getRequestName(), connectionID, roomNID, mediaSourceNID, mediaDestNID, description, 
						profileNID, profileXML, relayNID,windowId,canvasId,xAuthAgentJID);
				
				String streamNID = MRXMLUtils.getValueXpath(response,"//streamNID", "");
				// connectionElement.setAttribute(SessionServiceConst.ERRORCODE, streamNID);
				streamNIDTable.put(connectionID, streamNID);				
			}
			catch(Exception e) {
				e.printStackTrace();
				sessionManager.handleSetupError(xmpp, request, log, db, roomNID, connectionID, e);
				if(mediaSourceElement != null && mediaDestElement != null) {
					ResourceManager resourceManager = AppServerAgentUtils.getResourceManager(serviceAgentFactory);
					List<Element> observerList = SessionServiceUtils.getMonitorAgentList(mediaSourceElement, SessionServiceConst.PORT, log);
					Document sessionDocument = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
					resourceManager.queueConnectionNotification(xmpp, request, log, db, sourcePortNID, destPortNID, sessionId, connectionID, sessionDocument, mediaSourceElement, mediaDestElement, observerList, SessionServiceConst.ADD);
				}
			}
		}		
		
		
		// Start the Connections
		XMPPTransaction.startBlock("StartConnections");
		for(Object o: connectionList) {
			connectionElement = (Element)o;
			String state = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.STATE);
			if((state != null) && (!"".equals(state))) // Error
				continue;
			String connectionID =MRXMLUtils.getAttributeValue( connectionElement,MediaRoomServiceConst.ID);
			String streamNID = streamNIDTable.get(connectionID);
			if(streamNID == null)
				continue; // Error in stream
			try {
				
				String profileNID = ""; // TBD
				Element profileXML = MRXMLUtils.stringToElement("<profileXML/>"); // TBD
				ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, 
						request.getRequestName(), roomNID, streamNID,  profileNID,  profileXML, MediaRoomServiceConst.START);
				
				String authEntityId = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.AUTH_ENTITY_ID,"");
				String type = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.TYPE_ATTRIBUTE);
				
				// Send AuthRequired only for VIEW connections
				if(!authEntityId.isEmpty() && type.equals(SessionServiceConst.VIEW )) {
					SessionAuthUtils.sendAuthRequiredNotification(request,xmpp, log, db,resourceFactory, serviceAgentFactory,connectionElement,sessionId,roomNID);
				}
				
			}
			catch(Exception e) {
				sessionManager.handleSetupError(xmpp, request, log, db, roomNID, connectionID, e);
				continue;
			}
		}
		XMPPTransaction.endBlock("StartConnections");

	}
	
	private static void stopConnection(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element connectionElement, String sessionId) throws Exception {
		XMPPTransaction.startBlock("StopConnections");
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		
		Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		/*
		String state = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.STATE);
		if(SessionServiceConst.ERROR.equals(state))
			throw new MRException(SessionServiceExceptionCodes.CONNECTION_STOP_FAILED, "Connection state=" + state);
		*/
		String connectionID =MRXMLUtils.getAttributeValue( connectionElement,MediaRoomServiceConst.ID);
		Element streamElement = MRXMLUtils.getElementXpath(roomdoc, "//Stream[@id='" + connectionID + "']");
		if(streamElement != null) {
			String streamNID = streamElement.attributeValue(SessionServiceConst.NID);
			String profileNID = ""; // TBD
			Element profileXML = MRXMLUtils.stringToElement("<profileXML/>"); // TBD
			ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, 
					request.getRequestName(), roomNID, streamNID,  profileNID,  profileXML, MediaRoomServiceConst.DELETE);
			
			SessionAuthUtils.deleteAuthEntityIfExists(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionElement, roomNID, sessionId, connectionID);
		}
		XMPPTransaction.endBlock("StopConnections");
	}

	public static void deleteConnection(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String connId, String sessionId) throws Exception {
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		if(connection == null) {
			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Connection not found connId=" + connId);
		}
		SessionMediaRoomUtils.stopConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connection, sessionId);
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).deleteConnection(request, xmpp, log, db, roomNID, connId);
		
		//After deleting the connection gc the destination entity
		String entityId = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.DSTENTITYID,"");
		Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();	
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		SessionMediaRoomUtils.gcDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, entityId, roomNID, roomDoc);
		
		return;
	}
	

	public static void addConnection(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element connection, String sessionId) throws Exception {
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String connId = connection.attributeValue(SessionServiceConst.ID);
		Element tmpConnection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		if(tmpConnection != null) {
			throw new MRException(SessionServiceExceptionCodes.DUPLICATE_CONNECTION, "Connection id already existsing connId=" + connId);
		}
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).addConnection(roomNID, connection);
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  sessionId);
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		SessionMediaRoomUtils.startConnectionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionList, sessionId);
		return;
	}

	private static void deleteEntityInternal(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String entityId,
			String roomNID, String NID) throws Exception {	
		log.addInfo("SessionMediaRoomUtils:gcEntity NID=" + NID + " entityId=" + entityId, MRLog.OK, MRLog.NOTIFY);
		RoomDB.getRoomDB().getRoom(roomNID, request, log, db).deleteElement(request, log, db, NID);
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).deleteEntity(roomNID, entityId);
	}
	
	public static boolean isUsedInNonPVRConnection(Document sessionDoc, String id) throws Exception {
		List<?> list = MRXMLUtils.getListXpath(sessionDoc, 
				"//Connection[(@dstEntityId='" + id + "' or @srcEntityId='" + id +"') and (@type!='PVR')]" );
		return !list.isEmpty();
	}
	
	public static void gcPVRFile(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String pvrFileNID) throws Exception {
        if(pvrFileNID == null)
        	return;
        if("".equals(pvrFileNID))
        	return;
        log.addInfo("SessionMediaRoomUtils:gcSrcEntity remove pvrFileNID=" + pvrFileNID, MRLog.OK, MRLog.NOTIFY);
        MRRequest assetRequest = MRRequest.createRequest(request.getUserJID());
        MediaRoomServiceUtils.getAssetAdminResponse(assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory,
	                    "assetadminservice", "MediaStreamFileResource_BASE_ALL_DELETE_NID.xml", pvrFileNID, null, null, "Delete");
	}

	public static void gcSrcEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String entityId,
			String roomNID, Document roomDoc, String sessionId) throws Exception {
		if(SessionMediaRoomUtils.isUsedInNonPVRConnection(sessionDoc, entityId))
			return;
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + entityId + "']");
		if(entity == null)
			return;
		if(!MRConst.TRUE.equals(entity.attributeValue(SessionServiceConst.GC)))
			return;
		SessionMediaRoomUtils.deleteSrcEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, entityId, roomNID, roomDoc, sessionId);
	}
	
	public static void deleteSrcEntity(MRRequest request, XMPPI xmpp, MRLog log,
				MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory,
				Document sessionDoc, String entityId,
				String roomNID, Document roomDoc, String sessionId) throws Exception {
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + entityId + "']");
		if(entity == null)
			return;

		Element mediaSource = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@id='" + entityId + "']");
		String NID = mediaSource.attributeValue(MediaRoomServiceConst.NID);

		log.addInfo("SessionMediaRoomUtils:gcSrcEntity NID=" + NID + " entityId=" + entityId, MRLog.OK, MRLog.NOTIFY);
		// Check for PVR
		Element pvrElement = entity.element(SessionServiceConst.PVR);
		if( 	(pvrElement != null) &&
				("true".equals(pvrElement.attributeValue("enable"))) &&
				(!"".equals(pvrElement.attributeValue(SessionServiceConst.CONNECTIONID))) ) {
			String pvrConnId = pvrElement.attributeValue(SessionServiceConst.CONNECTIONID);
			Element pvrConnection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + pvrConnId + "']");
			String pvrFileEntityId = pvrConnection.attributeValue(SessionServiceConst.DSTENTITYID);
			Element pvrDestEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + pvrFileEntityId + "']");
			String pvrFileNID = pvrDestEntity.element(SessionServiceConst.FILE).attributeValue(SessionServiceConst.RESOURCENID);
            Element stream = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='" + pvrConnId + "']");

            // Delete the connection and stream
			SessionMediaRoomUtils.deleteConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, pvrConnId, sessionId);
			AppServerAgentUtils.getSessionManager(serviceAgentFactory).deleteEntity(roomNID, pvrFileEntityId);
	        
	        // Delete the file
			SessionMediaRoomUtils.gcPVRFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, pvrFileNID);
	        
            // Delete the mediaSource and mediaDest
	        // System.out.println("SessionMediaRoomUtils:gcSrcEntity roomdoc=" + MRXMLUtils.documentToString(roomDoc));
	        if(stream != null) {
	            String pvrMediaSourceNID = MRXMLUtils.getValueXpath(stream, ".//mediaSourceNID", "");
	            String pvrMediaDestNID = MRXMLUtils.getValueXpath(stream, ".//mediaDestNID", "");
		        log.addInfo("SessionMediaRoomUtils:gcSrcEntity remove mediaSource=" + pvrMediaSourceNID + " mediaDest=" + pvrMediaDestNID, MRLog.OK, MRLog.NOTIFY);
	            RoomDB.getRoomDB().getRoom(roomNID, request, log, db).deleteElement(request, log, db, pvrMediaSourceNID);
	    		RoomDB.getRoomDB().getRoom(roomNID, request, log, db).deleteElement(request, log, db, pvrMediaDestNID);
	        }
		}
		
		SessionMediaRoomUtils.deleteEntityInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, NID);
	}

	public static void gcDstEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String entityId,
			String roomNID, Document roomDoc) throws Exception {
		if(SessionResourceUtils.isUsedInConnection(sessionDoc, entityId))
			return;
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + entityId + "']");
		if(entity == null)
			return;
		if(!MRConst.TRUE.equals(entity.attributeValue(SessionServiceConst.GC)))
			return;
		SessionMediaRoomUtils.deleteDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, entityId, roomNID, roomDoc);
	}
		
	public static void deleteDstEntity(MRRequest request, XMPPI xmpp, MRLog log,
				MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory,
				Document sessionDoc, String entityId,
				String roomNID, Document roomDoc) throws Exception {
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + entityId + "']");
		if(entity == null)
			return;
		Element mediaDest = MRXMLUtils.getElementXpath(roomDoc, "//MediaDest[@id='" + entityId + "']");
		String NID = mediaDest.attributeValue(MediaRoomServiceConst.NID);
		SessionMediaRoomUtils.deleteEntityInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, NID);
	}

	public static Element switchConnection(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, 
			String connId, String newSrcEntityId, String newDstEntityId, 
			String sessionId, Element connection,String connectionType,String connName,
			String canvasId,String windowId,String profileId) throws Exception {
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		
		// Clean out the previous connection
		SessionMediaRoomUtils.stopConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connection, sessionId);
		
		
		String prevSrcEntityId = connection.attributeValue(SessionServiceConst.SRCENTITYID);
		String prevDstEntityId = connection.attributeValue(SessionServiceConst.DSTENTITYID);
		
		
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).updateConnection(roomNID, connId, newSrcEntityId, newDstEntityId,connectionType,connName,canvasId,windowId,profileId);
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		
		// Check for GC
		// String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
		
		// GC prevSrcEntityId
		SessionMediaRoomUtils.gcSrcEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, prevSrcEntityId, roomNID, roomDoc, sessionId);
		
		// GC prevDstEntityId
		SessionMediaRoomUtils.gcDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, prevDstEntityId, roomNID, roomDoc);

		
		
		// Create the new connection
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		SessionMediaRoomUtils.startConnectionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionList, sessionId);
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		String errorCode = connection.attributeValue(SessionServiceConst.ERROR_CODE);
		if((errorCode != null) && !"".equals(errorCode)) {
			throw new MRException(Integer.parseInt(errorCode), connection.attributeValue(SessionServiceConst.ERROR_DATA));
		}
		return connection;
	}

	public static void validateSrcEntityId(Document sessionDoc, String srcEntityId) throws Exception {
		Element srcEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + srcEntityId + "']");
		if(srcEntity == null) {
        	throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Src entity not found srcEntityId=" + srcEntityId);	
		}
		String type = srcEntity.attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);
		if(!SessionServiceConst.SRC_PORT.equals(type) && !SessionServiceConst.PLYFILE.equals(type)) {
        	throw new MRException(SessionServiceExceptionCodes.NOT_SRC_ENTITY, "Not a src entity entityId=" + srcEntityId);				
		}
	}

	public static void validateDstEntityId(Document sessionDoc, String dstEntityId) throws Exception {
		Element dstEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + dstEntityId + "']");
		if(dstEntity == null) {
        	throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Dst entity not found dstEntityId=" + dstEntityId);	
		}
		String type = dstEntity.attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);
		if(!SessionServiceConst.DST_PORT.equals(type) && !SessionServiceConst.RECFILE.equals(type)) {
        	throw new MRException(SessionServiceExceptionCodes.NOT_DEST_ENTITY, "Not a dst entity entityId=" + dstEntityId);				
		}
	}
	
	public static void validateProfileId(Document sessionDoc, String profileId) throws Exception {
		Element profileEL = MRXMLUtils.getElementXpath(sessionDoc, "//Profile[@id='" + profileId + "']");
		if(profileEL == null) {
        	throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Profile Id not found profileId=" + profileId);	
		}
	}
	
	/*
	 * @returns roomNID
	 */
	public static String createSession(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc) throws Exception {
		// Create the Room
		String requestName = request.getRequestName();
		String sessionId = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(),SessionServiceConst.ID);
		String roomTitle = "SessionTitle";
		String roomDescription = "SessionDescription";
		String roomType = "Session";
		Element groups = MRXMLUtils.getElementXpath(sessionDoc, "//Session/Info/Groups");
		String syncData = "";
		/*
		String syncData = 				if(entityID.equals(syncSourceEntityID)) {
	    	MediaRoomServiceUtils.updateTrackSyncData(roomNID, mediaSourceNID, request, db, log);
		}
		*/
		
		XMPPTransaction.startBlock("AddRoom");
	
		Document response = ConnectionUtils.addMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestName, sessionId, roomTitle, roomDescription, syncData, roomType, groups);
		String roomNID = MRXMLUtils.getValueXpath(response, "//roomNID","");
		XMPPTransaction.endBlock("AddRoom");
		Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		
		// Update the timelineId
		Element streamTrack = MRXMLUtils.getElementXpath(roomdoc, "//" + MediaRoomServiceConst.STREAMTRACK);
		String streamTrackNID = streamTrack.attributeValue(SessionServiceConst.NID);
		sessionDoc.getRootElement().addAttribute("timelineId", streamTrackNID);
		Element recordTrack = MRXMLUtils.getElementXpath(roomdoc, "//" + MediaRoomServiceConst.RECORDTRACK);
		String recordTrackNID = recordTrack.attributeValue(SessionServiceConst.NID);
		sessionDoc.getRootElement().addAttribute("recordTimelineId", recordTrackNID);
		sessionDoc.getRootElement().addAttribute("state", SessionServiceConst.STARTED);
		
		// Create the timelineList element if not present
		Element timelineList = MRXMLUtils.getElementXpath(sessionDoc, "//TimelineList");
		if(timelineList == null) {
			timelineList = MRXMLUtils.stringToElement("<TimelineList/>");
			sessionDoc.getRootElement().add(timelineList);
		}
	
		//add properties if properties element missing in start session request
		Element propertiesEl = MRXMLUtils.getElementXpath(sessionDoc, "//Properties");
		if(propertiesEl == null) {
			Document sessionTemplateDoc = Session.getSessionTemplate(log);
			propertiesEl = (Element) MRXMLUtils.getElementXpath(sessionTemplateDoc, "//Properties").detach();
			MRXMLUtils.addElement(sessionDoc.getRootElement(),propertiesEl);
		}
		
		// Update the owner
		String ownerUserJID = MRXMLUtils.getValueXpath(roomdoc, "//roomOwnerJID", "");
		MRXMLUtils.setAttributeValueXPath(sessionDoc, "//Properties", "ownerUserJID", ownerUserJID);
		String dateCreated = MRXMLUtils.getValueXpath(roomdoc, "//creationWallClock", "");
		MRXMLUtils.setAttributeValueXPath(sessionDoc, "//Properties", "dateCreated", dateCreated);
		
		try
		{
			// Add to the SessionList
			SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
			XMPPTransaction.startBlock("addSession");
			sessionManager.addSession(request, log, db, roomNID, sessionDoc, roomdoc);
			XMPPTransaction.endBlock("addSession");
		}
		catch (MRException e)
		{
			try {
				ConnectionUtils.deleteMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID);
			}
			catch(Exception e1) {
				e1.printStackTrace();
			}
			throw e;
		}		
		
		return roomNID;
	}


	public static void createSourcesAndDestinations(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String roomNID, List<?> entityList, boolean checkIsUsedInConnection) throws Exception {
		
		SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		//String sessionId = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(),SessionServiceConst.ID);

		// Set the sync entitiy
 		String syncEntityId = "";
		String sessionType = sessionDoc.getRootElement().attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);
		if(SessionServiceConst.PLAYBACK.equals(sessionType)) {
			List<?> playbackGroupFileElementList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.GRPFILE + "[@type='" + SessionServiceConst.SYNCPLY + "']");
			if(playbackGroupFileElementList.size() == 1) {
				Element playbackGroupFileElement = (Element)playbackGroupFileElementList.get(0);
				// System.out.println(MRXMLUtils.elementToString(playbackElement));
				syncEntityId = MRXMLUtils.getAttributeValue(playbackGroupFileElement,SessionServiceConst.SYNCENTITYID);
			}
 		}
		else if(SessionServiceConst.LIVE.equals(sessionType)) { // Set the syncEntityId to be first connection source
			List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.CONNECTION);
			if(!connectionList.isEmpty()) {
				Element connection = (Element)connectionList.get(0);
				syncEntityId = connection.attributeValue(SessionServiceConst.SRCENTITYID);
			}
		}

		// Create all the entities		
		for(Object o: entityList) {
			Element entityElement = (Element)o;
			String id = MRXMLUtils.getAttributeValue(entityElement,SessionServiceConst.ID);
			/*if(checkIsUsedInConnection && !SessionResourceUtils.isUsedInConnection(sessionDoc, id)){
				if(!SessionResourceUtils.isDestEntity(sessionDoc, id))
					continue;
			}*/
			
			String state = MRXMLUtils.getAttributeValue(entityElement,SessionServiceConst.STATE);
			if((state != null) && (!"".equals(state))) // Error
				continue;
			
			if(MRXMLUtils.getAttributeValue(entityElement, SessionServiceConst.TYPE_ATTRIBUTE,"").equals(SessionServiceConst.AUTH_CLIENT)) {
				continue;
			}
			try {
				String nid = null;
				if(SessionResourceUtils.isDestEntity(sessionDoc, id) ) {
					nid = SessionMediaRoomUtils.createMediaDest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, id, sessionDoc, roomNID);
				}
				else {
					nid = SessionMediaRoomUtils.createMediaSource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, id, sessionDoc, roomNID);
					if(id.equals(syncEntityId))
				    	MediaRoomServiceUtils.updateTrackSyncData(roomNID, nid, request, db, log);
				}
			}
			catch(Exception e) {
				sessionManager.handleSetupError(xmpp, request, log, db, roomNID, id, e);
				e.printStackTrace();
				continue;
			}
		}

	}


	public static void createSourcesAndDestinations(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String roomNID, List<?> entityList) throws Exception {
		SessionMediaRoomUtils.createSourcesAndDestinations(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, roomNID, entityList, true);
	}
	
	public static Document stopViewSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document doc, String sessionId) throws Exception {
        log.addInfo("SessionMediaRoomUtils:stopViewSession sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		Document stopSessionRequestDoc = MRXMLUtils.stringToDocument("<StopSession/>");
		stopSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.SESSION_ID, sessionId);
		MRRequest startSessionRequest = MRRequest.createRequest(request, stopSessionRequestDoc, 
				request.getRequestName(), request.getServiceName());
		//StopSessionRequestHandler handler = new StopSessionRequestHandler();
		Document responseDoc =  AppServerAgentUtils.getSessionManager(serviceAgentFactory).stopSession(startSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		request.addAuditEvent(SessionAuditEventUtils.getSessionAuditEventObject(responseDoc, AuditEventLogger.NAME_STOP, request, AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, log, ""));
		return responseDoc;
	}
}
