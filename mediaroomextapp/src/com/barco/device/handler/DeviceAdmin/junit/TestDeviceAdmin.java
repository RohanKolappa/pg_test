package com.barco.device.handler.DeviceAdmin.junit;

//
//import static org.junit.Assert.fail;
//
//import org.junit.Test;

import com.barco.device.base.DeviceI;
import com.barco.device.base.DeviceImpl;
import com.barco.device.handler.DeviceAdmin.impl.DeviceAdminAgentImpl;
import com.barco.device.handler.DeviceAdmin.impl.deviceserver.DeviceServerImpl;
import com.barco.utils.mr.MRLog;

public class TestDeviceAdmin {

	//	@Test 
	//	public void test() {
	//		try {
	//			String deviceNID = "deviceNID";		
	//			MRLog log = MRLog.getInstance(MRLog.CONSOLE);
	//			String configFile = "/etc/config/Device.xml";
	//			//TODO Using SPI Architecture invoke the deviceServer.
	//			DeviceServerImpl deviceServer = new DeviceServerImpl(log,configFile);
	//			DeviceAdminAgentImpl deviceAdminAgent  = new DeviceAdminAgentImpl(log, configFile,deviceServer);
	//
	//			BaseDeviceI device = new BaseDeviceImpl(deviceAdminAgent, null, null, null, 0, deviceNID);
	//			System.out.println("device:="+device);			
	//
	//		} catch (Exception e) {
	//			fail("Failed Device Admin Test"+e.getMessage());
	//			e.printStackTrace();
	//		}
	//
	//	}


	public static void main(String[] args) {
		try{
			String deviceNID = "deviceNID";		
			MRLog log = MRLog.getInstance(MRLog.CONSOLE);
			String configFile = "/etc/config/Device.xml";
			//TODO Using SPI Architecture invoke the deviceServer.
			DeviceServerImpl deviceServer = new DeviceServerImpl(log,configFile);
			DeviceAdminAgentImpl deviceAdminAgent  = new DeviceAdminAgentImpl(log, configFile,deviceServer);

			DeviceI device = new DeviceImpl(deviceAdminAgent, null, null, null, 0, deviceNID);
			System.out.println("device:="+device);	
			deviceServer.resetServer();
			System.out.println("reset Server");

		} catch (Exception e) {
			e.printStackTrace();
		}

	}
}
