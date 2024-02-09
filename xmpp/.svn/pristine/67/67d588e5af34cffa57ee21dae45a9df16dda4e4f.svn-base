package com.ipvsserv.nxd.migration;

import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.server.IpvsServer;

public class ExternalDBMigrationModule extends DBMigrationModuleBase {

    private String scriptPath = "";
    private String scriptServiceVer = "";

    public ExternalDBMigrationModule() {
        super("ExternalDBMigration");
        setMigrationType(MIGRATION_TYPE_EXTERNAL);
    }

    public ExternalDBMigrationModule(String moduleName) {
        super(moduleName);
        setMigrationType(MIGRATION_TYPE_EXTERNAL);
    }

  
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            startDate = System.currentTimeMillis();
            curDate = System.currentTimeMillis();


            dbMigrationfolder = IpvsServGlobals.getProperty(getIpvsModuleName() + ".dbMigrationfolder", "data/dbMigrationfolder");
            ////////////////////////////////////
            classLoader = Thread.currentThread().getContextClassLoader();

            scriptPath = IpvsServGlobals.getProperty(getIpvsModuleName() + ".DBMigration_xml", "DBMigration.xml"); //getProperty("DBMigration_xml", "DBMigration.xml");
            scriptServiceVer = IpvsServGlobals.getProperty(getIpvsModuleName() + ".serviceVer", IpvsServer.getInstance().getServerVersion()); // getProperty("serviceVer", IpvsServer.getInstance().getServerVersion());
            initialized = true;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void start() throws Exception {
        try {
            dbadmin.waitForDBModuleStart();
            dBDocuments = new DBDocuments();
            //StringBuffer xml = new StringBuffer();
            //dbadmin.getIpvsDBDriver().execute("dbToDirectory", "", "", "document('$documents')", xml, 1, 1);
            //dBDocuments.init(xml.toString());
            dBDocuments.setDomainNames(dbadmin.getIpvsDBDriver().getDomainNames());
            softwareServiceVersion = IpvsServer.getInstance().getServerVersion();
            try {
                dbServiceVersion = dbadmin.getIpvsDBDriver().getDBServiceVersion();
            } catch (Exception ex) {
                dbServiceVersion = softwareServiceVersion;
            }
            ScriptVariablesGlobal.setModuleName(getIpvsModuleName());
            scriptGlobals = new ScriptVariablesGlobal(softwareServiceVersion, dbServiceVersion, dbadmin.getIpvsDBDriver(), getMigrationType(), scriptPath);
            scriptGlobals.setServiceVersion(scriptServiceVer);
            //do migrate from current version to next post migration version
            migrateServiceVersion = new ServiceVersionMigrateImpl(this);
            migrateServiceVersion.init(migrateServiceVersion, migrateServiceVersion, null);
            migrateServiceVersion.start();
        } catch (Exception ex) {
            MRLog.error("ERROR Database migration", ex);
        } finally {
            ServiceVersionMigrateImpl.removeInstance();
            debug("Database Migration Completed Successfully.");
        }
    }

    @Override
    public void destroy() throws Exception {
        try {
            if (migrateServiceVersion != null) {
                migrateServiceVersion.destroy();
            }
            migrateServiceVersion = null;
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    /////////////////////////loadModule/////////////////////////
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }

    @Override
    public void verifyState() throws Exception {

    }

}