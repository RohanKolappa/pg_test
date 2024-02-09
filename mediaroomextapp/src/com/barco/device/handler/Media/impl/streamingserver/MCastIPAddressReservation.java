package com.barco.device.handler.Media.impl.streamingserver;

import java.net.InetAddress;
import java.util.Hashtable;
import java.util.Iterator;


public class MCastIPAddressReservation extends Reservation{

	public static final String DEFAULT_MCAST_IPADDRESS = "226.1.1.1";
	Hashtable<String, String> outputMCastIPAddress = new Hashtable<String, String>();
    Hashtable<String, String> inputMCastIPAddress = new Hashtable<String, String>();
    
    
    public String reserve(String outputInstanceID, String inputInstanceID) throws Exception {
		
		if (isUseDefault()) {
			return DEFAULT_MCAST_IPADDRESS;
		}
        String ipaddress = inputMCastIPAddress.get(inputInstanceID);
        if (ipaddress == null) {
            ipaddress = NetworkResource.intToIPAddress(this.reserveResource());
            inputMCastIPAddress.put(inputInstanceID, ipaddress);
            outputMCastIPAddress.put(outputInstanceID, ipaddress);
        }
        return ipaddress;
	}
	
    public void release(String outputInstanceID) {
    	try {
    		if (isUseDefault() || !outputMCastIPAddress.containsKey(outputInstanceID)) {
    			return;
    		}
    		String ipAddress = outputMCastIPAddress.get(outputInstanceID);
    		outputMCastIPAddress.remove(outputInstanceID);        
    		if (!outputMCastIPAddress.containsValue(ipAddress)) {
    			String inputInstanceID = getInputInstanceIDFromMulticastCache(ipAddress);        
    			inputMCastIPAddress.remove(inputInstanceID);
    			int ipaddressInt = NetworkResource.ipAddresstoInt(InetAddress.getByName(ipAddress));
    			this.releaseResource(ipaddressInt);
    		}
    	}catch(Exception e) {
    		e.printStackTrace();
    	}

    }
    //Get the InputInstance from input MCast Cache based on given IP Address.
    public String getInputInstanceIDFromMulticastCache(String IPAddress) {
        if (inputMCastIPAddress.isEmpty()) {
            return null;
        }
       Iterator<String> usedMuticastIPAddressIterator = inputMCastIPAddress.keySet().iterator();
        String cacheIPAddress = null;
        String key = null;

        while (usedMuticastIPAddressIterator.hasNext()) {
            key = usedMuticastIPAddressIterator.next();
            cacheIPAddress = inputMCastIPAddress.get(key);
            if (cacheIPAddress.equals(IPAddress)) {
                return key;
            }
        }

        return null;
    }


    
}
