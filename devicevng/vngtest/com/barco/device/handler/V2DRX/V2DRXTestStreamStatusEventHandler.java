package com.barco.device.handler.V2DRX;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStreamStatusEventHandler;

public class V2DRXTestStreamStatusEventHandler extends BaseTestStreamStatusEventHandler{

	@BeforeClass
	public static void setUpBeforeClass(){		
		String propertiesFilePath = "/resources/services/v2drx-test.properties";		
		BaseTestStreamStatusEventHandler.setDevicePropertiesPath(propertiesFilePath);
		BaseTestStreamStatusEventHandler.setUpBeforeClass();
	}

/*	@AfterClass
	public static void tearDownAfterClass(){
	}
*/

}
