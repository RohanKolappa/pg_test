/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved.
 * 
 * This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement you entered into with Barco.
 */

package com.barco.nimbus.license.api;

/**
 * Enumeration of possible license related status values.
 * 
 * @author JMRA
 */
public enum LicenseStatus 
{
    /**
     * There is a valid license file and pin and project ID match the system identifier data that has been created
     * when the system was launched for the first time.
     */
    LicenseAccepted,
    
    /**
     * The license is expired.
     */
    LicenseExpired,
    
    /**
     * The license file is invalid or the pin and project ID do not match the system identifier data that has been
     * created when the system was launched for the first time.
     */
    LicenseInvalid,
    
    /**
     * The license is a demo license.
     */
    TrialLicense,
    
}
