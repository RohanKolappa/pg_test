package com.barco.device.handler.V2DXP;

import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Media.base.BaseTestStartStreamRequestHandler;
import com.barco.device.handler.Media.base.BaseTestStopStreamRequestHandler;

public class XPTestStopStreamRequestHandler extends BaseTestStopStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
		
		String propertiesFilePath = "/resources/services/v2dxp-test.properties";		
		BaseTestStartStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);	
		BaseTestStartStreamRequestHandler.setUpBeforeClass();
	}

	/*@AfterClass
	public static void tearDownAfterClass(){
	}*/
	@Test
	public void testStopStreamRequestHanlder() throws Exception{
		super.testStopStreamRequestHanlder();
		/*assertNotNull("Test: XPTestStartStreamRequestHandler: Result is null",startStreamRequestResult);
		if (startStreamRequestResult != null)
			System.out.println("Test: XPTestStartStreamRequestHandler: Result is: "+startStreamRequestResult.asXML());*/
	}
}
