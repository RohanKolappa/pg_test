package com.ipvs.app;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class IPVSClientApp implements IPVSMessageHandler {

	public IPVSClient ipClient;
		
	public void sendPacket(String requestXML) throws Exception{
		ipClient.sendPacket(requestXML);
	}

	public void login(String loginXML){
		IPVSClient client = null;
		try {
			//client = IPVSClient.login(loginXML, this);
		} catch (Exception e1) {			
			e1.printStackTrace();
		}		
		
		ipClient = client;
	}

	public void logout(){
		IPVSClient.logout(ipClient);
	}


	public static void main(String args[]) throws Exception{
		IPVSClientApp clientApp = new IPVSClientApp();
		
		BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
		
		System.out.println("Enter Login XML");		
		String loginXML = input.readLine();
		clientApp.login(loginXML);
		
		System.out.println("Enter Request XML");
		String requestXML = input.readLine();
		clientApp.sendPacket(requestXML);
		
		while(true);
		
	}

	
	public void handleMessage(String responseXML) {
		System.out.println(responseXML);		
		
	}




}
