package com.ipvs.sessionservice.resource.handler;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.HashMap;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.IPVSClientApplet;
import com.ipvs.client.ServiceClient;
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
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class UpgradeDirRequestHandler extends FileBaseRequestHandler {
	public static final String GET_FILES_WITH_BOOKMARKS_XQUERY_PATH = SessionServiceConst.XQUERYPACKAGE + "GetReadOnlyFilesAndBookmarksFromDir"+ SessionServiceConst.XQUERYXML;  
	public static final String _DIRNID_ = "_DIRNID_";

	@Override
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

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document requestDoc = request.getMessage().getDocument();

		Element requestDataElement = MRXMLUtils.getElementXpath(requestDoc, "//" + MRRequest.getRequestRootName(request.getRequestName()));
		String dirNID = MRXMLUtils.getAttributeValue(requestDataElement, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");
		String title = MRXMLUtils.getAttributeValue(requestDataElement, SessionServiceResourceConst.BY_TITLE_ATTRIBUTE, "");
		boolean upgradeFiles = Utils.booleanValue(MRXMLUtils.getAttributeValue(requestDataElement, "upgradeFiles"), false);

		if (dirNID.isEmpty() && title.isEmpty()) {
			throw new MRException(MediaRoomServiceExceptionCodes.MEDIA_STREAM_DIR_NID_NOT_FOUND, "MediaStreamDirNID Missing in request");
		}
		//TODO get dirNID when only title is given to support byTitle
		// 1- Upgrade the directory
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");

		Document mediaStoreRequestDoc = (Document) request.getMessage().getDocument().clone();
		MRXMLUtils.setAttributeValueXPath(mediaStoreRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		Element UpgradeDirEl = MRXMLUtils.getElementXpath(mediaStoreRequestDoc, ".//UpgradeDir");
		MRXMLUtils.addAttribute(UpgradeDirEl,SessionServiceResourceConst.BY_DIR_ID_ATTRIBUTE, dirID);
		Element mediaStorePortResourceEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStoreRequestDoc, mediaStorePortResourceEL, this.getClass().getName());

		// 2- upgrade file 
		if (upgradeFiles) {			
			handleUpgradeFile(request, dirNID, dirID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory,mediaStorePortResourceEL);
		}

		return null;
	}

	@Override
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc,
			MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {

		Document requestDoc = userRequest.getMessage().getDocument();
		Element UpgradeDirEl = MRXMLUtils.getElementXpath(requestDoc, ".//UpgradeDir");
		String mediaStreamDirNID =MRXMLUtils.getAttributeValue(UpgradeDirEl,SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");
		String revision = MRXMLUtils.getAttributeValueXpath(agentResponseDoc, "//UpgradeDirResponse", "rev", "");
		MRXMLUtils.getElementXpath(agentResponseDoc, "//UpgradeDirResponse").detach();
		//set write = true and change the revision returned as response
		Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(mediaStreamDirNID, userRequest, db, log);
		MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
		MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//MediaStreamDirResource", MediaStoreServiceConst.REVISION, revision);
		MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		// Restart persistent pvr sessions			
		if (!mediaStreamDirNID.isEmpty()) {			
			AppServerAgent theAppServerAgent = (AppServerAgent) serviceAgentFactory.getServiceAgent(IPVSClientApplet.APPSERVERAGENT);			
			theAppServerAgent.handlePersistentPVRSessionRestart(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamDirNID, true);			
		}		
		RequestProcessor requestProcessor = new RequestProcessor(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.DIR);
		Document dirResponse =  GetObjectUtils.getResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, requestProcessor, 
				SessionServiceConst.SELECTFORMAT_EXTENDED, SessionServiceConst.GET_DIR_REQUEST_XML, GetDirRequestHandler.class.getName());
		Element dirResponseEl=MRXMLUtils.getElementXpath(dirResponse, "//Dir");
		MRXMLUtils.addElementXpath(agentResponseDoc, ".//UpgradeDirResponseData", (Element)dirResponseEl.clone());
		return agentResponseDoc;
	}

	public void handleUpgradeFile(MRRequest request, String dirNID, String dirID, String userID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory,Element mediaStorePortResourceEL)
			throws Exception {
		
		log.addInfo("UpgradeDirRequestHandler.handleUpgradeFile : dirNID= "+dirNID+" :: dirID="+dirID);
		String requestName = MRRequest.getRequestName(UpgradeFileRequestHandler.class.getName());
		Document upgradeFileSampleRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/"+ requestName + ".xml", log);
		Element upgradeFileEL = MRXMLUtils.getElementXpath(upgradeFileSampleRequestDoc,"//UpgradeFile");
		MRXMLUtils.addAttribute(upgradeFileEL, SessionServiceResourceConst.BY_DIR_NID, dirNID);
		MRXMLUtils.addAttribute(upgradeFileEL, SessionServiceResourceConst.BY_DIR_ID_ATTRIBUTE, dirID);
		Document upgradeFileRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, upgradeFileEL);
		int countToFetch = 10;
		//fill file data in above doc		
		int startFrom = 1;
		int filesLeft=11;
		String clientData = URLDecoder.decode(request.getClientData(),"UTF-8");
		try{
			Element callBackInfo = MRXMLUtils.stringToElement(clientData);		
			filesLeft = Integer.parseInt(MRXMLUtils.getAttributeValue(callBackInfo, "filesLeft", "11"));
		} catch(Exception e){
			log.addInfo("Error while parsing clientData");
		}
		int listCount =-1;
		countToFetch = ((filesLeft > 0 && filesLeft < 10) ? filesLeft : 10);
		int bookmarkCount = 0;
		UpgradeFileRequestHandler upgradeFileRequestHandler= new UpgradeFileRequestHandler();     
		Element finalFileListEl = MRXMLUtils.stringToElement("<FileList/>");		
		if(filesLeft < 10 ){ // last loop 
			log.addInfo("UpgradeDir last "+filesLeft +" to upgrade");
		}
		if(filesLeft > 0  && countToFetch <= filesLeft )
		{
			int fileCountInOneIteration=10;
			log.addInfo("UpgradeDir Query loop start: read more");
			while(bookmarkCount < 100 && fileCountInOneIteration <= 100 && filesLeft > 0){ // if bookmark count not reached only send 100 files at a time
				String query = FileUtils.readFileResource(GET_FILES_WITH_BOOKMARKS_XQUERY_PATH, request.getClass());				
				query = query.replaceAll(_DIRNID_, dirNID);
				query = query.replace(SessionServiceConst.STARTFROM, String.valueOf(startFrom));
				query = query.replace(SessionServiceConst.COUNTTOFETCH, String.valueOf(countToFetch));	
				log.addInfo("UpgradeDir Fetching files from query: startFrom: "+startFrom+" ::countToFetch:"+countToFetch);
				String responseXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query,
						new HashMap<String, String>(), null);
				Element fileListEl = MRXMLUtils.stringToElement(responseXML);	
				
				String bookmarkSize = MRXMLUtils.getAttributeValue(fileListEl, "bookmarkCount","");

				String fileListSize = MRXMLUtils.getAttributeValue(fileListEl, "total","");
				log.addInfo("UpgradeDir Done Fetching files from query: bookmarkSize: "+bookmarkSize+" ::fileListSize:"+fileListSize);
				
				bookmarkCount = bookmarkSize.isEmpty()?0:Integer.parseInt(bookmarkSize);
				listCount = Integer.parseInt(fileListSize);
				startFrom = startFrom + countToFetch;				
				filesLeft = listCount- (startFrom - 1);
				fileCountInOneIteration=fileCountInOneIteration+countToFetch;					
				MRXMLUtils.addContent(finalFileListEl, MRXMLUtils.getChildren(fileListEl, "File"));				
			}
			log.addInfo("UpgradeDir Query loop end: send request");
			MRXMLUtils.addElementXpath(upgradeFileRequestDoc, ".//UpgradeFile", (Element) finalFileListEl.clone());
			Element callbackInfo = MRXMLUtils.stringToElement("<UpgradeDirCallBack />");
			MRXMLUtils.addAttribute(callbackInfo, "filesLeft", String.valueOf(filesLeft));
			MRXMLUtils.addAttribute(callbackInfo, "dirNID", dirNID);
			MRXMLUtils.addAttribute(callbackInfo, "dirID", dirID);
			
			MRRequest upgradeFileRequest = MRRequest.createRequest("managementserver.localhost", request, upgradeFileRequestDoc, requestName,
					ServiceClient.SESSIONSERVICE_RESOURCE, URLEncoder.encode(callbackInfo.asXML(), "UTF-8"));   
			upgradeFileRequestHandler.handleUpgradeFileRequest(upgradeFileRequest, xmpp, log, db, resourceFactory,
					profileFactory, mediaStorePortResourceEL);

		}		
	}
}
