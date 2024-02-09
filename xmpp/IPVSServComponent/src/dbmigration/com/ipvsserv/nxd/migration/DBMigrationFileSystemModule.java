package com.ipvsserv.nxd.migration;

import java.io.File;
import java.util.Map;

import org.dom4j.Document;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServer;

public class DBMigrationFileSystemModule extends DBMigrationModule {
    
    public DBMigrationFileSystemModule() {
       super("DBMigrationFileSystem");
       setMigrationType(MIGRATION_TYPE_FILE_SYSTEM);
    }
 
    public DBMigrationFileSystemModule(String moduleName) {
        super(moduleName);
        setMigrationType(MIGRATION_TYPE_FILE_SYSTEM);
    }
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        File fileFlag = new File(IpvsFileStatus.FILE_MIGRATION_PROGRESS);
        fileFlag.delete();
        File file = new File(IpvsFileStatus.FILE_PRE_MIGRATION_DATABASE_TGZ);
        file.delete();
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

            //create a flag to indicate the migration_progress
            databaseMigrationFlag = new IpvsFileStatus(IpvsFileStatus.FILE_MIGRATION_PROGRESS);
            databaseMigrationFlag.init(true);
            
            //create a copy for xsl appling 
            copyFiles();
            String loopServiceVersion=dbServiceVersion;
            MRLog.debug("Apply xsl on local file system on folder '"+xslApplyFolder+"'");
            while (!scriptGlobals.getNextServiceVersion(loopServiceVersion).equals("")) {
                try {
                    //MRLog.debug("sv-" + serviceVersion + " dv-" + scriptGlobals.getaftermigration_serviceVersion() +" l " + loopServiceVersion);
                    serviceVersion = scriptGlobals.getServiceVersion();
                    //status_startDBMigration
                    status_startDBMigration();

                    //do migrate from current version to next post migration version
                    migrateServiceVersion = new ServiceVersionMigrateImpl(this);
                    migrateServiceVersion.init(migrateServiceVersion, migrateServiceVersion, null);
                    migrateServiceVersion.start();

                    //set version in db
                    loopServiceVersion = scriptGlobals.getaftermigration_serviceVersion();
                    updateDBServiceVersion(scriptGlobals.getaftermigration_serviceVersion());
                    
                    //status_doneDBMigration
                    status_doneDBMigration();
                } finally {
                    ServiceVersionMigrateImpl.removeInstance();
                }
            }
            //put back the data into database
            uploadFiles();
            
            //removeMigrationStatus
            removeMigrationStatus(dbServiceVersion, softwareServiceVersion);
            //upgradeIndexes();
            try {
                if(databaseMigrationFlag !=null){
                    databaseMigrationFlag.removeStatusFile(true);
                }
                MRLog.debug("remove databaseMigrationFlag " );
                databaseMigrationFlag=null;
            } catch (Exception ex) {
                MRLog.error("removeMigrationStatus 3 " + ex.getMessage());
            }            
            return true;
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration", ex);
            migrationError = message_mig + " Database migration failed, during migration ver " + serviceVersion + " to " + scriptGlobals.getaftermigration_serviceVersion() + " " + ex.getMessage();
            debug(migrationError);
            //IpvsServer.getInstance().stop();
            return false;
        }

    }

    private void copyFiles() throws Exception {
        long t0=System.currentTimeMillis();
        try{
            //scriptGlobals.getSettings()
            int retVal = 0;
            StringBuffer buf = new StringBuffer();
            File fromDir = new File(backFolderPath);
            MRLog.debug("DB_MIG: coping files from='" + fromDir.getAbsolutePath() + "' to='" + getXslApplyFolder()+"'");

            if (IpvsServGlobals.isWindowsOS()) {
                String[] cmd = { "xcopy", "/EQYI", fromDir.getAbsolutePath(), getXslApplyFolder()};
                if ( MRLog.isDbadminLogEnable() ) {
                    MRLog.debug("DB_MIG:  coping files " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
                retVal = CompUtils.executeProcess(cmd, buf, true);
                if (retVal != 0) {
                    MRLog.debug("DB_MIG: FAILED coping files " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
            } else {        
                File dir = new File(xslApplyFolder);
                CompUtils.deleteChildrenDirectory(dir.getParentFile());
                dir.mkdirs();
                String[] cmd = { "cp", "-rf", fromDir.getAbsolutePath(), dir.getParentFile().getAbsolutePath()};
                if (MRLog.isDbadminLogEnable()) {
                    MRLog.debug("DB_MIG:  coping files " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
                retVal = CompUtils.executeProcess(cmd, buf);
                if (retVal != 0) {
                    MRLog.debug("DB_MIG: FAILED coping files " + CompUtils.joinArray(cmd, " ") + "" + buf.toString());
                }
            }
        }finally{
            MRLog.debug("DB_MIG: copy files done "+ System.currentTimeMillis() +"("+(System.currentTimeMillis()-t0)+")"+"from='" + backFolderPath + "' to='" + getXslApplyFolder()+"'");

        }
    }
    private void uploadFiles() throws Exception {
        DBAdminParams params = new DBAdminParams();
        params.setSaveForDiff(saveForDiff);
        params.setSaveToColEnable(saveToColEnable);
        File directory = new File(xslApplyFolder);
        String xml = "";
        try {
            xml = dbadmin.getIpvsDBDriver().getDBMigrationProgressStatus();
        } catch (Exception ex) {
        }   
        
        File migration_status_file = new File(directory,IpvsServGlobals.MIGRATION_PROGRESS_STATUS);
        CompUtils.writeFile(migration_status_file.getAbsolutePath(),xml);
        
        File service_version_file = new File(directory,IpvsServGlobals.SERVICE_VERSION);
        //String xQuery = CompUtils.getContents(service_version_file);
        //MRLog.debug("uploadFiles SERVICE_VERSION "+ xQuery);
        xml = "";
        try {
            Document doc = Dom4jUtils.loadXMLResource(IpvsServer.SERVER_VERSION_XML);
            xml = doc.asXML();
            CompUtils.writeFile(service_version_file.getAbsolutePath(),xml);
        } catch (Exception ex) {
        }    
        //FASTER: remove database -- by deleting binary files 
        // dbadmin.createEmptyDatabase();
        //restoredb directory 
        restoredb(directory, params);
    }
   
}
