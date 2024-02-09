/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license;

import java.util.Date;


/**
 * This interface has to be implemented by license scheduler impl.
 * 
 * @author JMRA
 */

public interface LicenseSchedulerI
{
    /**
     * Will be called on license service initialization or on loading new license
     * 
     * @param aLicenseExpiry
     */
    public void schedule (Date aLicenseExpiry);

}
