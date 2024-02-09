package com.ipvsserv.nxd.adminapi;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;
import org.xmldb.api.modules.XMLResource;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.xmldb.exist.ExistDBUtils;
import com.ipvsserv.nxd.api.xmldb.exist.ExistXmlDBModule;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServModule;
import com.ipvsserv.server.IpvsServer;

public class ExistXmlDBAdminApi implements DBAdminApi {
    private ExistXmlDBModule dbModule = null;
    private IpvsDBDriver db = null;
    private boolean import_useSednaUtils = false;
    private boolean connect = false;
    //private String MANUAL_FILE = "/var/run/ms.manual";
    public static final String DB_SCRIPT = "/etc/scripts/ipvsxmldbctl.sh";
    public static final String DB_UTIL_SCRIPT = "/etc/scripts/ipvsxmldbutils.sh";
    
    private boolean dbOnRemoteBox = false;

    //private boolean restoreMode = false;
    public String getDbSscript() {
        return DB_SCRIPT;
    } 
    
    public String getDbUtilSscript() {
        return DB_UTIL_SCRIPT;
    } 
    
    public String getDBConnectionName(){
        return DBAdminApi.DB_CONNECTION_EXIST_XMLDB;
    } 
    
    public ExistXmlDBAdminApi() {
        dbModule = (ExistXmlDBModule) IpvsServer.getInstance().getModule(ExistXmlDBModule.class);
        dbOnRemoteBox = IpvsServGlobals.getBooleanProperty("admindbapi.dbserver_running_onremotebox", false);
        try {
            db = dbModule.getDefaultIpvsDBDriver();
            import_useSednaUtils = IpvsServGlobals.getBooleanProperty("admindbapi.import_useSednaUtils", false);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    public boolean isDBModuleInited(){
        if(dbModule==null){
            return false;
        }
        return true;
    }

    public boolean isDBModuleStarted() {
        return dbModule.isStarted();
    }

    public void waitForDBModuleStart() throws InterruptedException {
        while (!dbModule.isStarted()) {
            Thread.sleep(1 * 1000);
        }
    }

    @Override
    public IpvsDBDriver getIpvsDBDriver() throws Exception {
        return db;
    }

    @Override
    public IpvsServModule getDBModule() throws Exception {
        return dbModule;
    }

    @Override
    public boolean isDBModuleStart() throws Exception {
        return dbModule.isStarted();
    }

    @Override
    public boolean isConnected() {
        return connect;
    }

    @Override
    public synchronized void connected() {
        connect = true;
        MRLog.debug("DB_ADMIN: database server connected");
    }

    @Override
    public synchronized void disconnected() {
        connect = false;
        MRLog.debug("DB_ADMIN: database server disconnected");
    }

    /////////
    private boolean isSystemCollection(String name) {
        if (name.equalsIgnoreCase("/db") || name.startsWith("/db/system") || name.startsWith("/db/system/config"))
            return true;
        return false;
    }

    /*private String getIpvsCollectionName(String collectionName) {
        return ExistDBUtils.getIpvsCollectionName(collectionName);
    }
     */
    public static boolean isRootCollection(String collectionName) {
        if (collectionName == null || collectionName.trim().equals("")) {
            return true;
        }
        return ExistDBUtils.isRootCollection(collectionName);
    }

    protected Collection getIpvsCollection(String name, Collection rootCol, String collectionName) throws Exception {
        Collection domainCol = rootCol;
        if (collectionName != null && !collectionName.trim().equals("")) {
            domainCol = rootCol.getChildCollection(collectionName);//rootCol.getChildCollection(getIpvsCollectionName(collectionName));
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
        }
        return domainCol;
    }

    //////
    @Override
    public boolean isDeleteResourceSkip(String documentName) {
        return false;
    }

    @Override
    public boolean isAddResourceSkip(String documentName) {
        return false;
    }

    @Override
    public boolean isDeleteCollectionSkip(String name) {
        if (name.equalsIgnoreCase("/db") || name.equalsIgnoreCase("/db/system") || name.startsWith("/db/system") || name.equalsIgnoreCase("/db/system/conf"))
            return true;
        return false;
    }

    @Override
    public boolean isAddCollectionSkip(String name) {
        return false;
    }

    ////////////////// FactoryReset ////////////////////////////////////
    private boolean isFactoryResetNeeded(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        List<String> list = db.getDomainNames();
        if (list == null || list.isEmpty()) {
            return true;
        }
        if (!directory.exists()) {
            MRLog.error("DB_ADMIN: isFactoryResetNeeded Factory reset directory/files does not exits. " + directory.getAbsolutePath());
            throw new DBExecutionException("isFactoryResetNeeded Factory reset directory/files does not exits. " + directory.getAbsolutePath());
        }

        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File file = dirs[i];
            if (file.isDirectory()) {
                boolean bExists = db.isCollectionExists("isFactoryResetNeeded", null, file.getName());
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: isFactoryResetNeeded "+ bExists +" "  + file.getAbsolutePath()  + " " + " NULL? childCollection " + file.getName());
                }
                if (!bExists)
                    return true;
            }
        }
        return isPrevFactoryInProgress();
    }

