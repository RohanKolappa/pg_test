package com.barco.device.common;

/*
 * The list of all the sources of messages in the system
 */
public class ServiceException extends Exception {

	private static final long serialVersionUID = 1L;
	
	int code; 	// one of ServiceExceptionCodes
	String data;
	
	public ServiceException(int code, String data) {
		this.code = code;
		this.data = data;
	}
	
	public int getCode() {
		return code;
	}

	public String getData() {
		return data;
	}
	
	public String toString() {
		return "<BarcoError code='" + this.code + "'><Description>" + this.data + "</Description></BarcoError>";
	}

}
