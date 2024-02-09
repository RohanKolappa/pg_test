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
import org.xmldb.api.base.XMLDBException;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.EngineMDCUtils;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.sednanative.SednaNativeModule;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServModule;
import com.ipvsserv.server.IpvsServer;

public class SednaNativeDBAdminApi implements DBAdminApi {
    private SednaNativeModule dbModule = null;
    private IpvsDBDriver db = null;
    private boolean import_useSednaUtils = false;
    private boolean connect = false;
    public static final String DB_SCRIPT = "/etc/scripts/ipvsxmldbctl.sh";
    //private String MANUAL_FILE = "/var/run/ms.manual";
    private boolean dbOnRemoteBox = false;
    //private boolean restoreMode = false;
    public String getDbSscript(){
        return DB_SCRIPT;
    }
    
    public String getDBConnectionName(){
        return DBAdminApi.DB_CONNECTION_SEDNA_NATIVE;
    }     
    
    public SednaNativeDBAdminApi() {
        dbModule = (SednaNativeModule) IpvsServer.getInstance().getModule(SednaNativeModule.class);
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

    //////
    @Override
    public boolean isDeleteResourceSkip(String documentName) {
        //if (documentName.startsWith("$db_security_data")) return true;
        if (documentName.startsWith("$"))
            return true;
        return false;
    }

    @Override
    public boolean isAddResourceSkip(String documentName) {
        if (documentName.startsWith("$"))
            return true;
        return false;
    }

    @Override
    public boolean isDeleteCollectionSkip(String name) {
        if (name.startsWith("$"))
            return true;
        return false;
    }

    @Override
    public boolean isAddCollectionSkip(String name) {
        if (name.startsWith("$"))
            return true;
        return false;
    }

    ////////////////// FactoryReset ////////////////////////////////////
    private boolean isFactoryResetNeeded(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        List<String> list = db.getDomainNames();
        if(list==null || list.isEmpty()) {
            return true;
        }
        if (!directory.exists()) {
            MRLog.error("DB_ADMIN: isFactoryResetNeeded Factory reset directory/files does not exits. " + directory.getAbsolutePath() );
            throw new DBExecutionException("isFactoryResetNeeded Factory reset directory/files does not exits. " + directory.getAbsolutePath());
        }
        
        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File file = dirs[i];
            if (file.isDirectory()) {
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: isFactoryResetNeeded " + file.getAbsolutePath());
                }
                boolean bExists = db.isCollectionExists("isFactoryResetNeeded", null, file.getName());
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_ADMIN: "+ bExists + " isFactoryResetNeeded NULL? childCollection " + file.getName());
                }
                if(!bExists)
                    return true;
            }
        }
        return isPrevFactoryInProgress();
    }
    private boolean isPrevFactoryInProgress() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException{
        try{
            String query ="for $b in document('factorydb_progress.status')/* return $b";
            StringBuffer bufNodes = new StringBuffer();
            db.execute("checkfactorydb_progress", query, bufNodes);
            if(bufNodes.length()>0 && bufNodes.indexOf("factorydb") !=-1) {
                return true;
            }
            return false;
        }catch(Exception ex){
        }
        return false;
    }
    private void deleteRootCollectionDocs() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException{
        db.deleteRootCollectionDocs();
    }
    @Override
    public boolean factoryReset(File directory, DBAdminParams params, boolean force) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        boolean resetDone=false;
        try {
            if(force || isFactoryResetNeeded(directory, params)){
                //manually delete DB.
                deleteDB(params);
                
                StringBuffer buf = new StringBuffer("");
                buf.append("<factorydb>");
                buf.append("<startDate>" + (new  Date()).toString() + "</startDate>");
                buf.append("<factorydbFolder>" + directory.getAbsolutePath() + "</factorydbFolder>");
                buf.append("</factorydb>");
                getIpvsDBDriver().addDocument("createFactoryDBStatus", "factorydb_progress.status", "", buf.toString());
               
                //should work for all vendors... now do the import 
                directoryToDB(directory, "", true, params);
                //TODO: triggers and indexes
                ensureIndexes("factoryReset", directory);
                //}
                
                getIpvsDBDriver().deleteDocument("createFactoryDBStatus", "", 2, "factorydb_progress.status", "");
                resetDone=true;
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
        List<String> domains =null;
        boolean delete=true;
        try{
            //may be same some time if database is already empty
            domains = db.getDomainNames();
            if(domains==null || domains.isEmpty()){
                //no need to delete db. but delete any documents on root collection
                try{
                    deleteRootCollectionDocs();
                }catch(Exception ex){
                    MRLog.error("deleteDB", ex);
                }
                delete=false;
            }
        }catch(Exception ex){
            MRLog.error("deleteDB", ex);
        }
        if(delete){
            //delete is much faster by deleting the binary files and create fresh db. also sedna file sizes are small with fresh db
            db.deleteDB();
        }
    }
    
    @Override
    public void backupdb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            dbToDirectory(directory, params);
            //TODO: triggers and indexes
            //ensureIndexes("factoryReset", directory);
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
            //TODO: triggers and indexes
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
            //TODO: triggers and indexes
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
            //TODO: triggers and indexes
            ensureIndexes("importdb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  importdb done dir='" + directory.getAbsolutePath() + "' ");
        }
    }

    @Override
    public void importdb(File directory, String collectionName, String domain, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            directoryToDB(directory, collectionName, true, params);
            //TODO: triggers and indexes
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
            dbToDirectory(directory, params);
            //TODO: triggers and indexes
            //ensureIndexes("exportdb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  exportdb done dir='" + directory.getAbsolutePath() + "'");
        }
    }

    @Override
    public void exportdb(File directory, DBAdminParams params, List<String> query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long to = System.currentTimeMillis();
        try {
            //should work for all vendors
            dbToDirectory(directory, params);
            //TODO: triggers and indexes
            //ensureIndexes("exportdb", directory);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  exportdb done dir='" + directory.getAbsolutePath() + "'");
        }
    }

    ////////////////////
    @Override
    public void upgradeIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                upgradeIndexes("upgradeIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error("upgradeIndexes", ex);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  upgradeIndexes done");
        }
    }

    @Override
    public void createIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                createIndexes("createIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error("createIndexes", ex);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  createIndexes done");
        }
    }

    @Override
    public void deleteIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        long to = System.currentTimeMillis();
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                deleteIndexes("deleteIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error("deleteIndexes", ex);
        } finally {
            MRLog.debug("DB_ADMIN: " + (System.currentTimeMillis() - to) + "  deleteIndexes done");
        }

    }
    /////////////////////////// admin api to start stop db /////////

    public synchronized int createEmptyDatabase() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        StringBuffer buf = new StringBuffer();
        int exitVal = 0;
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: emptyDatabase");
        }
        String[] cmdArgs = { getDbSscript(), "reset", "Factory" };
        try {
            exitVal = CompUtils.executeProcess(cmdArgs, buf);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " DB_ADMIN: isDatabaseServerRunning  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
            }
        } catch (Exception ex) {
            MRLog.error("DB_ADMIN: emptyDatabase " + ex.toString() + " " + buf.toString());
        }
        restart();
        MRLog.debug("DONE DB_ADMIN: emptyDatabase");
        return exitVal;
    }

    public synchronized int createFactoryDatabase() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return 0;
        }
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: createFactoryDatabase");
        }
        //int exitVal = createEmptyDatabase();
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
        MRLog.debug("DONE DB_ADMIN: createEmptyDatabase");
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
            return ;
        }
        StringBuffer buf = new StringBuffer();
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: setManualMode  setManualRestarting true");
            }
            SednaNativeModule dbModule2 = (SednaNativeModule) dbModule;
            dbModule2.setManualRestarting(true);
        } catch (Exception ex) {
            MRLog.error("setManualMode 2 " + ex.toString() + " " + buf.toString());
        }
    }

    public void setAutoMode() throws DBExecutionException, InterruptedException {
        if (dbOnRemoteBox) {
            return ;
        }
        StringBuffer buf = new StringBuffer();
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug(" DB_ADMIN: setAutoMode  setManualRestarting false");
            }
            SednaNativeModule dbModule2 = (SednaNativeModule) dbModule;
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
        if(file2.exists()){
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
        try{
            factodryDBStatusFile.write("DB_ADMIN: database server restarting", false);
            factodryDBStatusFile2.write("DB_ADMIN: database server restarting", false);
            MRLog.debug("DB_ADMIN: database server restarting");
            StringBuffer buf = new StringBuffer();
            setManualMode();
            exitVal=1;
            while (exitVal!=0) {
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
                if(exitVal!=0){
                    //wait before try next time .. 
                    try {
                        Thread.sleep(1000);
                    } catch (Exception ex) {
                    }
                }                
            }
            setAutoMode();
            MRLog.debug("DB_ADMIN: database server restarted");
        }finally{
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
        exitVal=1;
        while(exitVal !=0 ) {
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
            if(exitVal!=0){
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
        exitVal=1;
        while (exitVal!=0) {
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
            if(exitVal!=0){
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
        if (import_useSednaUtils) {
            directoryToDB_xq(directory, "", collectionName, isImport, params);
        } else {
            directoryToDB(directory, "", collectionName, isImport, params);
        }
    }

    private void directoryToDB(File directory, String parentCollectionName, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            String ID = "directoryToDB-" + directory.getName();
            boolean isRootCollection = collectionName == null || collectionName.trim().equals("");

            if (!isImport) {
                if (isRootCollection) {
                    deleteDB(params);
                } else {
                    try {
                        db.deleteCollection(ID, "", 1, collectionName);
                    } catch (Exception ex) {
                    }
                }
            }
            if (!isRootCollection) {
                boolean bExist = db.isCollectionExists(ID, "", collectionName);
                if (!bExist) {
                    db.addChildCollection(ID, "", 1, collectionName, "");
                }
            }

            boolean saveToCol = false;
            String domainName = "";
            if (params.isSaveToColEnable()) {
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collectionName);
                if (domainName != null) {
                    saveToCol = true;
                }
            }
            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    addFileToDB(file, collectionName, isRootCollection, params, isImport);
                }
            }
            if (saveToCol) {
                for (int i = 0; i < dirs.length; i++) {
                    File file = dirs[i];
                    if (file.isDirectory()) {
                        File[] childirs = file.listFiles();
                        for (int k = 0; k < childirs.length; k++) {
                            File childFile = childirs[k];
                            if (!childFile.isDirectory()) {
                                addFileToDB(childFile, collectionName, false, params, isImport);
                            }
                        }
                    }
                }
                return;
            }
            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    String collectionNameChild = dir.getName();
                    directoryToDB(dir, collectionName, collectionNameChild, isImport, params);
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

    private void directoryToDB_xq(File directory, String parentCollectionName, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        File xquery_file = null;
        BufferedWriter queries = null;
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

            createCollections(directory, parentCollectionName, collectionName, isImport, params);
            xquery_file = DBAdminUtils.getScript("loaddocs");
            queries = new BufferedWriter(new FileWriter(xquery_file, false));
            create_load_docs_xquery(xquery_file.getAbsolutePath(), queries, directory, parentCollectionName, collectionName, isImport, params);
            queries.write(" <a/>");
            queries.flush();
            queries.close();
            queries = null;

            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: "+CompUtils.getContents(xquery_file));
            }

            StringBuffer buf = new StringBuffer();
            DBAdminUtils.execute_xquery_file_native(xquery_file.getAbsolutePath(), buf);
            //MRLog.debug(xquery_file.getAbsolutePath() + " : " + buf.toString());

        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("directoryToDB: ", ex);
            throw new DBExecutionException("directoryToDB failed." + ex.getMessage());
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

    private void createCollections(File directory, String parentCollectionName, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            String ID = "createCollections-" + directory.getName();
            boolean isRootCollection = collectionName == null || collectionName.trim().equals("");
            if (!isImport) {
                if (isRootCollection) {
                    deleteDB(params);
                    //isImport = false;
                } else {
                    try {
                        if (!isDeleteCollectionSkip(collectionName)) {
                            db.deleteCollection(ID, "", 1, collectionName);
                        }
                    } catch (Exception ex) {
                    }
                }
            }
            if (!isRootCollection) {
                boolean bExist = db.isCollectionExists(ID, "", collectionName);
                if (!bExist) {
                    db.addChildCollection(ID, "", 1, collectionName, "");
                }
            }

            boolean saveToCol = false;
            String domainName = "";
            if (params.isSaveToColEnable()) {
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collectionName);
                if (domainName != null) {
                    saveToCol = true;
                }
            }
            //restore files from current directory
            File[] dirs = directory.listFiles();
            if (saveToCol) {
                return;
            }
            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    String collectionNameChild = dir.getName();
                    createCollections(dir, collectionName, collectionNameChild, isImport, params);
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("createCollections: ", ex);
            throw new DBExecutionException("createCollections failed." + ex.getMessage());
        }
    }

    private void insertUsers(File file, String docuri) {
        StringBuffer listNodes = new StringBuffer();
        try {
            db.execute("insertUsers", "document('" + docuri + "')", listNodes);
        } catch (Exception e) {

        }
        if (!listNodes.equals("")) {
            try {
                String xml = CompUtils.getContents(file);
                xml = CompUtils.removeXmlProlog(xml);
                db.execute("insertUsers", "update replace $b in document('" + docuri + "')/* with " + xml, listNodes);
            } catch (Exception e) {
            }
        } else {
            try {
                db.addDocument("addFileToDB", docuri, "", DBAdminUtils.getContents(file));
            } catch (Exception e) {
            }

        }
    }

    private void create_load_docs_xquery(String xquery_file, BufferedWriter queries, File directory, String parentCollectionName, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, IOException {

        boolean isRootCollection = collectionName == null || collectionName.trim().equals("");
        boolean saveToCol = false;
        String domainName = "";
        if (params.isSaveToColEnable()) {
            domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collectionName);
            if (domainName != null) {
                saveToCol = true;
            }
        }
        //restore files from current directory

        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File file = dirs[i];
            if (!file.isDirectory()) {
                String docuri = getFileToDBName(file, collectionName, isRootCollection, params, isImport);
                String addquery = "";
                if (isRootCollection) {
                    if (docuri.equalsIgnoreCase("$db_security_data")) {
                        //users needs to be managed by separate db module...
                        insertUsers(file, docuri);
                    } else {
                        addquery = "declare boundary-space preserve; LOAD \"" + file.getAbsolutePath().replace('\\', '/') + "\" \"" + docuri.trim() + "\" &\n";
                        queries.write(addquery);
                    }
                } else {
                    addquery = "declare boundary-space preserve; LOAD \"" + file.getAbsolutePath().replace('\\', '/') + "\" \"" + docuri.trim() + "\" \"" + collectionName.trim() + "\" &\n";
                    queries.write(addquery);
                }

            }
        }
        if (saveToCol) {
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (file.isDirectory()) {
                    File[] childirs = file.listFiles();
                    for (int k = 0; k < childirs.length; k++) {
                        File childFile = childirs[k];
                        if (!childFile.isDirectory()) {
                            String docuri = getFileToDBName(childFile, collectionName, false, params, isImport);
                            String addquery = "";
                            addquery = "declare boundary-space preserve; LOAD \"" + childFile.getAbsolutePath().replace('\\', '/') + "\" \"" + docuri.trim() + "\" \"" + collectionName.trim() + "\" &\n";
                            queries.write(addquery);
                        }
                    }
                }
            }
            return;
        }
        for (int i = 0; i < dirs.length; i++) {
            File dir = dirs[i];
            if (dir.isDirectory()) {
                String collectionNameChild = dir.getName();
                create_load_docs_xquery(xquery_file, queries, dir, collectionName, collectionNameChild, isImport, params);

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
                if (docname.equalsIgnoreCase("$db_security_data")) {
                    //users needs to be managed by separate db module...
                    insertUsers(file, docname);
                } else {
                    db.addDocument("addFileToDB", docname, collectionName, DBAdminUtils.getContents(file));
                }
            } else {
                //MRLog.debug("before add to db  t0 " + (System.currentTimeMillis() - t0));
                db.addDocument("addFileToDB", docname, collectionName, DBAdminUtils.getContents(file));
                //MRLog.debug("after add to db  t0 " + (System.currentTimeMillis() - t0));
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

    private void dbToDirectory(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        StringBuffer xml = new StringBuffer();
        while (true) {
            try {
                db.execute("dbToDirectory_$documents", "for $b in document('$documents')/* return $b", xml);
                break;
            } catch (Exception ex) {
            }
        }
        Element colEL = EngineMDCUtils.stringToElement(xml.toString());
        //backupToDirectory(new File(folder), colEL);
        dbToDirectory(directory, "", colEL, params);
    }

    private void dbToDirectory(File directory, String collectionName, Element colEL, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            SaveFileQueue saveFileQueue = new SaveFileQueue();
            SaveFileQueueCompletedListenerImpl listener = new SaveFileQueueCompletedListenerImpl();
            saveFileQueue.addSaveFileQueueCompletedListener(listener);
            if (!directory.exists() && !directory.mkdir())
                throw new DBExecutionException("Could not create directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canWrite())
                throw new DBExecutionException("Insufficient permissions to write to the directory '" + directory + "'.");

            boolean isRootCollection = collectionName == null || collectionName.trim().equals("");

            // Get all resources, save them to the given local directory.
            boolean saveToCol = false;
            String domainName = "";
            if (params.isSaveToColEnable()) {
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collectionName);
                if (domainName != null) {
                    saveToCol = true;
                }
            }
            SaveParams saveParams = new SaveParams(directory, params, isRootCollection, saveToCol);
            saveFileQueue.setSaveParams(saveParams);
            saveFileQueue.start();
            StringBuffer query = new StringBuffer();
            if (isRootCollection) {
                //query.append("for $b in data(document('$documents')//documents/document/@name )");
                //query.append("let $d := document($b)");
                //query.append("return <a> {$b , $d }</a>  ");
                /*
                   for $b in data(document('$documents')//documents/document/@name )
                       let $d := document($b)
                   return $d 
                   for $b in data(document('$documents')//documents/document/@name )
                       let $d := document($b)
                   return    <a> {$b , $d }</a>  
                                      
                */
                List<?> listDocs = Dom4jUtils.getList(colEL, "document");
                for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                    Element childColEL = (Element) i.next();
                    String docUri = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                    String xml = db.getDocument("dbToDirectory", domainName, 2, docUri, collectionName);
                    //add to save queue
                    saveFileQueue.addDocumentToQueue(docUri, xml);
                }
            } else {
                if (domainName != null) {
                    query.append("for $b in collection('" + collectionName + "')/*");
                    query.append("return $b  ");
                    db.execute("dbToDirectory", collectionName, query.toString(), saveFileQueue);
                } else {
                    List<?> listDocs = Dom4jUtils.listXpath(colEL, "//collection[@name='" + collectionName + "']/document");
                    for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                        Element childColEL = (Element) i.next();
                        String docUri = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                        String xml = db.getDocument("dbToDirectory", domainName, 2, docUri, collectionName);
                        //add to save queue
                        saveFileQueue.addDocumentToQueue(docUri, xml);
                    }
                }
            }
            /* List<?> listDocs = Dom4jUtils.getList(colEL, "document");
            for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                Element childColEL = (Element) i.next();
                String docUri = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                String xml = db.getDocument("dbToDirectory", domainName, 2, docUri, collectionName);
                //add to save queue
                saveFileQueue.addDocumentToQueue(xml, docUri);
            }*/
            //indicate that queue is over
            saveFileQueue.setStop(true);
            listener.wait_to_complete();
            List<?> list = Dom4jUtils.getList(colEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element childColEL = (Element) i.next();
                String childCollectionName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                File childDirectory = new File(directory.getPath() + "/" + childCollectionName);
                childDirectory.mkdirs();
                dbToDirectory(childDirectory, childCollectionName, childColEL, params);
            }

        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            MRLog.error("Exception: ", ex);
            throw new DBExecutionException("dbToDirectory failed. " + ex.getMessage());
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
    private void ensureIndexes(String packetID, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            String domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix);
            if (MRLog.isDbadminLogEnable()) {
                MRLog.info("DB_ADMIN: create index for domainName '" + domainName + "'");
            }
            upgradeIndexes(packetID + "_" + directory.getAbsolutePath(), domainName);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void upgradeIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            StringBuffer xml = new StringBuffer();
            db.execute("dbToDirectory", "for $b in document('$indexes') return $b", xml);
            Element EL = EngineMDCUtils.stringToElement(xml.toString());

            if (MRLog.isDbadminLogEnable()) {
                MRLog.info("DB_ADMIN: upgradeIndexes for domainName '" + domainName + "'");
            }
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                Element indexEL = Dom4jUtils.getElementXpath(EL, "//index[@name='" + node.getIndexName() + "']");
                if (indexEL == null) {
                    createIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                    createIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
                } else {
                    String noPath_new = "child::*/attribute::NID";
                    String noPath = Dom4jUtils.getAttributeValue(indexEL, "on_path", "").trim();
                    if (!noPath_new.equalsIgnoreCase(noPath)) {
                        deleteIndex(ID, node.getIndexName(), null);
                        deleteIndex(ID, node.getIndexNameCol(), null);
                        createIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                        createIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }

    }

    private void createIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.info("DB_ADMIN: Create Indexes for domainName '" + domainName + "'");
            }
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                createIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                createIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void createIndex(String ID, String indexName, String docName, String collectionName, String onPath) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.info("DB_ADMIN: Create Index for domainName '" + indexName + "' collectionName " + collectionName + " onPath " + onPath);
            }
            db.addIndex(ID, indexName, docName, collectionName, onPath);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void deleteIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                deleteIndex(ID, node.getIndexName(), null);
                deleteIndex(ID, node.getIndexNameCol(), null);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void deleteIndex(String ID, String indexName, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            if (MRLog.isDbadminLogEnable()) {
                MRLog.info("DB_ADMIN: delete Index indexName='" + indexName + "'");
            }
            db.deleteIndex(ID, indexName, collectionName);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    public void deletedb_xquery(String dbquery, String domainName) throws Exception {
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(domainName);
        String collectionName = requestMDCParams.getCollectionName().trim();
        List<String> listNodes = new ArrayList<String>();
        //MRLog.debug( "dbadmin_DeleteQuery \n "+ dbquery);
        db.execute("dbadmin_DeleteQuery", dbquery, listNodes);
        boolean delete_useSednaUtils = IpvsServGlobals.getBooleanProperty("admindbapi.delete_useSednaUtils", false);

        if (!delete_useSednaUtils) {
            for (String docURI : listNodes) {
                String dropdoc = "";
                try {
                    docURI = docURI.trim();
                    dropdoc = "DROP DOCUMENT '" + docURI + "' IN COLLECTION '" + collectionName + "'  ";
                    db.executeXQueryToUpdate("dbadmin_Delete_" + docURI, collectionName, dropdoc);
                } catch (Exception ex) {
                    MRLog.error(docURI +" " + dropdoc, ex);
                }
            }
        } else {
            File xquery_file = null;
            BufferedWriter queries = null;
            StringBuffer buf = new StringBuffer();
            try {
                xquery_file = DBAdminUtils.getScript("delet_docs");
                queries = new BufferedWriter(new FileWriter(xquery_file, false));
                for (String docURI : listNodes) {
                    String dropdoc = "";
                    try {
                        docURI =docURI.trim();
                        dropdoc = "declare boundary-space preserve; DROP DOCUMENT \"" + docURI + "\" IN COLLECTION \"" + collectionName + "\" &\n";
                        //db.executeXQueryToUpdate("dbadmin_Delete_" + docURI, collectionName, dropdoc);
                        queries.write(dropdoc);
                        queries.flush();
                    } catch (Exception ex) {
                        MRLog.error(docURI +" " + dropdoc, ex);
                    }
                }
                queries.write(" for $b in document('$indexes') return $b/dummyname");
                queries.flush();
                queries.close();
                //MRLog.debug(CompUtils.getContents(xquery_file));
                DBAdminUtils.execute_xquery_file_native(xquery_file.getAbsolutePath(), buf);
                //MRLog.debug(xquery_file.getAbsolutePath() + " : " + buf.toString());

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
}
