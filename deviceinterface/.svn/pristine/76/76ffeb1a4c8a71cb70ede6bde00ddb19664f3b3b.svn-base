package com.barco.device.handler.MediaStore;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class ExportImportAbortFileRequestHandler implements MessageHandlerI {

	@SuppressWarnings("unchecked")
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		List<Element> list = (List<Element>) XMLUtils.getListOfNodesXpath(doc, "//FileInfo");
		if(list !=null && list.size()>0){
			MediaStoreAgentI mediaStoreAgentImpl = device.getMediaStoreAgentImpl();
			for (Element element : list) {
				String newUUID = XMLUtils.getValueXpath(element, "//NewUUID", "");
				String oldUUID = XMLUtils.getValueXpath(element, "//OldUUID", "");
				String dirNID = XMLUtils.getAttributeValueWithDefault(element, "dirNID", "");
				String type = XMLUtils.getAttributeValueWithDefault(element, "type", "");
				String format = XMLUtils.getAttributeValueWithDefault(element, "format", "");
				mediaStoreAgentImpl.abortMediaPackagingProcess(newUUID, oldUUID, dirNID, type, format);
			}
		}
	}

}
