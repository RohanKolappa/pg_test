package com.barco.device.handler.MediaStore;

import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;


public class AddBookmarkRequestHandler implements MessageHandlerI {
	
	
public void handleMessage(Message message, DeviceI device) throws Exception {
		
		Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//AddBookmarkRequest");
		String mediaUUID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_FILE_ID,"");
		Document file =createBookmarkStoreFile(requestElement, device, false, mediaUUID, UUID.randomUUID().toString(),"");		
		message.setResult(file);
	}


public Document createBookmarkStoreFile(Element bookmarkElement,DeviceI device,boolean publishToSC,String mediaUUID,String bookmarkUUID,String fileNID) throws Exception {
	
	String tc = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.TC,"0");
	String offset = XMLUtils.getAttributeValueWithDefault(bookmarkElement,MediaStoreServiceConst.OFFSET ,"0");
	String fNID = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME,"");
	String dirID = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.DIR_ID_ATTRIBUTE_NAME,"");
	String title = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,"");
	String comment = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.COMMENT,"");
	String bookmarkNID = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.BY_FILE_NID,"");
	String synchClipId = XMLUtils.getAttributeValueWithDefault(bookmarkElement, MediaStoreServiceConst.SYNC_CLIP_ID,"");
	
	Element metaDataEL = XMLUtils.getElementXpathFromElement(bookmarkElement, ".//MetaData");	
	
	
	Document bookmarkFileDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/BookmarkStoreFile.xml");
	String currentTime = String.valueOf(System.currentTimeMillis());
	XMLUtils.setValueXpath(bookmarkFileDoc, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
	XMLUtils.setValueXpath(bookmarkFileDoc, "//"+MediaStoreServiceConst.COMMENT, comment);
	XMLUtils.setValueXpath(bookmarkFileDoc, "//"+MediaStoreServiceConst.DATE_CREATED,currentTime);
	XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,currentTime);
	XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.TC,tc);
	XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.OFFSET,offset);
	System.out.println(bookmarkFileDoc.asXML());
	if(metaDataEL!=null) {
		XMLUtils.replaceContent(XMLUtils.getElementXpath(bookmarkFileDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEL);
	}
	Document file=device.getMediaStoreAgentImpl().addBookmark(dirID, fNID, mediaUUID,bookmarkUUID,synchClipId,bookmarkFileDoc);
	
	Document resultDoc = DocumentHelper.createDocument();
	
	Element bookmarkEl =  DocumentHelper.createElement("Bookmark");
	resultDoc.add(bookmarkEl);
	XMLUtils.addAttribute(bookmarkEl, "UUID", bookmarkUUID);
	XMLUtils.addAttribute(bookmarkEl, "NID", bookmarkNID);
	XMLUtils.addAttribute(bookmarkEl, MediaStoreServiceConst.TC, String.valueOf(XMLUtils.getValueXpath(file, "//"+ MediaStoreServiceConst.TC, "0")));
	XMLUtils.addAttribute(bookmarkEl, MediaStoreServiceConst.OFFSET, String.valueOf(XMLUtils.getValueXpath(file, "//"+MediaStoreServiceConst.OFFSET, "0")));
	
	return XMLUtils.elementToDocument(bookmarkEl);
}




}
