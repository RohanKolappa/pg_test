package com.ipvsserv.nxd.migration;

import java.util.Map;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.server.IpvsServer;

public class DBMigrationFileSystemMediaModule extends DBMigrationModule {
    private String dbImportFolder;
    private String serviceVersionMadia;

    public DBMigrationFileSystemMediaModule() {
        super("DBMigrationFileSystemMedia");
        setMigrationType(MIGRATION_TYPE_MEDIA);
    }

    public DBMigrationFileSystemMediaModule(String moduleName) {
        super(moduleName);
        setMigrationType(MIGRATION_TYPE_MEDIA);
    }

    protected boolean do_migrate() throws Exception {
        String message_mig = "";
        IpvsServer server = IpvsServer.getInstance() ;
        softwareServiceVersion = server.getServerVersion();
        dbServiceVersion = getServiceVersionMadia();
        scriptGlobals = new ScriptVariablesGlobal(softwareServiceVersion, dbServiceVersion, DBAdmin.getDBAdminApi().getIpvsDBDriver(), DBMigrationModuleBase.MIGRATION_TYPE_MEDIA, "", getDBImportFolder());
        try {
            String loopServiceVersion = dbServiceVersion;
            while (!scriptGlobals.getNextServiceVersionMedia(loopServiceVersion, softwareServiceVersion).equals("")) {
                try {
                    serviceVersion = scriptGlobals.getServiceVersion();
                    MRLog.debug("media migrate " + serviceVersion);
                    migrateServiceVersion = new ServiceVersionMigrateImpl(this, scriptGlobals);
                    migrateServiceVersion.init(migrateServiceVersion, migrateServiceVersion, null);
                    migrateServiceVersion.start();
                    loopServiceVersion = scriptGlobals.getaftermigration_serviceVersion();
                } finally {
                    ServiceVersionMigrateImpl.removeInstance();
                }
            }
            return true;
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration", ex);
            migrationError = message_mig + " Database migration failed, during migration ver " + serviceVersion + " to " + scriptGlobals.getaftermigration_serviceVersion() + " " + ex.getMessage();
            throw new Exception(migrationError);
        }
    }

    @Override
    public void start() throws Exception {
        do_migrate();
        started = true;
    }

    @Override
    public void stop() throws Exception {
        initialized = false;
        started = false;
    }

    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            startDate = System.currentTimeMillis();
            curDate = System.currentTimeMillis();
            curDateMenu = System.currentTimeMillis();
            classLoader = Thread.currentThread().getContextClassLoader();
            initialized = true;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        dbadmin = DBAdmin.getDBAdminApi();

    }

    public void setDBImportFolder(String dbImportFolder) {
        this.dbImportFolder = dbImportFolder;
    }

    public String getDBImportFolder() {
        return this.dbImportFolder;
    }

    public String getServiceVersionMadia() {
        return serviceVersionMadia;
    }

    public void setServiceVersionMadia(String serviceVersionMadia) {
        this.serviceVersionMadia = serviceVersionMadia;
    }
    public String  getXslApplyFolder() {
        return getDBImportFolder();
    }
}
