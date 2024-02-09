package com.ipvsserv.server;

import java.lang.reflect.Method;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.jivesoftware.whack.ExternalComponentManager;
import org.jivesoftware.whack.container.ServerContainer;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsServInfo;
import com.ipvsserv.common.util.IpvsServInfoImpl;
import com.ipvsserv.common.util.Version;
import com.ipvsserv.common.util.Version.ReleaseStatus;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.IpvsDBDriverFactory;
import com.ipvsserv.nxd.migration.DBMigrationFileSystemModule;
import com.ipvsserv.nxd.migration.DBMigrationModule;
import com.ipvsserv.starter.IpvsServClassLoader;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class IpvsServer extends ServerContainer {
    private static final String STARTER_CLASSNAME = "com.ipvsserv.starter.ServerStarter";
    private static final String WRAPPER_CLASSNAME = "org.tanukisoftware.wrapper.WrapperManager";
    public static final String SERVER_VERSION_XML = "/com/ipvs/xsd/objectXml/service.version";

    private IpvsServClassLoader loader;
    private Map<String, IpvsServModule> modulesByName = new LinkedHashMap<String, IpvsServModule>();

    public final static int SERVER_STARTED = 200;
    public final static int SERVER_STARTING = 100;
    public final static int SERVER_STOPPING = 10;
    public final static int SERVER_STOPPED = 0;
    public final static int SERVER_RESTARTED = 110;
    public final static int SERVER_RESTARTING = 120;

    private Version version = null;
    private boolean setupMode;
    private String name;
    private Date startDate;
    private boolean initialized = false;
    private boolean started = false;
    private String host = "localhost";

    private boolean shuttingDown;

    private boolean isFactoryDBCheckDone=false;
    private boolean migrationComplete=false;
    private static IpvsServer instance;
    private ExternalComponentManager manager;
    private IpvsServInfo xmppServerInfo;
    private int priorityLevels = 10;

    private IpvsFileStatus initializingFile = null;
    private IpvsFileStatus runningFile = null;
    private static String managementserver = "managementserver.";
    private static String ipvsuserauthservice = "ipvsuserauthservice.";
    private static String ipvsdbservice = "ipvsdbservice.";

    private List<ServerEventListener> serverEventListeners = Collections.synchronizedList(new ArrayList<ServerEventListener>());
    private int serverState = 0;

    private String serverVersion = null;
    private Object mrclass = null;
    private IpvsDBDriver db = null;
    private String dbFailedError="";
    public static IpvsServer getInstance() {
        return instance;
    }

    public IpvsServer() {
        // We may only have one instance of the server running on the JVM
        if (instance != null) {
            throw new IllegalStateException("A server is already running");
        }
        instance = this;
        start();
    }
    
    public boolean isFactoryDBCheckDone() {
        return isFactoryDBCheckDone;
    }

    public void setFactoryDBCheckDone(boolean isFactoryDBCheckDone) {
        this.isFactoryDBCheckDone = isFactoryDBCheckDone;
    }

    public boolean isMigrationComplete() {
        return migrationComplete;
    }

    public void setMigrationComplete(boolean migrationComplete) {
        this.migrationComplete = migrationComplete;
    }

    public static boolean isComponentJID(String userJID) {
        if (userJID.indexOf("@") != -1) {
            return false;
        }
        if (userJID.startsWith(managementserver)) {
            return true;
        }
        if (userJID.startsWith(ipvsuserauthservice)) {
            return true;
        }
        if (userJID.startsWith(ipvsdbservice)) {
            // return true;
        }
        return false;
    }

    public IpvsServClassLoader getIpvsClassLoader() {
        if (loader == null) {
            loader = (IpvsServClassLoader) Thread.currentThread().getContextClassLoader();
        }
        return loader;
    }

    //////////////////////////////////////// modules /////////////////////////////////////

    /////////////////////////loadModule/////////////////////////
    private void loadModules(Map<String, Map<String, String>> modules) {
        if (shuttingDown)
            return;
        for (int i = 1; i < priorityLevels; i++) {
            loadModules(modules, i);
        }
        MRLog.info("done loadModules ");
    }

    private void loadModules(Map<String, Map<String, String>> modules, int priority) {
        if (shuttingDown)
            return;
        for (String moduleName : modules.keySet()) {
            Map<String, String> prams = modules.get(moduleName);
            String moduleClass = prams.get("class");
            int modulePriority = CompUtils.intValue(prams.get("priority"), 10);
            String name = prams.get("name");
            boolean moduleEnabled = CompUtils.booleanValue(prams.get("enable"), false);
            boolean completBeforeStartNext = CompUtils.booleanValue(prams.get("completBeforeStartNext"), false);
            if (priority == modulePriority) {
                MRLog.info(moduleName + " loading " + moduleClass + " modulePriority " + modulePriority + " " + moduleEnabled);
                loadModule(moduleClass, name, modulePriority, moduleEnabled, completBeforeStartNext);
                MRLog.info(moduleName + " loaded " + moduleClass + " modulePriority " + modulePriority + " " + moduleEnabled);
            }
        }
    }

    private void loadModule(String module, String ipvsModuleName, int modulePriority, boolean moduleEnabled, boolean completBeforeStartNext) {
        if (shuttingDown)
            return;
        try {
            Class<?> modClass = loader.loadClass(module);
            IpvsServModule mod = (IpvsServModule) modClass.newInstance();
            //todo: support same class with multiple modules
            mod.setIpvsModuleName(ipvsModuleName);
            mod.setModulePriority(modulePriority);
            mod.setModuleEnabled(moduleEnabled);
            mod.setCompletBeforeStartNext(completBeforeStartNext);
            this.modulesByName.put(ipvsModuleName, mod);
        } catch (Exception ex) {
            MRLog.error("ERROR: loadModule " + ipvsModuleName + "  " + module, ex);
        }
    }

    /////////////////////////initModule/////////////////////////
    private void initModules(Map<String, Map<String, String>> modules) {
        if (shuttingDown)
            return;
        for (int i = 1; i < priorityLevels; i++) {
            initModules(modules, i);
        }
    }

    private void initModules(Map<String, Map<String, String>> modules, int priority) {
        if (shuttingDown)
            return;
        String[] arr = new String[modulesByName.size()];
        arr = modulesByName.keySet().toArray(arr);
        for (String moduleName : arr) {
            IpvsServModule module = modulesByName.get(moduleName);
            if (module == null)
                continue;
            boolean isInitialized = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        MRLog.info("initializing " + moduleName + " " + module.getClass().getName());
                        //module.initialize( IpvsServGlobals.getModuleParams( module.getIpvsModuleName() ) );
                        module.initialize(modules.get(moduleName));
                        MRLog.info("initialized " + moduleName + " " + module.getClass().getName());
                        isInitialized = true;
                    } else {
                        MRLog.info("skip initializing- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                // Remove the failed initialized module
                this.modulesByName.remove(moduleName);
                if (isInitialized) {
                    try {
                        module.stop();
                        module.destroy();
                    } catch (Exception ex2) {
                        MRLog.error(ex2);
                    }
                }
                MRLog.error("ERROR: initModules " + moduleName + " ", ex);
            }
        }
    }

    /////////////////////////startModules/////////////////////////
    private void startModules() {
        if (shuttingDown)
            return;
        //MRLog.info("starting Modules. number of modules " + modulesByName.size());
        for (int i = 1; i < priorityLevels; i++) {
            startModules(i);
        }
        //MRLog.info("started Modules. number of modules " + modulesByName.size());
    }

    private void startModules(int priority) {
        if (shuttingDown)
            return;
        String[] arr = new String[modulesByName.size()];
        arr = modulesByName.keySet().toArray(arr);
        for (String moduleName : arr) {
            IpvsServModule module = modulesByName.get(moduleName);
            if (module == null)
                continue;
            boolean started = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        //MRLog.info("starting " + moduleName + " " + module.getClass().getName());
                        module.start();
                        MRLog.info("started " + moduleName + " " + module.getClass().getName());
                        started = true;
                    } else {
                        MRLog.info("skip starting- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                if (started && module != null) {
                    try {
                        module.stop();
                        module.destroy();
                    } catch (Exception ex2) {
                        MRLog.error(ex2);
                    }
                }
                MRLog.error("ERROR: startModules " + moduleName + " " + modulesByName, ex);
            }
        }
    }

    /////////////////////////startModules/////////////////////////
    private void stopModules() {
        // Get all modules and stop and destroy them
        for (int i = priorityLevels; i > 0; i--) {
            stopModules(i);
        }

    }

    private void stopModules(int priority) {
        String[] arr = new String[modulesByName.size()];
        arr = modulesByName.keySet().toArray(arr);
        for (String moduleName : arr) {
            IpvsServModule module = modulesByName.get(moduleName);
            if (module == null)
                continue;
            //boolean stopped = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        //MRLog.info("stop " + module.getClass().getName());
                        try {
                            module.stop();
                        } catch (Exception ex2) {
                            MRLog.error(ex2);
                        }
                        try {
                            module.destroy();
                        } catch (Exception ex2) {
                            MRLog.error(ex2);
                        }
                        MRLog.info("stopped " + module.getClass().getName());
                        //stopped=true;
                    } else {
                        MRLog.info("skip stopping- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                MRLog.error("ERROR: stopModules", ex);
            }
        }
    }

    //////////////////////////////////////// modules /////////////////////////////////////

    //////////////////////////////////////// initialize/start/stop /////////////////////////////////////
    public void initialize() {
        priorityLevels = IpvsServGlobals.getIntProperty("modules.priorityLevels", 10);

        MRLog.setLogToFile(IpvsServGlobals.getXMLProperty("logs.enable.logToFile", true));
        MRLog.setLogLevels();
        //other logs
        System.setProperty("org.mortbay.log.class", "com.ipvsserv.common.log.JettyLog");
        // JSP 2.0 uses commons-logging, so also override that implementation.
        System.setProperty("org.apache.commons.logging.LogFactory", "com.ipvsserv.common.log.CommonsLogFactory");

        // Listen to system property events
        if (isStandAlone()) {
            Runtime.getRuntime().addShutdownHook(new ShutdownHookThread());
        }
        ////////////////////////////////////
        name = IpvsServGlobals.getXMLProperty("xmpp.domain", "127.0.0.1").toLowerCase();
        try {
            host = InetAddress.getLocalHost().getHostName();
        } catch (UnknownHostException ex) {
            MRLog.warn("ERROR: Unable to determine local hostname.", ex);
        }
        if ("true".equals(IpvsServGlobals.getXMLProperty("setup"))) {
            setupMode = false;
        }

        int major = IpvsServGlobals.getXMLProperty("version.major", 1);
        int minor = IpvsServGlobals.getXMLProperty("version.minor", 1);
        int micro = IpvsServGlobals.getXMLProperty("version.micro", 1);
        String status = IpvsServGlobals.getXMLProperty("version.status", "RCAA");
        int statusVersion = IpvsServGlobals.getXMLProperty("version.statusVersion", 1);

        startDate = new Date();
        version = new Version(major, minor, micro, ReleaseStatus.valueOf(status), statusVersion);
        xmppServerInfo = new IpvsServInfoImpl(host, host, version, startDate);

        ////////////////////////////////////
        loader = getIpvsClassLoader();
        //System.out.println("aaa "+loader.getName());
        initialized = true;
    }

    public Object getMRClass() {
        return mrclass;
    }

    public String getServerVersion() {
        return serverVersion;
    }

    public void loadMRClass() {
        try {
            if (loader == null) {
                loader = getIpvsClassLoader();
            }
            /*String[] libDirs = IpvsServGlobals.getValues("library.lib");
            if (libDirs != null) {
                for (int i = 0; i < libDirs.length; i++) {
                    String libFolder = libDirs[i];
                    loader.addDirectory(libFolder);
                }
            }*/
            //mrclass = getIpvsClassLoader().loadClass("com.ipvs.test.TestDBCache").newInstance();
            try {
                Document doc = Dom4jUtils.loadXMLResource(SERVER_VERSION_XML);//, mrclass.getClass());
                MRLog.debug("doc " + doc.asXML());
                serverVersion = Dom4jUtils.getAttributeValue(doc.getRootElement(), "Ver", "");

            } catch (Exception ex) {
                ex.printStackTrace();
            }
            MRLog.debug("serverVersion " + serverVersion);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void start() {
        try {
            loadMRClass();
            boolean enableStatus = true;
            if (IpvsServGlobals.getIpvsAppName().equalsIgnoreCase(IpvsServGlobals.APP_IPVS_COMPONENT)) {
                enableStatus = false;
            }

            initializingFile = new IpvsFileStatus(IpvsFileStatus.STATUS_INITIALIZING_FILE, enableStatus);
            initializingFile.isStatusFileExists(true);

            runningFile = new IpvsFileStatus(IpvsFileStatus.STATUS_RUNNIG_FILE, enableStatus);
            runningFile.init(true);

            shuttingDown = false;
            setServerState(SERVER_STARTING);
            initialize();
            startDate = new Date();
            // If the server has already been setup then we can start all the server's modules
            if (!setupMode) {
                verifyDataSource();
                // First load all the modules so that modules may access other modules while
                // being initialized
                Map<String, Map<String, String>> modules = IpvsServGlobals.getModules();
                loadModules(modules);
                initModules(modules);
                startModules();
            }
            // Log that the server has been started
            String startupBanner = "started " + IpvsServGlobals.getIpvsAppName() + "  " + version.getVersionString() + " [" + IpvsServGlobals.formatDateTime(new Date()) + "]";
            MRLog.debug(startupBanner);
            started = true;

        } catch (Exception ex) {
            MRLog.error("ERROR: start ", ex);
            shutdownServer();
        } finally {
            //MRLog.debug("calling initializingFile  removeStatusFile  isFileCreated ? " + initializingFile.isFileCreated());
            initializingFile.removeStatusFile(true);
            if (started) {
                setServerState(SERVER_STARTED);
            }
            if (shuttingDown) {
                if (!IpvsServGlobals.isIpvsComponetApp()) {
                    stop();
                }
            }
        }
    }

    public boolean isStarted() {
        if(!started){
            return started;
        }
        /*if(!DBAdmin.getDBAdminApi().isDBModuleStarted()){
            return false;
        }     
        DBMigrationModule inst =(DBMigrationModule)IpvsServer.getInstance().getModule(DBMigrationModule.class);
        if(inst ==null || !inst.isCompleted()){
            return false;
        } */ 
        return started;
    }
    public boolean isDBReady() {
        if(!started){
            return started;
        }
        if(!DBAdmin.getDBAdminApi().isDBModuleInited()){
            dbFailedError="DB not inited";
            return false;
        }          
        if(!DBAdmin.getDBAdminApi().isDBModuleStarted()){
            dbFailedError="DB not connected";
            return false;
        }     
        DBMigrationModule inst =(DBMigrationModule)IpvsServer.getInstance().getModule(DBMigrationModule.class);
        if(inst==null){
            inst =(DBMigrationFileSystemModule)IpvsServer.getInstance().getModule(DBMigrationFileSystemModule.class);
        }
        if(inst==null ){
            dbFailedError="DB migration pending";;
            return false;
        }  
        else if( !inst.isCompleted()){
            dbFailedError=inst.getMigrationError();
            return false;
        }  
        return started;
    }
    public String getDBFailedError() {
        return dbFailedError;
    }
    //called from other interfaces 
    public void finishSetup() {
        if (!setupMode) {
            return;
        }
        if ("true".equals(IpvsServGlobals.getXMLProperty("setup"))) {
            //todo:
        }
    }

    public void restart() {
        if (isStandAlone() && isRestartable()) {
            try {
                MRLog.debug(name + "   initialized " + initialized + " started  " + started + " " + host);
                Class<?> wrapperClass = Class.forName(WRAPPER_CLASSNAME);
                Method restartMethod = wrapperClass.getMethod("restart", (Class[]) null);
                restartMethod.invoke(null, (Object[]) null);
            } catch (Exception ex) {
                MRLog.error("ERROR: Could not restart container", ex);
            }
        }
    }

    public void stop() {
        if (shuttingDown)
            return;
        // Only do a system exit if we're running standalone
        if (isStandAlone()) {
            // if we're in a wrapper, we have to tell the wrapper to shut us down
            if (isRestartable()) {
                try {
                    Class<?> wrapperClass = Class.forName(WRAPPER_CLASSNAME);
                    Method stopMethod = wrapperClass.getMethod("stop", Integer.TYPE);
                    stopMethod.invoke(null, 0);
                } catch (Exception ex) {
                    MRLog.error("ERROR: Could not stop container", ex);
                }
            } else {
                shutdownServer();
                Thread shutdownThread = new ShutdownThread();
                shutdownThread.setDaemon(true);
                shutdownThread.start();
            }
        } else {
            // Close listening socket no matter what the condition is in order to be able
            // to be restartable inside a container.
            shutdownServer();
        }
    }

    //////////////////////////////////////// initialize/start/stop /////////////////////////////////////

    /////////////////// todo /////////////////////////////////
    public boolean isSetupMode() {
        return setupMode;
    }

    public boolean isRestartable() {
        boolean restartable;
        try {
            restartable = Class.forName(WRAPPER_CLASSNAME) != null;
        } catch (ClassNotFoundException exx) {
            restartable = false;
        }
        return restartable;
    }

    public boolean isStandAlone() {
        boolean standalone;
        try {
            standalone = Class.forName(STARTER_CLASSNAME) != null;
        } catch (ClassNotFoundException e) {
            standalone = false;
        }
        return standalone;
    }

    private void verifyDataSource() {
        //todo: tet db is ok  as per the ipvs schema
    }

    ///////////////////////////////// init /////////////////////////////
    //////////////////////////////////////////// shut down ////////////////////////////
    private class ShutdownThread extends Thread {
        @Override
        public void run() {
            //Shuts down the JVM after a 5 second delay.
            try {
                int sleep = IpvsServGlobals.getIntProperty("ShutdownThread.sleepSecsBeforeShutdown", 0);
                if (sleep > 0) {
                    Thread.sleep(sleep * 1000);
                }
                // No matter what, we make sure it's dead
                System.exit(0);
            } catch (InterruptedException ex) {
                // Ignore.
            }
        }
    }

    /**
     * Makes a best effort attempt to shutdown the server
     */
    private void shutdownServer() {
        setServerState(SERVER_STOPPING);
        try {
            shuttingDown = true;
            // If we don't have modules then the server has already been shutdown
            if (modulesByName.isEmpty()) {
                return;
            }
            stopModules();
            modulesByName.clear();
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            try {
                setServerState(SERVER_STOPPED);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            initializingFile.removeStatusFile(true);
            runningFile.removeStatusFile(true);
            MRLog.info(IpvsServGlobals.getIpvsAppName() + " stopped");
        }
    }

    public boolean isShuttingDown() {
        return shuttingDown;
    }

    private class ShutdownHookThread extends Thread {
        @Override
        public void run() {
            MRLog.info("Server halting startDate  " + startDate);
            shutdownServer();
            MRLog.info("Server halted startDate  " + startDate);
        }
    }

    //////////////////////////////////////////// shut down ////////////////////////////
    public IpvsServModule getModule(String modeName) {
        return modulesByName.get(modulesByName);
    }

    private IpvsServModule getModuleByClassName(String className) {
        for (String moduleName : modulesByName.keySet()) {
            try {
                IpvsServModule module = modulesByName.get(moduleName);
                if (module.getClass().getName() == className)
                    return module;
            } catch (Exception ex) {
            }
        }
        return null;
    }

    public IpvsServModule getModule(Class<?> cls) {
        return getModuleByClassName(cls.getName());
    }

    /*public XMPPExtComponentModule getMCModule() {
        return (XMPPExtComponentModule)IpvsServer.getInstance().getModule(XMPPExtComponentModule.class);
    }

    public SednaNativeModule getSednaModule() {
        return (SednaNativeModule)IpvsServer.getInstance().getModule(SednaNativeModule.class);
    }
       
    public ExistXmlDBModule getExistXmlDBModule() {
        return (ExistXmlDBModule)IpvsServer.getInstance().getModule(ExistXmlDBModule.class);
    }
    
    public SednaXmlDBModule getSednaXmlDBModule() {
        return  (SednaXmlDBModule)IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
    }
    
    public XmlDBModule getXmlDBModule() {
        return (XmlDBModule)IpvsServer.getInstance().getModule(XmlDBModule.class);
    }
    public JettyModule getJettyModule() {
        return (JettyModule)IpvsServer.getInstance().getModule(JettyModule.class);   

    }
    public PluginManager getPluginManager() {
      return (PluginManager)IpvsServer.getInstance().getModule(PluginManager.class);   
    }  */
    /////////////////////// new methods todo: //////////////
    public IpvsServInfo getServerInfo() {
        return xmppServerInfo;
    }

    public void setManager(ExternalComponentManager manager) {
        this.manager = manager;

    }

    public IpvsDBDriver getDefaultIpvsDBDriver() {
        if (db == null) {
            try {
                /* if (IpvsServGlobals.getIpvsAppName().equalsIgnoreCase(IpvsServGlobals.APP_EXIST_DBTEST)) {
                     //todo:
                     db = getExistXmlDBModule().getDefaultIpvsDBDriver();
                     MRLog.debug(name + "db " + (db == null));
                 } else if (IpvsServGlobals.getIpvsAppName().equalsIgnoreCase(IpvsServGlobals.APP_SEDNA_NATIVE_DBTEST)) {
                     //todo:
                     db = getSednaModule().getDefaultIpvsDBDriver();
                     MRLog.debug(name + "db " + (db == null));
                 } else if (IpvsServGlobals.getIpvsAppName().equalsIgnoreCase(IpvsServGlobals.APP_SEDNA_XMLDB_DBTEST)) {
                     //todo:
                     db = getSednaXmlDBModule().getDefaultIpvsDBDriver();
                     MRLog.debug(name + "db " + (db == null));
                 } else {
                     //todo:
                     MRLog.debug(name + "IpvsServGlobals.getIpvsAppName()  " + IpvsServGlobals.getIpvsAppName());
                     db = IpvsDBDriverFactory.getIpvsDBDriver("");
                 }*/
                db = IpvsDBDriverFactory.getIpvsDBDriver(IpvsServGlobals.APP_SEDNA_NATIVE_DBTEST);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return db;
    }

    ////////////////////////////////////////////////////////////////////////////

    public void addServerEventListener(ServerEventListener serverEventListener) {
        synchronized (serverEventListeners) {
            serverEventListeners.add(serverEventListener);
        }
    }

    public void removeServerEventListener(ServerEventListener serverEventListener) {
        synchronized (serverEventListeners) {
            serverEventListeners.remove(serverEventListener);
        }
    }

    public void setServerState(int serverState) {
        synchronized (serverEventListeners) {
            this.serverState = serverState;
        }
        for (Iterator<ServerEventListener> iterator = serverEventListeners.iterator(); iterator.hasNext();) {
            try {
                ServerEventListener serverEventListener = iterator.next();
                switch (serverState) {
                case SERVER_STARTED:
                    serverEventListener.serverStarted();
                    break;
                case SERVER_STARTING:
                    serverEventListener.serverStarting();
                    break;
                case SERVER_STOPPING:
                    serverEventListener.serverStopping();
                    break;
                case SERVER_STOPPED:
                    serverEventListener.serverStopped();
                    break;
                case SERVER_RESTARTED:
                    serverEventListener.serverRestarted();
                    break;
                case SERVER_RESTARTING:
                    serverEventListener.serverRestarting();
                    break;
                default:
                    break;
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }

    }

    public int getServerState() {
        return serverState;
    }

}

/*private static String getCurrentFolderPath() {
    String path = "";
    try {
        String name = "temp.log";
        File file = new File(name);
        FileWriter os = new FileWriter(file, false);
        os.write(" Server started " + (new Date()) + " " + System.currentTimeMillis());
        path = file.getCanonicalPath().replace('\\', '/');
        if (path.indexOf(name) != 1) {
            path = path.substring(0, path.indexOf(name));
            if (path.endsWith("/")) {
                path = path.substring(0, path.length() - 1);
            }
        }
    } catch (Exception ex) {

    }
    MRLog.debug("getCurrentFolderPath " + path);
    return path.replace('\\', '/');
}*/

/*public void restartHTTPServer() {
    Thread restartThread = new Thread() {
        public void run() {
            if (isStandAlone()) {
                // Restart the HTTP server manager. This covers the case
                // of changing the ports, as well as generating self-signed certificates.
                
                // Wait a short period before shutting down the admin console.
                // Otherwise, this page won't render properly!
                try {
                    Thread.sleep(1000);
                    //((AdminConsolePlugin) pluginManager.getPlugin("admin")).shutdown();
                    // ((AdminConsolePlugin) pluginManager.getPlugin("admin")).startup();
                } catch (Exception ex) {
                    MRLog.error(ex)
                }
            }
        }
    };
    restartThread.setContextClassLoader(loader);
    restartThread.start();
}*/
/////////////////// todo /////////////////////////////////
///////////////////////////////// init /////////////////////////////
/* private String getConfFile(String home, String fileName) {
    if (home == null || home.equals("")) {
        return (fileName).replace('/', File.separatorChar);
    }
    return (home + File.separatorChar + fileName).replace('/', File.separatorChar);
}

private boolean verifyPropertyFile(String jiveConfigName) throws FileNotFoundException {
    File configFile = new File(jiveConfigName);
    if (!configFile.exists()) {
        MRLog.error("does not exists? ipvscomponentHome " + ipvscomponentHome);
        MRLog.error("does not exists? ipvs.properties " + jiveConfigName);
        throw new FileNotFoundException();
    }
    return true;
}*/

/*private String getHome() {
    ipvscomponentHome = System.getProperty("ipvscomponentHome");
    //if (applicationHome == null || applicationHome.equals("")) {
    //    applicationHome = getCurrentFolderPath();
    //}
    MRLog.info("applicationHome " + ipvscomponentHome);
    ipvscomponentHome = ipvscomponentHome.replace('\\', File.separatorChar);
    if (ipvscomponentHome.endsWith(File.separatorChar + "")) {
        ipvscomponentHome = ipvscomponentHome.substring(0, ipvscomponentHome.length() - 1);
    }
    System.setProperty("ipvscomponentHome", ipvscomponentHome);
    return ipvscomponentHome;
}*/
