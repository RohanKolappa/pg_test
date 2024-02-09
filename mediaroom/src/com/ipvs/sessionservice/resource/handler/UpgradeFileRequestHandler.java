package com.ipvs.sessionservice.resource.handler; 

import java.net.URLDecoder;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
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
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class UpgradeFileRequestHandler extends FileBaseRequestHandler {
	public static final String GET_FILE_INFO_WITH_BOOKMARKS_XQUERY_PATH = SessionServiceConst.XQUERYPACKAGE + "GetFileInfowithBookMarkList"+ SessionServiceConst.XQUERYXML;  
	public static final String _FILENID_ = "_FILENID_";

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

		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.MEDIA_STREAM_FILE_RESOURCE);
		String fileNID = requestObject.getNID(); 
		log.addInfo("UpgradeFileRequestHandler.getResponse fileNID :"+  fileNID);
		Document fileDoc=MediaStoreServiceUtils.checkMediaStreamFileState(fileNID, log, db, request);
		String dirNID = MRXMLUtils.getValueXpath(fileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");
		//prepare upgradeFilerequest for DI and send
		Document requestDoc= request.getMessage().getDocument();
		Element UpgradeFileEl = MRXMLUtils.getElementXpath(requestDoc, ".//UpgradeFile");
		MRXMLUtils.addAttribute(UpgradeFileEl,SessionServiceResourceConst.BY_DIR_NID, dirNID);
		MRXMLUtils.addAttribute(UpgradeFileEl,SessionServiceResourceConst.BY_DIR_ID_ATTRIBUTE, dirID);
		String mediaType = MRXMLUtils.getValueXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		Element fileEl = createUpgradeFileElement(request,fileNID,db,log);
		MRXMLUtils.addElementXpath(requestDoc, ".//UpgradeFile", (Element)fileEl.clone());
		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			//bypass all clips in media group

			String mediaGroupID = MRXMLUtils.getValueXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
			List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
			for (Object mediaFileObj : listOfFiles){
				Element mediaClipFile = (Element)mediaFileObj;
				String mediaClipFileNID=MRXMLUtils.getAttributeValue(mediaClipFile, "NID");
				Element mediaClipFileEL = createUpgradeFileElement(request,mediaClipFileNID,db,log);
				log.addInfo("UpgradeFileRequestHandler.getResponse mediaClipFileNID :"+  mediaClipFileNID);
				MRXMLUtils.addElementXpath(requestDoc, ".//UpgradeFile", (Element)mediaClipFileEL.clone());
			}		
		}


		Element mediaStorePortResourceEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(fileDoc, true, request, db, log);
		handleUpgradeFileRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory,mediaStorePortResourceEL);    

		return null;
	}

	@Override
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc,
			MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {			  			  
		List<?> upgradedFiles = MRXMLUtils.getListXpath(agentResponseDoc,"//File");	        
		Element UpgradeFileEl = DocumentHelper.createElement("UpgradeFile");
		String revision = MRXMLUtils.getAttributeValueXpath(agentResponseDoc, "//File/MediaStoreFile", MediaStoreServiceConst.REVISION, "");		
		Document fileResponse;
		String dirNID="";
		String dirID="";
		List<String>upgradeSuccessNIDList = MRXMLUtils.getValuesXpath(agentResponseDoc, "//File[not(./@upgradeState='"+MediaStoreServiceConst.ERROR+"')]/@fileNID");
		List<String>upgradeFailureNIDList = MRXMLUtils.getValuesXpath(agentResponseDoc, "//File[@upgradeState='"+MediaStoreServiceConst.ERROR+"']/@fileNID");
		List<String> upgradeBookmarkNIDsList = MRXMLUtils.getValuesXpath(agentResponseDoc, "//bookmark/@NID");
		log.addInfo("Before bulk update of mediatreamfiles");
		if(upgradeSuccessNIDList != null){
			String updateSuccessfulFilesUpgrades = "for $list1 in collection('ipvs.default.mediastreamfileresourcelist')/*[";
			//updateSuccessfulFilesUpgradesBuf.append("for $list1 in collection('ipvs.default.mediastreamfileresourcelist')/*[");
			for(String successFileNID : upgradeSuccessNIDList){
				updateSuccessfulFilesUpgrades +=".[@NID = '"+successFileNID+"'] or ";

			}
			updateSuccessfulFilesUpgrades= updateSuccessfulFilesUpgrades.substring(0, updateSuccessfulFilesUpgrades.length() - 3);
			updateSuccessfulFilesUpgrades +="] let $updateState := update replace $list1//Info/Properties/Permissions/Write with <Write>true</Write> ";
			//updateSuccessfulFilesUpgradesBuf.append("] let $updateState := update replace $list1//Info/Properties/Permissions/Write with <Write>true</Write> ");
			if(revision != null && !revision.isEmpty()) 
				updateSuccessfulFilesUpgrades +=" let $updateRevision := update value $list1//Info/MediaStoreFileInfo/@MediaStoreFile-Rev with '"+revision+"' ";
			updateSuccessfulFilesUpgrades +="return $list1";
			try{
				MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, userRequest, log, db, updateSuccessfulFilesUpgrades,
						new HashMap<String, String>(), null);
			}
			catch(Exception e){
				log.addInfo("Error while executing success NID list: "+updateSuccessfulFilesUpgrades);
				e.printStackTrace();
			}

		}
		if(upgradeFailureNIDList!=null){
			String updatefailedFilesUpgrades = "for $list1 in collection('ipvs.default.mediastreamfileresourcelist')/*[";
			//updatefailedFilesUpgradesBuf.append("for $list1 in collection('ipvs.default.mediastreamfileresourcelist')/*[");
			for(String failureFileNID : upgradeFailureNIDList){
				updatefailedFilesUpgrades +=".[@NID = '"+failureFileNID+"'] or ";

			}
			updatefailedFilesUpgrades= updatefailedFilesUpgrades.substring(0,updatefailedFilesUpgrades.length() - 3);
			updatefailedFilesUpgrades+="] let $updateStateMsg := update replace $list1//State/StateMsg with <StateMsg>"+MediaStoreServiceConst.UPGRADE_ERROR+"</StateMsg> "
					+ "let $updateStateFlag := update replace $list1//State/StateFlag with <StateFlag>"+MediaStoreServiceConst.ERROR+"</StateFlag> return $list1";
			//log.addInfo("The failure query is: "+updatefailedFilesUpgradesBuf.toString());
			try{
				MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, userRequest, log, db, updatefailedFilesUpgrades,
						new HashMap<String, String>(), null);
			}
			catch(Exception e){
				log.addInfo("Error while executing failure NID list: "+updatefailedFilesUpgrades);
				e.printStackTrace();
			}

		}
		log.addInfo("after bulk update of mediatreamfiles");
		if(upgradeBookmarkNIDsList != null){
			String updateBookmarkUpgrades = "for $list1 in collection('ipvs.default.bookmarklist')/*[";
			//updateBookmarkUpgradesBuf.append("for $list1 in collection('ipvs.default.bookmarklist')/*[");
			for(String bookmarkNID : upgradeBookmarkNIDsList){
				updateBookmarkUpgrades+=".[@NID = '"+bookmarkNID+"'] or ";

			}
			updateBookmarkUpgrades = updateBookmarkUpgrades.substring(0, updateBookmarkUpgrades.length() - 3);
			updateBookmarkUpgrades+="] return if ($list1[not(.//UUID)] 	) then update insert <UUID>{data($list1//@NID)}</UUID>  following $list1//MetaData ";
			updateBookmarkUpgrades+="else if ($list1[(.//UUID='')]) then update replace $list1//UUID with <UUID>{data($list1//@NID)}</UUID> ";
			updateBookmarkUpgrades+="else <Bookmark/>";
			try{
				MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, userRequest, log, db, updateBookmarkUpgrades,
						new HashMap<String, String>(), null);
			}
			catch(Exception e){
				log.addInfo("Error while executing failure NID list: "+updateBookmarkUpgrades);
				e.printStackTrace();
			}
		}
		log.addInfo("after bulk update of bookmarks");
		if(!userRequest.getUserJID().equals("managementserver.localhost")){
			for(Object object : upgradedFiles){			
				Element fileEl  = (Element) object;
				String fileNID = MRXMLUtils.getAttributeValue(fileEl, "fileNID","");
				Document fileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(fileNID, log, db, userRequest);
				String mediaType = MRXMLUtils.getValueXpath(fileDoc, "//MediaStoreFileInfo/Type", "");				
				fileResponse = getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fileNID, "",mediaType);				
				UpgradeFileEl.add((Element)MRXMLUtils.getElementXpath(fileResponse, "//File").clone());
			} 
		}
		//In case of upgrade dir call, we bypass the getfile call, so send dummy data back to request handler
		if(userRequest.getUserJID().equals("managementserver.localhost")){
			UpgradeFileEl = MRXMLUtils.getElementXpath(agentResponseDoc, "//Data");
		}
		Document upgradedFilesDoc = MRXMLUtils.elementToDocument(UpgradeFileEl);
		String clientData = URLDecoder.decode(userRequest.getClientData(),"UTF-8");
		try{
			Element callBackInfo = MRXMLUtils.stringToElement(clientData);
			dirNID = MRXMLUtils.getAttributeValue(callBackInfo, "dirNID", "");
			dirID = MRXMLUtils.getAttributeValue(callBackInfo, "dirID", "");
		} catch(Exception e){
			log.addInfo("UpgradeFileRequestHandler Error while parsing clientData");
		}
		if(!dirNID.isEmpty()){
			Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, userRequest, db, log);		    		
			Element mediaStorePortResourceEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log);
			new UpgradeDirRequestHandler().handleUpgradeFile(userRequest, dirNID,dirID, userRequest.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory,mediaStorePortResourceEL);
		}		        	
		return upgradedFilesDoc;		         
	}


	public Element createUpgradeFileElement(MRRequest request, String fileNID,MRDB db, MRLog log) throws Exception
	{

		//fill file data in above doc
		String query = FileUtils.readFileResource(GET_FILE_INFO_WITH_BOOKMARKS_XQUERY_PATH, request.getClass());
		query = query.replaceAll(_FILENID_, fileNID);
		String file  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null); 
		Element fileEl = MRXMLUtils.stringToElement(file);
		return fileEl;
	}

	public void handleUpgradeFileRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Element mediaStorePortResourceEL) throws Exception {
		Document mediaStoreRequestDoc = (Document) request.getMessage().getDocument().clone();
		MRXMLUtils.setAttributeValueXPath(mediaStoreRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		log.addInfo("UpgradeDir Sending UpgradeFilerequest to DMS");
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStoreRequestDoc, mediaStorePortResourceEL, this.getClass().getName());

	}

}
