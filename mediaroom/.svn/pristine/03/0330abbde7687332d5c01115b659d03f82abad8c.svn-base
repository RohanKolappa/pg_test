package com.ipvs.sessionservice.service.handler;

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
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.handler.SystemServiceUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;

public class BackupDatabaseRequestHandler extends AsyncBaseRequestHandler
		implements MRRequestHandlerI, EventLoggerI {

	@Override
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

		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL,
				MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID,
				token, authority);
		log.addTraceEnd(request);
		return auth;

	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document requestDoc = request.getMessage().getDocument();

		PolicyAdminServiceUtils.checkPolicyPermissions(
				PolicyAdminConstants.BACKUP_ROLE_ACTION,
				PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db,
				resourceFactory, serviceAgentFactory);

		StringBuffer buf = new StringBuffer();
		String[] cmdArgs = { IPVSConst.getInstance().getIpvsxmldbactl(),
				"export", "snapshot" };

		int exitVal = SystemServiceUtils.executeProcess(cmdArgs, buf);
		// check response
		if (exitVal != 0) {
			// script should not return non zero value.
			throw new MRException(SystemServiceConst.SAVE_DB_ERROR,
					MRXMLUtils.generateErrorXML("scriptError", "exitVal="
							+ exitVal + " '" + buf.toString() + "'."));
		}
		if (buf.toString().toLowerCase().indexOf("error") != -1) {
			// there was some error in script.
			throw new MRException(SystemServiceConst.SAVE_DB_ERROR,
					MRXMLUtils.generateErrorXML("scriptError",
							"'" + buf.toString() + "'."));
		}

		String toFileName = MRXMLUtils.getValueXpath(requestDoc,
				"//LocalFileName", "default");

		String url = getUrl(request, xmpp, log, db, resourceFactory,
				serviceAgentFactory);
		String requestName = "StartDownloadingRequest";
		Document downloadDocument = SystemServiceUtils
				.getDownloadRequestDocument(requestName, log, toFileName,
						request.getUserJID(), url, "dbBackup");
		sendRequestToAgent(request, xmpp, log, db, resourceFactory,
				serviceAgentFactory, requestName, downloadDocument,
				MRConst.FTP_SERVICE, request.getUserJID(), MRConst.FTP_SERVICE,
				this.getClass().getName());
		return null;
	}

	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String status, String level, String errorDescription)
			throws Exception {
		return new AuditEvent.Builder(AuditEventLogger.SYSTEM_EVENT,
				AuditEventLogger.NAME_BACKUP_DATABASE, request.getUserJID(),
				level).target("PC").descriptionText("" + errorDescription)
				.status(status).build();
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
				.append("/mediajs/db/download?timestamp=").append(timestamp)
				.append("&apiKey=").append(apiKey).append("&signature=")
				.append(signature).append("&type=dbbackup");

		return sb.toString();
	}

	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc, MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {		
		
		String toFileName = MRXMLUtils.getValueXpath(userRequestdoc,
				"//LocalFileName", "default");

		Document doc = MRXMLUtils.loadXML(
				SessionServiceConst.BACKUPDATABASERESPONSEFILE, log);
		MRXMLUtils.setValueXpath(doc, "//ToFileName", toFileName);
	
		Element el = MRXMLUtils.getElementXpath(doc,
				"//BackupDatabaseResponseData");
		el.detach();
				
		return MRXMLUtils.elementToDocument(el);
	}
}
