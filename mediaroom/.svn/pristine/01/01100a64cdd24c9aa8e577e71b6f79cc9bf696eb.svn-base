package com.ipvs.mediaroomservice.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;

public final class ConnectionUtils {
	
	public static final String MEDIAROOM_XMLFILEPATH = "/com/ipvs/mediaroomservice/xml/";
	/*private static final Content getResponseDataElement(Document doc, String name) throws Exception {
		return doc.getRootElement().element(name).detach();
	}
	
	private static final Element getRequestDataElement(String requestName) throws Exception {
		return MRXMLUtils.stringToElement("<" + requestName + "Data/>");		
	}*/
	
	private static final void cleanConnection (String roomNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName) {
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "DeleteMediaRoomRequest.xml";
		try {
			
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//data");
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
			MRXMLUtils.setValueXpath(dataEL, "//roomGC", MRConst.TRUE);
			MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","DeleteMediaRoomRequest", MRXMLUtils.elementToDocument(dataEL));
		}
		catch(MRException me) {
			log.addInfo("CreateConnectionUtils:=" + request.getUserJID() + "/ Error encountered when deleting media room, Error Code = " + me.getCode() + " , Message = " + me.getData(), MRLog.ERROR, MRLog.NOTIFY);
		}
		catch (Exception e) {
			log.addInfo("CreateConnectionUtils:=" + request.getUserJID() + "/ Error encountered when deleting media room, Error String = " + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
			e.printStackTrace();
		}
		
	}

	public static final Document addMediaRoom(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String roomID, String roomTitle, String roomDescription, String syncData,
			String roomType, Element groups) throws Exception {
			String xmlFilePath = MEDIAROOM_XMLFILEPATH + "AddMediaRoomRequest.xml";
		try {
			Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			MRXMLUtils.setValueXpath(requestDoc, "//" + MediaRoomServiceConst.ID, roomID);
			//System.out.println("Setting room type : " + roomType );
			MRXMLUtils.setValueXpath(requestDoc, "//roomType", roomType);
			MRXMLUtils.setValueXpath(requestDoc, "//Title", roomTitle);
			MRXMLUtils.setValueXpath(requestDoc, "//Description", roomDescription);
			MRXMLUtils.setValueXpath(requestDoc, "//" + MediaRoomServiceConst.SYNCDATA, syncData);
			if(groups != null) {
				Element groupsParent = MRXMLUtils.getElementXpath(requestDoc, "//Groups").getParent();
				MRXMLUtils.removeChild(groupsParent,"Groups");
				MRXMLUtils.addElement(groupsParent,(Element)groups.clone());
			}
			 //System.out.println(MRXMLUtils.elementToString(dataEL));
			Element dataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					"mediaroomservice","AddMediaRoomRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
		
	}
	
	
	public static final Document addMediaStream(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String id, 
			String roomNID, String mediaSourceNID, String mediaDestNID, String description, 
			String profileNID, Element profileXML, String relayNID,String windowId,String canvasId) throws Exception {
		
		return addMediaStream(request,xmpp, log, db, resourceFactory, serviceAgentFactory,requestName, 
				id, roomNID, mediaSourceNID, mediaDestNID, description, profileNID, profileXML, relayNID,windowId,canvasId,"");

	}

	public static final Document addMediaStream(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String id, 
			String roomNID, String mediaSourceNID, String mediaDestNID, String description, 
			String profileNID, Element profileXML, String relayNID,String windowId,String canvasId,String xAuthAgentJID) throws Exception {
		
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "AddStreamRequest.xml";
		try {
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
			MRXMLUtils.setValueXpath(dataEL, "//" + MediaRoomServiceConst.ID, id);
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
			MRXMLUtils.setValueXpath(dataEL, "//mediaSourceNID", mediaSourceNID);
			MRXMLUtils.setValueXpath(dataEL, "//mediaDestNID", mediaDestNID);
			MRXMLUtils.setValueXpath(dataEL, "//description", description);
			MRXMLUtils.setValueXpath(dataEL, "//profileNID", profileNID);
			MRXMLUtils.setValueXpath(dataEL, "//relayNID", relayNID);
			MRXMLUtils.setValueXpath(dataEL, "//destNID", "");
			MRXMLUtils.setValueXpath(dataEL, "//destIP", "");
			MRXMLUtils.setValueXpath(dataEL, "//destUDPStartPort", "");
			MRXMLUtils.setValueXpath(dataEL, "//windowId", windowId);
			MRXMLUtils.setValueXpath(dataEL, "//canvasId", canvasId);
			MRXMLUtils.setValueXpath(dataEL, "//xAuthAgentJID", xAuthAgentJID);			
			MRXMLUtils.removeChild(dataEL.element("AddStreamRequestData"),"profileXML");
			Element profileXMLElement = (Element)profileXML.clone();
			MRXMLUtils.addElement(dataEL.element("AddStreamRequestData"), profileXMLElement);		
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","AddStreamRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}

	}
	public static final Document addStreamAction(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String roomNID, String streamNID, String profileNID, Element profileXML, String action) throws Exception {
		return addStreamAction(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, roomNID, streamNID, profileNID, profileXML, action, "");
	}
	
	public static final Document addStreamAction(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String roomNID, String streamNID, String profileNID, Element profileXML, String action,String actionData) throws Exception {
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "AddStreamActionRequest.xml";
		try {
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
			MRXMLUtils.setValueXpath(dataEL, "//streamNID", streamNID);
			MRXMLUtils.setValueXpath(dataEL, "//profileNID", profileNID);
			MRXMLUtils.setValueXpath(dataEL, "//updateAction", action);
			MRXMLUtils.setValueXpath(dataEL, "//actionData", actionData);
			MRXMLUtils.removeChild(dataEL.element("AddStreamActionRequestData"),"profileXML");
			Element profileXMLElement = (Element)profileXML.clone();
			MRXMLUtils.addElement(dataEL.element("AddStreamActionRequestData"), profileXMLElement);
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","AddStreamActionRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
	}
	
	public static final Document addMediaSource(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String requestName, 
			String roomNID, String sourceNID, String userRoomMediaSourceNID, String description, 
			String startOffset, String startTimeCode, String id) throws Exception {
		return ConnectionUtils.addMediaSourceWithLookbackInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, 
				roomNID, sourceNID, userRoomMediaSourceNID, description, startOffset, startTimeCode, id, "", "", "", "" ,"","");
	}
	
	public static final Document addMediaSourceWithLookbackInfo(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String requestName, 
				String roomNID, String sourceNID, String userRoomMediaSourceNID, String description, 
				String startOffset, String startTimeCode, String id,
				String lookbackSourceNID, String lookbackSourceAgentJID, 
				String lookbackSourceFileUUID, String lookbackSourceparentDirNID, String lookbackSourceFilePlayLength,String lookbackSourceFileStartTC) throws Exception {
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "AddMediaSourceRequest.xml";
		try {
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
			MRXMLUtils.setValueXpath(dataEL, "//" + MediaRoomServiceConst.ID, id);
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
			MRXMLUtils.setValueXpath(dataEL, "//description", description);
			MRXMLUtils.setValueXpath(dataEL, "//sourceNID", sourceNID);
			MRXMLUtils.setValueXpath(dataEL, "//startOffset", startOffset);
			MRXMLUtils.setValueXpath(dataEL, "//startTimeCode", startTimeCode);
			MRXMLUtils.setValueXpath(dataEL, "//userRoomMediaSourceNID", userRoomMediaSourceNID);
			
			Element sourceNIDElement = MRXMLUtils.getElementXpath(dataEL, "//sourceNID");
			sourceNIDElement.addAttribute("lookbackSourceNID", lookbackSourceNID);
			sourceNIDElement.addAttribute("lookbackSourceAgentJID", lookbackSourceAgentJID);
			sourceNIDElement.addAttribute("lookbackSourceFileUUID", lookbackSourceFileUUID);
			sourceNIDElement.addAttribute("lookbackSourceparentDirNID", lookbackSourceparentDirNID);
			sourceNIDElement.addAttribute("lookbackSourceFilePlayLength", lookbackSourceFilePlayLength);
			sourceNIDElement.addAttribute("lookbackSourceFileStartTC", lookbackSourceFileStartTC);
			
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","AddMediaSourceRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
	}
	
	public static final Document addMediaDest(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String roomNID, String destNID, String description, String id, String destIP, 
			String destUDPStartPort, String trimLength,Element fileInfoElement) throws Exception {
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "AddMediaDestRequest.xml";
		try {
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
			MRXMLUtils.setValueXpath(dataEL, "//" + MediaRoomServiceConst.ID, id);
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
			MRXMLUtils.setValueXpath(dataEL, "//description", description);
			MRXMLUtils.setValueXpath(dataEL, "//destNID", destNID);
			MRXMLUtils.setValueXpath(dataEL, "//destIP", destIP);
			MRXMLUtils.setValueXpath(dataEL, "//destUDPStartPort", destUDPStartPort);
			MRXMLUtils.setValueXpath(dataEL, "//trimLength", trimLength);
			
			Element originalFileInfoElement = MRXMLUtils.getElementXpath(dataEL, "//" + MediaRoomServiceConst.FILE_INFO_ELEMENT);
			Element fileInfoParent = originalFileInfoElement.getParent();
			MRXMLUtils.removeChild(fileInfoParent, MediaRoomServiceConst.FILE_INFO_ELEMENT);
			if(fileInfoElement != null) {
				MRXMLUtils.addElement(fileInfoParent,fileInfoElement);
			}
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "mediaroomservice","AddMediaDestRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
	}

	public static final Document deleteMediaRoom(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String roomNID) throws Exception {
		String xmlFilePath = MEDIAROOM_XMLFILEPATH + "DeleteMediaRoomRequest.xml";
		try {
			Document msgdoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
			Element dataEL = MRXMLUtils.getElementXpath(msgdoc, "//Data");
			MRXMLUtils.setValueXpath(dataEL, "//roomNID", roomNID);
	        MRXMLUtils.getElementXpath(dataEL, "//roomGC").setText(MRConst.TRUE);
			Document response = MediaRoomServiceUtils.getMediaRoomResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					"mediaroomservice","DeleteMediaRoomRequest", MRXMLUtils.elementToDocument(dataEL));
			return response;
		}catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
	}

	public static final Document CreateConnection(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String requestName, String roomType) throws Exception {

// com.ipvs.common.XMPPTransaction.setDEBUG(true);
com.ipvs.common.XMPPTransaction.startBlock("CreateConnection");
		log.addTraceBegin(request);
		Element data = MediaRoomServiceUtils.cloneDataElement(request);
		String id = MRXMLUtils.getValueXpath(data, MediaRoomServiceConst.ID, "");
		String title = MRXMLUtils.getValueXpath(data, "Title", "");
		String sourceNID = MRXMLUtils.getValueXpath(data, "sourceNID", "");
		String destNID = MRXMLUtils.getValueXpath(data, "destNID", "");
		String profileNID = MRXMLUtils.getValueXpath(data, "profileNID", "");
		String description = MRXMLUtils.getValueXpath(data, "Description", "");
		String relayNID = MRXMLUtils.getValueXpath(data, "relayNID", "");
		String startTimeCode = MRXMLUtils.getValueXpath(data, "startTimecode", "");
		String startOffset = MRXMLUtils.getValueXpath(data, "startOffset", "");
		String destIP = MRXMLUtils.getValueXpath(data, "destIP", "");
		String destUDPStartPort = MRXMLUtils.getValueXpath(data, "destUDPStartPort", "");
		String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(data, "userRoomMediaSourceNID", "");
		Element profileXML = MRXMLUtils.getElementXpath(data, "profileXML");
		Element groups = MRXMLUtils.getElementXpath(data, "Groups");
		String syncData = "";
		if(roomType== null || "".equalsIgnoreCase(roomType.trim())) {
			roomType = MediaRoomServiceConst.CONNECTION;
		}	
		Document response = ConnectionUtils.addMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, 
				id, title, description, syncData, roomType, groups);
		String roomNID = MRXMLUtils.getValueXpath(response, "//roomNID","");
		Element roomNIDElement = MRXMLUtils.stringToElement("<roomNID>" + roomNID + "</roomNID>");
		Element responseElement = null;
		try {
			response = ConnectionUtils.addMediaSource(request,xmpp, log, db,resourceFactory, serviceAgentFactory, requestName,  
					roomNID,  sourceNID,  userRoomMediaSourceNID,  description,  startOffset,  startTimeCode, "");
			String	mediaSourceNID = MRXMLUtils.getValueXpath(response,"//mediaSourceNID", "");
			response = ConnectionUtils.addMediaDest(request,xmpp, log, db,resourceFactory, serviceAgentFactory, roomNID,  
						destNID,  description, "", destIP, destUDPStartPort, "",null);
			String	mediaDestNID = MRXMLUtils.getValueXpath(response,"//mediaDestNID", "");
			id = "";
			responseElement = ConnectionUtils.createConnectionInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, 
					id, mediaSourceNID, mediaDestNID, roomNID, description, profileNID, profileXML, relayNID,"","");
		}catch(MRException me) {
			cleanConnection (roomNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName);
			throw new MRException(me.getCode(),me.getData());
		}
		log.addTraceEnd(request);

		Element streamElement = MRXMLUtils.getElementXpath(responseElement, "streamNID" );
		Element mediaSourceElement = MRXMLUtils.getElementXpath(responseElement, "mediaSourceNID" );
		Document doc = request.createResponseData(roomNIDElement);
		MRXMLUtils.addElement(doc.getRootElement(),(Element)streamElement.detach());
		MRXMLUtils.addElement(doc.getRootElement(),(Element)mediaSourceElement.detach());
com.ipvs.common.XMPPTransaction.endBlock("CreateConnection");
// com.ipvs.common.XMPPTransaction.setDEBUG(false);
		return doc;
	}
	
	public static Element createConnectionInternal(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String requestName, 
			String id, String mediaSourceNID, String mediaDestNID,  
			String roomNID, String description, String profileNID, Element profileXML, String relayNID,String windowId,String canvasId) throws Exception {
		Document response = null;
		response = ConnectionUtils.addMediaStream(request,xmpp, log, db, resourceFactory, serviceAgentFactory,requestName, 
				id, roomNID, mediaSourceNID, mediaDestNID, description, profileNID, profileXML, relayNID,windowId,canvasId);
		String streamNID = MRXMLUtils.getValueXpath(response,"//streamNID", "");
		Element responseElement = MRXMLUtils.stringToElement("<data></data>");
		Element streamNIDElement = MRXMLUtils.stringToElement("<streamNID>" + streamNID + "</streamNID>");
		Element mediaSourceNIDElement = MRXMLUtils.stringToElement("<mediaSourceNID>" + mediaSourceNID + "</mediaSourceNID>");
		MRXMLUtils.addElement(responseElement,(Element)mediaSourceNIDElement.detach());
		MRXMLUtils.addElement(responseElement,(Element)streamNIDElement.detach());
		return responseElement;
		
	}


}
