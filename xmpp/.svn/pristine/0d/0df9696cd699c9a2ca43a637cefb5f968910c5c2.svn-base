package com.ipvsserv.nxd.dbtask.test;

import java.io.File;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.config.XMLProperties;
import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class TestGlobals {
    private static XMLProperties xmlProperties = null;
    private static String home;
    private static boolean failedLoading;
    private static String configName = "TestDB.xml";
    
    private synchronized static void loadSetupProperties() {
        if (xmlProperties == null) {
            if (home == null) {
                home = System.getProperty("ipvscomponentHome", null);
            }
            // If home is null then log that the application will not work correctly
            if (home == null && !failedLoading) {
                failedLoading = true;
                StringBuilder msg = new StringBuilder();
                msg.append("Critical Error! The home directory has not been configured, \n");
                msg.append("which will prevent the application from working correctly.\n\n");
                MRLog.error(msg.toString());
            }
            // Create a manager with the full path to the xml config file.
            else {
                String path = "";
                try {
                    path = getConfigPath();
                    MRLog.debug("LOADING TEST XML " + path);
                    xmlProperties = new XMLProperties(path);
                } catch (Exception ex) {
                    MRLog.error("loadSetupProperties failed " + path);
                    MRLog.error(ex);
                    failedLoading = true;
                }
            }
        }
    }
    
    public static String getConfigPath() {
        return TestGlobals.configName;
    }
    
    public static void setConfigPath(String configName) throws Exception {
        verifyConfigPath(configName);
    }
    
    private static void verifyConfigPath(String pathname) throws Exception {
        File mh = new File(pathname);
        if (!mh.exists()) {
            pathname = IpvsServGlobals.getHomeDirectory() + "" + File.separator + IpvsServGlobals.TEST_XMLS + File.separator + mh.getName();
            mh = new File(pathname);
            MRLog.error("Error -  not found ? " + mh.getCanonicalPath() + "");
        }
        // Do a permission check on the new home directory
        if (!mh.exists()) {
            MRLog.error("Error - the specified config directory does not exist (" + mh.getCanonicalPath() + ")");
            throw new Exception("Error - the specified config directory does not exist (" + mh.getCanonicalPath() + ")");
        } else if (!mh.canRead() || !mh.canWrite()) {
            MRLog.error("Error - the user running this application can not read " + "and write to the specified file (" + mh.getCanonicalPath() + "). " + "Please grant the executing user read and write permissions.");
            throw new Exception("Error - the user running this application can not read " + "and write to the specified file (" + mh.getCanonicalPath() + "). " + "Please grant the executing user read and write permissions.");
        } else {
            TestGlobals.configName = pathname;
        }
    }
    
    //////////////////////new ////////////////////////
    public static int getIntProperty(String name, int defaultValue) {
        String value = getProperty(name, defaultValue + "");
        if (value != null) {
            try {
                return Integer.parseInt(value);
            } catch (NumberFormatException nfe) {
                // Ignore.
            }
        }
        return defaultValue;
    }
    
    public static long getLongProperty(String name, long defaultValue) {
        String value = getProperty(name, defaultValue + "");
        if (value != null) {
            try {
                return Long.parseLong(value);
            } catch (NumberFormatException nfe) {
                // Ignore.
            }
        }
        return defaultValue;
    }
    
    public static boolean getBooleanProperty(String name) {
        return Boolean.valueOf(getProperty(name, null));
    }
    
    public static boolean getBooleanProperty(String name, boolean defaultValue) {
        String value = getProperty(name, defaultValue + "");
        if (value != null) {
            return Boolean.valueOf(value);
        } else {
            return defaultValue;
        }
    }
    
    public static String getXMLProperty(String name) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // home not loaded?
        if (xmlProperties == null) {
            return null;
        }
        
        return xmlProperties.getProperty(name);
    }
    
    public static String getXMLProperty(String name, String defaultValue) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // home not loaded?
        if (xmlProperties == null) {
            return null;
        }
        
        String value = xmlProperties.getProperty(name);
        if (value == null) {
            value = defaultValue;
        }
        return value;
    }
    
    public static int getXMLProperty(String name, int defaultValue) {
        String value = getXMLProperty(name);
        if (value != null) {
            try {
                return Integer.parseInt(value);
            } catch (NumberFormatException nfe) {
                // Ignore.
            }
        }
        return defaultValue;
    }
    
    public static boolean getXMLProperty(String name, boolean defaultValue) {
        String value = getXMLProperty(name);
        if (value != null) {
            return Boolean.valueOf(value);
        }
        return defaultValue;
    }
    
    public static void setXMLProperty(String name, String value) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // jiveHome not loaded?
        if (xmlProperties != null) {
            xmlProperties.setXMLProperty(name, value);
        }
    }
    
    public static Element getElement(String propName) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // jiveHome not loaded?
        if (xmlProperties != null) {
            return xmlProperties.getElement(propName);
        }
        return null;
    }
    
    public static Element getElementXpath(String xpah) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return null;
        }
        return xmlProperties.getElementXpath(xpah);
    }
    
    public static void setXMLProperties(Map<String, String> propertyMap) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        if (xmlProperties != null) {
            xmlProperties.setXMLProperties(propertyMap);
        }
    }
    
    public static void fillModuleParams(String moduleName, Map<String, String> list) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return;
        }
        xmlProperties.fillModuleParams(moduleName, list);
    }
    
    public static Map<String, String> getModuleParams(String moduleName) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return new Hashtable<String, String>();
        }
        Hashtable<String, String> list = new Hashtable<String, String>();
        xmlProperties.fillModuleParams(moduleName, list);
        return list;
        
    }
    
    public static Map<String, Map<String, String>> getModules(String priorityLevelsELName, String modulesElName, String moduleElName) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return null;
        }
        return xmlProperties.getModules(priorityLevelsELName, modulesElName, moduleElName);
    }
    
    public static Map<String, Map<String, String>> getModulesXpath(String priorityLevelsELName, String modulesElName, String moduleElName) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return null;
        }
        return xmlProperties.getModulesXpath(priorityLevelsELName, modulesElName, moduleElName);
    }
    
    public static Map<String, String> getParams(String name) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return null;
        }
        return xmlProperties.getParams(name);
    }
    
    public static List<String> getXMLProperties(String parent) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return new ArrayList<String>();
        }
        
        String[] propNames = xmlProperties.getChildrenProperties(parent);
        List<String> values = new ArrayList<String>();
        for (String propName : propNames) {
            String value = getXMLProperty(parent + "." + propName);
            if (value != null) {
                values.add(value);
            }
        }
        
        return values;
    }
    
    public static void deleteXMLProperty(String name) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        xmlProperties.deleteXMLProperty(name);
    }
    
    public static String getProperty(String name, String defaultValue) {
        return getXMLProperty(name, defaultValue);
    }
}
