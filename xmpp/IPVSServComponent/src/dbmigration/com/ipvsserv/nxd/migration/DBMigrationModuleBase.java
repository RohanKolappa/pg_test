package com.ipvsserv.nxd.migration;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServBasicModule;

public abstract class DBMigrationModuleBase extends IpvsServBasicModule {
    //protected SednaXmlDBModule DBModule = null;
    protected DBAdminApi dbadmin = null;
    protected DBDocuments dBDocuments = null;

    protected ServiceVersionMigrateImpl migrateServiceVersion = null;
    protected ClassLoader classLoader;
    protected ScriptVariablesGlobal scriptGlobals = null;
    protected String dbMigrationfolder = "";
    protected long startDate;
    protected long curDate;
    protected long curDateMenu;
    protected IpvsFileStatus databaseMigrationStatus = null;
    protected IpvsFileStatus databaseMigrationFlag = null;
    protected IpvsFileStatus menuStatusFile = null;
    protected String softwareServiceVersion = "";
    protected String dbServiceVersion = "";
    protected String message_mig = "";
    protected String dbVendor;
    protected boolean completed = false;
    protected String backFolderPath;
    protected String serviceVersion = "";
    protected String migrationError = "Abort all request. isDBMigrationComplete=false.";
    public final static int MIGRATION_TYPE_DB_LIVE=1;
    public final static int MIGRATION_TYPE_FILE_SYSTEM=2;
    public final static int MIGRATION_TYPE_EXTERNAL=3;
    public final static int MIGRATION_TYPE_MEDIA=4;
    
    protected int migrationType=MIGRATION_TYPE_DB_LIVE;
    protected String xslApplyFolder =null;
   
    public DBMigrationModuleBase(String moduleName) {
        super(moduleName);

    }

    public DBDocuments getDBDocuments() {
        return dBDocuments;
    }

    public void setDBDocuments(DBDocuments documents) {
        dBDocuments = documents;
    }

    ////////////////
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            startDate = System.currentTimeMillis();
            curDate = System.currentTimeMillis();
            curDateMenu = System.currentTimeMillis();
            /*DBModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
            if (DBModule == null) {
                throw new Exception("Database could not connect.");
            }*/
            dbMigrationfolder = IpvsServGlobals.getProperty(getIpvsModuleName() + ".dbMigrationfolder", "/data/dbtask/upgrade");
            ////////////////////////////////////
            classLoader = Thread.currentThread().getContextClassLoader();
            initialized = true;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        dbadmin = DBAdmin.getDBAdminApi();
        
    }

    @Override
    public void stop() throws Exception {
        destroy();
    }

    
    @Override
    public void destroy() throws Exception {
        //stop if the app is started as taskName = SednaNativeDBTest /exist test / back up /bulk load only
    	super.destroy();
    	try {
            if (migrateServiceVersion != null) {
                migrateServiceVersion.destroy();
            }
            migrateServiceVersion = null;
            classLoader = null;
            scriptGlobals = null;
            databaseMigrationStatus = null;
            menuStatusFile = null;
            dbadmin = null;
            dBDocuments = null;
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    ////////////////////////
    public void debug(String msg) {
        String status = "(" + message_mig + " :V" + serviceVersion + " S:" + (System.currentTimeMillis() - curDate) + " T:" + (System.currentTimeMillis() - startDate + ")");
        StringBuffer buf = new StringBuffer();
        buf.append("Migration=\"" + status + " " + msg.replace('\"', '\'') + "\"");
        //System.out.println(status + " " + msg);
        MRLog.debug(status + " " + msg);
        curDate = System.currentTimeMillis();
        //databaseMigrationStatus.write("DB Migration: " + msg, false);
    }

    public void println(String msg) {
        System.out.print("DB_MIG:" + serviceVersion + " S:" + (System.currentTimeMillis() - curDate) + " T:" + (System.currentTimeMillis() - startDate) + " " + msg);
        curDate = System.currentTimeMillis();
    }

    public ScriptVariablesGlobal getScriptGlobals() {
        return scriptGlobals;
    }

    public ScriptVariablesGlobal getServiceVersion() {
        return scriptGlobals;
    }

    public ServiceVersionMigrateImpl getMigrateServiceVersion() {
        return migrateServiceVersion;
    }

    public void setMigrateServiceVersion(ServiceVersionMigrateImpl migrateServiceVersion) {
        this.migrateServiceVersion = migrateServiceVersion;
    }

    public ClassLoader getClassLoader() {
        return classLoader;
    }

    ///////////
    //todo: add the code to dependencies between modules .. rather using this hook (hack)
    public boolean isCompleted() {
        return completed;
    }

    public String getMigrationError() {
        return migrationError;
    }

    public long getStartDate() {
        return startDate;
    }

   /* public SednaXmlDBModule getDBModule() {
        return DBModule;
    }*/
    public DBAdminApi getDBAdminApi() {
        return dbadmin;
    }

    public void setDBAdminApi(DBAdminApi dbadmin) {
        this.dbadmin = dbadmin;
    }
    
    public int getMigrationType() {
        return migrationType;
    }

    public void setMigrationType(int migrationType) {
        this.migrationType = migrationType;
    }
    
    ///////////
    public String getDBMigrationfolder() {
        return dbMigrationfolder;
    }
    
    public String  getXslApplyFolder() {
        if(xslApplyFolder == null || xslApplyFolder.equals("")){
            File fromDir = new File(backFolderPath);
            String apply_workarea = IpvsServGlobals.getProperty("admindbapi.dbMigrationWorkArea_xslapply", "/data/dbmigration/xslapply");
            File toDir = new File(apply_workarea);
            CompUtils.deleteChildrenDirectory(toDir);
            toDir = new File(toDir, fromDir.getName());
            toDir.mkdirs();
            xslApplyFolder = toDir.getAbsolutePath();
        }
        return xslApplyFolder;
    }

    public void setXslApplyFolder(String xslApplyFolder) {
        this.xslApplyFolder = xslApplyFolder;
    }

   /* public void extractObjectxml_jar() {
        try {
            String ObjectXmlFiles = IpvsServGlobals.getProperty("shared.ObjectXmlFiles", "");
            File objectXmlFilesFile = new File(ObjectXmlFiles);
            if (objectXmlFilesFile.exists()) {
                return;
            } else {
                objectXmlFilesFile.getParentFile().mkdirs();
            }
            String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbMigrationBackupArea", "");
            File dbMigrationBackupAreaFile = new File(dbMigrationBackupArea);
            File tempfile = new File(dbMigrationBackupAreaFile.getParent(), "ObjectXmlJar");

            String jarFile = "/home/onair/modules/mediaroom/public/objectxml.jar";

            Vector<String> vec = new Vector<String>();
            vec.add("tar ");
            vec.add(" -C ");
            vec.add(tempfile.getAbsolutePath());
            vec.add(" -xf ");
            vec.add(jarFile);
            String[] cmdArgs = new String[vec.size()];
            vec.copyInto(cmdArgs);
            StringBuffer buf = new StringBuffer();
            int exitVal = CompUtils.executeProcess(cmdArgs, buf);
            if (exitVal != 0) {
                //script should not return non zero value.
                throw new Exception("ERROR "+CompUtils.joinArray(cmdArgs, " ")+" "+buf.toString());
            }
            File tempObjectXml = new File(tempfile, "com/ipvs/xsd/objectXml");
            CompUtils.copyDirectory(tempObjectXml, objectXmlFilesFile);

            //com/ipvs/xsd/objectXml
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }*/

}
