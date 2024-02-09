package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class ClearDownloadCacheRequestHandler implements MessageHandlerI {

	
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		Element clearDwnldCacheEl = XMLUtils.getElementXpath(doc, "//ClearDownloadCache");
		String byNID = clearDwnldCacheEl.attributeValue("byNID","");
		String dirID = clearDwnldCacheEl.attributeValue("dirID","");
		String fileUUID = clearDwnldCacheEl.attributeValue(MediaStoreServiceConst.UUID,"");
		String dirNID = clearDwnldCacheEl.attributeValue(MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME,"");
		device.getMediaStoreAgentImpl().clearDownloadCache(byNID, dirID, fileUUID, dirNID);
	}	
}
