/*package com.barco.device.handler.V2DXP;

import static org.junit.Assert.*;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Base.BaseTestSetup;
import com.barco.device.handler.Media.base.BaseTestTearDownStreamRequestHandler;
import com.barco.test.utils.TestUtils;

public class XPTestTeardownStreamRequestHandler extends BaseTestTearDownStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){		
		
		String propertiesFilePath = "/resources/services/v2dxp-test.properties";		
		BaseTestTearDownStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);	
		BaseTestTearDownStreamRequestHandler.setUpBeforeClass();
	}

	@AfterClass
	public static void tearDownAfterClass(){
	}
	@Test
	public void testTeardownStreamRequestHandler() throws Exception{
		super.testTeardownStreamRequestHandler();
		assertNotNull("Test: XPTestStartStreamRequestHandler: Result is null",startStreamRequestResult);
		if (startStreamRequestResult != null)
			System.out.println("Test: XPTestStartStreamRequestHandler: Result is: "+startStreamRequestResult.asXML());
	}
}
*/