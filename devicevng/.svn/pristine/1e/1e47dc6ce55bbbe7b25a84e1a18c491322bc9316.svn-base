package com.barco.device.handler.V2DTX;

import static org.junit.Assert.assertNotNull;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStartStreamRequestHandler;

public class V2DTXTestStartStreamRequestHandler extends BaseTestStartStreamRequestHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
		
		String propertiesFilePath = "/resources/services/v2dtx-test.properties";		
		BaseTestStartStreamRequestHandler.setDevicePropertiesPath(propertiesFilePath);
	
		BaseTestStartStreamRequestHandler.setUpBeforeClass();
	}

	/*@AfterClass
	public static void tearDownAfterClass(){}*/
	
	public void testStartStreamRequestHandler(){
		super.testStartStreamRequestHandler();
		assertNotNull("Request is null",startStreamRequestResult);
		if(startStreamRequestResult != null){
			System.out.println(startStreamRequestResult.asXML());
		}
	}
}
