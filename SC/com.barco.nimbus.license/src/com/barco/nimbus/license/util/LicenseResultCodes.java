/**
 * Copyright (c) 2007 Barco n.v. All Rights Reserved.
 * 
 * This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement you entered into with Barco.
 */

package com.barco.nimbus.license.util;


/**
 * Enumeration values for return codes of BLM. These is a subset of defined values.
 * 
 * @author JMRA
 */
public enum LicenseResultCodes 
{
	/**
	 * An error state for Unknown errors
	 */
	UNKNOWN("Unknown", -1, "Unknown error"),

	/**
	 * State of License success
	 */
	SUCCESS("BLM_ERR_SUCCESS", 0, "License check successfull"),

	/**
	 * An error state if BLM Cannot open license file
	 */
	FILE_OPEN_FAILED("BLM_ERR_LFILE", 1, "Cannot open license file"),
	
	/**
	 * An error state if BLM Cannot open Execution file
	 */
	EXEC_OPEN_FAILED("BLM_ERR_XFILE", 2, "Cannot open Exec file"),
	
	/**
	 * An error state if No license string found
	 */
	LICENSE_STRING_MISSING("BLM_ERR_NOSTR", 3, "No license string found"),
	
	/**
	 * An error state if String/key invalid (KEY)
	 */
	ERR_BADKEY("BLM_ERR_BADKEY", 4, "String/key invalid (KEY)"),
	
	/**
	 * An error state if Process not found in active process file (PROCESS) 
	 */
	ERR_PROC("BLM_ERR_PROC", 5, "Process not found in active process file (PROCESS)"),

	/**
	 * An error state for Process / product mechanism
	 */
	ERR_MATCH("BLM_ERR_MATCH", 6, "Process / product mechanism"),
	
	/**
	 * An error state if Hostname check failed (HOSTNAME)
	 */
	ERR_HOST("BLM_ERR_HOST", 7, "Hostname check failed (HOSTNAME)"),

	/**
	 * An error state if IP Address check failed (HOSTIP) 
	 */
	ERR_IP("BLM_ERR_IP", 8, "IP Address check failed (HOSTIP)"),
	
	/**
	 * An error state if MAC Address check failed (HOSTMAC) 
	 */
	ERR_MAC("BLM_ERR_MAC", 9, "MAC Address check failed (HOSTMAC)"),
	
	/**
	 * An error state if Quantity check failed (QTY) 
	 */
	ERR_QTY("BLM_ERR_QTY", 10, "Quantity check failed (QTY)"),
	
	/**
	 * An error state if Custom field check failed 
	 */
	ERR_CUST("BLM_ERR_CUST", 11, "Custom field check failed"),
	
	/**
	 * State represents License is expiry
	 */
	LICENSE_EXPIRED("BLM_ERR_DATE", 12, "License is expired"),

	/**
	 * An error state if Requested feature is not enabled 
	 */
	FEATURE_NOT_ENABLED("BLM_ERR_FEAT", 13, "Requested feature is not enabled"),
	
	/**
	 * An error state if Error while connecting to Server 
	 */
	ERROR_CONNECT_SERVER("BLM_ERR_SERV", 14, "Error connecting to Server"),
	
	/**
	 * An error state for Socket I/O problems
	 */
	ERR_SOCKET("BLM_ERR_SOCK", 15, "Socket I/O problems"),
	
	/**
	 * An error state for Site IP check failed
	 */
	ERR_IPCHECK_SITE("BLM_ERR_SITE", 16, "Site IP check failed"),
	
	/**
	 * An error state if Error connecting to BLM Execution 
	 */
	ERR_CONNECT_BLMEXEC("BLM_ERR_EXEC", 17, "Error connecting to blmexec"),
	
	/**
	 * An error state for Bad error code from BLM Execution
	 */
	ERR_UNKNOWN("BLM_ERR_UNKNOWN", 18, "Bad error code from blmexec"),

	/**
	 * An error state for Memory allocation error
	 */
	ERR_MALLOC("BLM_ERR_MALLOC", 19, "Memory allocation error"),
	
	/**
	 * An error state if Remote license check failed 
	 */
	ERR_REMOTE_LICENSE_CHECK("BLM_ERR_RCHK", 20, "Remote license check failed"),
	
