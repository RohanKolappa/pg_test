package com.ipvs.mediaroomservice.streamingserver;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import com.ipvs.common.MRException;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;

public class Reservation {

	int rangeFrom = 0;
    int rangeTo = 0;
    int increment = 0;
    int numberAllocated = 0;
    boolean useDefault = false;
    List<Integer> availableList = new ArrayList<Integer>();
    private Hashtable<Integer,Integer> usedList = new Hashtable<Integer,Integer>();
    
    public boolean inRange(int value) {
    	return (value >= rangeFrom) && (value <= rangeTo);
    }
    
    public void init(String rangeFromStr, String rangeToStr, String incrementStr) {
    	
    	availableList = new ArrayList<Integer>();
    	numberAllocated = 0;
    	if ((rangeFromStr != null) && (rangeFromStr.trim().length() != 0)) {
    		try {
    			if(rangeFromStr.indexOf(".") < 0 ) {
    				this.rangeFrom = Integer.parseInt(rangeFromStr.trim());
    			} else {
    				rangeFrom = NetworkResource.ipAddresstoInt(InetAddress.getByName(rangeFromStr));
    			}	
    		}catch(Exception e) {
    			e.printStackTrace();
    			useDefault = true;
    		}
        }
    	
    	if ((rangeToStr != null) && (rangeToStr.trim().length() != 0)) {
    		try {
    			if(rangeToStr.indexOf(".") < 0 ) {
    				this.rangeTo = Integer.parseInt(rangeToStr.trim());
    			} else {
    				rangeTo = NetworkResource.ipAddresstoInt(InetAddress.getByName(rangeToStr));
    			}
    		}catch(Exception e) {
    			e.printStackTrace();
    			useDefault = true;
    		}
        }
    	
    	if ((incrementStr != null) && (incrementStr.trim().length() != 0)) {
    		try {
    			this.increment = Integer.parseInt(incrementStr.trim());
    		}catch(Exception e) {
    			e.printStackTrace();
    			this.increment = 1;
    		}
        }
    }
    
    /**
     * Update used list information for reliability if SC reboots.
     * This method only called when SC reboots and found some used informations
     * Streams GC will take care to remove resources. 
     * @param number
     */
    protected void updateReservationForReliability(int number) {
    	  this.usedList.put(number,number);
    	  if(this.availableList.contains(number)) {
    		  this.availableList.remove(number);
    	  }
    }
    
    protected int reserveResource () throws Exception {
    	
    	if(!this.availableList.isEmpty()) {
			int number = this.availableList.get(0);
			this.availableList.remove(0);
			populateUsedPortList(number);
			return number;
		}
		int nextAvailableNumber = 0;
    	do {
    		nextAvailableNumber = getNextNumber();
    	} while(isNumberUsed(nextAvailableNumber));
    	populateUsedPortList(nextAvailableNumber);
    	return nextAvailableNumber;
    }
    
    private int getNextNumber() throws Exception {
    	int port = rangeFrom + numberAllocated;
    	numberAllocated = numberAllocated + increment;
    	if(!isNumberInRange(port) || (rangeFrom == 0 && rangeTo == 0 && increment == 0)) {
    		throw new MRException(MediaRoomServiceExceptionCodes.NOAVAILABLEPORTS, "There are no available ports");
    	}
    	return port;
    }
    
    public void populateUsedPortList(int number) {
    	for(int i=0;i<increment;i++) {
			this.usedList.put(number+i, number + i);
		}
    }

    public boolean isNumberUsed(int number) {
    	for(int i=0;i<increment;i++) {
			if(this.usedList.containsKey(number+i)) {
				return true;
			}
		}
    	return false;
    }
    
	private boolean isNumberInRange(int port) {
		if (port >= rangeFrom && port + increment -1 <= rangeTo){
			return true;
		}
		return false;
	}
	
    protected void releaseResource (int number) {
    	if((!availableList.contains(number)) && isNumberInRange(number)) {
			this.availableList.add(number);
		}	
		for(int i=0;i<increment;i++) {
			usedList.remove(number+i);
		}
    	
    }


	public boolean isUseDefault() {
		return useDefault;
	}


	public void setUseDefault(boolean useDefault) {
		this.useDefault = useDefault;
	}
    
    
}
