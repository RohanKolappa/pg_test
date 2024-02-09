package com.barco.device.handler.V2DRX;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStartStreamRequestHandler;

public class V2DRXTestStartStreamRequestHandler extends BaseTestStartStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){	
		String propertiesFilePath = "/resources/services/v2drx-test.properties";		
		BaseTestStartStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);
		BaseTestStartStreamRequestHandler.setUpBeforeClass();
	}

/*	@AfterClass
	public static void tearDownAfterClass(){
	}
*/

}
