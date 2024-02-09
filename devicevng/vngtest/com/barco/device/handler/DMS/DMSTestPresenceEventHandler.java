/*package com.barco.device.handler.DMS;

import static org.junit.Assert.*;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.handler.Base.BaseTestSetup;
import com.barco.device.handler.Media.base.BaseTestPresenceEventHandler;
import com.barco.test.utils.TestUtils;

public class DMSTestPresenceEventHandler extends BaseTestPresenceEventHandler{

	@BeforeClass
	public static void setUpBeforeClass(){		
		String propertiesFilePath = "/resources/services/dms-test.properties";		
		BaseTestPresenceEventHandler.setDevicePropertiesPath(propertiesFilePath);
	
		BaseTestPresenceEventHandler.setUpBeforeClass();
	}

	@AfterClass
	public static void tearDownAfterClass(){
	}

	@Test
	public void testPresenceEventHandlerReady() {
		super.testPresenceEventHandlerReady();
		assertNotNull("Test: dmsTestPresenceEventHandler result null",super.presenceEventHandlerReady); //async no assertion
		
	}

}
*/