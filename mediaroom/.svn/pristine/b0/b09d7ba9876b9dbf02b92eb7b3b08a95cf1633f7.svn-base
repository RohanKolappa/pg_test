package com.ipvs.systemservice.handler;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
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
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;

public class DownloadDBRequestHandler implements MRRequestHandlerI,AgentResponseHandlerI {

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
        //Check HA configuration file is exists or not. If not then ha service is not running just return.    	
    	if(!HAUtils.isHAServiceEnabled()){
    		throw new MRException(SystemServiceConst.HA_SERVICE_NOT_RUNNING, "HA Serivce is not active to start download");	
    	}
    	
    	// Create file for replication
    	HAUtils.handleDownloadDbRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
    	
    	// Generate download DB url
    	String url = getUrl(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	log.addInfo("DownloadDBRequestHandler: download db url: " + url);
    	Document responseDoc = createResponse(request, url);
		return responseDoc;
    }
    
    public Document createResponse(MRRequest request, String url) throws Exception{
    	Element responseEl = MRXMLUtils.newElement("DownloadDB");
    	MRXMLUtils.addElement(responseEl, "URL", url);
    	return request.createResponseData(responseEl);
    }
    
	private String getUrl(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Element ApiKeyTableEl = AppServerAgentUtils.getAPIKeyTable();

		long now = xmpp.getCurrentTimeMillis();
		String timestamp = String.valueOf(now
				+ (SessionServiceResourceConst.REDIRECT_EXPIRE_TIME * 1000));
		String signature = HttpAuthentication.generateExpiryBasedSignature(ApiKeyTableEl,
				timestamp);
		String apiKey = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./"
				+ SessionServiceResourceConst.API_KEY_ELEMENT_NAME,
				SessionServiceResourceConst.KEY_ATTRIBUTE, "");

		String agentName = ServiceAgentUtils.getAgentName(request.getUserJID());
		Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log,
				xmpp, db, serviceAgentFactory, request, ApiKeyTableEl,
				agentName, request.getUserJID());

		String msIPAddress = MRXMLUtils.getAttributeValue(appResourceInfo, "ipAddress", "");

		StringBuilder sb = new StringBuilder();
		sb.append("https://").append(msIPAddress)
				.append("/mediajs/db/download?timestamp=").append(timestamp)
				.append("&apiKey=").append(apiKey).append("&signature=")
				.append(signature).append("&type=dbdownload");

		log.addInfo("DownloadDBReqhestHandler: url: " + sb.toString());
		return sb.toString();
	}
	
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
    		MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	Document responseDoc = request.getResult();
    	Document userRequestXMLDoc = MRXMLUtils.loadXMLResource(DeviceConst.DOWNLOAD_DB_REQUEST_XML_PATH, log);
	    String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
	    Document marshalledUserRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
	    MRXMLUtils.setValueXpath(userRequestXMLDoc, "//"+MRConst.CLIENT_DATA, 
	      			MRXMLUtils.getValueXpath(marshalledUserRequestDoc, "//"+MRConst.CLIENT_DATA, ""));
	    MRXMLUtils.setValueXpath(userRequestXMLDoc, "//"+MRConst.CLIENT_CALLBACK, 
      			MRXMLUtils.getValueXpath(marshalledUserRequestDoc, "//"+MRConst.CLIENT_CALLBACK, ""));
	    MRRequest userRequest = MRRequest.unMarshalRequestObject(marshalledUserRequestDoc, 
	    		   userRequestXMLDoc, "", log);
	    int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, "0"));
	    userRequest.setState(state);
	    Document userResponseDoc = userRequest.createResponseData();
	    userRequest.setResult(userResponseDoc);	  
	    responseHandler.handleResponse(userRequest, "");
	}
}

