package com.ipvsserv.nxd.api.xmldb;

import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import org.xmldb.api.DatabaseManager;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.Database;
import org.xmldb.api.base.XMLDBException;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.IpvsDBDriverFactory;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServBasicModule;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
//keep it abstract until its being used for any xmldb need arises other than sedna / exist
public abstract class XmlDBModule extends IpvsServBasicModule implements XmlDBModuleI {
    protected Vector<Collection> freeConnections = new Vector<Collection>();
    protected int checkOutCount = 0;
    protected long timeout = 30 * 1000;
    protected int corePoolSize = 5;
    protected int maxPoolSize = 5;

    protected String host;
    protected String db;
    protected String user;
    protected String pwd;
    protected String driver = "";
    protected boolean isRegisteredDriver = false;
    protected String databaseurl = "";
    protected ServiceManagerWrapper serviceManager = new ServiceManagerWrapper();
    protected IpvsFileStatus databaseConnectionStatus = null;

    public int getMaxPoolSize() {
        return maxPoolSize;
    }

    public void setMaxPoolSize(int maxPoolSize) {
        this.maxPoolSize = maxPoolSize;
    }

    public int getCorePoolSize() {
        return corePoolSize;
    }

    public void setCorePoolSize(int corePoolSize) {
        this.corePoolSize = corePoolSize;
    }

    public XmlDBModule() {
        super("XmlDB Pool");
    }

    public XmlDBModule(String moduleName) {
        super(moduleName);
    }

    public int getCheckInCount() {
        if (corePoolSize > checkOutCount) {
            return corePoolSize - checkOutCount;
        }
        return 0;
    }

    public int getCheckOutCount() {
        return checkOutCount;
    }

    public String getDriver() {
        return driver;
    }

    public void setDriver(String driver) {
        this.driver = driver;
    }

    public ServiceManagerWrapper getServiceManager() {
        return serviceManager;
    }

    public void setServiceManager(ServiceManagerWrapper serviceManager) {
        this.serviceManager = serviceManager;
    }

    private synchronized boolean isConnectionAvailable() {
        if (checkOutCount < maxPoolSize) //corePoolSize)
            return true;
        return false;
    }

    protected String getAudit() {
        return name + " (out:" + checkOutCount + " cached:" + freeConnections.size() + " core:" + corePoolSize + " max:" + maxPoolSize + ")";
    }

