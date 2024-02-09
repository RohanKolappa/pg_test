package com.barco.device.app;


import java.io.File;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ServiceConfigurationError;
import java.util.ServiceLoader;

import org.dom4j.Document;

import com.barco.device.agent.AgentImpl;
import com.barco.device.base.DeviceImpl;
import com.barco.device.service.ServiceI;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;

public class DeviceApp {

	protected static String configFile = null;
	protected static String statusFile = null;	
	protected static String loginStatusFile = null;
	protected static String providerClassName = null;
	static String supportedDevice = null;


	static ServiceLoader<ServiceI> providers= null;
	String loginFileName = null;
	DeviceImpl deviceImpl = null;
	String appServerAgentJID =null; 
	 private static final String STATUS_INITIALIZING_FILE ="/var/status/DeviceApp.java.initializing";

	public DeviceApp(){
		//loads all the service providers from classpath
		providers = ServiceLoader.load(ServiceI.class);
	}

	/*
	 * adds the given url classloader  to current classpath(used for junit testing of the devicevng)
	 */
	public DeviceApp(URL[] urls){
		//loads all the service providers from given url classpath	
		this();
		ClassLoader contextLoader = Thread.currentThread().getContextClassLoader();
		ClassLoader classLoader = new URLClassLoader(urls, contextLoader);				
		providers = ServiceLoader.load(ServiceI.class, classLoader);
		Thread.currentThread().setContextClassLoader(contextLoader);
	}
/*
 * Service Access API
 * This method loads all the service providers of the type ServiceI
 * and returns the AgentImpl object of the provider which accepts the given device type (description)
 */	
	public AgentImpl getAgentImpl() throws Exception{
		
		if(providerClassName != null) {
			Class<?> c = Class.forName(providerClassName);
			ServiceI provider = (ServiceI)c.newInstance();
			return provider.getAgentImpl();
		}

		ServiceI defaultProvider = null;
		for(ServiceI provider : providers){
			try{
				if(provider.getSupportedDevices().contains("default")){
					defaultProvider = provider;
					continue;
				}
				//if(provider.getSupportedDevices().contains(description)){
				System.out.println(provider.getSupportedDevices());
				return provider.getAgentImpl();
				//}
			}
			catch(ServiceConfigurationError err){
				err.printStackTrace();
			}
		}

		System.out.println("Provider not found make sure:\n 1. our provider is in the classpath: Usage: java -classpath <classpath_containing_implementations_of_service> com.barco.device.app.DeviceApp\n" +
				"2. device xml contains valid device type\n**Using default provider**");
		return defaultProvider.getAgentImpl();

	}


	public void start(String []args) throws Exception {
		this.processCmdLine(args);
		
		//if the login file does not exists then device engine notification will trigger login
		//if the login file exists this will trigger login. If server does not reachable then login daemon will retry login to the system till succeeded
		String loginRequest = null;
		try {
			loginRequest =  FileUtils.readFile(this.loginFileName);
			Document loginRequestDoc = XMLUtils.stringToDocument(loginRequest);
			String user = XMLUtils.getValueXpath(loginRequestDoc, "//user", "");
			String resource = XMLUtils.getValueXpath(loginRequestDoc, "//resource", "");
			String password = XMLUtils.getValueXpath(loginRequestDoc, "//password", "");
			String serverName = XMLUtils.getValueXpath(loginRequestDoc, "//server", "");
			this.deviceImpl.login(serverName, user, password, resource, this.appServerAgentJID);
		} catch (Exception e) {				
			e.printStackTrace();
			loginRequest = null;
		}
		
		try {
			//remove the initializing file. DeviceAppCtl is looking for this file to delete.
			File fileObj = new File(STATUS_INITIALIZING_FILE);
			if(fileObj.exists())
				 fileObj.delete();
			
			// Create the re-announce SC file which will be deleted once the SC discovered.
			if(loginRequest == null) {
				File reannonunceSC = new File("/var/status/reannounce.sc");
				if(!reannonunceSC.exists())
					reannonunceSC.createNewFile();
			}
			
		} catch (Exception e1) {
			e1.printStackTrace();
		}
    	
		//wait in a loop
		//main thread can not die
		while(true){
			try {
				Thread.sleep(60000);
			} catch (Exception e) {
				//ignore error
			}
		}
	}

	private void printOptionString(){
		System.out.print("Options: ");
		System.out.print("login <loginfilename> " + "loginstatus <loginstatusfile> "+"config <configfilename> " + " status <statusfilename> [appserveragentjid string]");
		System.exit(1);
	}

	public void processCmdLine(String[] args) {

		if (args.length < 1) {
			printOptionString();
			return;
		}

		for (int index = 0; index < args.length; index++) {
			String command = args[index];
			
			if (command.equals("login")) {                
				loginFileName = args[++index];
			} else if (command.equals("loginstatus")) {
				loginStatusFile = args[++index];                	
			} else if (command.equals("config")) {
				configFile = args[++index];                	
			} else if (command.equals("status")) {
				statusFile = args[++index];                	
			} else if (command.equals("appserveragentjid")) {
				appServerAgentJID = args[++index];                	
			}  else if(command.equals("reload") && providers != null){				
				providers.reload();
			} else if (command.equals("provider")) {
				providerClassName = args[++index];                	
			} else {
				printOptionString();
				return;              
			}
		}
		//set default value for appserveragentjid
		if(appServerAgentJID == null) {
			appServerAgentJID = "managementserver.localhost";
		}
		
		
		if(loginFileName ==null || loginStatusFile ==null || configFile==null || statusFile == null) {
			printOptionString();
			return;  
		}
		try{
			AgentImpl agentImpl = getAgentImpl();
			agentImpl.deviceDB.setDeviceConfigLoader(configFile);
			agentImpl.deviceDB.setDeviceStatusLoader(statusFile);
			deviceImpl = new DeviceImpl(agentImpl, agentImpl.HTTPServerPort, DeviceImpl.DefaultHandlerPackage,appServerAgentJID,loginStatusFile);
		}catch(Exception exp){exp.printStackTrace();}
	}
	
	public static void main(String []args) throws Exception {
		DeviceApp deviceApp = new DeviceApp();
		deviceApp.start(args);
	}

}
