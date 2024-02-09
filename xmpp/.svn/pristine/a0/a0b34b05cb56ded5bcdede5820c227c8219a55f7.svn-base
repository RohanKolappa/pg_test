package com.ipvsserv.nxd.migration;

import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.migration.tag.MigrationTag;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.migration.task.MigrationTasksSequence;
import com.ipvsserv.server.IpvsServer;

public class ServiceVersionMigrateImpl extends MigrationTagImpl {
    private DBMigrationModuleBase migrationModule = null;
    private MigrationTasksSequence migrationTasksSequence = null;
    protected String templateDefinationXmlPath;
    protected String dbMigrationXmlPath = "DBMigration.xml";

    private DBMigrationSettings settings = null;
    private ScriptVariablesGlobal scriptGlobals=null;
    private ClassLoader classLoader=null;
    private static ServiceVersionMigrateImpl instance = null;

    public static ServiceVersionMigrateImpl getInstance() {
        return instance;
    }

    public static void removeInstance() {
        instance = null;
    }

    public ServiceVersionMigrateImpl(String moduleName) {
        super(moduleName);
        instance = this;
        root = this;
        parent = this;
    }
    public ServiceVersionMigrateImpl(DBMigrationModuleBase migrationModule) {
        this("MigrateServiceVersion");
        instance = this;
        this.migrationModule = migrationModule;
        settings = migrationModule.getScriptGlobals().getSettings();
        scriptGlobals = migrationModule.getScriptGlobals();
        classLoader = migrationModule.getClassLoader();
    }
    public ServiceVersionMigrateImpl(DBMigrationModuleBase migrationModule,ScriptVariablesGlobal scriptGlobals) {
        this("MigrateServiceVersion");
        instance = this;
        this.migrationModule = migrationModule;
        settings = scriptGlobals.getSettings();
        this.scriptGlobals = scriptGlobals;
        classLoader = IpvsServer.getInstance().getIpvsClassLoader();//migrationModule.getClassLoader();
    }

    //protected TemplateDefination templateDefination = null;
    public DBMigrationSettings getSettings() {
        return settings;
    }

    public ScriptVariablesGlobal getScriptGlobals() {
        return scriptGlobals;
    }

    public DBMigrationModuleBase getMigrationModule() {
        return migrationModule;
    }

    public void setMigrationModule(DBMigrationModuleBase migrationModule) {
        this.migrationModule = migrationModule;
    }

    @Override
    public void init(ServiceVersionMigrateImpl root, MigrationTag parent, Element element) throws Exception {

    }

    public void start() throws Exception {
        try {
            migrationTasksSequence = new MigrationTasksSequence(this, classLoader);
            migrationTasksSequence.start();
            if (!migrationTasksSequence.isShutdown()) {
                Thread.sleep(2 * 1000);
            }
            if (migrationTasksSequence.isShutdown()) {
                //migrationModule.debug("Completed migration " + scriptGlobals.getServiceVersion() + getMigrationModule().getServiceVersion().getServiceVersion() + "\n\n");
                MRLog.debug("Completed migration " + scriptGlobals.getServiceVersion() + "\n\n");
            } else {
            	MRLog.debug("ERROR completed migration " + scriptGlobals.getServiceVersion() + "\n\n");
            }
            //Thread.sleep(2 * 1000);
            //give os some time 
            Thread.sleep(500);
        } catch (Exception ex) {
            MRLog.error(" " + dbMigrationXmlPath + " app: " + IpvsServGlobals.getIpvsAppName(), ex);

        }
    }

    @Override
    public void destroy() throws Exception {

    }

    public String getTemplateDefinationXmlPath() {
        return templateDefinationXmlPath;
    }

    public String getDBMigrationXmlPath() {
        return dbMigrationXmlPath;
    }

}