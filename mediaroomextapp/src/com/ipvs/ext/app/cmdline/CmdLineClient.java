package com.ipvs.ext.app.cmdline; // *** change this to your package name

import java.io.BufferedReader;
import java.io.InputStreamReader;

import com.ipvs.ext.xmppclient.IpvsXmppLog;
import com.ipvs.ext.xmppclient.IpvsXmppLoggerI;
import com.ipvs.ext.xmppclient.IpvsXmppXMLClient;

public class CmdLineClient extends IpvsXmppXMLClient implements IpvsXmppLoggerI {

	/*
	 * Whenever there is a response or a message this method is called
	 */
	public void handleXML(String xml) {
		System.out.println(xml);
	}
	
	public static void main(String[] args) throws Exception {
		
		// Create the client
		CmdLineClient client = new CmdLineClient();
		
		// Setup the logging
		client.setLogLevel(IpvsXmppLoggerI.DEBUG);
		client.setLogger(client);

		// Sample commands that you can send on the cmdLine
		// <login ip='10.51.48.190' user='superuser-default' password='superuser-default' resource='mypc'/>
		// <iq><query><ServiceLogin clientVersion=''><clientPrivateKey/></ServiceLogin></query></iq>
		// <iq><query><GetSession/></query></iq>
		// <iq><query><GetPort/></query></iq>
		// <logout/>
		// <login ip='10.51.48.190' user='xpi' password='Barco$123' resource='xpi'/>
		
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
	    	client.handleRequest(request);
	    }
	    
	    // Logout client
	    client.logout();
	    System.out.println("Done");
	}

	@Override
	public void log(String s, int level, int ref) {
		System.out.println("CmdLineClient:log level=" + level + " ref=" + ref + " txt=" + s);
		// TODO Auto-generated method stub
		
	}

}