	/**
	 * An error state for Bad or missing string REV
	 */
	ERR_REV_MISSING("BLM_ERR_REV", 21, "Bad or missing string REV"),
	
	/**
	 * An error state for Unknown division code
	 */
	ERR_UNKNOWN_DIVCODE("BLM_ERR_DIV", 22, "Unknown division code"),
	
	/**
	 * An Error state if Primary server denied
	 */
	ERR_PRIMARY_SERVER_DENY("BLM_ERR_PSRV", 23, "Primary server denied"),
	
	/**
	 * An Error state if Backup server denied
	 */
	ERR_BACKUP_SERVER_DENY("BLM_ERR_BSRV", 24, "Backup server denied"),
	
	/**
	 * An error state for Software version error
	 */
	ERR_SOFTWARE_VERSION("BLM_ERR_VER", 25, "Software version error"),
	
	/**
	 * An error state for Badly formed IP address
	 */
	ERR_MALFORMED_IP("BLM_ERR_BAD_IP", 26, "Badly formed IP address"),
	
	/**
	 * An error state for Badly formed date field
	 */
	ERR_MALFORMED_EXPDATE("BLM_ERR_EXPDATE", 27, " Badly formed date field");
	
	// short error code
	private String mName;

	// error value as defined by BLM
	private int mErrorCode;
	
	// friendly error code
	private String mFriendlyName;


	/**
	 * Creates a new LicenseResultCodes value
	 * 
	 * @param aName - short name of the result code as defined by BLM
	 * @param aValue - result code
	 * @param aFriendlyName - friendly name of the result code
	 */
	private LicenseResultCodes(String aName, int aValue, String aFriendlyName)
	{
		mErrorCode = aValue;
		mName = aName;
		mFriendlyName = aFriendlyName;
	}


	/**
	 * Returns the error code for the LicenseResultCodes value.
	 * 
	 * @return associated error code
	 */
	public int getErrorCode ()
	{
		return mErrorCode;
	}


	/**
	 * Returns the short name for the LicenseResultCodes value.
	 * 
	 * @return error code short name
	 */
	public String getName ()
	{
		return mName;
	}


	/**
	 * @see java.lang.Enum#toString()
	 */
	@Override
	public String toString ()
	{
		return mName;
	}


    /**
     * Returns the friendly name for the LicenseResultCodes value.
     * 
     * @return error code friendly name
     */
	public String getFriendlyName ()
	{
		return mFriendlyName;
	}


	/**
	 * Returns the LicenseResultCodes value corresponding to the specified error code
	 */
	public static LicenseResultCodes translate(int anErrorCode)
	{
		switch (anErrorCode)
		{
			case 0 :
				return SUCCESS;
				
			case 1:
				return FILE_OPEN_FAILED;
				
            case 2:
                return EXEC_OPEN_FAILED;
                
			case 3:
				return LICENSE_STRING_MISSING;
				
            case 4:
                return ERR_BADKEY;
                
            case 5:
                return ERR_PROC;
                
            case 6:
                return ERR_MATCH;
                
            case 7:
                return ERR_HOST;
                
            case 8:
                return ERR_IP;
                
            case 9:
                return ERR_MAC;
                                
            case 10:
                return ERR_QTY;
                
            case 11:
                return ERR_CUST;
                
			case 12:
				return LICENSE_EXPIRED;

			case 13:
				return FEATURE_NOT_ENABLED;
				
            case 14:
                return ERROR_CONNECT_SERVER;
                
            case 15:
                return ERR_SOCKET;
                
            case 16:
                return ERR_IPCHECK_SITE;
                
            case 17:
                return ERR_CONNECT_BLMEXEC;
                               
            case 18:
                return ERR_UNKNOWN;
                
            case 19:
                return ERR_MALLOC;
                
            case 20:
                return ERR_REMOTE_LICENSE_CHECK;
                
            case 21:
                return ERR_REV_MISSING;                
                
            case 22:
                return ERR_UNKNOWN_DIVCODE;
                
            case 23:
                return ERR_PRIMARY_SERVER_DENY;
                
            case 24:
                return ERR_BACKUP_SERVER_DENY;
                
            case 25:
                return ERR_SOFTWARE_VERSION;
                
            case 26:
                return ERR_MALFORMED_IP;
                
            case 27:
                return ERR_MALFORMED_EXPDATE;
                
			default :
				return UNKNOWN;
		}
	}
}
