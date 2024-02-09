package com.ipvs.mediaroomservice.streamingserver;

public class UDPReservation extends Reservation{

	public String reserve() throws Exception {
		return String.valueOf(this.reserveResource());
	}
	
	public void release(String resourceNumber) {
		this.releaseResource(Integer.valueOf(resourceNumber));
	}
	
}