    ///////////////////////////// checkOut ///////////////////////////// 
    public Collection checkOutLowPriority() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if (MRLog.isConnectionpoolLogEnable()) {
            MRLog.debug("checkOutLowPriority request queue :" + getAudit());
        }
        return checkOut(false);
    }

    public Collection checkOut() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if (MRLog.isConnectionpoolLogEnable()) {
            MRLog.debug("checkOut request queue :" + getAudit());
        }
        return checkOut(true);
    }

    protected synchronized Collection checkOut(boolean bpriority) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        while (!isConnectionAvailable()) {
            try {
                if (getLastKnownState() == UNHEALTHY) {
                    MRLog.debug("DB Module is not in improper state=" + state + getAudit());
                    throw new DBNotConnectedException("DB Module is not in improper state=" + state + getAudit());
                }
            } catch (Exception ex) {
                MRLog.debug("DB Module is not in improper state=" + state + getAudit() + " " + ex.getMessage());
                throw new DBNotConnectedException("DB Module is not in improper state=" + state + getAudit() + " " + ex.getMessage());
            }
            if (bpriority) {
                long timeElapsed = System.currentTimeMillis() - t0;
                if (timeElapsed > timeout) { // a minute 
                    MRLog.debug("Connection Pool is not available for last milsec " + timeElapsed + getAudit());
                    throw new DBConnectTimedoutException("Checkout timed out waiting for free connection. " + getAudit());
                }
                wait(5 * 1000);
            } else {
                break;
            }
        }
        if (isConnectionAvailable()) {
            Collection con = getConnection();
            try {
                if (MRLog.isConnectionpoolLogEnable()) {
                    MRLog.debug("checkOut inside queue :" + getAudit());
                }
                checkOutCount++;
                return con;
            } catch (Exception ex) {
                throw new DBNotConnectedException("DB Connection failed. " + getAudit() + " " + ex.getMessage());
            }
        }
        throw new DBNotConnectedException("DB Connection failed. " + getAudit());
    }

    public synchronized void checkIn(Collection con, boolean kill) {//throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (con == null) {
                return;
            }
            if (kill || checkOutCount > corePoolSize) {
                try {
                    con.close();
                } catch (Exception ex) {
                }
                try {
                    con = null;
                    if (checkOutCount < corePoolSize) {
                        con = getConnection();
                    }
                } catch (Exception ex) {
                    MRLog.error("Exception ConnectionPool checkIn. " + getAudit(), ex);
                }
            }
            checkOutCount--;
            if (con != null) {
                freeConnections.add(con);
            }
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("checkin :" + getAudit());
            }
        } catch (Exception ex) {
            MRLog.error("Exception ConnectionPool checkIn " + getAudit(), ex);
        } finally {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("checkIn:" + getAudit());
            }
            notify();
        }
    }

    ///////////////////////////// checkOut ///////////////////////////// 

    ///////////////////////////// initialize / shutdown ///////////////////////////// 

    protected synchronized void registerDriver() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (isRegisteredDriver){
                return;
            }
            Class cl = Class.forName(getDriver());
            Database database = (Database) cl.newInstance();
            Map<String, String> params = IpvsServGlobals.getParams(getIpvsModuleName() + ".databaseProperties");
            for (Iterator<String> iterator = params.keySet().iterator(); iterator.hasNext();) {
                String param = iterator.next();
                String value = params.get(param);
                if (value != null) {
                    database.setProperty(param, value);
                }
            }
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("registerDriver :" + name + " " + getDriver());
            }

            DatabaseManager.registerDatabase(database);
            isRegisteredDriver = true;
        } catch (Exception ex) {
            MRLog.error(ex);
            MRLog.error(""+ex.getClass().getName()+": " + ex.getMessage());
            throw new DBNotConnectedException("registerDatabase failed. " + ex.getMessage());
        }        
    }

    protected void fillServicesList() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        Collection col = checkOut();
        try {
            serviceManager.fillServicesList(col);
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("fillServicesList " + name);
            }
        } finally {
            checkIn(col, false);
        }
    }

    protected synchronized void createPool() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        initialized = false;
        started = false;
        //register xml db
        if (!isRegisteredDriver)
            registerDriver();

        if (freeConnections == null) {
            freeConnections = new Vector<Collection>();
        }
        if (checkOutCount == 0 && freeConnections.size() == 0) {
            Collection con = getConnection();
            if (con != null) {
                freeConnections.add(con);
                if (MRLog.isConnectionpoolLogEnable()) {
                    MRLog.debug("createPool." + getAudit());
                }
            } else {
                MRLog.debug("FAILED  createPool." + getAudit());
            }
        }
        //query all supported xml db services and their versions 
        fillServicesList();
        initialized = true;
        started = true;
    }

    ////////////////////////// remove pool
    protected synchronized void deregisterDatabases() {
        try {
            Database[] databases = DatabaseManager.getDatabases();
            if (databases == null || databases.length == 0)
                return;
            for (int i = 0; i < databases.length; i++) {
                Database database = databases[i];
                try {
                    DatabaseManager.deregisterDatabase(database);
                } catch (XMLDBException ex) {
                    MRLog.error(ex);
                }
            }
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("deregisterDatabases." + name + " " + getDriver());
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    protected synchronized void removePool() {
        //deregisterDatabases
        deregisterDatabases();
        try {
            while (freeConnections.size() > 0) {
                Collection con = null;
                try {
                    con = freeConnections.remove(0);
                    con.close();
                    if (MRLog.isConnectionpoolLogEnable()) {
                        MRLog.debug("Closed connection form pool. " + getAudit());
                    }
                } catch (Exception ex) {
                    MRLog.error("Can't close connection from  pool." + getAudit(), ex);
                }
                con = null;
            }
        } catch (Exception ex) {
            MRLog.error("ConnectionPool release." + getAudit(), ex);
        }
        freeConnections.removeAllElements();
        checkOutCount = 0;
        initialized = false;
        started = false;
    }

    ///////////////////////////// initialize / shutdown///////////////////////////// 

    ///////////////////////////// getConnection///////////////////////////// 

    protected Collection getConnection() throws DBNotConnectedException {
        if (!isRegisteredDriver){
            try {
                registerDriver();
            } catch (Exception e) {
                e.printStackTrace();
            }  
        }
        if(manualRestarting){
            MRLog.debug("Database in being Restarted Manually ");
            throw new DBNotConnectedException("Database in being Restarted Manually ");
        }
        long t0 = System.currentTimeMillis();
        Collection col = null;
        try {
            if (freeConnections.size() > 0) {
                if (MRLog.isConnectionpoolLogEnable()) {
                    MRLog.debug("get old Connection. " + getAudit());
                }
                return freeConnections.remove(0);
            }
            col = getConnection(databaseurl, db, user, pwd);
            databaseConnectionStatus.removeStatusFile();
        } finally {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("getConnection: " + (System.currentTimeMillis() - t0) + " msec" + getAudit());
            }
        }
        return col;
    }

    protected Collection getConnection(String url, String DBName, String user, String password) throws DBNotConnectedException {
        try {
            return DatabaseManager.getCollection(url, user, password);
        } catch (XMLDBException ex) {
            String error = ex.getMessage();
            if (error.indexOf("Could not connect") != -1) {
                initialized = false;
            } else if (error.indexOf("SE4409") != -1) {
                initialized = false;
            } else if (error.indexOf("Could not get a connection within") != -1) {
                if (error.indexOf("connections already open") != -1) {
                    initialized = false;
                } else {
                    initialized = false;
                }
            }
            MRLog.error("XMLDBException: " + ex.getMessage());
            MRLog.error(ex);
            databaseConnectionStatus.createStatusFile();
            throw new DBNotConnectedException("XMLDB getConnection failed." + getAudit());
        } catch (Exception ex) {
            MRLog.error("Exception: " + ex.getMessage());
            MRLog.error(ex);
            databaseConnectionStatus.createStatusFile();
            throw new DBNotConnectedException("XMLDB getConnection failed." + getAudit());
        }
    }

    ///////////////////////////// getConnection///////////////////////////// 

    //////////////////// module /////////////////////////////////////
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            corePoolSize = CompUtils.intValue(params.get("corePoolSize"), 1);
            maxPoolSize = CompUtils.intValue(params.get("maxPoolSize"), 1);
            host = params.get("host");
            db = params.get("database");
            user = params.get("user");
            pwd = params.get("pwd");
            databaseurl = params.get("databaseurl");
            timeout = CompUtils.intValue(params.get("getconn_timeout_msec"), 1200000);
            if (MRLog.isConnectionpoolLogEnable() || MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug(" initialize databaseurl :" + databaseurl + " " + getAudit());
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    protected void startFileStatus() {
        databaseConnectionStatus = new IpvsFileStatus(IpvsFileStatus.STATUS_DB_FAILED_FILE);
        databaseConnectionStatus.init(true);
    }

    @Override
    public void start() throws Exception {
        try {
            startFileStatus();
            super.startMonitor();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void stop() throws Exception {
        try {
            destroy();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void destroy() throws Exception {
        super.destroy();
        removePool();
    }

    //////////////////// module /////////////////////////////////////
    protected boolean reconnecting = false;
    protected int errorCount = 0;

    @Override
    public void verifyState() throws Exception {
        if (getLastKnownState() == UNHEALTHY) {
            // Ensure that only one thread will try to reconnect.
            synchronized (this) {
                if (reconnecting) {
                    return;
                }
                reconnecting = true;
            }
            try {
                if(!manualRestarting){
                    createPool();
                }                
            } catch (DBNotConnectedException ex) {
                String error = ex.getMessage();
                errorCount++;
                if (errorCount > 30) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState db connect. " + error + getAudit());
                    //throw ex;
                }
            } catch (DBConnectTimedoutException ex) {
                String error = ex.getMessage();
                errorCount++;
                if (errorCount > 30) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState db connect. " + error + ". " + getAudit());
                    //throw ex;
                }
            } catch (Exception ex) {
                String error = ex.getMessage();
                errorCount++;
                if (errorCount > 30) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState db connect. " + error + ". " + getAudit());
                }
            } finally {
                reconnecting = false;
            }
        }
    }
    
    protected boolean manualRestarting = false;
    public  synchronized void setManualRestarting(boolean restarting) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if (restarting) {
            manualRestarting = true;
            isRegisteredDriver=false;
            removePool();
        } else {
            manualRestarting = false;
            createPool();
        }
    }

    @Override
    public int getLastKnownState() throws Exception {
        if (freeConnections == null) {
            state = UNHEALTHY;
        } else if (freeConnections.size() == 0 && checkOutCount == 0) {
            state = UNHEALTHY;
        } else if (!initialized) {
            state = UNHEALTHY;
        } else {
            state = HEALTHY;
        }
        return state;
    }

    /////////////////////////////////////

    public IpvsDBDriver getDefaultIpvsDBDriver() throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriverVendor(IpvsDBDriverFactory.XMLDB);
    }

    public IpvsDBDriver getDefaultIpvsDBDriver(String parentIpvsModuleName) throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriver(parentIpvsModuleName, IpvsDBDriverFactory.XMLDB);
    }
}
