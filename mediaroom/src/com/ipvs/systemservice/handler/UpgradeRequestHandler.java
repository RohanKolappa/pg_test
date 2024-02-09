package com.ipvs.systemservice.handler;

import java.io.File;
import java.net.URLDecoder;
import java.util.Hashtable;

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
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;

public class UpgradeRequestHandler implements MRRequestHandlerI,AgentResponseHandlerI,EventLoggerI {
	
	

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

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	throws Exception {
		Document requestDoc = request.getMessage().getDocument();
		
		log.addInfo("requestDoc::"+requestDoc.asXML());
		String fromDirPath = MRXMLUtils.getValueXpath(requestDoc, "//FromDirPath", "");
		if(fromDirPath.isEmpty()) {
			throw new MRException(SystemServiceConst.FROM_DIR_PATH_REQUIRED, "Path Required");			
		}		
		Element upgradeRequestEle = MRXMLUtils.getElementXpath(requestDoc, "//Upgrade");
		String deviceType= MRXMLUtils.getAttributeValue(upgradeRequestEle, "deviceType", "");
		PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.UPGRADE_ROLE_ACTION,PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
	
		//when upgrading MS, delete all binaries if already present to avoid issues during upgrade and down grade
		if(!SystemServiceConst.NSGD200_DEVICE.equals(deviceType)){
			FileUtils.removeFileOrDir(SystemServiceConst.UPGRADE_DIR);
		}
		
		String downloadArea = SystemServiceConst.UPGRADE_DIR + "/" + SystemServiceConst.DOWNLOAD;
		String toFileName = downloadArea + "/" +SystemServiceConst.IMAGE_TAR_FILE_NAME;
		
		// delete temporary area and create it again
		File fileObject = null;
		try {
			fileObject = new File(downloadArea);
			if (!fileObject.exists()) {
				fileObject.mkdirs();
			}
			//remove the tar file if exits
			FileUtils.removeFileOrDir(toFileName);
			
		} catch (Exception e) {			
		}	 
	
		String url = getUrl(request, xmpp, log, db, resourceFactory,
				serviceAgentFactory);
		Document fileUploadRequestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERHTTPUPLOADREQUESTFILE, log);
	    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//filePath", fromDirPath);     
	    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//url", url);
	    MRXMLUtils.setValueXpath(fileUploadRequestDoc, "//type", "upgrade");
		String requestName = MRXMLUtils.getAttributeValueXpath(fileUploadRequestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
	    MRRequest ftpRequest = MRRequest.createRequest(request, fileUploadRequestDoc, requestName, request.getServiceName());
		Element data = MRXMLUtils.getElementXpath(fileUploadRequestDoc, ftpRequest.getRequestDataXPath());
	    String dataStr = MRXMLUtils.elementToString(data);
		String deprecated = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(fileUploadRequestDoc, "Header"), MRConst.DEPRECATED, "false");  
		AppServerAgent.sendDeprecatedAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MediaRoomServiceConst.APPSERVERAGENT,
				request.getUserJID(), requestName, MRConst.FTP_SERVICE, dataStr, this.getClass().getName(), deprecated);
		return null;
	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler,	MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
	    Document responseDoc = request.getResult();
    	Document userRequestXMLDoc = MRXMLUtils.loadXMLResource(DeviceConst.UPGARDE_REQUEST_XML_PATH, log);
	    String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
	    Document marshalledUserRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
	    MRXMLUtils.setValueXpath(userRequestXMLDoc, "//"+MRConst.CLIENT_DATA, 
	      			MRXMLUtils.getValueXpath(marshalledUserRequestDoc, "//"+MRConst.CLIENT_DATA, ""));
	    MRXMLUtils.setValueXpath(userRequestXMLDoc, "//"+MRConst.CLIENT_CALLBACK, 
      			MRXMLUtils.getValueXpath(marshalledUserRequestDoc, "//"+MRConst.CLIENT_CALLBACK, ""));
	    MRRequest userRequest = MRRequest.unMarshalRequestObject(marshalledUserRequestDoc, 
	    		   userRequestXMLDoc, "", log);
	    Element upgradeRequestEle = MRXMLUtils.getElementXpath(marshalledUserRequestDoc, "//Upgrade");
	    String deviceType= MRXMLUtils.getAttributeValue(upgradeRequestEle, "deviceType", "");
	    String msServiceVersion= MRXMLUtils.getServiceVersion(log);
	    int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, "0"));
	    
	    userRequest.setState(state);
	    Document userResponseDoc = userRequest.createResponseData(MRXMLUtils.stringToElement("<" + SystemServiceConst.UPGRADE_EL_NAME + "/>"));
	    userRequest.setResult(userResponseDoc);  
	    if(state == MRRequest.STATE_EXCEPTION) {	    		  
	    	userRequest.addAuditEvent(getAuditEvent(userRequest, xmpp, log, db, AuditEventLogger.STATUS_FAILURE, AuditEventLogger.LEVEL_ERROR, "Error - Upgrade Failed"));
	    	responseHandler.handleResponse(userRequest, "");	
		    return;
	    }
	    
	   
	    String tarfilePath =  SystemServiceConst.UPGRADE_DIR + "/" + SystemServiceConst.DOWNLOAD +"/" +SystemServiceConst.IMAGE_TAR_FILE_NAME;
	    String upgradeImageArea =  SystemServiceConst.UPGRADE_DIR ;
	    String softwareURL = SystemServiceConst.SOFTWARE_URL;
	    if(SystemServiceConst.NSGD200_DEVICE.equals(deviceType)){
	    	upgradeImageArea+= "/"+SystemServiceConst.NGSD200;
	    	softwareURL+= "/"+SystemServiceConst.NGS_SOFTWARE_URL;
		}else
		{
			//upgradeImageArea+= "/"+SystemServiceConst.DMS;
			softwareURL+= "/"+SystemServiceConst.DMS_SOFTWARE_URL;
		}		
		
	    File fileObject = null;
		try {
			fileObject = new File(upgradeImageArea);
			if (!fileObject.exists()) {
				fileObject.mkdirs();
			}
						
		} catch (Exception e) {			
		}	
		softwareURL = softwareURL.replace(SystemServiceConst._MS_IP_ADDRESS, SystemServiceConst.LOCAL_HOST);
	
		log.addInfo("Dowloaded Image to :"+tarfilePath);
		
		String[] systemCmd = {"sh" , "/etc/scripts/mediaroom_software_upgrade.sh", tarfilePath,upgradeImageArea,softwareURL,deviceType,msServiceVersion};
		int result = MediaRoomServiceUtils.executeSystemCommand(systemCmd);
		log.addInfo("result::"+result);
		if(result != 0) {
			userRequest.setState(MRRequest.STATE_EXCEPTION);
			MRException exp =  new MRException(SystemServiceConst.UPGRADE_DEVICE_FAILED, Error.getErrorDescription(result));
			userRequest.setException(exp);
			userRequest.addAuditEvent(getAuditEvent(userRequest, xmpp, log, db, AuditEventLogger.STATUS_FAILURE, AuditEventLogger.LEVEL_ERROR, "Error - Upgrade Failed"));
			responseHandler.handleResponse(userRequest, "");	
		    return;
		}
		userRequest.addAuditEvent(getAuditEvent(userRequest, xmpp, log, db, AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, ""));
		
		// Add NGSD200 device software image version if available.
		String deviceSoftwareVersion = SystemServiceUtils.getDeviceSoftwareVersionOnMS(deviceType, log);
		Element deviceSWVersionEl = MRXMLUtils.addElement(MRXMLUtils.getElementXpath(userResponseDoc, "//"+ SystemServiceConst.UPGRADE_EL_NAME),SystemServiceConst.DEVICE_SOFTWARE_VERSION,	deviceSoftwareVersion);
		MRXMLUtils.addAttribute(deviceSWVersionEl, "deviceType", deviceType);
		
		userRequest.setResult(userResponseDoc);
		responseHandler.handleResponse(userRequest, "");
	}

	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String status, String level, String errorDescription) throws Exception {
		
		return new AuditEvent.Builder(AuditEventLogger.SYSTEM_EVENT, AuditEventLogger.NAME_UPGRADE, request.getUserJID(), level)
		.target(AuditEventLogger.NAME_UPGRADE).descriptionText(""+ errorDescription).status(status).build();
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
				.append("/mediajs/upgrade/upload?timestamp=")
				.append(timestamp).append("&apiKey=")
				.append(apiKey).append("&signature=").append(signature)
				.append("&type=upgrade");

		return sb.toString();
	}

	private enum Error {
	
	    ERROR_01(1, "Missing the file or directory name for upgrade."),
	    ERROR_02(2, "File not found for upgrade."),
	    ERROR_03(3, "Invalid tar. Please use a valid software image tar."),
	    ERROR_04(4, "Failed to extract the software image tar."),
	    ERROR_05(5, "Invalid Device-Software Image. Please use a valid tar."),
	    ERROR_06(6, "Invalid URL. Cannot upgrade."),
	    ERROR_07(7, "Upgrade failed for the URL.");

	    private final int retcode;
	    private final String description;
	    private static final Hashtable<Integer, String> errorMap =
	        new Hashtable<Integer, String>();

	    static {
	        for (Error error : Error.values()) {
	            errorMap.put(error.retcode, error.description);
	        }
	    }

	    Error(final int code, final String description) {
	        this.retcode = code;
	        this.description = description;
	    }

	    public static String getErrorDescription(int code) {
	    	String description = "Upgrade Error!";
	    	if(errorMap != null && errorMap.containsKey(code))
	    		description = errorMap.get(code);
	    	
	        return description;
	    }
	}
}
