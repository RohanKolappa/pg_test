package com.ipvsserv.nxd.migration;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServer;

public class DBMigrationModule extends DBMigrationModuleBase {
    protected boolean runningInsert = false;
    protected boolean skipCreateBackup = true;
    protected boolean upgradeIndex = true;
    protected boolean saveToColEnable = true;
    protected boolean saveForDiff = false;
    protected boolean createHistory = false;  
    public DBMigrationModule() {
        super("DBMigration");
        setMigrationType(MIGRATION_TYPE_DB_LIVE);
    }

    public DBMigrationModule(String moduleName) {
        super(moduleName);
        setMigrationType(MIGRATION_TYPE_DB_LIVE);
    }
    public void migrate() throws Exception {
        createHistory = false;
        skipCreateBackup = IpvsServGlobals.getBooleanProperty("test.skipCreateBackup", false);
        databaseMigrationStatus = new IpvsFileStatus(IpvsFileStatus.STATUS_DB_MIGRATING_FILE);
        databaseMigrationStatus.init(true);
        menuStatusFile = new IpvsFileStatus(IpvsFileStatus.MIGRATION_STATUS_FILE, true);
        menuStatusFile.init(true);
        dbadmin.clearManualMode();
        try{
            CompUtils.deleteChildrenDirectory(new File( IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup"), "scripts") );
        } catch (Exception ex) {
        }
        try {
            IpvsServer server = IpvsServer.getInstance() ;
            softwareServiceVersion = server.getServerVersion();
            int counter=0;
            long to=System.currentTimeMillis();
            while(!server.isFactoryDBCheckDone()){
                Thread.sleep(500);
                counter++;
                if(counter>20){
                    MRLog.debug("Database Migration: Wait to connect to database and fectory db status check...(" +(System.currentTimeMillis()-to)+")");
                    counter=0;
                }
            }
            
            dBDocuments = new DBDocuments();
            List<String> domains = dbadmin.getIpvsDBDriver().getDomainNames();
            dBDocuments.setDomainNames(domains);
            if(domains==null || domains.isEmpty()){
                debug("migration: No valid databse found.");
            }
            dbServiceVersion = getDBServiceVersion();
            message_mig = "DB_MIG " + dbServiceVersion + " TO " + softwareServiceVersion;
            debug("Database is at service version " + dbServiceVersion + " software version is " + softwareServiceVersion);
            debugForMenu("Database is at service version " + dbServiceVersion + " software version is " + softwareServiceVersion);
            if (dbServiceVersion == null || dbServiceVersion.trim().equals("")) {
                debug("Service Version not found in database.");
                throw new Exception("Service Version not found in database.");
            }

            //check the db is not in future
            if (checkDBInFutureVersion(dbServiceVersion, softwareServiceVersion)) {
                debug("WARNING: Database is in future version ("+dbServiceVersion+")..... reseting to default.");
                DBFactoryResetModule dbFactoryResetModule = (DBFactoryResetModule)IpvsServer.getInstance().getModule(DBFactoryResetModule.class);
                dbFactoryResetModule.reset(true);
                completed=true;
                return;
            }
            
            //fix a bug 
            String xml = "";
            try {
                xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
            } catch (Exception ex) {
            }
            if (xml != null && !xml.equals("")) {
                //looks like there is incomplete one from previous migration
            } else if (softwareServiceVersion.equals(dbServiceVersion)) {
                debug("Database is upto date, no need for migration.");
                completed=true;
                return;
            }
            
            
            completed = do_migrate();
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration", ex);
        } finally {
            if(databaseMigrationStatus !=null){
                databaseMigrationStatus.removeStatusFile(true);
            }
            if (!completed) {
                debug("Database Migration Failed.");
                IpvsFileStatus.createDBHistory("DATABASE MIGRATION FAILED", "Database was at service version " + dbServiceVersion + " \n Software version is " + softwareServiceVersion + " \n Time " + (System.currentTimeMillis() - startDate) + " milisec ");
                debugForMenu("DATABASE MIGRATION FAILED. Service Version from " + dbServiceVersion + " to " + softwareServiceVersion + " (" + (System.currentTimeMillis() - startDate) + " milisec) ");
            } else {
                IpvsServer.getInstance().setMigrationComplete(true);
            	if(createHistory){
                    debug("Database Migration Completed Successfully.");
                    IpvsFileStatus.createDBHistory("DATABASE MIGRATION COMPLETED SUCCESSFULLY", "Database was at service version " + dbServiceVersion + " \n Software version is " + softwareServiceVersion + " \n Time " + (System.currentTimeMillis() - startDate) + " milisec ");
                    debugForMenu("DATABASE MIGRATION COMPLETED SUCCESSFULLY. Service Version from " + dbServiceVersion + " to " + softwareServiceVersion + " (" + (System.currentTimeMillis() - startDate) + " milisec) ");
            	} else {
            		//do nothing ....
            	}
            }
        }
    }
    protected boolean do_migrate() throws Exception {
        try {
        	createHistory=true;
            upgradeIndexes();
            //deleteIndexes();
            ScriptVariablesGlobal.setModuleName(getIpvsModuleName());
            scriptGlobals = new ScriptVariablesGlobal(softwareServiceVersion, dbServiceVersion, dbadmin.getIpvsDBDriver(), getMigrationType());
            //createMigrationStatus
            createMigrationStatus(dbServiceVersion, softwareServiceVersion);
            while (!scriptGlobals.getNextServiceVersion().equals("")) {
                try {
                    serviceVersion = scriptGlobals.getServiceVersion();
                    //status_startDBMigration
                    status_startDBMigration();

                    //do migrate from current version to next post migration version
                    migrateServiceVersion = new ServiceVersionMigrateImpl(this);
                    migrateServiceVersion.init(migrateServiceVersion, migrateServiceVersion, null);
                    migrateServiceVersion.start();

                    //set version in db
                    updateDBServiceVersion(scriptGlobals.getaftermigration_serviceVersion());

                    //status_doneDBMigration
                    status_doneDBMigration();
                } finally {
                    ServiceVersionMigrateImpl.removeInstance();
                }
            }
            //removeMigrationStatus
            removeMigrationStatus(dbServiceVersion, softwareServiceVersion);
            //createIndexes();
             
            return true;
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration", ex);
            migrationError = message_mig + " Database migration failed, during migration ver " + serviceVersion + " to " + scriptGlobals.getaftermigration_serviceVersion() + " " + ex.getMessage();
            debug(migrationError);
            //IpvsServer.getInstance().stop();
            return false;
        }

    }
   
    @Override
    public void start() throws Exception {
        super.startMonitor();
    }

    @Override
    public void verifyState() throws Exception {
        synchronized (this) {
            if (runningInsert) {
                return;
            }
            runningInsert = true;
        }
        try {

            migrate();
            started=true;
            initialized = true;
           
            try {
                destroy();
                MRLog.debug("DBMigration process end........ completed ? " + completed +" createHistory? " + createHistory);
                return;
            } catch (Exception ex) {

            }
            executed = true;
        } catch (Exception ex) {
            MRLog.error("DBMigration", ex);
        } finally {
            synchronized (this) {
                runningInsert = false;
            }
        }
    }
    
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
 

    @Override
    public void destroy() throws Exception {
        //stop if the app is started as taskName = SednaNativeDBTest /exist test / back up /bulk load only
        try{
            if(dbadmin !=null){
                dbadmin.clearManualMode();
            }
            if(databaseMigrationStatus !=null){
                databaseMigrationStatus.removeStatusFile(true);
            }
            /*if(databaseMigrationFlag !=null){
                databaseMigrationFlag.removeStatusFile(true);
            }*/
            if(menuStatusFile !=null){
                menuStatusFile.removeStatusFile(true);
            }
        } catch (Exception ex) {
            MRLog.debug(ex.toString()+ "  menuStatusFile=null "+(menuStatusFile==null)  +" databaseMigrationStatus=null "+(databaseMigrationStatus==null) );
        }
        super.destroy();
    }

    /////////////////////// private methods ////////////////////
 
    protected void upgradeIndexes() {
        try {
            if (upgradeIndex) {
                upgradeIndex = false;
                DBAdminParams params = new DBAdminParams();
                dbadmin.upgradeIndexes(params);
            }
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration upgradeIndexes", ex);
        } finally {

        }
    }

    protected void deleteIndexes() {
        try {
            DBAdminParams params = new DBAdminParams();
            dbadmin.deleteIndexes(params);
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration deleteIndexes", ex);
        } finally {

        }
    }

    protected void createIndexes() {
        try {
            DBAdminParams params = new DBAdminParams();
            dbadmin.createIndexes(params);
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration createIndexes", ex);
        } finally {

        }
    }

    
    //////////////////
    public void debug(String msg) {
        String status = "(" + message_mig + " :V" + serviceVersion + " S:" + (System.currentTimeMillis() - curDate) + " T:" + (System.currentTimeMillis() - startDate + ")");
        StringBuffer buf = new StringBuffer();
        buf.append("Migration=\"" + status + " " + msg.replace('\"', '\'') + "\"");
        MRLog.debug(status + " " + msg);
        curDate = System.currentTimeMillis();
    }

    public void debugForMenu(String msg) {
        String status = "(" + message_mig + " :V" + serviceVersion + " S:" + (System.currentTimeMillis() - curDateMenu) + " T:" + (System.currentTimeMillis() - startDate + ")");
        StringBuffer buf = new StringBuffer();
        buf.append("Migration=\"" + status + " " + msg.replace('\"', '\'') + "\"");
        menuStatusFile.write(buf.toString(), false);
        MRLog.debug(status + " " + msg);
        curDateMenu = System.currentTimeMillis();
    }

    public void println(String msg) {
        System.out.print("DB_MIG:" + serviceVersion + " S:" + (System.currentTimeMillis() - curDate) + " T:" + (System.currentTimeMillis() - startDate) + " " + msg);
        curDate = System.currentTimeMillis();
    }

    ///////////////

    /////////////////////////loadModule/////////////////////////
    protected String getDBServiceVersion() {
        try {
            String dbServiceVersion = dbadmin.getIpvsDBDriver().getDBServiceVersion();
            if(dbServiceVersion !=null && !dbServiceVersion.equals(""))
                return dbServiceVersion;
        } catch (Exception ex) {
            MRLog.error("Service version read from db failed " + ex.getMessage());
        }
        String empty_dbServiceVersion = null;
        try {
            empty_dbServiceVersion = IpvsServGlobals.getProperty(getIpvsModuleName() + ".empty_dbServiceVersion", "1.0");
            Document doc = Dom4jUtils.loadXMLResource(IpvsServer.SERVER_VERSION_XML);
            Dom4jUtils.addAttribute(doc.getRootElement(), "Ver", empty_dbServiceVersion);
            File file = new File(IpvsServer.SERVER_VERSION_XML);
            dbadmin.getIpvsDBDriver().addDocument("add_default_serviceVersion", file.getName(), "", doc.getRootElement().asXML());
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return empty_dbServiceVersion;
    }

    protected void updateDBServiceVersion(String ver) {
        try {
            Document doc = Dom4jUtils.loadXMLResource(IpvsServer.SERVER_VERSION_XML);
            Dom4jUtils.addAttribute(doc.getRootElement(), "Ver", ver);
            File file = new File(IpvsServer.SERVER_VERSION_XML);
            dbadmin.getIpvsDBDriver().updateDocument("updateDBServiceVersion", "", 2, file.getName(), "", doc.getRootElement().asXML(), "", "");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    protected boolean checkDBInFutureVersion(String dbServiceVersion, String softwareServiceVersion) {
        //dbServiceVersion="1.200c.30";
        //softwareServiceVersion="1.3c.30";
        boolean retult = false;
        try {
            String[] arrdbServiceVersion = dbServiceVersion.split("[.]");
            String[] arrsoftwareServiceVersion = softwareServiceVersion.split("[.]");
            if (arrdbServiceVersion.length > arrsoftwareServiceVersion.length) {
                return true;
            }
            for (int i = 0; i < arrdbServiceVersion.length; i++) {
                if (i < arrsoftwareServiceVersion.length) {
                    String dbVersion = arrdbServiceVersion[i];
                    String softVersion = arrsoftwareServiceVersion[i];
                    int idbVersion = CompUtils.intValue(dbVersion, -1);
                    int isoftVersion = CompUtils.intValue(softVersion, -1);
                    if (idbVersion != -1 && isoftVersion != -1) {
                        if (idbVersion > isoftVersion) {
                            //System.out.println("idbVersion > isoftVersion " + idbVersion + ">" + isoftVersion);
                            retult = true;
                            return retult;
                        } else if (idbVersion < isoftVersion) {
                            //System.out.println("idbVersion < isoftVersion " + idbVersion + "<" + isoftVersion);
                            retult = false;
                            return retult;
                        } else if (idbVersion == isoftVersion) {
                            //System.out.println("idbVersion == isoftVersion " + idbVersion +"=="+isoftVersion);
                        }
                    } else {
                        MRLog.error("service version contains non digits dbVersion='" + dbVersion + "' softVersion='" + softVersion + "'. Doing alphanumeric comparision");
                        for (int k = 0; k < dbVersion.length(); k++) {
                            if (k < softVersion.length()) {
                                String cdbVersion = dbVersion.charAt(k) + "";
                                String csoftVersion = softVersion.charAt(k) + "";
                                if (Character.isDigit(dbVersion.charAt(k)) && Character.isDigit(softVersion.charAt(k))) {
                                    idbVersion = CompUtils.intValue(cdbVersion, 0);
                                    isoftVersion = CompUtils.intValue(csoftVersion, 0);
                                    if (idbVersion > isoftVersion) {
                                        //System.out.println("idbVersion  >isoftVersion " + idbVersion + ">" + isoftVersion);
                                        retult = true;
                                        return retult;
                                    } else if (idbVersion < isoftVersion) {
                                        //System.out.println("idbVersion  < isoftVersion " + idbVersion + "<" + isoftVersion);
                                        retult = false;
                                        return retult;
                                    } else if (idbVersion < isoftVersion) {
                                        //System.out.println("idbVersion  == isoftVersion " + idbVersion +"=="+isoftVersion);
                                    }
                                } else {
                                    if (dbVersion.charAt(k) > softVersion.charAt(k)) {
                                        //System.out.println("cdbVersion  > csoftVersion " + cdbVersion + ">" + csoftVersion);
                                        retult = true;
                                        return retult;
                                    } else if (dbVersion.charAt(k) < softVersion.charAt(k)) {
                                        //System.out.println("cdbVersion  < csoftVersion " + cdbVersion + ">" + csoftVersion);
                                        retult = false;
                                        return retult;
                                    } else if (dbVersion.charAt(k) == softVersion.charAt(k)) {
                                        //System.out.println("cdbVersion  == csoftVersion " + cdbVersion +"=="+csoftVersion);
                                    }
                                }

                            }
                        }
                    }
                }
            }
            return false;
        } finally {
            //System.out.println(retult + " dbServiceVersion  > softwareServiceVersion " + dbServiceVersion + " >? " + softwareServiceVersion);
            MRLog.debug(retult + " dbServiceVersion  > softwareServiceVersion " + dbServiceVersion + " >? " + softwareServiceVersion);
        }
    }

    /////////////////////////////////////////////////// 
    protected static boolean useUniqueName=false;
    protected String getBackFolderPath(String dbServiceVersion, String softwareServiceVersion) {
        String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbMigrationBackupArea", "");        
        String suffix = "";
        if(useUniqueName){
            Date date = new Date();
            SimpleDateFormat format = new SimpleDateFormat("EEE-MMM-dd-HH-mmsszzzyyyy");
            suffix = format.format(date);
           
        } else {
            suffix = "dbmigration";
            
        }
        String fileName = "From-" + dbServiceVersion.trim() + "-To-" + softwareServiceVersion.trim() + "-"+suffix;
        File dir = new File(dbMigrationBackupArea + "/db");
        if (!dir.exists()) {
            dir.mkdirs();
        }
        File dbFile = new File(dir, fileName);
        dbFile.mkdirs();
        String path = dbFile.getAbsolutePath().replace('\\', '/');
        return path;
    }

    protected void createMigrationStatus(String dbServiceVersion, String softwareServiceVersion) throws Exception {
        try {
            /*
                <migration_status startVer="1.0" targetVer="1.5" backupPath="C:/data/dbmigration/backup/From1.0To1.5DateTueSep01134939PDT2009">
                 <completed fromVer="1.0" toVer="1.3" backupfile=""/>
                 <completed fromVer="1.3" toVer="1.4" backupfile=""/>
                 <progress fromVer="1.4" toVer="1.5" backupfile=""/>
                </migration_status>
            */

            DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
            String xml = "";
            try {
                xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
            } catch (Exception ex) {
            }
            if (!useUniqueName || xml == null || xml.equals("")) {
                backFolderPath = getBackFolderPath(dbServiceVersion, softwareServiceVersion);
                debug("START ver " + dbServiceVersion + " to " + softwareServiceVersion + "  Create backup of database '" + backFolderPath + "'");
                File directory = new File(backFolderPath);
                CompUtils.deleteChildrenDirectory(directory.getParentFile());
                directory.mkdirs();
                DBAdminParams params = new DBAdminParams();
                params.setSaveForDiff(saveForDiff);
                params.setSaveToColEnable(saveToColEnable);

                if (skipCreateBackup) {
                } else {
                    debug("ver " + dbServiceVersion + " to " + softwareServiceVersion + "  pre_migration_backup '" + IpvsFileStatus.FILE_PRE_MIGRATION_DATABASE_TGZ + "'");
                    File fileFlag = new File(IpvsFileStatus.FILE_MIGRATION_PROGRESS);
                    fileFlag.delete();
                    File file = new File(IpvsFileStatus.FILE_PRE_MIGRATION_DATABASE_TGZ);
                    file.delete();
                    String[] cmdArgs = { dbadmin.getDbSscript(), "export", IpvsFileStatus.FILE_PRE_MIGRATION_DATABASE_TGZ , "pre_migration_backup", "usefullpath"};
                    while(!file.exists()){
                        /*if(IpvsServGlobals.isWindowsOS()){
                            File file_backup = new File("C:/data/sedna_dbsnapshot/backup/msdb.tgz"); 
                            file_backup.renameTo(file);
                            break;
                        }*/                               
                        //create a pre_migration_backup
                        StringBuffer buf = new StringBuffer();
                        try {
                            //first attempt could fail if the sednactl is busy
                            int exitVal = CompUtils.executeProcess(cmdArgs, buf);
                            if (MRLog.isDbadminLogEnable()) {
                                MRLog.debug("DB_ADMIN: exitVal=" + exitVal + " pre_migration_backup  " + CompUtils.joinArray(cmdArgs, " ") + buf.toString());
                            }
                        } catch (Exception ex) {
                            MRLog.error("DB_ADMIN: pre_migration_backup " + ex.toString() +" " + CompUtils.joinArray(cmdArgs, " ")+ " " + buf.toString());
                        }   
                        if(!file.exists()){
                            //wait before try next time .. 
                            try {
                                Thread.sleep(1000);
                            } catch (Exception ex) {
                            }
                        }
                    }
                     
                    //create the db file 
                    StringBuffer buf = new StringBuffer("");
                    buf.append("<migration_status startVer='" + dbServiceVersion + "'  targetVer='" + softwareServiceVersion + "' backupPath='" + backFolderPath + "' />");
                    dbadmin.getIpvsDBDriver().addDocument("createMigrationStatus", IpvsServGlobals.MIGRATION_PROGRESS_STATUS, "", buf.toString());
                    
                    //continue to back component way... todo: should use the sedna.tgz    
                    backupdb(directory, params);
                }
                debug("DONE ver " + dbServiceVersion + " to " + softwareServiceVersion + "  Create backup of database '" + backFolderPath + "'");
            } else {
                Document doc = null;
                try {
                    doc = Dom4jUtils.stringToDocument(xml);
                    backFolderPath = Dom4jUtils.getAttributeValue(doc.getRootElement(), "backupPath", "");
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                //todo: validate the folder exists and has files in it...
                File directory = null;
                if (backFolderPath != null && !backFolderPath.equals("")) {
                    directory = new File(backFolderPath);
                }
                if (directory == null || !directory.exists()) {
                    //todo: throw error and shut down ?
                    MRLog.error("backup directory does not exists... Resetting to factory");
                    DBFactoryResetModule dbFactoryResetModule = (DBFactoryResetModule)IpvsServer.getInstance().getModule(DBFactoryResetModule.class);
                    dbFactoryResetModule.reset(true);
                } else {
                    debug("START ver " + dbServiceVersion + " to " + softwareServiceVersion + " Previous migration incomplete. Restarting migration with Restore database from '" + backFolderPath + "'");
                    //no need to restoredb.... it can just do xsl apply and than upload
                    DBAdminParams params = new DBAdminParams();
                    params.setSaveForDiff(saveForDiff);
                    params.setSaveToColEnable(saveToColEnable);
                    if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_FILE_SYSTEM){
                        
                    } else {
                        //restoredb directory 
                        restoredb(directory, params);
                    }
                    //create the db file 
                    StringBuffer buf = new StringBuffer("");
                    buf.append("<migration_status startVer='" + dbServiceVersion + "'  targetVer='" + softwareServiceVersion + "' backupPath='" + backFolderPath + "' />");
                    dbadmin.getIpvsDBDriver().addDocument("createMigrationStatus", IpvsServGlobals.MIGRATION_PROGRESS_STATUS, "", buf.toString());
                    
                    debug("DONE ver " + dbServiceVersion + " to " + softwareServiceVersion + " Previous migration incomplete. Restarting migration with Restore database from '" + backFolderPath + "'");
                }
            }


        } catch (Exception ex) {
            MRLog.error("Service version read from db failed " + ex.getMessage());
        }
    }
    protected void restoredb(File directory, DBAdminParams params){
        MRLog.debug("restoredb " + directory.getAbsolutePath());
        try {
            if (skipCreateBackup) {
            } else {
                dbadmin.restoredb(directory, params);
            }
        } catch (Exception ex) {
            MRLog.error("restore db failed " + ex.getMessage(), ex);
        }
        MRLog.debug("restoredb done  " + directory.getAbsolutePath());
    }    
    protected void backupdb(File directory, DBAdminParams params){
        MRLog.debug("backupdb   " + directory.getAbsolutePath());
        try {
            dbadmin.backupdb(directory, params);
        } catch (Exception ex) {
            MRLog.error("backupdb db failed " + ex.getMessage(), ex);
        }
        MRLog.debug("backupdb done  " + directory.getAbsolutePath());
    }
    
    protected void removeMigrationStatus(String dbServiceVersion, String softwareServiceVersion) throws Exception {
        String xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
        Document statusdoc = null;
        try {
            if(xml==null || xml.equals("")){
                debug("ERROR: no "+IpvsServGlobals.MIGRATION_PROGRESS_STATUS+" ??? incomplete migration???? from " + dbServiceVersion + " to " + softwareServiceVersion);
                return;
            }
            debug("Complete migrating from " + dbServiceVersion + " to " + softwareServiceVersion);
            statusdoc = Dom4jUtils.stringToDocument(xml);
            backFolderPath = Dom4jUtils.getAttributeValue(statusdoc.getRootElement(), "backupPath", "");

        } catch (Exception ex) {
            MRLog.error("removeMigrationStatus 1 " + ex.getMessage());
        }
        try {
            File delFile = new File(backFolderPath);
            //debug("(migration "+dbServiceVersion + " to "+ softwareServiceVersion +") Remove backup of database '" + backFolderPath +"'");
            debug("Remove backup of database '" + backFolderPath + "'");
            CompUtils.deleteDirectory(delFile);
        } catch (Exception ex) {
            MRLog.error("removeMigrationStatus 2 " + ex.getMessage());
        }
        try {
            MRLog.debug("removeMigrationStatus:" + statusdoc.getRootElement().asXML());
            dbadmin.getIpvsDBDriver().deleteDocument("removeMigrationStatus", "", 2, IpvsServGlobals.MIGRATION_PROGRESS_STATUS, "");
        } catch (Exception ex) {
            MRLog.error("removeMigrationStatus 3 " + ex.getMessage());
        }
    }

    protected void status_startDBMigration() throws Exception {
        String xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
        Document statusdoc = null;
        try {
            debug("migrating from " + scriptGlobals.getServiceVersion() + " to " + scriptGlobals.getaftermigration_serviceVersion());
            debugForMenu("Start Migrating from " + scriptGlobals.getServiceVersion() + " to " + scriptGlobals.getaftermigration_serviceVersion());

            statusdoc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.addElement(statusdoc.getRootElement(), "progress");
            //<progress Ver="1.4"  backupfile="" migratedVer="1.4" />
            Dom4jUtils.addAttribute(el, "fromVer", scriptGlobals.getServiceVersion());
            Dom4jUtils.addAttribute(el, "toVer", scriptGlobals.getaftermigration_serviceVersion());
            Dom4jUtils.addAttribute(el, "backupPath", "");
            MRLog.debug("status_startDBMigration:" + statusdoc.getRootElement().asXML());
            dbadmin.getIpvsDBDriver().updateDocument("status_startDBMigration", "", 2, IpvsServGlobals.MIGRATION_PROGRESS_STATUS, "", statusdoc.getRootElement().asXML(), "", "");
        } catch (Exception ex) {
            MRLog.error("status_startDBMigration " + ex.getMessage());
        }
    }

    protected void status_doneDBMigration() throws Exception {
        String xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
        Document statusdoc = null;
        try {
            debug("completed migration from " + scriptGlobals.getServiceVersion() + " to " + scriptGlobals.getaftermigration_serviceVersion());
            statusdoc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.element(statusdoc.getRootElement(), "progress");
            el.setName("completed");
            MRLog.debug("status_doneDBMigration:" + statusdoc.getRootElement().asXML());
            dbadmin.getIpvsDBDriver().updateDocument("updateDBServiceVersion", "", 2, IpvsServGlobals.MIGRATION_PROGRESS_STATUS, "", statusdoc.getRootElement().asXML(), "", "");
        } catch (Exception ex) {
            MRLog.error("status_doneDBMigration " + ex.getMessage());
        }
    }
}