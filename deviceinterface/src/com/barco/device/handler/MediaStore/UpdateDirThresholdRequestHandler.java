package com.barco.device.handler.MediaStore;

import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class UpdateDirThresholdRequestHandler implements MessageHandlerI {

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		
		Element SetFileRequestEL = XMLUtils.getElementXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.UPDATE_DIR_THRESHOLD_REQUEST_ELEMENT);
		
		String dirNID = XMLUtils.getAttributeValueWithDefault(SetFileRequestEL, MediaStoreServiceConst.BY_DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(SetFileRequestEL, MediaStoreServiceConst.BY_DIR_ID_ATTRIBUTE_NAME, "");
		String diskFullThreshold = XMLUtils.getAttributeValueWithDefault(SetFileRequestEL, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, "");
		if(diskFullThreshold.isEmpty()) {
			return;
		}
		device.getMediaStoreAgentImpl().updateDirThreshold(dirID, dirNID, Utils.getLongValue(diskFullThreshold,0));
	}

}
