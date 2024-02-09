package com.barco.device.vng.Media.streamingserver;

/**
 * Acquire/release a block of ports.
 */
public class UDPReservation extends Reservation{

	public String reserve() throws Exception {
		return String.valueOf(this.reserveResource());
	}
	
	public void release(String resourceNumber) {
		this.releaseResource(Integer.valueOf(resourceNumber));
	}
	
}
