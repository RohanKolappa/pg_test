package com.ipvsserv.nxd.api;

import java.util.Hashtable;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsDBDriverFactory {
    private static Hashtable<String, IpvsDBDriver> ipvsDBDrivers = new Hashtable<String, IpvsDBDriver>();
    public static String XMLDB = "xmldb";
    public static String SENDA_NATIVE = "sednaNative";
    public static String XMLDB_EXIST = "xmldbExist";
    public static String XMLDB_SEDNA = "xmldbSedna";

    private static IpvsDBDriver loadIpvsDBDriver(String className) throws Exception {
        if (!className.equals("")) {
            if(ipvsDBDrivers.containsKey(className)){
                return ipvsDBDrivers.get(className);
            }
            MRLog.debug(" loadClass CLASS using " + className);
            try {
                IpvsDBDriver ipvsDBDriver = (IpvsDBDriver) Class.forName(className).newInstance();
                ipvsDBDrivers.put(className, ipvsDBDriver);
                return ipvsDBDriver;
            } catch (InstantiationException ex) {
                MRLog.error(ex);
            } catch (IllegalAccessException ex) {
                MRLog.error(ex);
            } catch (ClassNotFoundException ex) {
                MRLog.error(ex);
            }
            MRLog.debug(" DBApiManager getIpvsDBDriver() not found  className='" + className + "'");
        }
        return null;
    }

    private static IpvsDBDriver getIpvsDBDriverDefault() throws Exception {
        String className = IpvsServGlobals.getXMLProperty("IpvsDBDriver.default", "");
        return loadIpvsDBDriver(className);
    }

    private static IpvsDBDriver getIpvsDBDriverDefault(String parentIpvsModuleName) throws Exception {
        String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".IpvsDBDriver.default", "");
        IpvsDBDriver ipvsDBDriver = loadIpvsDBDriver(className);
        if (ipvsDBDriver == null) {
            return getIpvsDBDriverDefault();
        }
        return ipvsDBDriver;
    }

    public static IpvsDBDriver getIpvsDBDriverVendor(String vendor) throws Exception {
        String className = IpvsServGlobals.getXMLProperty("IpvsDBDriver." + vendor, "");
        return loadIpvsDBDriver(className);
    }

    private static IpvsDBDriver getIpvsDBDriverVendor(String parentIpvsModuleName, String vendor) throws Exception {
        String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".IpvsDBDriver." + vendor, "");
        IpvsDBDriver ipvsDBDriver = loadIpvsDBDriver(className);
        if (ipvsDBDriver == null) {
            return getIpvsDBDriverVendor(vendor);
        }
        return ipvsDBDriver;
    }

    public static synchronized IpvsDBDriver getIpvsDBDriver(String parentIpvsModuleName) throws Exception {
        if (!ipvsDBDrivers.containsKey(parentIpvsModuleName)) {
            IpvsDBDriver ipvsDBDriver = getIpvsDBDriverDefault(parentIpvsModuleName);
            if (ipvsDBDriver == null) {
                MRLog.debug(" getIpvsDBDriver '" + parentIpvsModuleName + " not found.");
                throw new Exception(" getIpvsDBDriver '" + parentIpvsModuleName + " not found.");
            }
            ipvsDBDrivers.put(parentIpvsModuleName, ipvsDBDriver);
        }
        return ipvsDBDrivers.get(parentIpvsModuleName);
    }

    public static synchronized IpvsDBDriver getIpvsDBDriver(String parentIpvsModuleName, String vendor) throws Exception {
        if (!ipvsDBDrivers.containsKey(parentIpvsModuleName + "." + vendor)) {
            IpvsDBDriver ipvsDBDriver = getIpvsDBDriverVendor(parentIpvsModuleName, vendor);
            if (ipvsDBDriver == null) {
                MRLog.debug(" getIpvsDBDriver '" + parentIpvsModuleName + " " + vendor + " not found.");
                throw new Exception(" getIpvsDBDriver '" + parentIpvsModuleName + " " + vendor + " not found.");
            }
            ipvsDBDrivers.put(parentIpvsModuleName + "." + vendor, ipvsDBDriver);
        }
        return ipvsDBDrivers.get(parentIpvsModuleName + "." + vendor);
    }
}
