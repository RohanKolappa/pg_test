/*
 * @(#)DNSNameServiceDescriptor.java	1.6 05/11/17
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

package sun.net.spi.nameservice.dns;

import sun.net.spi.nameservice.*;

public final class DNSNameServiceDescriptor implements NameServiceDescriptor {
    /**
     * Create a new instance of the corresponding name service.
     */
    public NameService createNameService() throws Exception {
	return new DNSNameService();
    }

    /**
     * Returns this service provider's name
     *
     */
    public String getProviderName() {
	return "sun";
    }

    /**
     * Returns this name service type
     * "dns" "nis" etc
     */
    public String getType() {
	return "dns";
    }
}
