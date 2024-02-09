package com.barco.device.handler.V2DXP;

import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Media.base.BaseTestStartStreamRequestHandler;

public class XPTestStartStreamRequestHandler extends BaseTestStartStreamRequestHandler{

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
	public void testStartStreamRequestHandler(){
		super.testStartStreamRequestHandler();
		//assertNotNull("Test: XPTestStartStreamRequestHandler: Result is null",startStreamRequestResult);
		System.out.println("*******Inside XPTestStartStreamRequestHandler::testStartStreamRequestHandler");
		if (startStreamRequestResult != null)
			System.out.println("*******Test: XPTestStartStreamRequestHandler: Result is: \n"+startStreamRequestResult.asXML());
	}
}
