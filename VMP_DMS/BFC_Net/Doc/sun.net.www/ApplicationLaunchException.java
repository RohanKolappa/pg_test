/*
 * @(#)ApplicationLaunchException.java	1.11 05/11/17
 *
 * Copyright 2006 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

package sun.net.www;

/**
 * An exception thrown by the MimeLauncher when it is unable to launch
 * an external content viewer.
 *
 * @version     1.11, 11/17/05
 * @author      Sunita Mani
 */

public class ApplicationLaunchException extends Exception {
    public ApplicationLaunchException(String reason) {
	super(reason);
    }
}
