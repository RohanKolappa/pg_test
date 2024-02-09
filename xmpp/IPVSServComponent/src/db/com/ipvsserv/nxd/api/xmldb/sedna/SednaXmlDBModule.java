package com.ipvsserv.nxd.api.xmldb.sedna;

import java.util.Iterator;
import java.util.Map;

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
import com.ipvsserv.nxd.api.xmldb.ServiceManagerWrapper;
import com.ipvsserv.nxd.api.xmldb.XmlDBModule;
import com.ipvsserv.server.IpvsFileStatus;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SednaXmlDBModule extends XmlDBModule {
    protected String driver = "";
    protected int checkOutCount = 0;
    protected String host = "";
    protected String user = "";
    protected String pwd = "";
    protected String databaseName = "";
    protected String databaseurl = "";

    protected long timeout = 30 * 1000; //2 minutes 
    protected Database database = null;
    protected int corePoolSize = 1;
    private int maxPoolSize = 5;

    protected ServiceManagerWrapper serviceManager = new ServiceManagerWrapper();

    public SednaXmlDBModule() {
        super("XmlDB sedna Pool");
        setDriver("net.cfoster.sedna.DatabaseImpl");
        //enableFreeConnectionsCache=false;
    }

    public SednaXmlDBModule(String moduleName) {
        super(moduleName);
        setDriver("net.cfoster.sedna.DatabaseImpl");
    }

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

    public int getCheckInCount() {
        if (corePoolSize > checkOutCount) {
            return corePoolSize - checkOutCount;
        }
        return 0;
    }

    public int getCheckOutCount() {
        return checkOutCount;
    }

    private synchronized boolean isConnectionAvailable() {
        if (checkOutCount < maxPoolSize) //corePoolSize)
            return true;
        return false;
    }

    protected String getAudit() {
        return name + " (out:" + checkOutCount + " cached:" + freeConnections.size() + " core:" + corePoolSize + " max:" + maxPoolSize + ")";
    }

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
        long to = System.currentTimeMillis();
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
                long timeElapsed = System.currentTimeMillis() - to;
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
            if (con != null) {
                checkOutCount--;
                try {
                    con.close();
                    con = null;
                } catch (Exception ex) {
                    MRLog.error("Exception ConnectionPool checkIn " + getAudit(), ex);
                }
            }
        } catch (Exception ex) {
            MRLog.error("Exception ConnectionPool checkIn " + getAudit(), ex);
        } finally {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("checkIn :" + getAudit());
            }
            notify();
        }
    }

    ///////////////////////////// initialize / shutdown ///////////////////////////// 
    protected boolean isRegisteredDriver = false;

    protected synchronized void registerDriver() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
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
            DatabaseManager.registerDatabase(database);
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("registerDriver:" + name + " " + getDriver());
            }
            isRegisteredDriver = true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            MRLog.error("XMLDBException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB registerDatabase failed. " + ex.getMessage());
        } catch (ClassNotFoundException ex) {
            MRLog.error(ex);
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB registerDatabase failed. " + ex.getMessage());
        } catch (InstantiationException ex) {
            MRLog.error(ex);
            MRLog.error("InstantiationException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB registerDatabase failed. " + ex.getMessage());
        } catch (IllegalAccessException ex) {
            MRLog.error(ex);
            MRLog.error("IllegalAccessException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB registerDatabase failed. " + ex.getMessage());
        }
    }

    protected void fillServicesList() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        Collection col = checkOut();
        try {
            serviceManager.fillServicesList(col);
        } finally {
            checkIn(col, false);
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("fillServicesList:" + name + " " + getDriver());
            }
        }
    }

    protected synchronized void createPool() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        initialized = false;
        started = false;
        try {
            registerDriver();
            checkOutCount = 0;
            fillServicesList();
            initialized = true;
            started = true;
        } catch (Exception ex) {
            MRLog.error(ex);
            MRLog.error("XMLDBException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB registerDatabase failed. " + ex.getMessage());
        }
    }

    ///////////////////////////// initialize / shutdown ///////////////////////////// 
    protected synchronized void removePool() {
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
        checkOutCount = 0;
        initialized = false;
        started = false;
    }

    ///////////////////////////// initialize / shutdown///////////////////////////// 

    ///////////////////////////// getConnection///////////////////////////// 
    protected Collection getConnection() throws DBNotConnectedException {
        long t0 = System.currentTimeMillis();
        Collection col = null;
        try {
            col = getConnection(databaseurl, databaseName, user, pwd);
            databaseConnectionStatus.removeStatusFile();
        } finally {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("getConnection: " + (System.currentTimeMillis() - t0) + " msec" + getAudit());
            }
        }
        return col;
    }

    ///////////////////////////// getConnection///////////////////////////// 
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            corePoolSize = CompUtils.intValue(params.get("corePoolSize"), 1);
            maxPoolSize = CompUtils.intValue(params.get("maxPoolSize"), 1);
            host = params.get("host");
            databaseName = params.get("database");
            user = params.get("user");
            pwd = params.get("pwd");
            databaseurl = params.get("databaseurl");
            if (MRLog.isConnectionpoolLogEnable() || MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug(name + " initialize databaseurl :" + databaseurl + "");
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
                createPool();
            } catch (DBNotConnectedException ex) {
                String error = ex.getMessage();
                if (errorCount > 500) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState " + name + " connect. " + error + getAudit());
                }
                errorCount++;
            } catch (DBConnectTimedoutException ex) {
                String error = ex.getMessage();

                if (errorCount > 500) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState  " + name + "  connect. " + error + ". " + getAudit());
                }
                errorCount++;
            } catch (Exception ex) {
                String error = ex.getMessage();
                if (errorCount > 500) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState  " + name + "  connect. " + error + ". " + getAudit());
                }
                errorCount++;
            } finally {
                reconnecting = false;
            }
        }
    }

    @Override
    public int getLastKnownState() throws Exception {
        if (!initialized)
            state = UNHEALTHY;
        else
            state = HEALTHY;
        return state;
    }

    ///////////////////////////// getConnection ///////////////////////////// 
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
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            MRLog.error(ex);
            databaseConnectionStatus.createStatusFile();
            throw new DBNotConnectedException("XMLDB getConnection failed." + getAudit());
        }
    }

    private IpvsDBDriver defaultIpvsDBDriver = null;
    private IpvsDBDriver defaultIpvsDBDriver2 = null;

    @Override
    public IpvsDBDriver getDefaultIpvsDBDriver() throws Exception {
        if (defaultIpvsDBDriver == null) {
            defaultIpvsDBDriver = IpvsDBDriverFactory.getIpvsDBDriverVendor(IpvsDBDriverFactory.XMLDB_SEDNA);
        }
        return defaultIpvsDBDriver;
    }

    @Override
    public IpvsDBDriver getDefaultIpvsDBDriver(String parentIpvsModuleName) throws Exception {
        if (defaultIpvsDBDriver2 == null) {
            defaultIpvsDBDriver2 = IpvsDBDriverFactory.getIpvsDBDriverVendor(IpvsDBDriverFactory.XMLDB_SEDNA);
        }
        return defaultIpvsDBDriver2;
    }
}
