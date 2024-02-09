/*
 * @(#)URLJarFileCallBack.java	1.7 05/11/17
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

package sun.net.www.protocol.jar;

import java.io.*;
import java.net.*;
import java.util.jar.*;


/*
 * This interface is used to call back to sun.plugin package.
 */
public interface URLJarFileCallBack
{
        public JarFile retrieve (URL url) throws IOException;
}
