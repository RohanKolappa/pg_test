/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved.
 * 
 * This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement you entered into with Barco.
 */

package com.barco.nimbus.license.api;

/**
 * Top level class for failures related to licensing data.
 * 
 * @author JMRA
 */
public class LicenseException extends RuntimeException {

	/*
	 * Generated serial version UID
	 */
	private static final long serialVersionUID = -8116282410515513900L;
	
	private int code; // one of License Service Exception Codes

	
	/**
	 * Creates a new instance of this class.
	 */
	public LicenseException() {
		super();
	}

	/**
	 * Creates a new instance of this class providing given detail information
	 * on a failure.
	 * 
	 * @param aMessage
	 *            - detail information on the failure
	 */
	public LicenseException(String aMessage) {
		super(aMessage);
	}

	/**
	 * Creates a new instance of this class providing given detail information
	 * and cause of a failure.
	 * 
	 * @param aMessage
	 *            - detail information on the failure
	 * @param aCause
	 *            - root cause
	 */
	public LicenseException(String aMessage, Throwable aCause) {
		super(aMessage, aCause);
	}

	/*
	 * @param code - the application code
	 * 
	 * @param aMessage - the exception description
	 */
	public LicenseException(int code, String aMessage) {
		super(aMessage);
		this.code = code;
	}

	/*
	 * @param code - the application code
	 * 
	 * @param aMessage - the exception description
	 * 
	 * @param aCause - the original exception which caused the LicenseException
	 */
	public LicenseException(int code, String aMessage, Exception aCause) {
		super(aMessage, aCause);
		this.code = code;
	}

	public int getCode() {
		return code;
	}

	public String toString() {
		return "<BarcoError code='" + this.code + "'><Description>"
				+ this.getMessage() + "</Description></BarcoError>";
	}
}
