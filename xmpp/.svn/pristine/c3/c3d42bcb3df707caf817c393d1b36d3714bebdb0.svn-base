package com.ipvsserv.nxd.adminapi;

import java.io.File;
import java.util.List;

import org.xmldb.api.base.XMLDBException;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.api.DBConnectionEventListener;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.server.IpvsServModule;

public interface DBAdminApi extends DBConnectionEventListener{
    //db_server=sedna_native/sedna_xmldb/exist_xmldb 
    public final static String DB_CONNECTION_SEDNA_XMLDB="sedna_xmldb";
    public final static String DB_CONNECTION_SEDNA_NATIVE="sedna_native";
    public final static String DB_CONNECTION_EXIST_XMLDB="exist_xmldb";
    
    public abstract boolean isDBModuleStarted() ;

    public abstract void waitForDBModuleStart() throws InterruptedException;

    public abstract boolean isDBModuleStart() throws Exception;

    public abstract boolean factoryReset(File directory, DBAdminParams adminParams, boolean force) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    //public abstract boolean isFactoryResetNeeded(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void backupdb(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void deleteDB(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;
    
    public abstract void restoredb(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void restoredb(File directory, String collectionName, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void importdb(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void importdb(File directory, String collectionName, String domain, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void upgradeIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void deleteIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract void createIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException;

    public abstract IpvsDBDriver getIpvsDBDriver() throws Exception;

    public abstract IpvsServModule getDBModule() throws Exception;

    //////////////// new api ///////////////////// 

    public abstract void exportdb(File directory, DBAdminParams adminParams, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void exportdb(File directory, DBAdminParams adminParams, List<String> query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    ////////////// delete bulk ////////////
    public abstract void deletedb_xquery(String dbquery, String domainName) throws Exception;    
    
    //////////////// execute ///////////////////// 
    public abstract int createEmptyDatabase () throws DBExecutionException, InterruptedException ;    
    public abstract int createFactoryDatabase() throws DBExecutionException, InterruptedException;
    public abstract int restart()throws DBExecutionException, InterruptedException;
    public abstract int stop()throws DBExecutionException, InterruptedException;
    public abstract int start()throws DBExecutionException, InterruptedException;
    public abstract boolean isDatabaseServerRunning()throws DBExecutionException, InterruptedException;
    public abstract boolean isConnected();
    public abstract boolean isDBModuleInited();
    public abstract void clearManualMode();
    
    public abstract boolean isDeleteResourceSkip(String documentName) ;
    public abstract boolean isAddResourceSkip(String documentName) ;
    public abstract boolean isDeleteCollectionSkip(String name) ;
    public abstract boolean isAddCollectionSkip(String name) ;
    
    public abstract String getDbSscript();
    public abstract String getDBConnectionName();
    
}
