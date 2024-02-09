package com.ipvs.nimbus.db;

import java.io.File;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Hashtable;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.TimeZone;

import org.dom4j.Element;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsServGlobals {
/*    public static final String DB_COL_LIST_PREFIX = "DB_COL_LIST_PREFIX";
    public static final String DB_COL_PREFIX = "DB_COL_PREFIX";
    public static final String DB_DOC_PREFIX = "DB_DOC_PREFIX";
    public static final String DB_INDEX_NODE_PREFIX = "DB_INDEX_NODE_PREFIX";
    public static final String DB_SUB_DOC_PREFIX = "DB_SUB_DOC_PREFIX";
    public static final String DB_SUB_COL_PREFIX = "DB_SUB_COL_PREFIX";
    public static final String COL_INDEX_NODE_PREFIX = "COL_INDEX_NODE_PREFIX";
    public static final int SLICE_LENGTH = 100;
    
    public static final String APP_IPVS_COMPONENT = "ipvscomponent";
    public static final String APP_DBTASK = "ipvsdbtask";
    public static final String APP_EXIST_DBTEST = "existdbtest";
    public static final String APP_SEDNA_NATIVE_DBTEST = "sednanativedbtest";
    public static final String APP_SEDNA_XMLDB_DBTEST = "sednaxmldbtest";
    public static final String BULK_UPLOAD = "bulkupload";
    public static String MIGRATION_PROGRESS_STATUS="migration_progress.status";
    public static String SERVICE_VERSION="service.version";
    public static final String TEST_XMLS = "testXmls";
    private static ProcessArguments argList = new ProcessArguments(null);
    //public  String DOMAIN_DEFAULT="default";
    private static String ipvsAppName = null;
    private static String dbTaskName = null;
    private static String dbTaskFolder = null;
    
    private static String home = null;
    
    public static boolean failedLoading = false;
    
    private static XMLProperties xmlProperties = null;
    private static IpvsServProperties properties = null;
    
    private static Locale locale = null;
    private static TimeZone timeZone = null;
    private static DateFormat dateFormat = null;
    private static DateFormat dateTimeFormat = null;
    private static DateFormat timeFormat = null;
    
    private static String configName;
    private static String pluginDir;
    //private static String keyStoreLocationPath;
    private static DomainDBSchema domainDBSchema = null;
    private static boolean use_list_col = false;
    private static String col_prefix = null;

    public static synchronized DomainDBSchema getDomainDBSchema() {
        if (domainDBSchema == null) {
            domainDBSchema = new DomainDBSchema();
            domainDBSchema.init();
        }
        return domainDBSchema;
    }
    
    /////////////////////////////// settings /////////////////////////// 
    public static String getHomeDirectory() {
        if (home == null || home.equals("")) {
            String ipvsConfigName = System.getProperty("ipvscomponentHome");
            try {
                setHomeDirectory(ipvsConfigName);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        return home;
    }
    
    public static void setProcessArguments(ProcessArguments argList) throws Exception {
        IpvsServGlobals.argList = argList;
    }
    
    public static ProcessArguments getProcessArguments() throws Exception {
        return IpvsServGlobals.argList;
    }
    
    public static void setHomeDirectory(String pathname) throws Exception {
        File mh = new File(pathname);
        // Do a permission check on the new home directory
        if (!mh.exists()) {
            MRLog.error("Error - the specified home directory does not exist (" + pathname + ")");
            throw new Exception("Error - the specified home directory does not exist (" + pathname + ")");
        } else if (!mh.canRead() || !mh.canWrite()) {
            MRLog.error("Error - the user running this application can not read " + "and write to the specified home directory (" + pathname + "). " + "Please grant the executing user read and write permissions.");
            throw new Exception("Error - the user running this application can not read " + "and write to the specified home directory (" + pathname + "). " + "Please grant the executing user read and write permissions.");
        } else {
            home = pathname;
            if (xmlProperties == null) {
                loadSetupProperties();
            }
        }
    }
    
    /////////////////////////////////////
    public static String verifyAndGetTestXmlPath(String folder, String configName) {
        File mh = new File(configName);
        if (!mh.exists()) {
            mh = new File(IpvsServGlobals.getHomeDirectory() + "" + File.separator + folder + File.separator + mh.getName());
        }
        return mh.getAbsolutePath();
    }
    
    public static String verifyAndGetTestXmlPath(String configName) throws Exception {
        File mh = new File(configName);
        if (!mh.exists()) {
            mh = new File(IpvsServGlobals.getHomeDirectory() + "" + File.separator + TEST_XMLS + File.separator + mh.getName());
        }
        if (!mh.exists()) {
            throw new Exception("verifyAndGetTestXmlPath.File does not exist '" + configName + "'");
        }
        return mh.getAbsolutePath();
    }
    
    public static String verifyAndGetFilePath(String configName) {
        File mh = new File(configName);
        if (!mh.exists()) {
            mh = new File(IpvsServGlobals.getHomeDirectory() + "" + File.separator + "conf" + "" + File.separator + getIpvsAppName() + File.separator + "" + mh.getName());
        }
        return mh.getAbsolutePath();
    }
    
    public static String getConfigPath() {
        if (configName == null || configName.equals("")) {
            String ipvsConfigName = System.getProperty("ipvsConfigFilePath");
            try {
                verifyConfigPath(ipvsConfigName);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return IpvsServGlobals.configName;
    }
    
    public static void setConfigPath(String configName) throws Exception {
        verifyConfigPath(configName);
    }
    
    private static void verifyConfigPath(String pathname) throws Exception {
        File mh = new File(pathname);
        // Do a permission check on the new home directory
        if (!mh.exists()) {
            MRLog.error("Error - the specified config directory does not exist (" + pathname + ")");
            throw new Exception("Error - the specified config directory does not exist (" + pathname + ")");
        } else if (!mh.canRead() || !mh.canWrite()) {
            MRLog.error("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
            throw new Exception("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
        } else {
            IpvsServGlobals.configName = pathname;
        }
    }
    
    ////////////// plugin //////////////////////
    
    public static String getPluginsDir() {
        if (pluginDir == null || pluginDir.equals("")) {
            String ipvsConfigName = System.getProperty("pluginDirs");
            try {
                verifyPluginDir(ipvsConfigName);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return IpvsServGlobals.pluginDir;
    }
    
    public static void setPluginsDir(String pluginDir) throws Exception {
        verifyPluginDir(pluginDir);
    }
    
    private static void verifyPluginDir(String pathname) throws Exception {
        File mh = new File(pathname);
        // Do a permission check on the new home directory
        if (!mh.exists()) {
            MRLog.error("Error - the specified plugins directory does not exist (" + pathname + ")");
            throw new Exception("Error - the specified plugins directory does not exist (" + pathname + ")");
        } else if (!mh.canRead() || !mh.canWrite()) {
            MRLog.error("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
            throw new Exception("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
        } else {
            IpvsServGlobals.pluginDir = pathname;
        }
    }
    
    public static void setIpvsAppName(String ipvsAppName) {
        IpvsServGlobals.ipvsAppName = ipvsAppName;
    }
    
    public static String getIpvsAppName() {
        return IpvsServGlobals.ipvsAppName;
        
    }
    
    public static void setDBTaskName(String dbTaskName) {
        IpvsServGlobals.dbTaskName = dbTaskName;
    }
    
    public static String getDBTaskName() {
        return IpvsServGlobals.dbTaskName;
    }
    
    public static void setDBTaskFolder(String dbTaskFolder) {
        IpvsServGlobals.dbTaskFolder = dbTaskFolder;
    }
    
    public static String getDBTaskFolder() {
        return IpvsServGlobals.dbTaskFolder;
        
    }
    
    public static boolean isIpvsComponetApp() {
        if (getIpvsAppName() != null && getIpvsAppName().equalsIgnoreCase(APP_IPVS_COMPONENT)) {
            return true;
        }
        return false;
    }
    
    ////////////// keyStoreLocationPath //////////////////////
    
     public static String getKeyStoreLocationPath(){
         if(keyStoreLocationPath==null || keyStoreLocationPath.equals("")){
             String ipvsConfigName = System.getProperty("keyStoreLocationPath");
             try{
                 verifyKeyStoreLocationPath(ipvsConfigName);
             }catch(Exception ex){
                 MRLog.error(ex);
             }              
         }
         return IpvsServGlobals.keyStoreLocationPath;
     }
     public static void setKeyStoreLocationPath(String keyStoreLocationPath) throws Exception {
         verifyKeyStoreLocationPath(keyStoreLocationPath);
     }   
     private static void verifyKeyStoreLocationPath(String pathname) throws Exception {
         File mh = new File(pathname);
         // Do a permission check on the new home directory
         if (!mh.exists()) {
             MRLog.error("Error - the specified keyStoreLocation directory does not exist (" + pathname + ")");
             throw new Exception("Error - the specified keyStoreLocation directory does not exist (" + pathname + ")");
         } else if (!mh.canRead() || !mh.canWrite()) {
             MRLog.error("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
             throw new Exception("Error - the user running this application can not read " + "and write to the specified file (" + pathname + "). " + "Please grant the executing user read and write permissions.");
         } else {
             IpvsServGlobals.keyStoreLocationPath = pathname;
         }
     }    
    /////////////////////////////// settings /////////////////////////// 
    public static Locale getLocale() {
        if (locale == null) {
            if (xmlProperties != null) {
                String[] localeArray;
                String localeProperty = xmlProperties.getProperty("locale");
                if (localeProperty != null) {
                    localeArray = localeProperty.split("_");
                } else {
                    localeArray = new String[] { "", "" };
                }
                
                String language = localeArray[0];
                if (language == null) {
                    language = "";
                }
                String country = "";
                if (localeArray.length == 2) {
                    country = localeArray[1];
                }
                // If no locale info is specified, return the system default Locale.
                if (language.equals("") && country.equals("")) {
                    locale = Locale.getDefault();
                } else {
                    locale = new Locale(language, country);
                }
            } else {
                return Locale.getDefault();
            }
        }
        return locale;
    }
    
    public static void setLocale(Locale newLocale) {
        locale = newLocale;
        // Save values to Jive properties.
        setXMLProperty("locale", locale.toString());
        
        // Reset the date formatter objects
        timeFormat = null;
        dateFormat = null;
        dateTimeFormat = null;
    }
    
    public static TimeZone getTimeZone() {
        if (timeZone == null) {
            if (properties != null) {
                String timeZoneID = properties.get("locale.timeZone");
                if (timeZoneID == null) {
                    timeZone = TimeZone.getDefault();
                } else {
                    timeZone = TimeZone.getTimeZone(timeZoneID);
                }
            } else {
                return TimeZone.getDefault();
            }
        }
        return timeZone;
    }
    
    public static void setTimeZone(TimeZone newTimeZone) {
        timeZone = newTimeZone;
        if (timeFormat != null) {
            timeFormat.setTimeZone(timeZone);
        }
        if (dateFormat != null) {
            dateFormat.setTimeZone(timeZone);
        }
        if (dateTimeFormat != null) {
            dateTimeFormat.setTimeZone(timeZone);
        }
        //setProperty("locale.timeZone", timeZone.getID());
        setXMLProperty("locale.timeZone", timeZone.getID());
    }
    
    public static String formatTime(Date date) {
        if (timeFormat == null) {
            if (properties != null) {
                timeFormat = DateFormat.getTimeInstance(DateFormat.SHORT, getLocale());
                timeFormat.setTimeZone(getTimeZone());
            } else {
                DateFormat instance = DateFormat.getTimeInstance(DateFormat.SHORT, getLocale());
                instance.setTimeZone(getTimeZone());
                return instance.format(date);
            }
        }
        return timeFormat.format(date);
    }
    
    public static String formatDate(Date date) {
        if (dateFormat == null) {
            if (properties != null) {
                dateFormat = DateFormat.getDateInstance(DateFormat.MEDIUM, getLocale());
                dateFormat.setTimeZone(getTimeZone());
            } else {
                DateFormat instance = DateFormat.getDateInstance(DateFormat.MEDIUM, getLocale());
                instance.setTimeZone(getTimeZone());
                return instance.format(date);
            }
        }
        return dateFormat.format(date);
    }
    
    public static String formatDateTime(Date date) {
        if (dateTimeFormat == null) {
            if (properties != null) {
                dateTimeFormat = DateFormat.getDateTimeInstance(DateFormat.MEDIUM, DateFormat.MEDIUM, getLocale());
                dateTimeFormat.setTimeZone(getTimeZone());
            } else {
                DateFormat instance = DateFormat.getDateTimeInstance(DateFormat.MEDIUM, DateFormat.MEDIUM, getLocale());
                instance.setTimeZone(getTimeZone());
                return instance.format(date);
            }
        }
        return dateTimeFormat.format(date);
    }
    
    //////////////////////////////////////////////////////////////// xmlProperties ////////////////////////////////////////////////////////////////   
 
    
    public void getXMLPropertiesRecur(String name, Map<String,String> params , String propPrefix) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return  ;
        }
        xmlProperties.getProperiesRecursive(name, params, propPrefix);
    }
    
    /////////////////////// xml properties /////////////////////////////////////////////\
    
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
    public static void setXMLProperties(Map<String, String> propertyMap) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        if (xmlProperties != null) {
            xmlProperties.setXMLProperties(propertyMap);
        }
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
    public static void setElement(Element el) {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        
        // jiveHome not loaded?
        if (xmlProperties != null) {
            xmlProperties.setElement(el);
        }
    }
    
    /////////////////////// cutom utility functions for xml properties ///////////////////
    
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
    
    public static Map<String, Map<String, String>> getModules() {
        if (xmlProperties == null) {
            loadSetupProperties();
        }
        // jiveHome not loaded?
        if (xmlProperties == null) {
            return null;
        }
        return xmlProperties.getModules();
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
    //////////////////////////////////////////////////////////////// xmlProperties ////////////////////////////////////////////////////////////////   
    
    //////////////////////////////////////////////////////////////// properties ////////////////////////////////////////////////////////////////   
    //    public static String getProperty(String name) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return null;
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        return properties.get(name);
    //    }
    //    
    //    public static String getProperty(String name, String defaultValue) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return defaultValue;
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        String value = properties.get(name);
    //        if (value != null) {
    //            return value;
    //        } else {
    //            return defaultValue;
    //        }
    //    }
    //    
    //    public static int getIntProperty(String name, int defaultValue) {
    //        String value = getProperty(name);
    //        if (value != null) {
    //            try {
    //                return Integer.parseInt(value);
    //            } catch (NumberFormatException nfe) {
    //                // Ignore.
    //            }
    //        }
    //        return defaultValue;
    //    }
    //    
    //    public static long getLongProperty(String name, long defaultValue) {
    //        String value = getProperty(name);
    //        if (value != null) {
    //            try {
    //                return Long.parseLong(value);
    //            } catch (NumberFormatException nfe) {
    //                // Ignore.
    //            }
    //        }
    //        return defaultValue;
    //    }
    //    
    //    public static boolean getBooleanProperty(String name) {
    //        return Boolean.valueOf(getProperty(name));
    //    }
    //    
    //    public static boolean getBooleanProperty(String name, boolean defaultValue) {
    //        String value = getProperty(name);
    //        if (value != null) {
    //            return Boolean.valueOf(value);
    //        } else {
    //            return defaultValue;
    //        }
    //    }
    //    
    //    public static List<String> getPropertyNames(String parent) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return new ArrayList<String>();
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        return new ArrayList<String>(properties.getChildrenNames(parent));
    //    }
    //    
    //    public static List<String> getProperties(String parent) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return new ArrayList<String>();
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        
    //        Collection<String> propertyNames = properties.getChildrenNames(parent);
    //        List<String> values = new ArrayList<String>();
    //        for (String propertyName : propertyNames) {
    //            String value = getProperty(propertyName);
    //            if (value != null) {
    //                values.add(value);
    //            }
    //        }
    //        
    //        return values;
    //    }
    //    
    //    public static List<String> getPropertyNames() {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return new ArrayList<String>();
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        return new ArrayList<String>(properties.getPropertyNames());
    //    }
    //    
    //    public static void setProperty(String name, String value) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return;
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        properties.put(name, value);
    //    }
    //    
    //    public static void setProperties(Map<String, String> propertyMap) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return;
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        
    //        properties.putAll(propertyMap);
    //    }
    //    
    //    public static void deleteProperty(String name) {
    //        if (properties == null) {
    //            if (isSetupMode()) {
    //                return;
    //            }
    //            properties = JiveProperties.getInstance();
    //        }
    //        properties.remove(name);
    //    }
    //////////////////////////////////////////////////////////////// properties ////////////////////////////////////////////////////////////////   
    
    private static boolean isSetupMode() {
        return !Boolean.valueOf(IpvsServGlobals.getXMLProperty("setup"));
    }

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
                    xmlProperties = new XMLProperties(path);
                    initListColVars();
                } catch (Exception ex) {
                    MRLog.error("loadSetupProperties failed " + path);
                    MRLog.error(ex);
                    failedLoading = true;
                }
            }
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
    
    /////////////////////////////
    
    public static String getProperty(String name, String defaultValue) {
        return getXMLProperty(name, defaultValue);
    }
    public static String[] getValues(String name) {
    	 if (xmlProperties == null) {
             loadSetupProperties();
         }
         
         // home not loaded?
         if (xmlProperties == null) {
             return null;
         }
         
         return xmlProperties.getValues(name);
          
    }
    
    public static void deleteProperty(String string) {
    // TODO:        
    
    }
    
    public static void setProperty(String string, String domainName) {
    // TODO:
    
    }
    
    public static void migrateProperty(String name) {
    // Convert XML based provider setup to Database based
    }
    
    ////////////////////////////
    private String getDomainName(String collectionName){
        if(collectionName==null || collectionName.equals("")){
            return "default";
        }
        String colIndex= IpvsServGlobals.getDomainDBSchema().getIndexNamePrefix("data");
        return collectionName.substring(colIndex.length());
    }    
    private static String os = null; 
    public static  String getOS(){
        try {
            if(os !=null && !os.equals(""))
                return os;  
            os = System.getProperty("os.name").toLowerCase();
            //default
            if(os == null || os.equals(""))
                os = "linux";
            if (os.indexOf("nt")!=-1 || os.indexOf("windows 20")!=-1  || os.indexOf("windows 9")!=-1 || os.indexOf("windows xp")!=-1 ) {
                os = "windows";
            }else if (os.startsWith("mac")  ) {
                os = "mac";
            }else if (os.indexOf("hp-ux") != -1 ) {
                os = "linux";
            }else if (os.indexOf("sunos") != -1 ) {
                os = "linux";
            }else if (os.indexOf("linux") != -1 ) {
                os = "linux";
            } else {
                os = "linux";
            }           
        }catch(Exception ex){
        }
        return  os;
    }   
    public static boolean isWindowsOS(){
        String os =getOS();
        if(os !=null && os.equals("windows")){
            return true;
        }
        return false;
    }
    public static String getDomainDefault(){
        
        return "default";
    }    

    private static void initListColVars() {
        if (col_prefix == null) {
            use_list_col = IpvsServGlobals.getBooleanProperty("admindbapi.use_list_col", false);
            col_prefix = IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_LIST_PREFIX);
        }
    }

    public static boolean isUseListCol() {
        return use_list_col;
    }

    public static String getListColPrefix() {
        return col_prefix;
    }    */
}
