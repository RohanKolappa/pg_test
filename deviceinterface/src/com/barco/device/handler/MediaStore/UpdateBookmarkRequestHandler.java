package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;


public class UpdateBookmarkRequestHandler implements MessageHandlerI {
	
	
public void handleMessage(Message message, DeviceI device) throws Exception {
		
		Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//UpdateBookmarkRequest");
		String mediaStroreFileId = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_FILE_ID,"");
		String bookmarkId = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_ID,"");
		String dirNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME,"");
		String dirID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_ID_ATTRIBUTE_NAME,"");
		String title = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,"");
		String comment = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.COMMENT,"");
		String bookmarkNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_FILE_NID,"");
		String timecode = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.TC,"");
		String offSet   =XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.OFFSET,"");
		String synchClipId = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.SYNC_CLIP_ID,"");
		Element metaDataEL = XMLUtils.getElementXpathFromElement(requestElement, ".//MetaData");	
		
		Document mediaStoreBookmarkDocument = device.getMediaStoreAgentImpl().getMediaStoreBookmarkDocument(dirID, dirNID, mediaStroreFileId,bookmarkId);
		
		
			
		if(!title.isEmpty())
			XMLUtils.setValueXpath(mediaStoreBookmarkDocument, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
		
		if(!comment.isEmpty())
			XMLUtils.setValueXpath(mediaStoreBookmarkDocument, "//"+  MediaStoreServiceConst.COMMENT, comment);
		
		if(metaDataEL != null) 
			XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreBookmarkDocument, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEL);
		
		
		String currentTime = String.valueOf(System.currentTimeMillis());
		XMLUtils.setValueXpath(mediaStoreBookmarkDocument,"//"+ MediaStoreServiceConst.DATE_MODIFIED,currentTime);
		
		mediaStoreBookmarkDocument =  device.getMediaStoreAgentImpl().updateBookmark(dirID, dirNID, mediaStroreFileId, bookmarkId, mediaStoreBookmarkDocument, timecode, offSet, synchClipId);	
		
		mediaStoreBookmarkDocument.getRootElement().addAttribute("NID", bookmarkNID);
		
		message.setResult(XMLUtils.elementToDocument(mediaStoreBookmarkDocument.getRootElement()));
	}

}
