package com.ipvs.assetadminservice.handler;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.DirSM;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.systemservice.impl.FileTransferAgent;
 

public class AttachDirRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        log.addTraceBegin(request);
        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);
        return auth;

    }
    

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

    	Document doc = request.getMessage().getDocument();
    	String dirNID = MRXMLUtils.getValueXpath(doc, "//MediaStreamDirNID", "");
    	
        Document mediaDirDoc = null;
        try {
        	mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
        }
        catch(Exception e) {
    		throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "<dirNID>" + dirNID + "</dirNID>");
        }
        String state = MRXMLUtils.getValueXpath(mediaDirDoc, "//State/StateFlag", "");
    	if(!MediaStoreServiceConst.DETACHED.equals(state))
    		throw new MRException(AMExceptionCodes.DIR_NOT_DETACHED, "Dir is not detached dirNID=" + dirNID + " state=" + state);
    	
    	PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.MODIFY_ROLE_ACTION, AMConst.MEDIA_STREAM_DIR_RESOURCE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	String dirAgentJID = DirSM.getDirAgentJID(request, xmpp, log, db, dirNID);
    	Element dirAgentJIDElement = MRXMLUtils.stringToElement("<dirAgentJID>" + dirAgentJID + "</dirAgentJID>");
    	MRXMLUtils.getElementXpath(doc, "//MediaStreamDirNID").getParent().add(dirAgentJIDElement);
    	String dirState = DirSM.DETACHED;
    	DirSM dirSM = DirSM.getDirSM(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, 
    			dirNID, dirState, dirAgentJID, null, this.getClass().getName());
    	String transition = DirSM.ATTACH_REQUEST_OK;
    	dirSM.setDirState(MediaStoreServiceConst.BUSY, DirSM.ATTACH_REQUEST_OK);
		dirSM.handleTransition(transition);
    	return null;
    }
    
	public void handleAgentResponse(MRResponseHandlerI responseHandler,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {		
		Document responseDoc = request.getResult();
		// System.out.println(MRXMLUtils.documentToString(responseDoc));
		String requestName = MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
		
		// Recreate the userRequest
		String userRequestXMLFilePath = "/com/ipvs/assetadminservice/xml/" + MediaRoomServiceUtils.ATTACH_DIR_REQUEST_XML;
		// System.out.println("AppServerAgent: agentResponse=" + MRXMLUtils.documentToString(responseDoc));	   
	    Document userRequestXMLDoc = MRXMLUtils.loadXMLResource(userRequestXMLFilePath, log);
	    String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
	    Document marshalledUserRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
	    MRXMLUtils.setValueXpath(userRequestXMLDoc, "//"+MRConst.CLIENT_DATA, 
	      			MRXMLUtils.getValueXpath(marshalledUserRequestDoc, "//"+MRConst.CLIENT_DATA, ""));
	    MRRequest userRequest = MRRequest.unMarshalRequestObject(marshalledUserRequestDoc, 
	    		   userRequestXMLDoc, "", log);
		
	    int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, "0"));
	    String transition = null;
	    String dirState = DirSM.ATTACH_REQUEST_OK;
	    String dirNID = null;
	    String dirAgentJID = null;
	    String remoteFilename = null;
		if(MediaRoomServiceUtils.ATTACH_DIR_REQUEST.equals(requestName)) {
			dirNID = MRXMLUtils.getValueXpath(responseDoc, "//MediaStreamDirNID", "");
			dirAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//dirAgentJID", "");
			remoteFilename = MRXMLUtils.getValueXpath(responseDoc, "//exportDirPath", "");
		    dirState = DirSM.GETTING_RECEIVE_PATH;
			if(state == MRRequest.STATE_DONE)
				transition = DirSM.GET_RECEIVE_PATH_OK;
			else
				transition = DirSM.GET_RECEIVE_PATH_ERROR;
		}
		if(FileTransferAgent.FILEXFERSETUPREQUEST.equals(requestName)) {
		    dirNID = MRXMLUtils.getValueXpath(responseDoc, "//FileXferID", "");
		    remoteFilename = MRXMLUtils.getValueXpath(responseDoc, "//toFileName", "");
			dirAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//fromAgentJID", "");
		    dirState = DirSM.RECEIVING_FILE;
			if(state == MRRequest.STATE_DONE)
				transition = DirSM.RECEIVE_FILE_OK;
			else
				transition = DirSM.RECEIVE_FILE_ERROR;
		}
		DirSM dirSM = DirSM.getDirSM(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				responseHandler, dirNID, dirState, dirAgentJID, remoteFilename, this.getClass().getName());
		dirSM.handleTransition(transition);
	}



}

