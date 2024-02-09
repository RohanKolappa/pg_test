/*
 * @(#)NameService.java	1.6 05/11/17
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

package sun.net.spi.nameservice;

import java.net.UnknownHostException;

public interface NameService {
    public java.net.InetAddress[] lookupAllHostAddr(String host) throws UnknownHostException;
    public String rslvRawToName(byte[] addr) throws UnknownHostException;
}
