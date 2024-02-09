/*
 * @(#)Handler.java	1.15 05/11/17
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/**
 * System Resources are of the form
 * 	systemresource:/FILE<path to the directory base>/+/<name of resource>
 * or
 * 	systemresource:/ZIP<path to the zip file>/+/<name of resource>
 *
 * @see		SystemResourceConnection
 * @version 	1.15, 11/17/05
 * @author	Eduardo Pelegri-Llopart
 */

package sun.net.www.protocol.systemresource;

import java.io.*;
import java.net.*;


public class Handler extends URLStreamHandler {
    public URLConnection openConnection(URL u) throws IOException {
	return new SystemResourceURLConnection(u);
    }
}