    private boolean isPrevFactoryInProgress() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String query = "for $b in document('factorydb_progress.status')/* return $b";
            StringBuffer bufNodes = new StringBuffer();
            db.execute("checkfactorydb_progress", query, bufNodes);
            if (bufNodes.length() > 0 && bufNodes.indexOf("factorydb") != -1) {
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: isPrevFactoryInProgress true ");
                }                
                return true;
            }
            return false;
        } catch (Exception ex) {
        }
        return false;
    }

    private void deleteRootCollectionDocs() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        db.deleteRootCollectionDocs();
    }

    @Override
    public boolean factoryReset(File directory, DBAdminParams params, boolean force) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        boolean resetDone = false;
        try {
            if (force || isFactoryResetNeeded(directory, params)) {
                //manually delete DB.
                deleteDB(params);

                StringBuffer buf = new StringBuffer("");
                buf.append("<factorydb>");
                buf.append("<startDate>" + (new Date()).toString() + "</startDate>");
                buf.append("<factorydbFolder>" + directory.getAbsolutePath() + "</factorydbFolder>");
                buf.append("</factorydb>");
                getIpvsDBDriver().addDocument("createFactoryDBStatus", "factorydb_progress.status", "", buf.toString());

                //TODO: triggers and indexes
                ensureIndexes("factoryReset", directory);
                //}
                //should work for all vendors... now do the import 
                directoryToDB(directory, "", true, params);

                getIpvsDBDriver().deleteDocument("createFactoryDBStatus", "", 2, "factorydb_progress.status", "");
                resetDone = true;
            }

        } catch (UnsupportedEncodingException ex) {
            MRLog.error(ex);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  factoryReset done dir='" + directory.getAbsolutePath() + "' ");
        }
        return resetDone;
    }

    ////////////////// FactoryReset ////////////////////////////////////

    @Override
    public void deleteDB(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        List<String> domains = null;
        boolean delete = true;
        try {
            //may be same some time if database is already empty
            domains = db.getDomainNames();
            if (domains == null || domains.isEmpty()) {
                //no need to delete db. but delete any documents on root collection
                try {
                    if (MRLog.isDbadminLogEnable()) {
                        MRLog.debug("DB_ADMIN: deleteRootCollectionDocs ");
                    }                    
                    deleteRootCollectionDocs();
                } catch (Exception ex) {
                    MRLog.error("deleteDB", ex);
                }
                delete = false;
            }
        } catch (Exception ex) {
            MRLog.error("deleteDB", ex);
        }
        if (delete) {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: deleteDB ");
            } 
            //delete is much faster by deleting the binary files and create fresh db. also sedna file sizes are small with fresh db
            db.deleteDB();
        }
    }

    @Override
    public void backupdb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            dbToDirectory_root(directory, params);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  backupdb done dir='" + directory.getAbsolutePath() + "' ");
        }
    }

    @Override
    public void restoredb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            directoryToDB(directory, "", false, params);
            ensureIndexes("restoredb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  factoryReset done dir='" + directory.getAbsolutePath() + "' ");
        }
    }

    //TODO: not used    
    @Override
    public void restoredb(File directory, String collectionName, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            directoryToDB(directory, collectionName, false, params);
            ensureIndexes("restoredb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  restoredb done dir='" + directory.getAbsolutePath() + "' " + collectionName);
        }
    }

    @Override
    public void importdb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            directoryToDB(directory, "", true, params);
            ensureIndexes("importdb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  importdb done dir='" + directory.getAbsolutePath() + "' ");
        }
    }

    @Override
    public void importdb(File directory, String collectionName, String domain, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            MRLog.debug("importdb collectionName='" + collectionName +"' '" + directory.getAbsolutePath() +"'");
            //should work for all vendors
            directoryToDB(directory, collectionName, true, params);
            upgradeIndexes(collectionName + "_" + directory.getAbsolutePath(), domain);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  importdb done dir='" + directory.getAbsolutePath() + "' " + collectionName);
        }
    }

    @Override
    public void exportdb(File directory, DBAdminParams params, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            dbToDirectory_root(directory, params);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  exportdb done dir='" + directory.getAbsolutePath() + "'");
        }
    }

    @Override
    public void exportdb(File directory, DBAdminParams params, List<String> query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            dbToDirectory_root(directory, params);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  exportdb done dir='" + directory.getAbsolutePath() + "'");
        }
    }

    ////////////////////

    /////////////////////////// admin api to start stop db /////////
    public synchronized int createEmptyDatabase() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: emptyDatabase START.");
        }
        String[] cmdArgs = { getDbSscript(), "reset", "DBCompEmptyDatabase" };
        try {
            exitVal = CompUtils.executeProcess(cmdArgs, buf);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " DB_ADMIN: isDatabaseServerRunning  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
            }
        } catch (Exception ex) {
            MRLog.error("DB_ADMIN: emptyDatabase " + ex.toString() + " " + buf.toString());
        }
        restart();
        MRLog.debug("DB_ADMIN: emptyDatabase DONE.");
        return exitVal;
    }

    public synchronized int createFactoryDatabase() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: createFactoryDatabase START");
        }
        try {
            String factorydbPath = IpvsServGlobals.getProperty("admindbapi.factorydb", "/home/onair/templates/factorydb");
            File directory = new File(factorydbPath);
            boolean saveToColEnable = true;
            DBAdminParams params = new DBAdminParams();
            params.setSaveToColEnable(saveToColEnable);
            factoryReset(directory, params, true);
        } catch (Exception ex) {
            MRLog.error("createEmptyDatabase " + ex.toString());
        }
        MRLog.debug("DB_ADMIN: createEmptyDatabase DONE");
        return 0;
    }

    public boolean isDatabaseServerRunning() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return true;
        }
        StringBuffer buf = new StringBuffer("");
        int exitVal = 0;
        String[] cmdArgs = { getDbSscript(), "status" };
        try {
            exitVal = CompUtils.executeProcess(cmdArgs, buf);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " DB_ADMIN: isDatabaseServerRunning  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
            }
            return buf.toString().equals("OK");
        } catch (Exception ex) {
            MRLog.error("isDatabaseServerRunning " + ex.toString() + " " + buf.toString());
        }
        return false;
    }

    public void setManualMode() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return;
        }
        StringBuffer buf = new StringBuffer();
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: setManualMode  setManualRestarting true");
            }
            ExistXmlDBModule dbModule2 = (ExistXmlDBModule) dbModule;
            dbModule2.setManualRestarting(true);
        } catch (Exception ex) {
            MRLog.error("setManualMode 2 " + ex.toString() + " " + buf.toString());
        }
    }

    public void setAutoMode() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return;
        }
        StringBuffer buf = new StringBuffer();
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug(" DB_ADMIN: setAutoMode  setManualRestarting false");
            }
            ExistXmlDBModule dbModule2 = (ExistXmlDBModule) dbModule;
            dbModule2.setManualRestarting(false);
        } catch (Exception ex) {
            MRLog.error("setAutoMode 2 " + ex.toString() + " " + buf.toString());
        }
    }

    @Override
    public void clearManualMode() {
        //MRLog.debug("Remove flag " + IpvsFileStatus.FILE_FLAG_STATUS_DOWN_ACTION_NONE);
        File file = new File(IpvsFileStatus.FILE_FLAG_STATUS_DOWN_ACTION_NONE);
        File file2 = new File(IpvsFileStatus.FILE_FLAG_STATUS_DOWN_ACTION_NONE_2);
        if (file2.exists()) {
            file.delete();
            file2.delete();
        }
    }

    @Override
    public synchronized int restart() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        IpvsFileStatus factodryDBStatusFile = new IpvsFileStatus(IpvsFileStatus.FILE_FLAG_STATUS_DOWN_ACTION_NONE);
        IpvsFileStatus factodryDBStatusFile2 = new IpvsFileStatus(IpvsFileStatus.FILE_FLAG_STATUS_DOWN_ACTION_NONE_2);
        int exitVal = 0;
        try {
            factodryDBStatusFile.write("DB_ADMIN: database server restarting", false);
            factodryDBStatusFile2.write("DB_ADMIN: database server restarting", false);
            MRLog.debug("DB_ADMIN: database server restarting");
            StringBuffer buf = new StringBuffer();
            setManualMode();
            exitVal = 1;
            while (exitVal != 0) {
                //first attempt could fail if the sednactl is busy
                String[] cmdArgs = { getDbSscript(), "restart" };
                try {
                    exitVal = CompUtils.executeProcess(cmdArgs, buf);
                    if (MRLog.isDbadminLogEnable()) {
                        MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " database server restart  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                    }
                } catch (Exception ex) {
                    MRLog.error("restart " + ex.toString() + " " + buf.toString());
                }
                if (exitVal != 0) {
                    //wait before try next time .. 
                    try {
                        Thread.sleep(1000);
                    } catch (Exception ex) {
                    }
                }
            }
            setAutoMode();
            MRLog.debug("DB_ADMIN: database server restarted");
        } finally {
            factodryDBStatusFile.removeStatusFile(true);
            factodryDBStatusFile2.removeStatusFile(true);
        }
        return exitVal;
    }

    @Override
    public synchronized int stop() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        MRLog.debug("DB_ADMIN: database server stopping");
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        setManualMode();
        exitVal = 1;
        while (exitVal != 0) {
            //first attempt could fail if the sednactl is busy        
            String[] cmdArgs = { getDbSscript(), "stop" };
            try {
                exitVal = CompUtils.executeProcess(cmdArgs, buf);
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " database server stop  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                }
            } catch (Exception ex) {
                MRLog.error("stop " + ex.toString() + " " + buf.toString());
            }
            if (exitVal != 0) {
                //wait before try next time .. 
                try {
                    Thread.sleep(1000);
                } catch (Exception ex) {
                }
            }
        }

        MRLog.debug("DB_ADMIN: database server stopped");
        return exitVal;
    }

    @Override
    public synchronized int start() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        MRLog.debug("DB_ADMIN: database server starting");
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        exitVal = 1;
        while (exitVal != 0) {
            //first attempt could fail if the sednactl is busy        
            String[] cmdArgs = { getDbSscript(), "start" };
            try {
                exitVal = CompUtils.executeProcess(cmdArgs, buf);
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " database server start  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                }
            } catch (Exception ex) {
                MRLog.error("start " + ex.toString() + " " + buf.toString());
            }
            if (exitVal != 0) {
                //wait before try next time .. 
                try {
                    Thread.sleep(1000);
                } catch (Exception ex) {
                }
            }
        }
        while (true) {
            try {
                if (isDatabaseServerRunning()) {
                    break;
                }
                //Thread.sleep(1000);
            } catch (Exception ex) {
                MRLog.error("DB_ADMIN: start 2 " + ex.toString());
            }
        }
        setAutoMode();
        MRLog.debug("DB_ADMIN: database server started");
        return exitVal;
    }

    //////////////////////
    private void directoryToDB(File directory, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        boolean isRootCollection = isRootCollection(collectionName);
        MRLog.debug("directoryToDB isRootCollection="+isRootCollection+" isImport "+isImport+" collectionName='" + collectionName +"' '" + directory.getAbsolutePath() +"'" );
        //todo: this only supports first child or root
        if (import_useSednaUtils) {
            directoryToDB_xq(directory, collectionName, isImport, params);
        }else{
            if (isRootCollection) {
                directoryToDB_root(directory, collectionName, isImport, params);
            } else {
                directoryToDB_fchild(directory, "", collectionName, isImport, params);
            }
        }
    }

    private void directoryToDB_root(File directory, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            if (!isImport) {
                deleteDB(params);
                isImport = true;//deleted any way
            }
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    addFileToDB(file, collectionName, true, params, isImport);
                }
            }
            
            boolean use_list_col = IpvsServGlobals.isUseListCol();
            if(use_list_col) {
                List<String> colList = new ArrayList<String>();
                String colPrefix = IpvsServGlobals.getListColPrefix();
                String domainName = getDomainNameFromRootFolder(colPrefix, directory); 
                colList = getCollectionListFromMRSchema(colPrefix, domainName);
                for (String colName : colList) {
                    db.addChildCollection("directoryToDB_root", domainName, 1, colName, "");
                }
                createIndexes(directory);
            }   
            
            //collectionName = ExistDBUtils.getRootCollectionName();
            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    directoryToDB_fchild(dir, collectionName, dir.getName(), isImport, params);
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB: ", ex);
            throw new DBExecutionException("directoryToDB failed." + ex.getMessage());
        }
    }

    private void directoryToDB_fchild(File directory, String parentCollectionName, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            boolean isIpvsCollection = ExistDBUtils.isIpvsCollection(collectionName);
            if (isIpvsCollection) {
                directoryToDB_ipvs(directory, collectionName, isImport, params);
            } else {
                directoryToDB_system(directory, isImport, params);
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB: ", ex);
            throw new DBExecutionException("directoryToDB failed." + ex.getMessage());
        }
    }

    private void directoryToDB_ipvs(File directory, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

            String ID = "directoryToDB_" + directory.getName();
            //delete if its not import
            if (!isImport) {
                boolean bExist = db.isCollectionExists(ID, "", 1, collectionName);
                if (bExist) {
                    db.deleteCollection(ID, "", 1, collectionName);
                }
            }
            String rootCollectionName = "";
            //add if does not exist
            boolean bExist = db.isCollectionExists(ID, rootCollectionName, collectionName);
            if (!bExist) {
                db.addChildCollection(ID, "", 1, collectionName, rootCollectionName);
            }

            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    addFileToDB(file, collectionName, false, params, isImport);
                } else {
                    File[] childirs = file.listFiles();
                    for (int k = 0; k < childirs.length; k++) {
                        File childFile = childirs[k];
                        if (!childFile.isDirectory()) {
                            addFileToDB(childFile, collectionName, false, params, isImport);
                        }
                    }
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB_direct_ipvs: ", ex);
            throw new DBExecutionException("directoryToDB_direct_ipvs failed." + ex.getMessage());
        }
    }

    private void directoryToDB_system(File directory, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            Collection collection = dbModule.checkOut();
            try {
                String childCollectionName = directory.getName();
                MRLog.debug("create " + collection.getName() + " child " + childCollectionName);
                CollectionManagementService cms = (CollectionManagementService) dbModule.getServiceManager().getCollectionManagementService(collection);
                Collection childcollection = collection.getChildCollection(childCollectionName);
                if (!isImport) {
                    if (childcollection != null) {
                        if (!isSystemCollection(childcollection.getName())) {
                            cms.removeCollection(childCollectionName);
                        }
                    }
                }
                childcollection = collection.getChildCollection(childCollectionName);
                if (childcollection == null) {
                    childcollection = cms.createCollection(childCollectionName);
                }
                if (childcollection == null) {
                    throw new Exception("failed to create " + collection.getName() + " child " + childCollectionName);
                }
                directoryToDB_system(directory, childcollection, isImport, params);
            } finally {
                try {
                    MRLog.debug("closing rootCollection");
                    dbModule.checkIn(collection, false);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB_direct_system: ", ex);
            throw new DBExecutionException("directoryToDB_direct_system failed." + ex.getMessage());
        }
    }

    private void directoryToDB_system(File directory, Collection collection, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    String docId = file.getName(); //getFileToDBName(file, collection.getName(), false, params, isImport);
                    XMLResource document = (XMLResource) collection.createResource(docId, "XMLResource");
                    document.setContent(DBAdminUtils.getContents(file));
                    collection.storeResource(document);
                }
            }
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    String childCollectionName = dir.getName();
                    MRLog.debug("create " + collection.getName() + " child " + childCollectionName);
                    CollectionManagementService cms = (CollectionManagementService) dbModule.getServiceManager().getCollectionManagementService(collection);
                    Collection childcollection = collection.getChildCollection(childCollectionName);
                    if (!isImport) {
                        if (childcollection != null) {
                            if (!isSystemCollection(childcollection.getName())) {
                                cms.removeCollection(childCollectionName);
                            }
                        }
                    }
                    childcollection = collection.getChildCollection(childCollectionName);
                    if (childcollection == null) {
                        childcollection = cms.createCollection(childCollectionName);
                    }
                    if (childcollection == null) {
                        throw new Exception("failed to create " + collection.getName() + " child " + childCollectionName);
                    }
                    directoryToDB_system(dir, childcollection, isImport, params);
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB_direct_system:  ", ex);
            throw new DBExecutionException("directoryToDB_direct_system failed." + ex.getMessage());
        }
    }

    //////////////////////////////
    private void createIndexFiles(List<String> colList,  File directory, String colPrefix, String domainName) throws Exception{
        String mr_schema_xml = "/home/onair/objectXml/MRSchema.xml";
        File mrschema_file = new File(mr_schema_xml);
        if(!mrschema_file.exists()){
            throw new DBExecutionException("directoryToDB_direct_system failed. file not found " + mrschema_file.getAbsolutePath());
        }
        File index_folder = new File(directory, "system/config/db");
        Document doc = Dom4jUtils.loadXMLFile(mrschema_file.getAbsolutePath());
        for (String list_name : colList) {
            if(list_name !=null){
                 Element el = Dom4jUtils.getElementXpath(doc, "//"+list_name+"/exist_xmldb");
                 if(el != null){
                     String colName = colPrefix + "" + domainName + "." + list_name.toLowerCase();
                     File col_index_folder = new File( index_folder, colName);
                     col_index_folder.mkdirs();
                     File col_db_folder = new File( directory, colName);
                     col_db_folder.mkdirs();
                     Element el_existDB = Dom4jUtils.element(el, "collection");
                     File index_file = new File( col_index_folder, "collection.xconf");
                     CompUtils.writeFile(index_file.getAbsolutePath(), el_existDB.asXML());
                 }
            }
        }
        
    }
    private String getDomainNameFromRootFolder(String colPrefix, File directory) throws Exception{
        File[] dirs = directory.listFiles();
        String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
        for (int i = 0; i < dirs.length; i++) {
            File dir = dirs[i];
            if (dir.isDirectory()) {
                String dirName = dir.getName();
                String domainName = ExistDBUtils.getDomainNameFromFolderName(dirName);
                if(domainName !=null && !domainName.equals("")){
                    return domainName ;
                }
                domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix); 
                if(domainName !=null && !domainName.equals("")){
                    return domainName ;
                }
            }
        }
        return IpvsServGlobals.getDomainDefault();
    }
    private List<String> getCollectionListFromMRSchema(String colPrefix, String domainName) throws Exception{
        List<String> colList = new ArrayList<String>();
        String mr_schema_xml = "/home/onair/objectXml/MRSchema.xml";
        File file = new File(mr_schema_xml);
        if(!file.exists()){
            throw new DBExecutionException("directoryToDB_direct_system failed. file not found " + file.getAbsolutePath());
        }
        Document doc = Dom4jUtils.loadXMLFile(file.getAbsolutePath());
        List<?> childList = Dom4jUtils.listXpath(doc, "//*[exist_xmldb]");
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childReqEL = (Element) i.next();
            String colname = childReqEL.getName();
            if(!colList.contains(colname)){
                colList.add(colname);
            }
        }
        getCollectionListFromMRSchema(colList, doc.getRootElement(), colPrefix, domainName);
        return colList;
    }
    
    private void getCollectionListFromMRSchema(List<String> colList, Element el, String colPrefix, String domainName) throws Exception {
        String elementName = el.getName();
        List<?> childList = Dom4jUtils.getList(el);
        if(childList.size()==0){
            String colname = elementName;
            if(!colList.contains(colname)){
                colList.add(colname);
            }
        }else{
            childList = Dom4jUtils.getList(el);
            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                Element childReqEL = (Element) i.next();
                if(childReqEL.getName().equals("exist_xmldb")){
                    
                }else{
                    getCollectionListFromMRSchema(colList, childReqEL, colPrefix, domainName);
                }
            }
        }
    }   
    private void convertSColDirToMColDir(File scol_folder, File mcol_folder, File xquery){
        if(IpvsServGlobals.isUseListCol()){
            StringBuffer buf = new StringBuffer();
            int exitVal = 0;
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: convertSColDirToMColDir from scol " + scol_folder.getAbsolutePath() +" to " + mcol_folder.getAbsolutePath());
            }
            String[] cmdArgs = { getDbUtilSscript(), "convertSColDirToMColDir", scol_folder.getAbsolutePath(),  mcol_folder.getAbsolutePath(),  xquery.getAbsolutePath()};
            try {
                exitVal = CompUtils.executeProcess(cmdArgs, buf);
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " convertSColDirToMColDir " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                }
            } catch (Exception ex) {
                MRLog.error("DB_ADMIN: convertSColDirToMColDir " + ex.toString() + " " + buf.toString());
            }
        }
    }
    private void convertMColDirToSColDir(File scol_folder, File mcol_folder, File xquery){
        if(IpvsServGlobals.isUseListCol()){
            StringBuffer buf = new StringBuffer();
            int exitVal = 0;
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: convertSColDirToMColDir from scol " + scol_folder.getAbsolutePath() +" to " + mcol_folder.getAbsolutePath());
            }
            String[] cmdArgs = { getDbUtilSscript(), "convertSColDirToMColDir", scol_folder.getAbsolutePath(),  mcol_folder.getAbsolutePath(),  xquery.getAbsolutePath()};
            try {
                exitVal = CompUtils.executeProcess(cmdArgs, buf);
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " convertSColDirToMColDir " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                }
            } catch (Exception ex) {
                MRLog.error("DB_ADMIN: convertSColDirToMColDir " + ex.toString() + " " + buf.toString());
            }
        }        
    }

    private void directoryToDB_xq(File directory,  String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        File xquery_file = null;
        File dir_workarea = null;
        BufferedWriter queries = null;
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            boolean isRootCollection = collectionName == null || collectionName.equals("");
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: directoryToDB_xq START isImport " + isImport);
            }            
            if (isRootCollection) {
                if (!isImport) {
                    deleteDB(params);
                    isImport = true;//deleted any way
                }
            } else {
                if (!isImport) {
                    db.deleteCollection("directoryToDB_xq", "", 1, collectionName);
                    isImport = true;//deleted any way
                }
            }

            String dirNameRoot = directory.getAbsolutePath();
            /*if (IpvsServGlobals.isWindowsOS()) {
                dirNameRoot = CompUtils.windowsToLinuxPath(dirNameRoot);
            }*/
            
            xquery_file = DBAdminUtils.getScript("loaddocs");
            boolean use_list_col = IpvsServGlobals.isUseListCol();
            dir_workarea =  DBAdminUtils.getScriptFolder(directory.getName());
            File dir_db =  new File(dir_workarea,"db");
            dir_db.mkdirs();
            
            queries = new BufferedWriter(new FileWriter(xquery_file, false));
            queries.append("\ndeclare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";");
            queries.append("\ndeclare namespace util=\"http://exist-db.org/xquery/util\";");
            if(isRootCollection){
                collectionName="";
                queries.write("\nxdb:store-files-from-pattern('"+collectionName+"','" + dirNameRoot + "' ,'" + IpvsServGlobals.MIGRATION_PROGRESS_STATUS + "*'),");
                queries.write("\nxdb:store-files-from-pattern('"+collectionName+"','" + dirNameRoot + "' ,'" + IpvsServGlobals.SERVICE_VERSION + "*'),");
                if(use_list_col) {
                    createIndexes_xq(directory, dir_db, queries);
                }
                //queries.write("\nxdb:store-files-from-pattern('','"+dirNameRoot+"' ,'*'),");
                File[] dirs = directory.listFiles();
                for (int i = 0; i < dirs.length; i++) {
                    File dir = dirs[i];
                    if (dir.isDirectory()) {
                        String dirName = dir.getName();
                        if (isRootCollection) {
                        } else {
                            dirName = collectionName + "/" + dirName;
                        }
                        String dirPath = CompUtils.windowsToLinuxPath(dir.getAbsolutePath());
                        boolean isIPVSCol = ExistDBUtils.isIpvsCollection(dirName);
                        MRLog.debug("----DB_TO_DIR----  " + dir.getName() +" " +dirPath);
                        if (isIPVSCol) { //non ipvs folder 
                            directoryToDB_scol_xq(dir, collectionName, dirName, isImport, params, queries); 
                        } else {    
                            queries.append("\nxdb:create-collection('"+collectionName+"','" + dirName + "'),");
                            queries.append("\nxdb:store-files-from-pattern('" + dirName + "','" + dirPath + "' ,'*'),");
                            createCollections(dir, dirName, queries);
                        }                    
                    }
                }
                //create_load_docs_xquery(xquery_file.getAbsolutePath(), queries, directory, parentCollectionName, collectionName, isImport, params);
                queries.write("\nxdb:store-files-from-pattern('"+collectionName+"','" + dirNameRoot + "' ,'*'),\n");
            } else {
                MRLog.debug("-------- DB_TO_DIR IMPORT/EXPORT--- " + collectionName +" " +directory.getAbsolutePath());
                directoryToDB_scol_xq(directory, "", collectionName, isImport, params, queries); 
            }
            queries.append("\nxdb:create-collection('','system')");
            queries.flush();
            queries.close();
            queries = null;
            String xQuery = CompUtils.getContents(xquery_file);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: " + xQuery);
            }
            /*if (IpvsServGlobals.isWindowsOS()) {
                //xQuery = xQuery.replace(directory.getAbsolutePath(), dirNameRoot);
                StringBuffer buf = new StringBuffer();
                String[] cmd = { "pscp", "-pw", "0at3n", "-scp", "-r", dirNameRoot+"\\*", "root@10.1.5.165:"+dirNameRoot };
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_MIG:  coping files " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
                int retVal = CompUtils.executeProcess(cmd, buf, true);
                if (retVal != 0) {
                    MRLog.debug("windows upload directory to db " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
            }*/ 
            db.executeXQueryToUpdate("directoryToDB_xq", "", xQuery);
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB: ", ex);
            throw new DBExecutionException("directoryToDB failed.  " + ex.getMessage());
        } finally {
            if (queries != null) {
                try {
                    queries.flush();
                    queries.close();
                } catch (Exception ex) {
                }
            }
            if (xquery_file != null) {
                xquery_file.delete();
            }            
            if (dir_workarea != null) {
                //CompUtils.deleteDirectory(dir_workarea);
            }
        }
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: directoryToDB_xq END");
        }  
    }
    private boolean isIpvsSCol(String collectionName){
        if(IpvsServGlobals.isUseListCol()) {
            if(ExistDBUtils.isIpvsSCol(collectionName)){
                return true;
            }
        }
        return false;
    }
    
    private void directoryToDB_scol_xq(File directory, String parentCollectionName,  String collectionName, boolean isImport, DBAdminParams params,  BufferedWriter queries) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, IOException {
        String dirColPath = null;
        boolean createcol = isIpvsSCol(collectionName);
        MRLog.debug("-----DB_TO_DIR---  directoryToDB_scol_xq " + createcol +" p:'" +parentCollectionName+"' c:"+collectionName);
        if(createcol) {
            File dir_workareaCol =  DBAdminUtils.getScriptFolder(directory.getName());
            File dir_dbCol =  new File(dir_workareaCol,"db");
            dir_dbCol.mkdirs();
            File xquery_fileCol = DBAdminUtils.getScript("loaddocs");
            dirColPath = dir_dbCol.getAbsolutePath();                    
            /*if(IpvsServGlobals.isWindowsOS()) {
                dirNameCol = CompUtils.windowsToLinuxPath(dirNameCol);
                MRLog.debug( " directoryToDB_scol_xq do upload files " + dirNameCol );
                StringBuffer buf = new StringBuffer();
                String[] cmd = { "pscp", "-pw", "0at3n", "-scp", "-r", dir_dbCol.getAbsolutePath()+"\\*", "root@10.1.5.165:"+CompUtils.windowsToLinuxPath(dir_dbCol.getAbsolutePath()) };
                MRLog.debug("" + CompUtils.joinArray(cmd, " ") );
                int retVal = 0;
                try {
                    retVal = CompUtils.executeProcess(cmd, buf, true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                if (retVal != 0) {
                    MRLog.debug("windows upload directory to db " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }                    
            } */
            convertSColDirToMColDir(directory, dir_dbCol, xquery_fileCol);
            queries.write("\nxdb:store-files-from-pattern('','" + dirColPath + "', '**/**','text/xml',true() ),");
        }else{
            dirColPath = directory.getAbsolutePath();
            queries.append("\nxdb:create-collection('"+parentCollectionName+"','" + collectionName + "'),");
            queries.write("\nxdb:store-files-from-pattern('"+collectionName+"','" + dirColPath + "', '**/**'),");
        }
     }

    
    private void createCollections(File directory, String parentCollectionName, BufferedWriter queries) throws Exception {
        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File dir = dirs[i];
            if (dir.isDirectory()) {
                String dirName = parentCollectionName + "/" + dir.getName();
                queries.append("\nxdb:create-collection('','" + dirName + "'),");
                String dirPath = CompUtils.windowsToLinuxPath(dir.getAbsolutePath());
                queries.append("\nxdb:store-files-from-pattern('" + dirName + "','" + dirPath + "' ,'*'),");
                createCollections(dir, dirName, queries);
            }
        }
    }

  

    private void addFileToDB(File file, String collectionName, boolean isRootCollection, DBAdminParams params, boolean isImport) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            //long t0 = System.currentTimeMillis();
            String docname = getFileToDBName(file, collectionName, isRootCollection, params, isImport);
            if (docname.equals("")) {
                throw new DBExecutionException("docuri is empty " + file.getAbsolutePath());
            }
            if (isRootCollection) {
                db.addDocument("addFileToDB", docname, collectionName, DBAdminUtils.getContents(file));
            } else {
                db.addDocument("addFileToDB", docname, collectionName, DBAdminUtils.getContents(file));
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error(file.getName(), ex);
        }
    }

    private String getFileToDBName(File file, String collectionName, boolean isRootCollection, DBAdminParams params, boolean isImport) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        String docname = "";
        if (!file.isDirectory()) {

            //if (isRootCollection) {
            docname = file.getName();
            //}            
            if (file.getName().endsWith(".xml")) {
                if (params.isSaveForDiff()) {
                    Document doc = Dom4jUtils.getDocument(file);
                    String nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                    NIDSchemeMDC node = new NIDSchemeMDC();
                    node.parse(nodeID);
                    docname = node.getDocUri();
                } else {
                    docname = file.getName().substring(0, file.getName().lastIndexOf("."));
                }
            }
        }

        return docname;

    }

    ///////////////////dbToDirectory //////////////////////////////// 
    private void dbToDirectory_root(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            String collectionName = "";
            if (!directory.exists() && !directory.mkdir())
                throw new DBExecutionException("Could not create directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canWrite())
                throw new DBExecutionException("Insufficient permissions to write to the directory '" + directory + "'.");

            boolean isRootCollection = true;//isRootCollection(collectionName);
            // Get all resources, save them to the given local directory.
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: dbToDirectory '" + collectionName + "' -to- '" + directory.getAbsolutePath() + "'.");
            }
            Collection rootCollection = dbModule.checkOut();
            List<String> ipvsCol = new ArrayList<String>();
            try {
                Collection collection = rootCollection;
                String[] childCollectionNames = collection.listChildCollections();
                dbToDirectory_system_save(isRootCollection, directory, collection, params);
                if (childCollectionNames != null) {
                    for (int i = 0; i < childCollectionNames.length; i++) {
                        String childCollectionName = childCollectionNames[i];
                        boolean isIPVSCol = ExistDBUtils.isIpvsCollection(collectionName);
                        //if ipvs collection save using custom method
                        if(isIPVSCol){
                            ipvsCol.add(childCollectionName);
                        } else {
                            //if not ipvs collection than recursively save in general way
                            File childDirectory = new File(directory.getPath() + "/" + childCollectionName);
                            childDirectory.mkdirs();
                            Collection childCollection = collection.getChildCollection(collection.getName() + "/" + childCollectionName);
                            if (childCollection != null) {
                                dbToDirectory_system(childDirectory, childCollection, params);
                            }
                        }
                    }
                }
            } finally {
                try {
                    dbModule.checkIn(rootCollection, false);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
            for (String childCollectionName : ipvsCol) {
                File childDirectory = new File(directory.getPath() + "/" + childCollectionName);
                dbToDirectory_ipvs(childDirectory, childCollectionName, params);
            }

        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("Exception: ", ex);
            throw new DBExecutionException("DB_ADMIN: dbToDirectory failed. " + ex.getMessage());
        }
    }

    private void dbToDirectory_system(File directory, Collection collection, DBAdminParams params) throws Exception {
        if (collection == null) {
            return;
        }
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: dbToDirectory_system '" + collection.getName() + "' - '" + directory.getAbsolutePath() + "'.");
        }
        String[] childCollectionNames = null;
        try {
            dbToDirectory_system_save(false, directory, collection, params);
            childCollectionNames = collection.listChildCollections();
            if (childCollectionNames != null) {
                for (int i = 0; i < childCollectionNames.length; i++) {
                    String childCollectionName = childCollectionNames[i];
                    File childDirectory = new File(directory.getPath() + "/" + childCollectionName);
                    childDirectory.mkdirs();
                    Collection childCollection = collection.getChildCollection(collection.getName() + "/" + childCollectionName);
                    if (childCollection != null) {
                        dbToDirectory_system(childDirectory, childCollection, params);
                    }
                }
            }
        } finally {
            MRLog.debug("DB_ADMIN: dbToDirectory " + collection.getName());
        }
    }

    private void dbToDirectory_ipvs(File directory, String collectionName, DBAdminParams params) throws Exception {
        SaveFileQueue saveFileQueue = new SaveFileQueue();
        SaveFileQueueCompletedListenerImpl listener = new SaveFileQueueCompletedListenerImpl();
        saveFileQueue.addSaveFileQueueCompletedListener(listener);
        SaveParams saveParams = new SaveParams(directory, params, false, false);
        saveFileQueue.setSaveParams(saveParams);
        saveFileQueue.start();
        try {
            //save to disk
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: dbToDirectory_ipvs '" + collectionName + "' - '" + directory.getAbsolutePath() + "'.");
            }
            StringBuffer query = new StringBuffer();
            query.append("for $b in collection('" + collectionName + "')/*");
            query.append("return $b  ");
            db.execute("dbToDirectory", collectionName, query.toString(), saveFileQueue);
        } finally {
            //indicate that queue is over
            saveFileQueue.setStop(true);
            listener.wait_to_complete();
        }
    }

    private void dbToDirectory_system_save(boolean isRootCollection, File directory, Collection collection, DBAdminParams params) throws Exception {
        SaveFileQueue saveFileQueue = new SaveFileQueue();
        SaveFileQueueCompletedListenerImpl listener = new SaveFileQueueCompletedListenerImpl();
        String collectionName = collection.getName();
        saveFileQueue.addSaveFileQueueCompletedListener(listener);
        SaveParams saveParams = new SaveParams(directory, params, isRootCollection, false);
        saveFileQueue.setSaveParams(saveParams);
        saveFileQueue.start();
        try {
            //save to disk
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: dbToDirectory_system_save '" + collectionName + "' - '" + directory.getAbsolutePath() + "'.");
            }
            String[] resourceNames = collection.listResources();
            for (int i = 0; i < resourceNames.length; i++) {
                String docuri = resourceNames[i];
                try {
                    XMLResource document = (XMLResource) collection.getResource(docuri);
                    if (document != null) {
                        String xml = (String) document.getContent();
                        saveFileQueue.addDocumentToQueue(docuri, xml);
                    }
                } catch (Exception ex) {
                    MRLog.error("DB_ADMIN: dbToDirectory_system_save trying to add to queue '" + docuri + "'.");
                }
            }
        } finally {
            //indicate that queue is over
            saveFileQueue.setStop(true);
            listener.wait_to_complete();
        }
    }

    //////////////// new api ///////////////////// 
    public void saveFile(File directory, DBAdminParams params, String xml, String documentName, boolean isRootCollection, boolean saveToCol) {
        try {
            String save_path = directory.getPath() + "/" + documentName + ".xml";
            if (isRootCollection) {
                save_path = directory.getPath() + "/" + documentName;
            }
            if (saveToCol) {
                String rootName = DBAdminUtils.getNodeName(xml);
                String colPath = directory.getPath() + "/" + rootName;
                File file = new File(colPath);
                if (!file.exists()) {
                    file.mkdirs();
                }
                save_path = colPath + "/" + documentName + ".xml";
                if (isRootCollection) {
                    save_path = colPath + "/" + documentName;
                }
                if (params.isSaveForDiff()) {
                    save_path = DBAdminUtils.getFilePath(isRootCollection, directory.getPath(), rootName, save_path);
                }
            }
            try {
                PrintWriter out = new PrintWriter(new FileOutputStream(save_path, false));
                out.println(xml);
                out.flush();
                out.close();
            } catch (IOException ex) {
                MRLog.error("IOException whilst trying to write file '" + directory.getPath() + "/" + documentName + "'.");
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    ///////////////////////////
    public static class SaveFileQueueCompletedListenerImpl implements SaveFileQueueCompletedListener {
        boolean completed_backup = false;

        public void wait_to_complete() {
            synchronized (this) {
                try {
                    while (true) {
                        if (completed_backup) {
                            break;
                        }
                        wait(5000);
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        }

        @Override
        public void saveQueueCompleted(SaveParams saveParams, int count) {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("dbadmin saveQueueCompleted");
            }
            synchronized (this) {
                try {
                    completed_backup = true;
                    notify();
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        }

        @Override
        public void saveQueueFailed(SaveParams saveParams, int count) {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("dbadmin saveQueueFailed");
            }
            synchronized (this) {
                try {
                    completed_backup = true;
                    notify();
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        }
    }

    ////////////////////
    @Override
    public void upgradeIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                upgradeIndexes("upgradeIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception e) {

            e.printStackTrace();
        }
    }

    @Override
    public void createIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                createIndexes("createIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void deleteIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                deleteIndexes("deleteIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void ensureIndexes(String packetID, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            String domainName =null; 
            boolean use_list_col = IpvsServGlobals.isUseListCol();
            if(use_list_col) {
                //String colPrefix = IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_LIST_PREFIX);
                //domainName = getDomainNameFromRootFolder(colPrefix, directory);
                return;
            }else {
                domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix);
            }
            MRLog.info("create index for domainName '" + domainName + "'");
            upgradeIndexes(packetID + "_" + directory.getAbsolutePath(), domainName);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void upgradeIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        deleteIndexes(ID, domainName);
        createIndexes(ID, domainName);
    }

    
    private void createIndexes_xq(File directory, File dir_index, BufferedWriter queries) throws Exception{
        if (IpvsServGlobals.isUseListCol()) {
            List<String> colList = new ArrayList<String>();
            String colPrefix = IpvsServGlobals.getListColPrefix();
            String domainName = getDomainNameFromRootFolder(colPrefix, directory);
            colList = getCollectionListFromMRSchema(colPrefix, domainName);
            createIndexFiles(colList, dir_index, colPrefix, domainName);
            queries.append("\nxdb:create-collection('system/config','db'),");
            String subdirPath = CompUtils.windowsToLinuxPath(dir_index.getAbsolutePath());
            queries.append("\nxdb:store-files-from-pattern('','" + subdirPath + "','**/**' ,'text/xml',true()),");
            File[] files = dir_index.listFiles();
            for (File file : files) {
                queries.append("\nxdb:create-collection('','" + file.getName() + "'),");
            }
        } else {
            String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            String domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix);
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.setDomain(domainName);
            String colName = node.getCollectionName();
            String subdirPath = IpvsServGlobals.getHomeDirectory() + "/conf/xmls/exist_db/collection.xconf";
            queries.append("\nxdb:create-collection('system/config','db'),");
            queries.append("\nxdb:create-collection('','" + colName + "'),");
            queries.append("\nxdb:create-collection('system/config/db','" + colName + "'),");
            queries.append("\nxdb:store-files-from-pattern('system/config/db/" + colName + "','" + subdirPath + "' ,'collection.xconf'),");
        }
    }
    
    private void createIndexes(File directory) throws Exception{
        File xquery_file = null;
        File dir_index =  null;
        try {
            xquery_file = DBAdminUtils.getScript("loaddocs");
            dir_index =  DBAdminUtils.getScriptFolder(directory.getName());
            CompUtils.deleteDirectory(dir_index);
            dir_index.mkdirs();
    
            if (IpvsServGlobals.isUseListCol()) {
                List<String> colList = new ArrayList<String>();
                String colPrefix = IpvsServGlobals.getListColPrefix();
                String domainName = getDomainNameFromRootFolder(colPrefix, directory);
                colList = getCollectionListFromMRSchema(colPrefix, domainName);
                createIndexFiles(colList, directory, colPrefix, domainName);
                db.addChildCollection("directoryToDB_root", domainName, 1, "config", "system");
                db.addChildCollection("directoryToDB_root", domainName, 1, "db", "system/config");
                for (String list_name : colList) {
                    if (list_name != null) {
                        String colName = colPrefix + "" + domainName + "." + list_name.toLowerCase();
                        File col_index_dir = new File(dir_index, list_name);
                        File index_file = new File(col_index_dir, "collection.xconf");
                        db.addChildCollection("directoryToDB_root", domainName, 1, colName, "");
                        db.addChildCollection("directoryToDB_root", domainName, 1, colName, "system/config/db");
                        db.addDocument("addIndex", domainName, 1, index_file.getName(), "system/config/db/"+colName, CompUtils.readFile(index_file.getAbsolutePath()));
                    }
                }
            } else {
                String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
                String domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix);
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                String colName = node.getCollectionName();
                String subdirPath = IpvsServGlobals.getHomeDirectory()+ "/conf/xmls/exist_db/collection.xconf";
                File index_file = new File(subdirPath);
                db.addChildCollection("directoryToDB_root", domainName, 1, "config", "system");
                db.addChildCollection("directoryToDB_root", domainName, 1, "db", "system/config");
                db.addChildCollection("directoryToDB_root", domainName, 1, colName, "");
                db.addChildCollection("directoryToDB_root", domainName, 1, colName, "system/config/db");
                db.addDocument("addIndex", domainName, 1, index_file.getName(), "system/config/db/"+colName, CompUtils.readFile(index_file.getAbsolutePath()));
            }
        } finally {
            if (xquery_file != null) {
                xquery_file.delete();
            }            
            if (dir_index != null) {
               CompUtils.deleteDirectory(dir_index);
            }
        }
        
    }  
    
    private void createIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.info("Create Indexes for domainName '" + domainName + "'");
            if(IpvsServGlobals.isUseListCol()){
                //donothing
            }else{
                if (domainName != null) {
                    NIDSchemeMDC node = new NIDSchemeMDC();
                    node.setDomain(domainName);
                    try {
                        db.addIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                    } catch (DBExecutionException ex) {
                        MRLog.error(ID, ex);
                    } catch (Exception ex) {
                        MRLog.error(ID, ex);
                    }
                    try {
                        db.addIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
                    } catch (DBExecutionException ex) {
                        MRLog.error(ID, ex);
                    } catch (Exception ex) {
                        MRLog.error(ID, ex);
    
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }


    private void deleteIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.info("Delete Indexes for domainName '" + domainName + "'");
            if(IpvsServGlobals.isUseListCol()){
                 //donothing
            }else{
                if (domainName != null) {
                    NIDSchemeMDC node = new NIDSchemeMDC();
                    node.setDomain(domainName);
                    try {
                        db.deleteIndex(ID, node.getIndexName(), null);
                    } catch (DBExecutionException ex) {
                        MRLog.error(ID, ex);
                    } catch (Exception ex) {
                        MRLog.error(ID, ex);
                    }
    
                    try {
                        db.deleteIndex(ID, node.getIndexNameCol(), null);
                    } catch (DBExecutionException ex) {
                        MRLog.error(ID, ex);
                    } catch (Exception ex) {
                        MRLog.error(ID, ex);
                    }
    
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    public void deletedb_xquery(String dbquery, String domainName) throws Exception {
        File xquery_file = null;
        BufferedWriter queries = null;
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: delete_using_query " + dbquery);
            }            
            String collectionName = "";
            List<String> listNodes = new ArrayList<String>();
            db.execute("dbadmin_DeleteQuery", dbquery, listNodes);
            boolean delete_useSednaUtils = IpvsServGlobals.getBooleanProperty("admindbapi.delete_useSednaUtils", false);
            if (!delete_useSednaUtils) {
                for (String docURI : listNodes) {
                    String dropdoc = "";
                    try {
                        docURI = docURI.trim();
                        db.deleteDocument("deletedb_xquery", domainName, 1, docURI, collectionName);
                    } catch (Exception ex) {
                        MRLog.error(docURI +" " + dropdoc, ex);
                    }
                }
            } else {            	            	
            	int sliceLength = IpvsServGlobals.SLICE_LENGTH;
            	int collectionSize = listNodes.size();            	
            	for(int i = 0; i < collectionSize; i = i + sliceLength) 
            	{            	            		          	
	                xquery_file = DBAdminUtils.getScript("deletedocs");
	                queries = new BufferedWriter(new FileWriter(xquery_file, false));
	                queries.append("\ndeclare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";");
	                queries.append("\ndeclare namespace util=\"http://exist-db.org/xquery/util\";");	                	                       	                           
	                try {		                		                
	                	for(int index = i; index < i + sliceLength; index++) 
	                	{	                			                		
	                		if(index >= collectionSize) {		                			
	                			break;
	                		}	                		
	                		String docURI = listNodes.get(index).trim();	                		
	                		String dropdoc = "";
	                        try {
	                            File docURIFile = new File(docURI);
	                            queries.write("\nxdb:remove('"+docURIFile.getParent()+"','" + docURIFile.getName() + "'),");
	                            queries.flush();
	                        } catch (Exception ex) {
	                            MRLog.error(docURI +" " + dropdoc, ex);
	                        }	 	                        
	                    }
	                    queries.write("\nxdb:get-current-user()\n");
	                    queries.flush();
	                    queries.close();
	                    String xQuery = CompUtils.getContents(xquery_file);
	                    MRLog.debug("deletedb_xquery xQuery " + xQuery);
	                    db.executeXQueryToUpdate("delete_using_query", "", xQuery);	                    
	                } finally {
	                    if (queries != null) {
	                        try {
	                            queries.flush();
	                            queries.close();
	                        } catch (Exception ex) {
	                        }
	                    }
	                    if (xquery_file != null) {
	                        xquery_file.delete();
	                    }	                    
	                }	                
            	}
            }                
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("delete_using_query: ", ex);
            throw new DBExecutionException("delete_using_query failed.  " + ex.getMessage());
        } finally {
            if (queries != null) {
                try {
                    queries.flush();
                    queries.close();
                } catch (Exception ex) {
                }
            }
            if (xquery_file != null) {
                xquery_file.delete();
            }
        }
    
    }    
    
}
