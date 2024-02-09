/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license.api;

import java.io.InputStream;

/**
 * This interface has to be implemented by license provider.
 * 
 * @author JMRA
 */

public interface LicenseServiceI {
	
	// License exception codes
	/*
	 * License expiration code
	 */
	public static final int LICENSE_EXPIRY_CODE = 1411;
	/*
	 * License invalid code
	 */
	public static final int LICENSE_INVALID_CODE = 1412;
	/*
	 * License code for file not found
	 */
	public static final int LICENSE_FILENOTFOUND_CODE = 1412;
	
	
	/**
	 * This method reads the file from existing location that is known to BLM library.
	 * </br> i.e if following environment variables were set properly. </br>
	 * BLM_LICENSE_FILE environment variable initialized with location of
	 * license file to be loaded.</br> BLM_DYNAMIC_DIR environment variable
	 * initialized with location, where BLM uses it for its execution. </br>
	 * 
	 * @throws LicenseException
	 *             - if unable to read license file from license file path, or
	 *             unable to upload given license file.
	 */
	public void put() throws LicenseException;

	/**
	 * Read license from given input file stream and update the database
	 * 
	 * @throws LicenseException
	 *             - if unable to read license file from input license file
	 *             path, or unable to upload given license file.
	 */
	public void put(String anInputFileLoacation) throws LicenseException;

	/**
	 * Read license from given input file stream and update the database.
	 * 
	 * @param aLicenseInputStream
	 *            - input license file stream
	 * @throws LicenseException
	 *             - if unable to read, invalid license file or unable to upload
	 *             given license file.
	 */
	public void put(InputStream aLicenseInputStream) throws LicenseException;

	/**
	 * Will return license details.
	 * 
	 * @return - License information
	 */
	public License get();

	/**
	 * Helps to register listeners, who wants update notifications from license service.
	 * 
	 * @param licenseServiceCallbackI
	 */
	public void registerCallback(LicenseServiceCallbackI licenseServiceCallbackI);
}
