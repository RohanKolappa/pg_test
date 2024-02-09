package com.barco.device.handler.MS;

import org.junit.BeforeClass;

import com.barco.device.handler.Media.base.BaseTestStreamStatusEventHandler;

public class MSTestStreamStatusEventHandler extends BaseTestStreamStatusEventHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
		/*
		BaseTestSetup.setJarPath("D:\\work\\devicevng_separate_source\\dist\\ms.jar");
		BaseTestSetup.setXmlPath("D:\\work\\model\\objectXml\\MS-25-Device.xml");
	*/		
		String propertiesFilePath = "/resources/services/ms-test.properties";		
		BaseTestStreamStatusEventHandler.setDevicePropertiesPath(propertiesFilePath);
		BaseTestStreamStatusEventHandler.setUpBeforeClass();
	}

	/*@AfterClass
	public static void tearDownAfterClass(){
	}*/

	

}
