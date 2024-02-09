package com.barco.device.vng.test;

import com.barco.device.app.DeviceApp;

public class DeviceTest extends DeviceApp {	
	
	public static void main(String []args) throws Exception {
		DeviceTest deviceTest = new DeviceTest();	
		deviceTest.processCmdLine(args);
	}
}
