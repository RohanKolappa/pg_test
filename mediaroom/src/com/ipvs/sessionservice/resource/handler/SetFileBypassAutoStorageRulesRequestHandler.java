package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;

/*
 * Sets the value of BypassAutoStorageRules flag in mediaStreamresource
 */
public class SetFileBypassAutoStorageRulesRequestHandler extends FileBaseRequestHandler {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addInfo("SetFileBypassAutoStorageRuleRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.MEDIA_STREAM_FILE_RESOURCE);

		Document mediaStreamFileDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, 
				serviceAgentFactory, requestObject, "", "");		
		Element setBypassRequest = requestObject.getRequestElement();		
		
		String bypassAutoStorageRules = MRXMLUtils.getAttributeValue(setBypassRequest, MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES, "");
		//String actionData = MRXMLUtils.elementToString(setBypassRequest);
		
		String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");		
		MRRequest newRequest = MRRequest.createRequest(request.getAppServerJID());		
		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			//bypass all clips in media group
			
			String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
			List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
			if ((listOfFiles != null) && !listOfFiles.isEmpty()) {
				Document MediaClipsDoc = ((Element)listOfFiles.get(0)).getDocument();				
				List<String> listOfParentDirFiles = MRXMLUtils.getValuesXpath(MediaClipsDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME);
				Set<String> distinctParentDirNIDs = new HashSet<String>(listOfParentDirFiles);
				for(String object : distinctParentDirNIDs){
					String query = "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE +
							" [(.//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+" = '"+object+"') and (.//"+MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT + "!= '"+MediaStoreServiceConst.DELETED+"')]";
					
					List<?> listOfMediaByParentDir = MRXMLUtils.getListXpath(MediaClipsDoc, query);
					
					if(listOfMediaByParentDir.size() > 0){
						this.sendSetFileRequestToMediaStore(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory,MediaStoreServiceConst.SET_FILE_REQUEST, object, listOfMediaByParentDir, bypassAutoStorageRules, this.getClass().getName());
					}
					
				}	

				
			}
		}
		else {			
				//bypass single mediaclip
				this.sendSetFileRequestToMediaStore(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileDoc, 
						"", "", "", bypassAutoStorageRules, null, this.getClass().getName());  
		}
		return request.createResponseUsingRequestRootName(null); 


	}	  
	
	
	private void sendSetFileRequestToMediaStore(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String mediaStoreRequestName,String mediaStreamFileDirNID, List<?> fileList, String bypassAutoStorageRules, String responseHandlerClass)  throws MRException,Exception {

		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(mediaStreamFileDirNID, userRequest, db, log);
		Element mediaStorePortElement =MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log,true);
		Document mediaStoreRequestDocument = generateRequestDocument(mediaStoreRequestName, getFileMapping(fileList), mediaStreamFileDirNID, mediaStorePortElement, 
				"", "", "", bypassAutoStorageRules, null, userRequest.getNID(), log);
		sendMediaStoreRequestToDMS(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStoreRequestDocument, mediaStorePortElement, this.getClass().getName());
	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		MRRequest newRequest = MRRequest.createRequest(request.getUserJID());
		Document responseDoc = request.getResult();
		
			
		try {			
			List<?> mediaStreamfileNIDList = MRXMLUtils.getListXpath(responseDoc,"//File");
				
			List<String>mediaGroupNIDs = new ArrayList<String>();
			for(Object object : mediaStreamfileNIDList){
				Element fileEl  = (Element) object;
				
				Element mediaStreamFileEL = createOrUpdateMediaStreamFileInDB(fileEl, newRequest, responseDoc, null, false, xmpp, log, db, resourceFactory, serviceAgentFactory);
				String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileEL, ".//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
				
				if(!mediaGroupID.isEmpty() && !mediaGroupNIDs.contains(mediaGroupID)) {
					mediaGroupNIDs.add(mediaGroupID);
					Document mediaGroupDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, newRequest.getUserJID(), log, db);
					Semaphore lock = IPVSMonitor.getMonitor().getLock(mediaGroupID, request.getNID());				    
					try {
						MediaStoreServiceUtils.updateMediaGroupProperties(mediaGroupDoc, mediaGroupID, newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
					} catch (Exception e) {	
						MRException me = new MRException(MediaStoreServiceExceptionCodes.UPDATE_MEDIA_STORE_FILE_FAILED, e.toString());
						request.setException(me);
						e.printStackTrace();
					} finally {
						IPVSMonitor.getMonitor().releaseLock(mediaGroupID, lock);
					}
				     
				}

			}
		} catch (MRException exp) {			
			exp.printStackTrace();
		} catch (Exception exp) {			
			exp.printStackTrace();
		}

	}

}
