package com.barco.device.handler.Media.impl.streamingserver;

import java.util.Hashtable;
import java.util.StringTokenizer;

public class V2DReservation extends Reservation{
	
	private Hashtable<String, String> portsInstanceIDMapping = new Hashtable<String, String>();
	private static final String DEFAULT_PORT = "6060"; 
	
	public String reserve(String instanceId) throws Exception {
		
		if (isUseDefault()) {
			return DEFAULT_PORT;
		}
		String reservedPort = (String.valueOf(this.reserveResource()));
		StringBuffer portList = new StringBuffer();   
        if (portsInstanceIDMapping.containsKey(instanceId)) {
        	portList = new StringBuffer(portsInstanceIDMapping.get(instanceId));
        	portsInstanceIDMapping.remove(instanceId);
        	if (portList != null) {
        		portList.append(" && ");
        	}
        }
        portList.append(reservedPort);
        portsInstanceIDMapping.put(instanceId, portList.toString());
        return reservedPort;
	}
	
	public void release(String instanceId) {
		if(isUseDefault()) {
			return;
		}
		int portInt = 0;
		StringTokenizer usedPort = null;
		if (portsInstanceIDMapping.containsKey(instanceId)) {
			usedPort = new StringTokenizer(portsInstanceIDMapping.get(instanceId), "&&");
			while ((usedPort != null) && usedPort.hasMoreTokens()) {
				portInt = Integer.parseInt(usedPort.nextToken().trim());
				this.releaseResource(Integer.valueOf(portInt));
			}
			portsInstanceIDMapping.remove(instanceId);
		}
		
	}

}
