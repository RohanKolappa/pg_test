package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class PrepareFileForDownloadRequestHandler implements MessageHandlerI {

	
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		Element prepareFileEl = XMLUtils.getElementXpath(doc, "//PrepareFile");
		String requestTimeStamp = prepareFileEl.attributeValue("requestTimeStamp");
		Element fileEl = prepareFileEl.element("File");
		String dirNID = fileEl.attributeValue("dirNID");
		String fileNID = fileEl.attributeValue("fileNID");
		String fileUUID = fileEl.attributeValue("UUID");
		String mediaFormat = fileEl.attributeValue("format");
		String startOffset = fileEl.attributeValue("startOffset");
		String endOffset = fileEl.attributeValue("endOffset");
		String type = fileEl.attributeValue("type");
		String mediaGroupPlaylength = fileEl.attributeValue("mediaGroupPlaylength");
		String new_fileUUID = fileEl.attributeValue("new_fileUUID");
		
		MediaStoreAgentI mediaStoreAgentImpl = device.getMediaStoreAgentImpl();
		mediaStoreAgentImpl.prepareFileForDownload(fileNID, fileUUID,
				dirNID, mediaFormat, startOffset, endOffset,
				requestTimeStamp, type, mediaGroupPlaylength, new_fileUUID);		
	}	

}
