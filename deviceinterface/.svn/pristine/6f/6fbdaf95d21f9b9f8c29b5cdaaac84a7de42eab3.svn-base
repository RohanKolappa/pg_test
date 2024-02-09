package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;


public class UpgradeDirRequestHandler implements MessageHandlerI {

    @Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document requestDoc = message.getMessageDoc();
		Element requestElement=XMLUtils.getElementXpath(requestDoc, "//UpgradeDir");
		String dirNID = XMLUtils.getAttributeValue(requestElement, "byNID"); 
		String dirId = XMLUtils.getAttributeValue(requestElement, MediaStoreServiceConst.BY_DIR_ID_ATTRIBUTE_NAME); 
		Document responseDoc =device.getMediaStoreAgentImpl().upgradeDir(dirId, dirNID);
		message.setResult(responseDoc);
	}
    
}
