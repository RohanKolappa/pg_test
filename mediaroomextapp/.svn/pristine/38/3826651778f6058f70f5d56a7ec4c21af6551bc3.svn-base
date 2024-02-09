package com.barco.device.example;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.barco.device.base.DeviceImpl;
import com.ipvs.ext.app.cmdline.CmdLineClient;

public class ExampleDeviceImpl {
	
	public static void main(String[] args) throws Exception {
		
		// Create the client
		// BaseDeviceImpl deviceImpl = BaseDeviceImpl.createInstance();
		
		// Request Loop
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
	    String request;
	    while (true) {
	    	// prompt
    		System.out.print(">");
    		
    		// Read the request
    		request = in.readLine();
	    	if("bye".equals(request))
	    		break;
	    	
	    	// Send the request
	    	// deviceImpl.handleSystemCommand(request);
	    }
	    
	    //<login ip='10.51.48.190' user='xpi' password='Barco$123' resource='xpi' managementserver='managementserver@localhost/managementserver'/>
	    //<iq><query><ServiceLogin clientVersion='2.30.4' ready='true'><clientPrivateKey>Barco$123</clientPrivateKey></ServiceLogin></query></iq>
	    // Logout client
	    System.out.println("Done");
	}

	
}

