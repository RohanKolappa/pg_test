package com.barco.device.handler.DeviceAdmin;

import java.io.File;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.impl.DeviceAdminConst;
import com.barco.utils.FileUtils;
import com.barco.utils.mr.MRXMLUtils;

public class GetDynamicDeviceStatus  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		
	    Element deviceStatusElement = null; 
         
    	String strDeviceXML = FileUtils.readFile(new File(DeviceAdminConst.DEVICESTATUSFILE));
    	Document deviceDoc = MRXMLUtils.stringToDocument(strDeviceXML);
    	deviceStatusElement = MRXMLUtils.getElementXpath(deviceDoc,"//DeviceStatus");

    	//update device status
    	device.getDeviceAdminAgentImpl().getDeviceServer().getDeviceStatus(deviceStatusElement);    	

    	//update device status locally
        FileUtils.writeFile(DeviceAdminConst.DEVICESTATUSFILE,MRXMLUtils.documentToString(deviceDoc));
          
	}
	
	
}
