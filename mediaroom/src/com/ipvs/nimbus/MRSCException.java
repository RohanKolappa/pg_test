package com.ipvs.nimbus;

/*
 * The list of all the sources of messages in the system
 */
public class MRSCException extends Exception {

	private static final long serialVersionUID = 1L;
	
	int code; 	// one of ServiceExceptionCodes
	String data;
	Exception originException;
	String originData;
	
	public MRSCException(int code, String data, Exception originException, String originData) {
		this.code = code;
		this.data = data;
		this.originException = originException;
		this.originData = originData;
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

	public Exception getOriginException() {
		return originException;
	}

	public String getOriginData() {
		return originData;
	}

}
