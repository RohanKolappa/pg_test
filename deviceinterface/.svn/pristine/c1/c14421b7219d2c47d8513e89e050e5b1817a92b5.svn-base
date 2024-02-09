package com.barco.device.handler.MediaStore;


import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;


public class SetFileRequestHandler implements MessageHandlerI{

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		
		Element SetFileRequestEL = XMLUtils.getElementXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.SET_FILE_REQUEST_ELEMENT);
		String dirNID = XMLUtils.getAttributeValueWithDefault(SetFileRequestEL, MediaStoreServiceConst.BY_DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(SetFileRequestEL, MediaStoreServiceConst.BY_DIR_ID_ATTRIBUTE_NAME, "");
		List<?> fileList = XMLUtils.getListOfNodesXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.FILE_ELEMENT);
		Element resultEl = DocumentHelper.createElement("FileList");
		
		for(Object obj: fileList) {
			Element fileEl = (Element)obj;
			String fileUUID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, "");
			String fileNID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.FILE_NID_ATTRIBUTE_NAME, "");
			
			String title = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.TITLE_ATTRIBUTE_NAME, "");
			String state = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.STATE_ATTRIBUTE_NAME, "");
			String bypassAutoStorageRules = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES, "");
			String description = XMLUtils.getValueXpath(fileEl, ".//"+  MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, "");
			Element metaDataEl = XMLUtils.getElementXpathFromElement(fileEl, ".//"+  MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
			
			Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument(dirID, "", fileUUID);
			
			
			if(!title.isEmpty())
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//"  +MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
			
			if(!state.isEmpty())
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//" + MediaStoreServiceConst.STATE_EL_NAME, state);
			
			if(!bypassAutoStorageRules.isEmpty())
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, bypassAutoStorageRules);
			
			if(!description.isEmpty())
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//"+  MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, description);
			
			if(metaDataEl != null) 
				XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDocument, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEl);
			
		    mediaStoreFileDocument =  device.getMediaStoreAgentImpl().setFileRequest(dirID, dirNID, fileNID, fileUUID, mediaStoreFileDocument);	
		    
		    resultEl.add(MediaStoreUtils.generateFileElement(fileUUID, fileNID, dirNID, mediaStoreFileDocument));
		}
		
		message.setResult(XMLUtils.elementToDocument(resultEl));
		
	}
	
}
