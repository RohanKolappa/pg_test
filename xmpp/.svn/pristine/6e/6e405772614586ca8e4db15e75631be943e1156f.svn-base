package com.ipvsserv.nxd.mdc.db;

import java.util.Hashtable;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.xmpp.mdc.DBApi;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBApiFactoryMDC {
    private static Hashtable<String, DBApi> dbApiIs = new Hashtable<String, DBApi>();
    public static String XMLDB = "xmldb";
    public static String SENDA_NATIVE = "sednaNative";
    public static String XMLDB_EXIST = "xmldbExist";
    public static String XMLDB_SEDNA = "xmldbSedna";
    
    private static DBApi loadDBApiI(String className) throws Exception {
        if (!className.equals("")) {
            try {
                if(!dbApiIs.containsKey(className)){
                    MRLog.debug(" loadClass CLASS using " + className);
                    dbApiIs.put(className, (DBApi)Class.forName(className).newInstance());
                }
                //return (DBApi) Class.forName(className).newInstance();
                return dbApiIs.get(className);
            } catch (InstantiationException ex) {
                MRLog.error(ex);
            } catch (IllegalAccessException ex) {
                MRLog.error(ex);
            } catch (ClassNotFoundException ex) {
                MRLog.error(ex);
            }
            MRLog.debug(" DBApiFactoryMDC getDBApiI() not found  className='" + className + "'");
        }
        return null;
    }
    
    private static DBApi getDBApiIDefault() throws Exception {
        String className = IpvsServGlobals.getXMLProperty("DBApiI.default", "");
        return loadDBApiI(className);
    }
    
    private static DBApi getDBApiIDefault(String parentIpvsModuleName) throws Exception {
        String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".impl.DBApiI.default", "");
        DBApi DBApiI = loadDBApiI(className);
        if (DBApiI == null) {
            return getDBApiIDefault();
        }
        return DBApiI;
    }
    
    private static DBApi getDBApiIVendor(String vendor) throws Exception {
        String className = IpvsServGlobals.getXMLProperty("DBApiI." + vendor, "");
        return loadDBApiI(className);
    }
    
    private static DBApi getDBApiIVendor(String parentIpvsModuleName, String vendor) throws Exception {
        String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + "impl.DBApiI." + vendor, "");
        DBApi DBApiI = loadDBApiI(className);
        if (DBApiI == null) {
            return getDBApiIVendor(vendor);
        }
        
        return DBApiI;
    }
    
    public static synchronized DBApi getDBApi(String parentIpvsModuleName) throws Exception {
        if (!dbApiIs.containsKey(parentIpvsModuleName)) {
            DBApi DBApiI = getDBApiIDefault(parentIpvsModuleName);
            if (DBApiI == null) {
                MRLog.debug(" getDBApiI '" + parentIpvsModuleName + " not found.");
                throw new Exception(" getDBApiI '" + parentIpvsModuleName + " not found.");
            }
            dbApiIs.put(parentIpvsModuleName, DBApiI);
        }
        return dbApiIs.get(parentIpvsModuleName);
    }
    
    public static synchronized DBApi getDBApi(String parentIpvsModuleName, String vendor) throws Exception {
        if (!dbApiIs.containsKey(parentIpvsModuleName + "." + vendor)) {
            DBApi DBApiI = getDBApiIVendor(parentIpvsModuleName, vendor);
            if (DBApiI == null) {
                MRLog.debug(" getDBApiI '" + parentIpvsModuleName + " " + vendor + " not found.");
                throw new Exception(" getDBApiI '" + parentIpvsModuleName + " " + vendor + " not found.");
            }
            dbApiIs.put(parentIpvsModuleName + "." + vendor, DBApiI);
        }
        return dbApiIs.get(parentIpvsModuleName + "." + vendor);
    }
}
