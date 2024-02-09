package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class GetDeviceRequestHandler implements MessageHandlerI{
	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		
		String state = "Registered";
		if(device.getMyJID() == null || device.getMyJID().isEmpty()){
			state = "UnRegistered";
		}
		Document configDocument = device.getDeviceAdminAgentImpl().getDeviceConfig("");
		String deviceTitle = XMLUtils.getValueXpath(configDocument, "//XMPPAgentServiceConfig/DeviceName", "");
		String password = XMLUtils.getValueXpath(configDocument, "//XMPPAgentServiceConfig/DevicePassword", "");
	    String responseXML ="<Device title='"+deviceTitle+"' type='"+device.getDeviceDB().getDeviceType()+"' state='"+state+"' password='"+password+"' ownerUserJID='"+device.getMyJID()+"'>" +
	    		"<Groups />" +
	    		"<DevicePrivateData /><TemplateNID />" +
	    		"" +
	    		"<PortList />" + 
	    		configDocument.getRootElement().asXML()+
	    		DocumentHelper.createXPath("//DeviceStatus").selectSingleNode(device.getDeviceAdminAgentImpl().getStatus("")).asXML()+
	    		"<DeviceList />" +
	    		"</Device>";
	    message.setResult(DocumentHelper.parseText(responseXML));
	}

}
