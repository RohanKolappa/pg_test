package com.barco.device.handler.Base;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import com.barco.device.app.DeviceApp;
import com.barco.device.base.DeviceImpl;
import com.barco.test.utils.TestUtils;
import com.barco.vng.common.DeviceDB;

public class BaseTestSetup {
	private static String jarPath;
	
	protected static DeviceImpl device=null;
	protected static DeviceDB deviceDB;
	protected static String supportedDevice;
	protected static DeviceApp deviceApp = null;
	private static String xmlPath;
	
	
	@BeforeClass
	public static void setUpBeforeClass() {		
		TestUtils testUtils = new TestUtils();
		try{			
			//deviceDB  = new DeviceDB(getXmlPath(), getXmlPath());
			//supportedDevice= deviceDB.getDeviceType();			
			deviceApp = testUtils.getDeviceAppInstance(getJarPath());
			// TBD - Commented out since it was giving compiler errors
			// device = new DeviceImpl(deviceApp.getAgentImpl(supportedDevice),DeviceImpl.DefaultHTTPPort, DeviceImpl.DefaultHandlerPackage,deviceDB,"");
		}catch(Exception e){
			System.out.println("Exception in creating deviceApp:");
			e.printStackTrace();
			deviceApp = null;
			device = null;
		}
	}
	@Test
	public void testProviderNotNull(){		
		assertNotNull("Test: testProviderNotNull : Provider is null", deviceApp);
	}
	@Test
	public void testServiceProviderNotDefault() {		
		try {			
			assertFalse("Test: testServiceProviderNotDefault: Default Provider is initiated", deviceApp.getAgentImpl()== deviceApp.getAgentImpl());
		} catch (Exception e) {				
			e.printStackTrace();
		}
	}
	@Test
	public void testDeviceNotNull(){	
		System.out.println("Inside Test provider loaded");
		assertNotNull("Provider is null", device);
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		jarPath = null;
		device = null;
		deviceDB = null;
		deviceApp = null;
		supportedDevice = null;
	}
	public static String getXmlPath() {
		return xmlPath;
	}
	public static void setXmlPath(String xmlPath) {
		BaseTestSetup.xmlPath = xmlPath;
	}public static String getJarPath() {
		return jarPath;
	}
	public static void setJarPath(String jarPath) {
		BaseTestSetup.jarPath = jarPath;
	}
	public static void setDevicePropertiesPath(String propFilePath){
		TestUtils testUtils = new TestUtils();
		String jarPath = testUtils.getProperties(propFilePath,"jarPath" );
    	String deviceXmlPath = testUtils.getProperties(propFilePath,"deviceXml" );    	
		setJarPath(jarPath);
		setXmlPath(deviceXmlPath);
	}
}
