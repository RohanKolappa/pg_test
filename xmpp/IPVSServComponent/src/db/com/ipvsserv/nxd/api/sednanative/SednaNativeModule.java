package com.ipvsserv.nxd.api.sednanative;

import java.util.Map;
import java.util.Vector;

import ru.ispras.sedna.driver.DatabaseManager;
import ru.ispras.sedna.driver.DriverException;
import ru.ispras.sedna.driver.SednaConnection;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
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
public class SednaNativeModule extends IpvsServBasicModule {
    private Vector<SednaConnection> freeConnections = new Vector<SednaConnection>();
    private int checkOutCount = 0;
    private long timeout = 25 * 1000; //2 minutes 
    private int corePoolSize = 5;
    private int maxPoolSize = 5;
    
    private String host;
    private String db;
    private String user;
    private String pwd;
    private long lastCheckedOutTime=0;
    //private DatabaseConnectionStatus databaseConnectionStatus=null;
    private IpvsFileStatus databaseConnectionStatus = null;

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

    public SednaNativeModule() {
        super("Sedna Native Pool");
    }

    public SednaNativeModule(String moduleName) {
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

    private String getAudit() {
        return name + " (out:" + checkOutCount + " cached:" + freeConnections.size() + " core:" + corePoolSize + " max:" + maxPoolSize + ")";
    }

    private synchronized boolean isConnectionAvailable() {
        if (checkOutCount < maxPoolSize)
            return true;
        return false;
    }

    ///////////////////////////// checkOut ///////////////////////////// 
    public SednaConnection checkOutLowPriority() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if (MRLog.isConnectionpoolLogEnable()) {
            MRLog.debug("checkOutLowPriority request queue :" + getAudit());
        }
        return checkOut(false);
    }

