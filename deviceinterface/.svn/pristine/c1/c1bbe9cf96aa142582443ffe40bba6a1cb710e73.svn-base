package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class CreateMediaFromCutListRequestHandler implements MessageHandlerI {

	
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		Element cutListEL = XMLUtils.getElementXpath(doc, "//CreateMediaFromCutList");
		Element metaDataEl = XMLUtils.getElementXpath(doc, "//MetaData");
		Element descriptionEl = XMLUtils.getElementXpath(doc, "//"+MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME);
		String byNID = cutListEL.attributeValue("byNID","");
		String dirID = cutListEL.attributeValue("dirID","");
		String cutList = cutListEL.attributeValue("cutList","");
		String mediaFormat = cutListEL.attributeValue("format","Proprietary");
		String title = cutListEL.attributeValue(MediaStoreServiceConst.TITLE_ATTRIBUTE_NAME,"");
		String fileUUID = cutListEL.attributeValue(MediaStoreServiceConst.UUID,"");
		String dirNID = cutListEL.attributeValue(MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME,"");
		String new_fileNID = cutListEL.attributeValue("new_fileNID", "");
		String new_UUID = cutListEL.attributeValue("new_UUID", "");
		
		Document resultDoc = createMediaStoreFile(title, descriptionEl, metaDataEl, byNID, dirID, cutList, fileUUID, dirNID, mediaFormat, device, new_fileNID, new_UUID);
	    message.setResult(resultDoc);
	}	
	
	public Document createMediaStoreFile(String title, Element descriptionEl, Element metaDataEL, String byNID, String dirID, String cutList, String fileUUID, String dirNID, String mediaFormat, DeviceI device, String new_fileNID, String new_UUID ) throws Exception {

		Element fileEl =  DocumentHelper.createElement("File");
	    XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
	    XMLUtils.addAttribute(fileEl, "id", new_UUID);
	    XMLUtils.addAttribute(fileEl, "type", "MediaClip");
	    XMLUtils.addAttribute(fileEl, "title", title);
		if (metaDataEL != null) {
			fileEl.add((Element) metaDataEL.clone());
		}
		if (descriptionEl != null) {
			fileEl.add((Element) descriptionEl.clone());
		}
	    AddFileRequestHandler addFileHandler = new AddFileRequestHandler();
		Document mediaStoreFileDoc = addFileHandler.createMediaStoreFile(fileEl, device, dirID, dirNID, MediaStoreServiceConst.NOT_READY, new_UUID, new_fileNID, "");
		return  device.getMediaStoreAgentImpl().createMediaFromCutList(byNID, dirID, cutList, title, fileUUID, dirNID, mediaFormat, new_fileNID, new_UUID, mediaStoreFileDoc);
	}
}
