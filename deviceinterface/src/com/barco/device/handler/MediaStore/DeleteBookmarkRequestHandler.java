package com.barco.device.handler.MediaStore;

import java.util.List;

import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class DeleteBookmarkRequestHandler implements MessageHandlerI{
	
public void handleMessage(Message message, DeviceI device) throws Exception {
		
		Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//DeleteBookmarkRequest");
		String dirNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_ID_ATTRIBUTE_NAME, "");
		Element resultEl = DocumentHelper.createElement("Bookmarks");
		
		List<?> bookmarkList = XMLUtils.getListOfNodesXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.DELETE_BOOKMARK);
		for(Object obj: bookmarkList) {
			Element bookmarkEl = (Element)obj;
			String bookmarkUUID = XMLUtils.getAttributeValueWithDefault(bookmarkEl, MediaStoreServiceConst.BY_FILE_ID,"");
			String mediaUUID = XMLUtils.getAttributeValueWithDefault(bookmarkEl, MediaStoreServiceConst.BY_ID,"");
			String whereNID = XMLUtils.getAttributeValueWithDefault(bookmarkEl, MediaStoreServiceConst.BY_FILE_NID,"");
			device.getMediaStoreAgentImpl().deleteBookmark(dirID, dirNID, mediaUUID, bookmarkUUID);
			Element bookmarkElememt = DocumentHelper.createElement("Bookmark");
			XMLUtils.addAttribute(bookmarkElememt, MediaStoreServiceConst.NID_ATTRIBUTE_NAME, whereNID);
			resultEl.add(bookmarkElememt);
		}				
		message.setResult(XMLUtils.elementToDocument(resultEl));
	}

}
