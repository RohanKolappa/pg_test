package com.barco.device.handler.V2DRX;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStopStreamRequestHandler;

public class V2DRXTestStopStreamRequestHandler extends BaseTestStopStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
				
		String propertiesFilePath = "/resources/services/v2drx-test.properties";		
		BaseTestStopStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);
		BaseTestStopStreamRequestHandler.setUpBeforeClass();
	}

/*	@AfterClass
	public static void tearDownAfterClass(){
	}
*/

}
