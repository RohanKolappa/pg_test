package com.ipvs.mediaroomservice.handler;

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
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;


public class EditConnectionRequestHandler implements MRRequestHandlerI {
	
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
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


    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
        Document requestDoc = request.getMessage().getDocument();      
        
		request.setCache(new MRRequestCache());

/*        
        String collaborationID = MRXMLUtils.getValueXpath(requestDoc, "//collaborationNID", "");
        String connectionID = MRXMLUtils.getValueXpath(requestDoc, "//connectionID", "");
        String sourceEntityID = MRXMLUtils.getValueXpath(requestDoc, "//sourceEntityID", "");

        Document roomDoc = MediaRoomServiceUtils.readElementByXpath(null, request, log, db, 
        		"MediaRoom[@id='" + collaborationID + "']");
        String roomNID = roomDoc.getRootElement().getAttributeValue("NID");        
        Element streamElement = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='" + connectionID + "']");
        String prevStreamNID = streamElement.getAttributeValue("NID");
        Element mediaSourceElement = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@id='" + sourceEntityID + "']");        
        String mediaSourceNID = mediaSourceElement.getAttributeValue("NID");
*/
        String prevStreamNID = MRXMLUtils.getValueXpath(requestDoc, "//prevStreamNID", "");
        String mediaSourceNID = MRXMLUtils.getValueXpath(requestDoc, "//mediaSourceNID", "");
        String profileNID = MRXMLUtils.getValueXpath(requestDoc, "//profileNID", "");
        String relayNID = MRXMLUtils.getValueXpath(requestDoc, "//relayNID", "");
        Element profileXML = MRXMLUtils.getElementXpath(requestDoc, "//profileXML");
        
        String roomNID = MRXMLUtils.getValueXpath(requestDoc, ".//roomNID", "");
  	  	if("".equals(roomNID))
  	  		roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, prevStreamNID);
  	  com.ipvs.common.XMPPTransaction.startBlock("GetStreamDataDoc");
        String streamDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, prevStreamNID);
  	  	com.ipvs.common.XMPPTransaction.endBlock("GetStreamDataDoc");
        if (streamDataDocStr == null) {
            throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND,MRXMLUtils.generateErrorXML("streamNID", prevStreamNID));
        }
    	Document streamDataDoc = MRXMLUtils.stringToDocument(streamDataDocStr);
    	//Document roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
    	// System.out.println("Before Switch: " + MRXMLUtils.documentToString(roomDoc));
    	
        String description = MRXMLUtils.getValueXpath(streamDataDoc, "//description", "");
        if("".equals(profileNID))
        	profileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//profileNID", "");
        if("".equals(relayNID))
        	relayNID = MRXMLUtils.getValueXpath(streamDataDoc, "//relayNID", "");
        String mediaDestNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaDestNID", "");
        Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, prevStreamNID);
        String id = MRXMLUtils.getAttributeValue(streamDoc.getRootElement(),MediaRoomServiceConst.ID);
        if(profileXML.elements().isEmpty())
        	profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");

    	String requestName = request.getRequestName();
		Document response = ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, requestName, 
				roomNID, prevStreamNID,  profileNID,  profileXML, MediaRoomServiceConst.DELETE);
		response = ConnectionUtils.addMediaStream(request,xmpp, log, db, resourceFactory, serviceAgentFactory,requestName, 
				id, roomNID, mediaSourceNID, mediaDestNID, description, profileNID, profileXML, relayNID,"","");
		String streamNID = MRXMLUtils.getValueXpath(response,"//streamNID", "");
		response = ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, requestName, 
				roomNID, streamNID,  profileNID,  profileXML, MediaRoomServiceConst.START);
    	
    	//Document roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
    	// System.out.println("After switch: " + MRXMLUtils.documentToString(roomDoc));

    	return request.createResponseData("streamNID", streamNID);
    }
}
