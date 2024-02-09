package com.barco.device.handler.DeviceAdmin;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class SetDeviceConfigRequestHandler implements MessageHandlerI {

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		Document requestDoc = message.getMessageDoc();

		Document docModify = null;
		String xPath = "";
		List<?> cleanList = null;
		
		// clean the list to remove empty nodes or clean nodes
		docModify = XMLUtils.stringToDocument("<DeviceModifications/>");		
		docModify.getRootElement().add((Element) requestDoc.getRootElement().clone());
		xPath = "//*[@" + DeviceAdminConst.DIRTY_TAG + "='"+ DeviceAdminConst.CLEAN_FLAG + "'" + "or @"
										+ DeviceAdminConst.DIRTY_TAG + "=''" + "]";
		cleanList =DocumentHelper.createXPath(xPath).selectNodes(docModify);
		for (Iterator<?> i = cleanList.iterator(); i.hasNext();) {
			Element cleanEL = (Element) i.next();
			cleanEL.detach();
		}
		
		 List<?> list = docModify.content();	       
         if ((list == null) || (list.size() == 0)) {
            return;
         }
         
         //update device given config 
         device.getDeviceAdminAgentImpl().setDeviceConfig(docModify); 
         
         //save to flash 
         String doNotSaveToFlash = XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(requestDoc, "//SetDeviceConfigRequestData"), "doNotSaveToFlash","false");
         boolean b_DoNotSaveToFlash = Boolean.valueOf(doNotSaveToFlash);
        
    	 if(!b_DoNotSaveToFlash){
    		 device.getDeviceAdminAgentImpl().saveConfiguration();
    	 }
	}

}
