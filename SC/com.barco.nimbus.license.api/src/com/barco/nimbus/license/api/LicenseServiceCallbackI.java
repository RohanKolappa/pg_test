/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license.api;


/**
 * This interface has to be implemented by license managers to receive notifications on license change.
 * 
 * @author JMRA
 */

public interface LicenseServiceCallbackI
{
    /**
     * Will be invoked on license change, for all registered listeners
     * 
     * @param aLicenseStatus
     */
    public void handleLicenseUpdated (License aLicense);

}
