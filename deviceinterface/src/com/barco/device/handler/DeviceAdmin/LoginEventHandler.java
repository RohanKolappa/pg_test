package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;

import com.barco.device.base.DeviceI;
import com.barco.device.base.LoginInfo.LoginInfoEvents;
import com.barco.device.common.Message;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;

public class LoginEventHandler extends BaseEventHandler {


	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document deviceConfigDoc = device.getDeviceDB().getDeviceConfig();		
		String xmppEnabled = XMLUtils.getValueXpath(deviceConfigDoc, "//XMPPAgentServiceConfig/Enable", "");	
		//set last event in login info class.
		if(xmppEnabled.equals("false")) {
			device.logout();
			device.getLoginInfo().setLastEvent(LoginInfoEvents.ConfigChanged);
			//add reanounce file
			FileUtils.writeFile("/var/status/reannounce.sc", "");
		} else {
			String xmppServerIp = XMLUtils.getValueXpath(deviceConfigDoc, "//XMPPAgentServiceConfig/XMPPServer", "");
			String deviceName = XMLUtils.getValueXpath(deviceConfigDoc, "//XMPPAgentServiceConfig/DeviceName", "");
			String devicePassword = XMLUtils.getValueXpath(deviceConfigDoc, "//XMPPAgentServiceConfig/DevicePassword", "");
			device.logout();
			device.getLoginInfo().setLastEvent(LoginInfoEvents.ConfigChanged);
			try {
				device.login(xmppServerIp, deviceName, devicePassword, deviceName, device.getLoginInfo().getAppserverJID());
			} catch (Exception e) {
				//if this happened we have a problem //TBD check how can we slove this problem
			}
			
		}
		
		
	}

	

	
}
