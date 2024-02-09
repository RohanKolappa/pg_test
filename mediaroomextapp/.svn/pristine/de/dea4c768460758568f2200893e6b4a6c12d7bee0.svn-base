package com.barco.device.handler.DeviceAdmin;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.impl.DeviceAdminConst;
import com.barco.utils.mr.MRXMLUtils;

public class SetDeviceConfigRequestHandler implements MessageHandlerI {

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		Document requestDoc = message.getMessageDoc();

		Document docModify = null;
		String xPath = "";
		List<?> cleanList = null;

		// clean the list to remove empty nodes or clean nodes
		docModify = MRXMLUtils.getDocument("<DeviceModifications/>");
		xPath = "//DeviceConfig";
		MRXMLUtils.addElement(docModify.getRootElement(), (Element) requestDoc.getRootElement().clone());
		xPath = "//*[@" + DeviceAdminConst.DIRTY_TAG + "='"+ DeviceAdminConst.CLEAN_FLAG + "'" + "or @"
										+ DeviceAdminConst.DIRTY_TAG + "=''" + "]";
		cleanList = MRXMLUtils.getListXpath(docModify, xPath);
		for (Iterator<?> i = cleanList.iterator(); i.hasNext();) {
			Element cleanEL = (Element) i.next();
			cleanEL.detach();
		}
		
		 List<?> list = MRXMLUtils.getList(docModify.getRootElement());
	     message.setResult(DocumentHelper.parseText("<SetDeviceConfig/>"));
	       
         if ((list == null) || (list.size() == 0)) {
            return;
         }
         
         //ask the device server to update device configuration
         device.getDeviceAdminAgentImpl().getDeviceServer().setDeviceConfig(docModify); 
	}

}
