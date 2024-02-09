package com.barco.device.handler.DeviceAdmin.junit;

import com.barco.device.handler.DeviceAdmin.impl.deviceserver.DeviceServerImpl;
import com.barco.utils.mr.MRLog;

public class TestDeviceServer {

	

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try{
			MRLog log = MRLog.getInstance(MRLog.CONSOLE);
			String configFile = "/etc/config/Device.xml";
			//TODO Using SPI Architecture invoke the deviceServer.
			DeviceServerImpl deviceServer = new DeviceServerImpl(log,configFile);
			//TODO check deviceserver started or not
			System.out.println("device Server Started");	
			deviceServer.resetServer();
			System.out.println("reset Server");
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

}
