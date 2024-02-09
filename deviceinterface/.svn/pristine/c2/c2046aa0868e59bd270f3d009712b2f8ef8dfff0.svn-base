package com.barco.device.handler.MediaStore;

import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.utils.MediaRoomServiceExceptionCodes;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class GetOffsetTimecodeRequestHandler implements MessageHandlerI {
	public void handleMessage(Message message, DeviceI device) throws Exception {

		Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//GetOffsetTimecodeRequest");
		String mediaUUID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_FILE_ID, "");
		String bookmarkNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.BY_FILE_NID, "");		
		long tc = Utils.getLongValue(XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.TC, "0"),0);
		long offset = Utils.getLongValue(XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.OFFSET, "0"),0);
		String dirNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.DIR_ID_ATTRIBUTE_NAME, "");

		if(tc==0 && offset==0){
			throw new ServiceException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, "Both offset and TC can't be 0");
		}else if(tc==0){	
			tc=	device.getMediaStoreAgentImpl().getTimecode(dirID, dirNID, mediaUUID, offset);
		}else if(offset==0){
			offset=device.getMediaStoreAgentImpl().getOffset(dirID, dirNID, mediaUUID, tc);
		} 
		
		Element DataEl =  DocumentHelper.createElement("Data");
		XMLUtils.addAttribute(DataEl, MediaStoreServiceConst.BY_FILE_NID, bookmarkNID);
		XMLUtils.addAttribute(DataEl, MediaStoreServiceConst.TC, String.valueOf(tc));
		XMLUtils.addAttribute(DataEl, MediaStoreServiceConst.OFFSET, String.valueOf(offset));

		message.setResult(XMLUtils.elementToDocument(DataEl));
	}
}
