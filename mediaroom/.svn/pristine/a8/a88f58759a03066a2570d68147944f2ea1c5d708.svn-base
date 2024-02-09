package com.ipvs.assetadminservice.handler;

import java.util.Hashtable;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.FileUtils;


public class GetMediaStreamFileMaxRevisionRequestHandler implements MRRequestHandlerI {
	public static final String MEDIA_STORE_REVISION_ELEMENT = "MediaStoreRevision";
	public static final String MEDIA_STREAM_FILE_MAX_REVISION_ELEMENT = "MediaStreamFileMaxRevision";
	public static final String MEDIA_STREAM_DIR_NID = "MediaStreamDirNID";

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
			throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);

		return auth;
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
			throws Exception {
		Document requestDoc = request.getMessage().getDocument();
		String mediaStoreRevision = MRXMLUtils.getValueXpath(requestDoc, "//" + MEDIA_STORE_REVISION_ELEMENT, "");
		String mediaStreamDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MEDIA_STREAM_DIR_NID, "");       
		String recyclebinDirNID = MediaStoreServiceUtils.getRecycleBinNID(mediaStreamDirNID, request, xmpp, log, db, resourceFactory, profileFactory);

		String mediaStreamFileMaxRevision = this.getMaxRevisionNumber(request, xmpp, log, db, resourceFactory, profileFactory, mediaStoreRevision, mediaStreamDirNID,recyclebinDirNID);
	
		return createResponse(request, mediaStoreRevision, mediaStreamFileMaxRevision, mediaStreamDirNID);
	}


	public String getMaxRevisionNumber(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory,
			String mediaStoreRevision,String mediaStreamDirNID,String recyclebinDirNID) throws Exception {

		if (mediaStoreRevision.isEmpty()) {
			throw new MRException(AMExceptionCodes.MEDIA_STORE_REVISION_MISSING, "Media Store Revision Missing");        }

		if (mediaStreamDirNID.isEmpty()) {
			throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "Media Stream Directory NID Missing");
		}

		
		String whereXpath = "[//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/"
				+MediaStoreServiceConst.PROPERTIES_EL_NAME +
				"/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + "=('" + mediaStreamDirNID + "','"+recyclebinDirNID+"')]";
		
		

		String mediaStreamFileMaxRevision = getMaxRevisionNumber(whereXpath, request.getUserJID(), log, db);

		if (mediaStreamFileMaxRevision == null || "".equalsIgnoreCase(mediaStreamFileMaxRevision.trim())) {
			mediaStreamFileMaxRevision = "0";
		}
		log.addInfo("Media Stream File Max Revision :=" + mediaStreamFileMaxRevision, MRLog.INFO, MRLog.NOTIFY);
		return mediaStreamFileMaxRevision;
	}


	public String getMaxRevisionNumber(String whereXpath, String userJID, MRLog log, MRDB db)
			throws MRException, Exception {
		String maxRevision = null;
		try {
			long dbtimeout = MRXMLDBUtils.TIME_OUT;
			MRRequest request = MRRequest.createRequest(userJID);
			Hashtable<String, String> table = new Hashtable<String, String>();
			String query = 	FileUtils.readFileResource(MediaStoreServiceConst.GET_MAX_REVISION_XQUERY, request.getClass());
			query = query.replace(MediaStoreServiceConst.XPATH, whereXpath);
			maxRevision = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return maxRevision;
	}


	private Document createResponse(MRRequest request, String mediaStoreRevision, String mediaStreamFileMaxRevision, String mediaStorePortResourceNID)
			throws MRException, Exception {
		Document responseDoc = null;
		String responseName = request.getResponseDataName();
		StringBuffer responseStr = new StringBuffer();
		responseStr.append("<" + responseName + ">");
		responseStr.append(MRXMLUtils.generateXML(MEDIA_STORE_REVISION_ELEMENT, mediaStoreRevision));
		responseStr.append(MRXMLUtils.generateXML(MEDIA_STREAM_FILE_MAX_REVISION_ELEMENT, mediaStreamFileMaxRevision));
		responseStr.append(MRXMLUtils.generateXML(MEDIA_STREAM_DIR_NID, mediaStorePortResourceNID));
		responseStr.append("</" + responseName + ">");
		responseDoc = MRXMLUtils.stringToDocument(responseStr.toString());

		return responseDoc;
	}
}
