package com.barco.device.handler.V2DTX;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStopStreamRequestHandler;

public class V2DTXTestStopStreamRequestHandler extends BaseTestStopStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
		
		String propertiesFilePath = "/resources/services/v2dtx-test.properties";		
		BaseTestStopStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);	
		BaseTestStopStreamRequestHandler.setUpBeforeClass();
	}

	/*@AfterClass
	public static void tearDownAfterClass(){
	}*/

	

}
