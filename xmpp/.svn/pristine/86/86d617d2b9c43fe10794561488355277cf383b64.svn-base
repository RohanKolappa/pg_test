package com.ipvsserv.nxd.dbtask;

import java.io.File;
import java.util.Map;

import org.xmldb.api.base.Collection;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;
import com.ipvsserv.nxd.backup.BackupService;
import com.ipvsserv.nxd.backup.SednaBackupService;
import com.ipvsserv.nxd.backup.VendorBackupService;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTask extends IpvsServBasicModule {
    protected SednaXmlDBModule xmlDBModule = null;
    
    public DBTask() {
        super("IpvsDBTask");
    }
    
    public DBTask(String moduleName) {
        super(moduleName);
    }
    
    protected VendorBackupService getVendorBackupService(String dbVendor) {
        if (dbVendor.equalsIgnoreCase("sedna")) {
            return new SednaBackupService();
        }
        //todo:dbVendor... for exists
        return new SednaBackupService(); 
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        //xmlDBModule = IpvsServer.getInstance().getSednaXmlDBModule(); //will always be null if not running as component
        xmlDBModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
        if (xmlDBModule == null) {
            throw new Exception("Database could not connect.");
        }
    }
    
    @Override
    public void start() throws Exception {
        try {
            String folder = IpvsServGlobals.getDBTaskFolder();
            String dbVendor =IpvsServGlobals.getProperty("admindbapi.dbVendor", "sedna");
            String appname = IpvsServGlobals.getIpvsAppName();
            if (appname != null && appname.equals(IpvsServGlobals.APP_DBTASK)) {
                String dbtask = IpvsServGlobals.getDBTaskName();
                String collectionName = getProperty("collectionName", null);
                if (dbtask.equals("backup")) {
                    backupdb(dbVendor, folder);
                } else if (dbtask.equals("export")) {
                    exportdb(dbVendor, folder, collectionName);
                } else if (dbtask.equals("restore")) {
                    restoredb(dbVendor, folder, collectionName);
                } else if (dbtask.equals("import")) {
                    importdb(dbVendor, folder, collectionName);                    
                } else {
                    throw new Exception("unknown dbtask");
                }
            }
        }catch(Exception ex){
            ex.printStackTrace();
        } finally {
            //Thread.sleep(3000);
            String appname = IpvsServGlobals.getIpvsAppName();
            if (appname != null && appname.equals(IpvsServGlobals.APP_DBTASK)) {
                IpvsServer.getInstance().stop();
            }
        }
    }
    
    public void backupdb(String dbVendor, String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            BackupService backupService = new BackupService(getVendorBackupService(dbVendor), xmlDBModule);
            File directory = new File(folder);
            //may be: make sure to delete old files.
            if (directory.exists()) {
                MRLog.debug("deleteFolder '" + directory.getAbsolutePath() + "'");
                backupService.deleteFolder(directory);
            }
            MRLog.debug("backup db from " + folder);
            backupService.backupdb(directory);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            MRLog.debug("backup done");
        }
    }
    public void exportdb(String dbVendor, String folder, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            //todo: partial export
            BackupService backupService = new BackupService(getVendorBackupService(dbVendor), xmlDBModule);
            File directory = new File(folder);
            //may be: make sure to delete old files.
            if (directory.exists()) {
                MRLog.debug("deleteFolder '" + directory.getAbsolutePath() + "'");
                backupService.deleteFolder(directory);
            }
            MRLog.debug("backup db from " + folder);
            backupService.backupdb(directory);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            MRLog.debug("backup done");
        }
    }
    public void restoredb(String dbVendor, String folder, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.debug("restoring db from " + folder);
            File directory = new File(folder);
            BackupService backupService = new BackupService(getVendorBackupService(dbVendor), xmlDBModule);
            backupService.restoredb(directory, collectionName);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            MRLog.debug("restore done");
        }
    }
    public void importdb(String dbVendor, String folder, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.debug("importing db from " + folder);
            File directory = new File(folder);
            BackupService backupService = new BackupService(getVendorBackupService(dbVendor), xmlDBModule);
            backupService.importdb(directory, collectionName);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            MRLog.debug("import done");
        }
    }
    public Collection createChildCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName) throws XMLDBException {
        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
        return cms.createCollection(collectionName);
    }
    
    @Override
    public void stop() throws Exception {
        destroy();
    }
    
    @Override
    public void destroy() throws Exception {
        try {
            if (IpvsServer.getInstance() != null) {
                if (IpvsServer.getInstance().getModule(SednaXmlDBModule.class) == null) {
                    // if (IpvsServer.getInstance().getSednaXmlDBModule() == null) {
                    MRLog.debug("closing db connection");
                    if (xmlDBModule != null) xmlDBModule.destroy();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public int getLastKnownState() throws Exception {
        // TODO Auto-generated method stub
        return 0;
    }
    
    @Override
    public void verifyState() throws Exception {

    }
    
}
