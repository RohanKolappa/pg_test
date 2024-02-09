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
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.systemservice.impl.FileTransferAgent;
 

public class DetachDirRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
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
    	String dirAgentJID = DirSM.getDirAgentJID(request, xmpp, log, db, dirNID);
    	Element dirAgentJIDElement = MRXMLUtils.stringToElement("<dirAgentJID>" + dirAgentJID + "</dirAgentJID>");
    	MRXMLUtils.getElementXpath(doc, "//MediaStreamDirNID").getParent().add(dirAgentJIDElement);
    	String state = DirSM.getDirState(request, xmpp, log, db, dirNID);
    	if(!MediaStoreServiceConst.READY.equals(state))
    		throw new MRException(AMExceptionCodes.DIR_NOT_READY, "Dir is not ready dirNID=" + dirNID + " state=" + state);
    	PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.MODIFY_ROLE_ACTION, AMConst.MEDIA_STREAM_DIR_RESOURCE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	String dirState = DirSM.READY; // Read from DB
    	DirSM dirSM = DirSM.getDirSM(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, 
    			dirNID, dirState, dirAgentJID, null, this.getClass().getName());
    	String transition = DirSM.DETACH_REQUEST_OK;
    	dirSM.setDirState(MediaStoreServiceConst.BUSY, DirSM.DETACH_REQUEST_OK);
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
		String userRequestXMLFilePath = "/com/ipvs/assetadminservice/xml/" + MediaRoomServiceUtils.DETACH_DIR_REQUEST_XML;
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
	    String dirState = DirSM.DETACH_REQUEST_OK;
	    String dirNID = null;
	    String dirAgentJID = null;
	    String remoteFilename = null;
		if(MediaRoomServiceUtils.DETACH_DIR_REQUEST.equals(requestName)) {
			dirNID = MRXMLUtils.getValueXpath(responseDoc, "//MediaStreamDirNID", "");
			dirAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//dirAgentJID", "");
			remoteFilename = MRXMLUtils.getValueXpath(responseDoc, "//exportDirPath", "");
		    dirState = DirSM.GETTING_SEND_PATH;
			if(state == MRRequest.STATE_DONE)
				transition = DirSM.GET_SEND_PATH_OK;
			else
				transition = DirSM.GET_SEND_PATH_ERROR;
		}
		if(FileTransferAgent.FILEXFERSETUPREQUEST.equals(requestName)) {
		    dirNID = MRXMLUtils.getValueXpath(responseDoc, "//FileXferID", "");
		    remoteFilename = MRXMLUtils.getValueXpath(responseDoc, "//toFileName", "");
			dirAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//toAgentJID", "");
		    dirState = DirSM.SENDING_FILE;
			if(state == MRRequest.STATE_DONE)
				transition = DirSM.SEND_FILE_OK;
			else
				transition = DirSM.SEND_FILE_ERROR;
		}
		DirSM dirSM = DirSM.getDirSM(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				responseHandler, dirNID, dirState, dirAgentJID, remoteFilename, this.getClass().getName());
		dirSM.handleTransition(transition);
	}



}

