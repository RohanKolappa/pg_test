package com.barco.test.utils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Properties;
import java.util.Scanner;

import com.barco.device.app.DeviceApp;
import com.barco.device.handler.Base.BaseTestSetup;

public class TestUtils {
	    
	    public URL[] getClassLoaderURL(String path){	    	
	    		    	
	    	try{
	    		File newFile = new File(path);
	    		URL[] urls = {newFile.toURI().toURL()};    	
	    		return urls;
	    	}
	    	catch(MalformedURLException e){
	    		e.printStackTrace();
	    		return null;
	    	} 	
	    		

	    }
	    public DeviceApp getDeviceAppInstance(String jarFilePath) throws MalformedURLException{
	    	
	    	DeviceApp deviceApp = new DeviceApp(getClassLoaderURL(jarFilePath));
	    	return deviceApp;	

	    }
	   
	    public void setUpBeforeClass(String propertiesFilePath){    	
		
	    	String jarPath = getProperties(propertiesFilePath,"jarPath" );
	    	String deviceXmlPath = getProperties(propertiesFilePath,"deviceXml" );
	    
			BaseTestSetup.setJarPath(jarPath);
			BaseTestSetup.setXmlPath(deviceXmlPath);
	    	
	    }
	    /*
	     * Generic method to get a value of property from device properties file
	     * Can be used when different request xml is to be used instead of the default ones 
	     */
	    public String getProperties(String propFileName, String property){
	    	Properties prop = new Properties();	    	
	    	String value = null;
	    	InputStream in = TestUtils.class.getResourceAsStream(propFileName);
	    	try {
				prop.load(in);			
				value = prop.getProperty(property);			
			} catch (IOException e) {				
				e.printStackTrace();
			}
	    	finally{
	    	try {
				in.close();
			} catch (IOException e) {				
				e.printStackTrace();
			}
	    	}
	    	return value;
	    }
	    public String loadXmlFromResource(String path) throws Exception {
	    	URL url = this.getClass().getResource(path);
	    	if(url == null) {
	    		System.out.println("URL is null for path: "+path);   		
	    	}
	    	@SuppressWarnings("resource")
			String xml = new Scanner(url.openStream(), "UTF-8").useDelimiter("\\A").next();
	    	return xml;
	    }
}
