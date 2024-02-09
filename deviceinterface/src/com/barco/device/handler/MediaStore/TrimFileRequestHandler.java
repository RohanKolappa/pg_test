package com.barco.device.handler.MediaStore;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class TrimFileRequestHandler implements MessageHandlerI {

	
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		Element cutListEL = XMLUtils.getElementXpath(doc, "//TrimFile");
		String byNID = cutListEL.attributeValue("byNID","");
		String dirID = cutListEL.attributeValue("dirID","");
		String cutList = cutListEL.attributeValue("cutList","");
		String dirNID = cutListEL.attributeValue(MediaStoreServiceConst.DIR_NID_ATTRIBUTE_NAME,"");
		String fileUUID = cutListEL.attributeValue(MediaStoreServiceConst.UUID,"");
		
		device.getMediaStoreAgentImpl().trimMediaFromCutList(byNID, dirNID, dirID, cutList, fileUUID);
		
		Document resultDoc = DocumentHelper.createDocument();
		Element respfileEl =  DocumentHelper.createElement("TrimFile");
		resultDoc.add(respfileEl);
		XMLUtils.addAttribute(respfileEl, "NID", byNID);
		XMLUtils.addAttribute(respfileEl, "id", fileUUID);
		XMLUtils.addAttribute(respfileEl, "dirNID", dirNID);
	    message.setResult(resultDoc);
	}	
}
