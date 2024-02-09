package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class OffloadFileRequestHandler implements MessageHandlerI {

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document requestDoc = message.getMessageDoc();
		Element requestElement = XMLUtils.getElementXpath(requestDoc, "//OffloadFile");
		String dirNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_DIR_ID_ATTRIBUTE_NAME, "");
		String format = XMLUtils.getAttributeValue(requestElement, MediaStoreServiceConst.FORMAT_ATTRIBUTE);
		String nasURL = XMLUtils.getAttributeValue(requestElement, MediaStoreServiceConst.NAS_URL_ATTRIBUTE);
		String fileNID = XMLUtils.getAttributeValue(requestElement, MediaStoreServiceConst.NID_ATTRIBUTE_NAME);
		String fileUUID = XMLUtils.getAttributeValue(requestElement, MediaStoreServiceConst.UUID);
		device.getLogger().info("OffloadFileRequestHandler : handleMessage:  dirNID= "+dirNID);
		device.getMediaStoreAgentImpl().offloadFileToNAS(dirNID, dirID, format, nasURL, fileNID, fileUUID);
	}

}
