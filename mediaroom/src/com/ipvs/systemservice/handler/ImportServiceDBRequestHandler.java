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
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.Utils;

public class ImportServiceDBRequestHandler extends AsyncBaseRequestHandler implements MRRequestHandlerI {

  public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
      MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
      ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    log.addTraceBegin(request);
    int authType = 1;
    int role = 2;
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

  public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
      MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
      throws Exception {
	Document requestDoc = request.getMessage().getDocument();
	PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.RESTORE_ROLE_ACTION, PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	String fromFileName = MRXMLUtils.getValueXpath(requestDoc, "//LocalFileName", ""); 
	
	if(fromFileName.isEmpty()) {
		throw new MRException(SystemServiceConst.FROM_DIR_PATH_REQUIRED, "File Path Required");			
	}	
    String url = getUrl(request, xmpp, log, db, resourceFactory,
			serviceAgentFactory);
	Document fileUploadRequestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERHTTPUPLOADREQUESTFILE, log);
    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//filePath", fromFileName);     
    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//url", url);
    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//type", "dbrestore");
	String requestName = MRXMLUtils.getAttributeValueXpath(fileUploadRequestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
    MRRequest ftpRequest = MRRequest.createRequest(request, fileUploadRequestDoc, requestName, request.getServiceName());
	Element data = MRXMLUtils.getElementXpath(fileUploadRequestDoc, ftpRequest.getRequestDataXPath());
    String dataStr = MRXMLUtils.elementToString(data);
	String deprecated = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(fileUploadRequestDoc, "Header"), MRConst.DEPRECATED, "false");  
	AppServerAgent.sendDeprecatedAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MediaRoomServiceConst.APPSERVERAGENT,
			request.getUserJID(), requestName, MRConst.FTP_SERVICE, dataStr, this.getClass().getName(), deprecated);  
	return null;
  }
  
	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
		Document agentResponseDoc = request.getResult();
		Document restoreDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.RESTORE_DB_REQUEST_XML, log);
		MRXMLUtils.setValueXpath(restoreDoc, "//RestoreFrom", "LastImported");     
		String requestName = MRXMLUtils.getAttributeValueXpath(restoreDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		String clientData = MRXMLUtils.getValueXpath(agentResponseDoc, "//"+MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
		//create user request
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);	
		
		int state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH,MRConst.STATE, "0"),0);
		MRException exception = null;
		String logData = "" , description ="";
		int exceptionCode = 0;
		if(state != MRRequest.STATE_EXCEPTION){
			try{
				MRRequest restoreRequest = MRRequest.createRequest(userRequest, restoreDoc, requestName, request.getServiceName());
				RestoreDBRequestHandler restoreDbHandler = new RestoreDBRequestHandler();
				restoreDbHandler.getResponse(restoreRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
			}catch(MRException dbrestoreException){
				state =0;
				log.addInfo(" Db Exception Log : "+dbrestoreException.getMessage()+ " : "+dbrestoreException.getCode()+"  "+ dbrestoreException.getData());
				exception = dbrestoreException;
				exceptionCode = dbrestoreException.getCode();
				description = dbrestoreException.getData();
			}
			
		}else{
			exceptionCode = SystemServiceConst.FILETRANSFER_GET_FILE_ERROR;
			description = "Server error while uploading";
		}
		

        Document userRequestDoc = userRequest.getMessage().getDocument();
        String fromFileName = MRXMLUtils.getValueXpath(userRequestDoc, "//LocalFileName", "");
       	Document importResponseDoc = MRXMLUtils.loadXML(SystemServiceConst.IMPORTSERVICEDBRESPONSEFILE, log);
	    MRXMLUtils.setValueXpath(importResponseDoc, "//toAgentJID", request.getAppServerJID());
	    MRXMLUtils.setValueXpath(importResponseDoc, "//fromAgentJID", request.getUserJID());
	    MRXMLUtils.setValueXpath(importResponseDoc, "//fromFilename", fromFileName);
	 
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			exception =new MRException(exceptionCode,description );
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exceptionCode), "Description", description);
		}
		userRequest.setResult(importResponseDoc);
		userRequest.setState(state);
		userRequest.setException(exception);
		userRequest.addAuditEvent( this.getAuditEvent(userRequest, xmpp, log, db, AuditEventUtils.getAuditEventStatus(userRequest), AuditEventUtils.getAuditEventLevel(userRequest), AuditEventUtils.getAuditEventErrorDescription(userRequest)));
		responseHandler.handleResponse(userRequest, logData);
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

		String msIPAddress = MRXMLUtils.getAttributeValue(appResourceInfo,
				"ipAddress", "");

		StringBuilder sb = new StringBuilder();
		sb.append("https://").append(msIPAddress)
				.append("/mediajs/dbrestore/upload?timestamp=")
				.append(timestamp).append("&apiKey=")
				.append(apiKey).append("&signature=").append(signature)
				.append("&type=dbrestore");

		return sb.toString();
	}
}
