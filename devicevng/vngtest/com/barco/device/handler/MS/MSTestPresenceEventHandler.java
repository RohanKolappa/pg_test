/*package com.barco.device.handler.MS;

import static org.junit.Assert.*;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Base.BaseTestSetup;
import com.barco.device.handler.Media.base.BaseTestPresenceEventHandler;
import com.barco.test.utils.TestUtils;

public class MSTestPresenceEventHandler extends BaseTestPresenceEventHandler{

	@BeforeClass
	public static void setUpBeforeClass(){
		
		BaseTestSetup.setJarPath("D:\\work\\devicevng_separate_source\\dist\\ms.jar");
		BaseTestSetup.setXmlPath("D:\\work\\model\\objectXml\\MS-25-Device.xml");
			
		String propertiesFilePath = "/resources/services/ms-test.properties";		
		BaseTestPresenceEventHandler.setDevicePropertiesPath(propertiesFilePath);
		BaseTestPresenceEventHandler.setUpBeforeClass();
	}

	@AfterClass
	public static void tearDownAfterClass(){
	}

	@Test
	public void testPresenceEventHandlerReady() {
		super.testPresenceEventHandlerReady();
		
	}

}
*/