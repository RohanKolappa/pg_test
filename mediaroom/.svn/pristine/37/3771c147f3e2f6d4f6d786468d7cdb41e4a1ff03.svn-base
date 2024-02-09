package com.ipvs.assetadminservice.file.handler;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
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


public class PublishFileRequestHandler extends BasePublishFileRequestHandler implements MRRequestHandlerI {

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactry) throws Exception {
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


	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {


		Document requestDocument = request.getMessage().getDocument();
		List<?> listOfFiles  =  MRXMLUtils.getListXpath(requestDocument, "//File");
		//If not called from startsync, wee need to lock on file UUID before reading. So skipping the db read here and will do it in the addorupdatefile method
		List<?> listOfDirSyncFiles = MRXMLUtils.getListXpath(requestDocument, "//File[@dirSync='true']");
		//read all the list from database
		Document resourceDoc = null;
		if(listOfDirSyncFiles!= null && !listOfDirSyncFiles.isEmpty()){
			log.addInfo("PublishFileRequestHandler: listOfDirSyncFiles is empty, skipping reading from db");
			resourceDoc = getData(request, xmpp, log, db, resourceFactory, profileFactory, requestDocument);
		}

		Document responseDoc = request.createResponseData();
		//List<Element> responseFileList = new ArrayList<Element> ();
		Hashtable<String,Document> groupsFileCache = new Hashtable<String, Document>();
		for(Object object:listOfFiles) {

			Element fileEl = (Element) object;		
			addOrUpdateFile(request, xmpp, log, db, resourceFactory, profileFactory, responseDoc, resourceDoc, fileEl, groupsFileCache);

		}

		return responseDoc;

	}

}
