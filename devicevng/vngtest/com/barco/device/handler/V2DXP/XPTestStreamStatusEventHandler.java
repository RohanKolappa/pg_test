package com.barco.device.handler.V2DXP;

import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Media.base.BaseTestStreamStatusEventHandler;

public class XPTestStreamStatusEventHandler extends BaseTestStreamStatusEventHandler{

	@BeforeClass
	public static void setUpBeforeClass(){		
		String propertiesFilePath = "/resources/services/v2dxp-test.properties";		
		BaseTestStreamStatusEventHandler.setDevicePropertiesPath(propertiesFilePath);	
		BaseTestStreamStatusEventHandler.setUpBeforeClass();
	}

	/*@AfterClass
	public static void tearDownAfterClass(){
	}*/
	@Test
	public void testStreamStatusEventHandler(){
		super.testStreamStatusEventHandler();
		/*assertNotNull("Test: XPTestStartStreamRequestHandler: Result is null",startStreamRequestResult);
		if (startStreamRequestResult != null)
			System.out.println("Test: XPTestStartStreamRequestHandler: Result is: "+startStreamRequestResult.asXML());*/
	}
}
