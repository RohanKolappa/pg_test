package com.barco.device.vng.Media.streamingserver;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import com.barco.device.common.ServiceException;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

/**
 * Base class for managing a pool of blocks. A block is a sequence of
 * consecutive numbers - say addresses.
 * 
 * All blocks have the same size, and contain addresses within a particular
 * range.
 */
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
    
    /**
     * @param rangeFromStr
     * @param rangeToStr
     * @param incrementStr the size of blocks being reserved 
     */
    void init(String rangeFromStr, String rangeToStr, String incrementStr) {
    	
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
     * @return a free block is acquired, the base address is returned
     */
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
    		throw new ServiceException(MediaRoomServiceExceptionCodes.NOAVAILABLEPORTS, "There are no available ports");
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
	
	/**
	 * Releases the block starting at that address.
	 */
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
