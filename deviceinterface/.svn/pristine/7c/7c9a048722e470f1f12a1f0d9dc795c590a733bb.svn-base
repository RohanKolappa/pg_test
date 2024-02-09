package com.barco.device.handler.DeviceAdmin;

import com.barco.device.base.DeviceI;
import com.barco.device.base.LoginInfo;
import com.barco.device.base.LoginInfo.LoginInfoEvents;
import com.barco.device.common.Message;

public class SCShutdownEventHandler extends BaseEventHandler {


	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		device.logout();
		device.getLoginInfo().setLastEvent(LoginInfoEvents.Shutdown);
		try {
			LoginInfo loginInfo = device.getLoginInfo();
			device.login(loginInfo.getIpAddress(), loginInfo.getUserName(), loginInfo.getPassword(), loginInfo.getResource(), loginInfo.getAppserverJID());
		} catch (Exception e) {
			//if this happened we have a problem //TBD check how can we slove this problem
		}
	}

}
