package com.barco.device.vng.Media;

import java.io.File;
import java.io.FileWriter;
import java.util.StringTokenizer;

import javax.jmdns.JmDNS;
import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;
import javax.jmdns.ServiceListener;

public class ZCListener implements ServiceListener {

	JmDNS mdnsService;
	public static final String HTTP_SERVICE_TYPE = "_http._tcp.local.";
	String path;
	public static final String BARCO = "Barco";
	
	public ZCListener(String path) throws Exception{
		this.path = path;
		mdnsService = JmDNS.create();
		mdnsService.addServiceListener(HTTP_SERVICE_TYPE, this);
	}
	
	public void serviceAdded(ServiceEvent serviceEvent) {
		mdnsService.requestServiceInfo(serviceEvent.getType(), serviceEvent.getName(), 1);
	}
	
	public void serviceRemoved(ServiceEvent serviceEvent) {
		// System.out.println("ZCListener:serviceRemoved event=" + String.valueOf(serviceEvent));
	}
	
	@Override
	public void serviceResolved(ServiceEvent serviceEvent) {
		String serviceUrl = serviceEvent.getInfo().getURL();
		String name = serviceEvent.getName();
		if(name.indexOf(BARCO) == -1)
			return;
		
		// To fix issue with 220 which does not return IP address on first event
		// This will cause a second serviceEvent which has the right IP address
		if(name.indexOf("220") != -1) {
			ServiceInfo[] list = serviceEvent.getDNS().list(HTTP_SERVICE_TYPE); 
		}		
		// System.out.println("name=" + name + " url=" + serviceUrl);
		
		StringTokenizer st = new StringTokenizer(name, " ");
		st.nextToken();
		name = st.nextToken();
		name = name.toLowerCase();
		try {
			File file = new File(path, name); // Create the file
			FileWriter writer = new FileWriter(file);
			writer.write(serviceUrl);
			writer.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String[] args) throws Exception {
		ZCListener zcListener = new ZCListener(args[0]);
	    int b;
        while ((b = System.in.read()) != -1 && (char) b != 'q') {
              /* Stub */
        }	
	}

}
