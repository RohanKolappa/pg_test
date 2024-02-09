package com.barco.nimbus.api.common;

/*
 * This is a common exception class which maintains all the data related to the exception
 */
public class SCException extends Exception {

	private static final long serialVersionUID = 1L;
	
	int code; 	// one of ServiceExceptionCodes
	String data;
	Exception originException;
	String originData;
	
	/*
	 * @param code - the application code
	 * @param description - the application description
	 */
	public SCException(int code, String data) {
		this.code = code;
		this.data = data;
		this.originException = null;
		this.originData = null;
	}

	/*
	 * @param code - the application code
	 * @param description - the application description
	 * @param originException - the original exception which caused the SCException
	 * @param originData - the original data associated with the exception
	 */
	public SCException(int code, String data, Exception originException, String originData) {
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