    public SednaConnection checkOut() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if (MRLog.isConnectionpoolLogEnable()) {
            MRLog.debug("checkOut request queue :" + getAudit());
        }
        SednaConnection con = checkOut(true);
        if (con != null) {
            lastCheckedOutTime = System.currentTimeMillis();
        }
        return con;
    }

    private synchronized SednaConnection checkOut(boolean bpriority) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
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
            SednaConnection con = getConnection();
            try {
                if (MRLog.isConnectionpoolLogEnable()) {
                    MRLog.debug("checkOut inside queue :" + getAudit());
                }
                //may be not in
                con.begin();
                checkOutCount++;
                return con;
            } catch (Exception ex) {
                try {
                    con = getConnection();
                } catch (DBNotConnectedException ex2) {
                    throw ex2;
                }
                try {
                    if (MRLog.isConnectionpoolLogEnable()) {
                        MRLog.debug("checkOut inside queue recreate :" + getAudit());
                    }
                    con.begin();
                    checkOutCount++;
                    return con;
                } catch (Exception ex2) {
                    throw new DBNotConnectedException("DB Connection failed. " + getAudit() + " " + ex2.getMessage());
                }
            }
        }
        throw new DBNotConnectedException("DB Connection failed. " + getAudit());
    }

    public synchronized void checkIn(SednaConnection con, boolean kill) {//throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (con == null) {
                return;
            }
            if (kill || checkOutCount > corePoolSize) {
                try {
                    con.close();
                } catch (DriverException ex) {
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

        } catch (Exception ex) {
            MRLog.error("Exception ConnectionPool checkIn " + getAudit(), ex);
        } finally {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("checkIn:" + getAudit());
            }
            lastCheckedOutTime = 0;
            notify();
        }
    }
    
    ///////////////////////////// initialize / shutdown ///////////////////////////// 
    private synchronized void createPool() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        initialized = false;
        started = false;
        if (freeConnections == null) {
            freeConnections = new Vector<SednaConnection>();
        }
        boolean timedOut = false;
        if (lastCheckedOutTime > 0 && (System.currentTimeMillis() - lastCheckedOutTime) > timeout) {
            timedOut = true;
            lastCheckedOutTime = 0;
        }
        //create one atleast 
        if (timedOut || (checkOutCount == 0 && freeConnections.size() == 0)) {
            SednaConnection con = getConnection();
            if (con != null) {
                freeConnections.add(con);
                if (MRLog.isConnectionpoolLogEnable()) {
                    MRLog.debug("createPool." + getAudit());
                }
            } else {
                MRLog.debug("FAILED  createPool." + getAudit());
            }
        }
        initialized = true;
        started = true;
        MRLog.debug("created connection. " + getAudit());
    }

    private synchronized void removePool() {
        try {
            while (true) {
                SednaConnection con = null;
                try {
                    if (freeConnections.size() == 0) {
                        break;
                    }
                    con = freeConnections.remove(0);
                    con.close();
                    if (MRLog.isConnectionpoolLogEnable()) {
                        MRLog.debug("Closed connection form pool. " + getAudit());
                    }
                } catch (DriverException ex) {
                    MRLog.error("Can't close Sedna connection from  pool." + getAudit(), ex);
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

    private synchronized SednaConnection getConnection() throws DBNotConnectedException {
        if(manualRestarting){
            MRLog.debug("Database in being Restarted Manually ");
            throw new DBNotConnectedException("Database in being Restarted Manually ");
        }
        if (freeConnections.size() > 0) {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("get old Connection. " + getAudit());
            }
            return freeConnections.remove(0);
        }
        SednaConnection con = getConnection(host, db, user, pwd);
        return con;
    }

    private SednaConnection getConnection(String url, String DBName, String user, String password) throws DBNotConnectedException {
        try {
            if (MRLog.isConnectionpoolLogEnable()) {
                MRLog.debug("get new Connection. " + getAudit());
            }
            SednaConnection con = DatabaseManager.getConnection(url, DBName, user, password);
            if (!connected) {
                setConnected(true);
            }
            databaseConnectionStatus.removeStatusFile();
            return con;
        } catch (DriverException ex) {
            /*
                ru.ispras.sedna.driver.DriverException: SEDNA Message: ERROR SE3003
                1-Failed to connect the host specified. 10.1.5.186
                2-Failed to open session: database is not started
             */
            //log("DriverException SednaConnectionPool createPool");
            if (connected) {
                setConnected(false);
            }
            databaseConnectionStatus.createStatusFile();
            databaseConnectionStatus.write("getConnection exception - " + ex.getErrorCode() + ":" + ex.getErrorMessage() + getAudit(), false);
            throw new DBNotConnectedException("getConnection exception - " + ex.getErrorCode() + ":" + ex.getErrorMessage() + getAudit());
        } catch (Exception ex) {
            if (connected) {
                setConnected(false);
            }
            MRLog.error("getConnection exception - " + ex.getMessage());
            databaseConnectionStatus.createStatusFile();
            databaseConnectionStatus.write("getConnection exception - " + ex.getMessage(), false);
            //databaseConnectionStatus.removeStatusFile();
            throw new DBNotConnectedException("getConnection exception - " + ex.getMessage() + ":" + getAudit());
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
            timeout = CompUtils.intValue(params.get("getconn_timeout_msec"), 25000);

        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void start() throws Exception {
        try {
            databaseConnectionStatus = new IpvsFileStatus(IpvsFileStatus.STATUS_DB_FAILED_FILE);
            databaseConnectionStatus.init(true);
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
    private boolean reconnecting = false;
    private int errorCount = 0;

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
                    MRLog.debug("------------ ERROR verifyState Sedna connect. " + error + getAudit());
                    //throw ex;
                }
            } catch (DBConnectTimedoutException ex) {
                String error = ex.getMessage();
                errorCount++;
                if (errorCount > 30) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState Sedna connect. " + error + ". " + getAudit());
                    //throw ex;
                }
            } catch (Exception ex) {
                String error = ex.getMessage();
                errorCount++;
                if (errorCount > 30) {
                    errorCount = 0;
                }
                if (errorCount == 0) {
                    MRLog.debug("------------ ERROR verifyState Sedna connect. " + error + ". " + getAudit());
                }
            } finally {
                reconnecting = false;
            }
        }
    }

    private boolean connected = false;
    public synchronized void setConnected(boolean started) {
        if (started) {
            connected = true;
            DBAdmin.getDBAdminApi().connected();
            //databaseConnectionStatus.removeStatusFile();
        } else {
            connected = false;
            DBAdmin.getDBAdminApi().disconnected();
            //databaseConnectionStatus.createStatusFile();
        }
    }
    private boolean manualRestarting = false;
    public  synchronized void setManualRestarting(boolean restarting) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        if (restarting) {
            manualRestarting = true;
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
        } else if (!connected) {
            state = UNHEALTHY;
        } else {
            state = HEALTHY;
        }
        return state;
    }

    /////////////////////////////////////

    public IpvsDBDriver getDefaultIpvsDBDriver() throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriverVendor(IpvsDBDriverFactory.SENDA_NATIVE);
    }

    public IpvsDBDriver getDefaultIpvsDBDriver(String parentIpvsModuleName) throws Exception {
        return IpvsDBDriverFactory.getIpvsDBDriver(parentIpvsModuleName, IpvsDBDriverFactory.SENDA_NATIVE);
    }
}
