package com.ipvsserv.nxd.migration;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.server.IpvsServer;

public class ScriptVariablesGlobal extends ScriptVariables {
    private static final long serialVersionUID = 2334172283843397281L;
    private IpvsDBDriver dbAPI = null;
    public static final String SERVICE_VERSION = "serviceVersion";
    public static final String DB_MIGRATION_FOLDER = "DBMigrationFolder";
    public static final String DB_MIGRATION_WORKAREA_FOLDER = "DBMigrationWorkareaFolder";
    public static final String DB_MIGRATION_WORKAREA_TEMP_FOLDER = "DBMigrationWorkareaTempFolder";
    public static final String DBM_IGRATION_WORKAREA_BACKUP_FOLDER = "DBMigrationWorkareaBackupFolder";
    public static final String SERVICE_DOMAIN = "servicedomain";
    //public static final String IPVS_COLLECTION = "ipvs_collection";
    //public static final String IPVS_COLINDEX = "ipvs_colindex";
    //public static final String IPVS_NODEINDEX = "ipvs_nodeindex";
    public static final String SERVICE_VERSION_DB = "serviceVersion_db";
    public static final String SERVICE_VERSION_SOFTWARE = "serviceVersion_software";
    public static final String AFTERMIGRATION_SERVICEVERSION = "aftermigration_serviceVersion";

    private String configNameDBLive = "DBMigration.xml";
    private String configNameFileSystem = "DBMigrationFileSystem.xml";
    private DBMigrationSettings settings = null;
    //private boolean externalMigration = false;
    private String scriptPath = "DBMigration.xml";
    private int migrationType=DBMigrationModuleBase.MIGRATION_TYPE_DB_LIVE;
	private String media_workFolder;
    
    public static long getSerialVersionUID() {
        return serialVersionUID;
    }

    public ScriptVariablesGlobal() {

    }
 
    public ScriptVariablesGlobal(String serviceVersion_software, String serviceVersion_db, IpvsDBDriver defaultIpvsDBDriver, int migrationType) {
        this.migrationType=migrationType;
        setVariable(SERVICE_VERSION_SOFTWARE, serviceVersion_software);
        setVariable(SERVICE_VERSION_DB, serviceVersion_db);
        this.dbAPI = defaultIpvsDBDriver;
    }
    public ScriptVariablesGlobal(String serviceVersion_software, String serviceVersion_db, IpvsDBDriver defaultIpvsDBDriver, int migrationType, String scriptPath) {
        this.scriptPath = scriptPath;
        this.migrationType = migrationType;
        setVariable(SERVICE_VERSION_SOFTWARE, serviceVersion_software);
        setVariable(SERVICE_VERSION_DB, serviceVersion_db);
        this.dbAPI = defaultIpvsDBDriver;
    }
    public ScriptVariablesGlobal(String serviceVersion_software, String serviceVersion_db, IpvsDBDriver defaultIpvsDBDriver, int migrationType, String scriptPath, String media_workFolder) {
        this.scriptPath = scriptPath;
        this.migrationType = migrationType;
        setVariable(SERVICE_VERSION_SOFTWARE, serviceVersion_software);
        setVariable(SERVICE_VERSION_DB, serviceVersion_db);
        this.dbAPI = defaultIpvsDBDriver;
        this.media_workFolder = media_workFolder;
    }
    public IpvsDBDriver getDBAPI() {
        return dbAPI;
    }

    public void setServiceDomain(String serviceDomain) {
        setVariable(SERVICE_DOMAIN, serviceDomain);
    }

    public void reset(String serviceVersion) {
        super.reset();
        if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_EXTERNAL) {
             File xmlSetting = new File(scriptPath);
            if (!xmlSetting.exists()) {
                System.out.println("File not found " + scriptPath);
            }
            settings = new DBMigrationSettings(xmlSetting.getAbsolutePath());
            settings.loadSetupProperties();
            Element elVars = settings.getElement("global_system_variables");
            List<?> list = elVars.elements("variable");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element elVar = (Element) iterator.next();
                String name = Dom4jUtils.getAttributeValue(elVar, "name", "").trim();
                String value = Dom4jUtils.value(elVar, "").trim();
                setVariable(name, value);
            }
        } else {

            String dbMigrationFolder = getDBMigrationFolderRoot() + File.separator + serviceVersion + File.separator + script_folder + File.separator;
            File file = new File(dbMigrationFolder);
            if (!file.exists()) {
                file.mkdirs();
            }
            File xmlSetting = null;//
            if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_DB_LIVE) {
                xmlSetting = new File(file, configNameDBLive);
            }else  if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_FILE_SYSTEM
            		|| migrationType==DBMigrationModuleBase.MIGRATION_TYPE_MEDIA) {
                xmlSetting = new File(file, configNameFileSystem);
            }
            settings = new DBMigrationSettings(xmlSetting.getAbsolutePath());
            settings.loadSetupProperties();
            Element elVars = settings.getElement("global_system_variables");
            List<?> list = elVars.elements("variable");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element elVar = (Element) iterator.next();
                String name = Dom4jUtils.getAttributeValue(elVar, "name", "").trim();
                String value = Dom4jUtils.value(elVar, "").trim();
                setVariable(name, value);
            }
        }
    }

    public void setServiceVersion(String serviceVersion) {
        reset(serviceVersion);
        if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_EXTERNAL) {
            setVariable(SERVICE_VERSION, serviceVersion);
            File xmlSetting = new File(scriptPath);
            File file = xmlSetting.getParentFile();
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            String wokarea ="";
            if (migrationType==DBMigrationModuleBase.MIGRATION_TYPE_MEDIA) {
            	wokarea =media_workFolder;
            }else{
            	wokarea =getDBMigrationWorkAreaRoot() + File.separator + "ext_" + getServiceVersion() + File.separator + work_folder + File.separator;
            }
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_WORKAREA_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            wokarea = getDBMigrationWorkAreaRoot() + File.separator + "ext_" + getServiceVersion() + File.separator + "temp" + File.separator;
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_WORKAREA_TEMP_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            wokarea = getDBMigrationWorkAreaRoot() + File.separator + "ext_" + getServiceVersion() + File.separator + getPreMigrationDBFolder() + File.separator;
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DBM_IGRATION_WORKAREA_BACKUP_FOLDER, file.getAbsolutePath().replace('\\', '/'));

        } else {
            setVariable(SERVICE_VERSION, serviceVersion);
            //todo:

            String dbMigrationFolder = getDBMigrationFolderRoot() + File.separator + getServiceVersion() + File.separator + script_folder + File.separator;
            File file = new File(dbMigrationFolder);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            String wokarea = getDBMigrationWorkAreaRoot() + File.separator + getServiceVersion() + File.separator + work_folder + File.separator;
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_WORKAREA_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            wokarea = getDBMigrationWorkAreaRoot() + File.separator + getServiceVersion() + File.separator + "temp" + File.separator;
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DB_MIGRATION_WORKAREA_TEMP_FOLDER, file.getAbsolutePath().replace('\\', '/'));

            wokarea = getDBMigrationWorkAreaRoot() + File.separator + getServiceVersion() + File.separator + getPreMigrationDBFolder() + File.separator;
            file = new File(wokarea);
            if (!file.exists()) {
                file.mkdirs();
            }
            setVariable(DBM_IGRATION_WORKAREA_BACKUP_FOLDER, file.getAbsolutePath().replace('\\', '/'));

        }

    }

    public String getNextServiceVersion() throws MigrationException, DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String serviceVersion = getServiceVersion();
        if (serviceVersion.equals("")) {
            serviceVersion = getServiceVersion_db();
            setServiceVersion(serviceVersion);
            return getServiceVersion();
        }
        String dbServiceVersion2 = dbAPI.getDBServiceVersion();
        if (dbServiceVersion2.equalsIgnoreCase(getServiceVersion_db())) {
            //the db version is not modified after the 
            MRLog.error("The service version from db is same as old one ");
            throw new MigrationException("The service version from db is same as old one " + dbServiceVersion2);
        }
        if (dbServiceVersion2.equalsIgnoreCase(IpvsServer.getInstance().getServerVersion())) {
            return "";
        }
        setServiceVersion(dbServiceVersion2);
        return serviceVersion;
    }
    public String getNextServiceVersion(String serviceVersiondb  ) throws MigrationException, DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String serviceVersion = getServiceVersion();
        if (serviceVersion.equals("")) {
            serviceVersion = getServiceVersion_db();
            setServiceVersion(serviceVersion);
            return getServiceVersion();
        }
        String dbServiceVersion2 = serviceVersiondb;//dbAPI.getDBServiceVersion();
        if (dbServiceVersion2.equalsIgnoreCase(getServiceVersion_db())) {
            //the db version is not modified after the 
            MRLog.error("The service version from db is same as old one ");
            throw new MigrationException("The service version from db is same as old one " + dbServiceVersion2);
        }
        if (dbServiceVersion2.equalsIgnoreCase(IpvsServer.getInstance().getServerVersion())) {
            return "";
        }
        setServiceVersion(dbServiceVersion2);
        return serviceVersiondb;
    }
    public String getNextServiceVersionMedia(String serviceVersion, String serviceVersiondb   ) throws MigrationException, DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        if(serviceVersion.equals(serviceVersiondb)){
            return "";
        }
        setServiceVersion(serviceVersion);
        return serviceVersion;
    }
    public DBMigrationSettings getSettings() {
        return settings;
    }

    public void setSettings(DBMigrationSettings settings) {
        this.settings = settings;
    }

    public String modifyQuery(String query) {
        StringBuffer buf = new StringBuffer(query.length());
        StringBuffer temp = new StringBuffer();
        for (int i = 0; i < query.length(); i++) {
            char c = query.charAt(i);
            if (c == '$') {
                int ipos1 = query.indexOf('{', i);
                int ipos2 = query.indexOf('}', ipos1);
                if (ipos1 == -1 || ipos2 == -1) {
                    buf.append(c);
                    continue;
                }
                String str1 = query.substring(i + 1, ipos1);
                if (!str1.trim().equals("")) {
                    buf.append(c);
                    continue;
                }
                String varName = query.substring(ipos1 + 1, ipos2);
                if (!isVariableExists(varName)) {
                    buf.append(c);
                    continue;
                }
                String varValue = getVariable(varName, "");
                //MRLog.debug( " varName '"+ varName+"' varValue '" + varValue +"'");
                if (varValue.equals("")) {
                    MRLog.error(" NULL ? varName '" + varName + "' varValue '" + varValue + "'");
                }
                buf.append(varValue);
                i = ipos2;
            } else {
                buf.append(c);
            }
        }
        return buf.toString();
    }

    /*
        <variable name="serviceVersion" description="current migration service version"/>
        <variable name="migrationFolder" description="migration folder e.g. /etc/migration/1.0/database"/>
        <variable name="workareafolder" description="work folder e.g. /data/dbmigration/workarea/1.0/migration"/>
        <variable name="workareatempfolder" description="work temp folder e.g. /data/dbmigration/workarea/1.0/migration/temp"/>
        <variable name="workareabackupfolder" description="work backup folder e.g. /data/dbmigration/workarea/1.0/pre_migration_bck_db"/>
        <variable name="servicedomain" description="servicedomain - e.g. default"/>
        <variable name="ipvs_collection" description="for current servicedomain - collection name e.g. IPVSc_default"/>
        <variable name="ipvs_colindex" description="for current servicedomain - col index name e.g. IPVSis_colnids_default"/>
        <variable name="ipvs_nodeindex" description="for current servicedomain - node index name e.g. IPVSi_nids_default"/>
        <variable name="serviceVersion_db" description="current database service version"/>
        <variable name="serviceVersion_software" description="current software service version"/>
     */

    public String getServicedomain() {
        return getVariable(SERVICE_DOMAIN, "");
    }

    /*public String getIpvs_collection() {
        return getVariable(IPVS_COLLECTION, "");
    }

    public String getIpvs_colindex() {
        return getVariable(IPVS_COLINDEX, "");
    }

    public String getIpvs_nodeindex() {
        return getVariable(IPVS_NODEINDEX, "");
    }*/

    public String getServiceVersion_software() {
        return getVariable(SERVICE_VERSION_SOFTWARE, "");
    }

    public String getServiceVersion_db() {
        return getVariable(SERVICE_VERSION_DB, "");
    }

    public String getServiceVersion() {
        return getVariable(SERVICE_VERSION, "");
    }

    public NIDSchemeMDC getDomain() {
        NIDSchemeMDC node = new NIDSchemeMDC();
        node.setDomain(getServicedomain());
        return node;
    }

    public String getDBMigrationWorkareaFolder() {
        return getVariable(DB_MIGRATION_WORKAREA_FOLDER, "");
    }

    public String getDBMigrationWorkareaTempFolder() {
        return getVariable(DB_MIGRATION_WORKAREA_TEMP_FOLDER, "");
    }

    public String getDBMigrationWorkareaBackupFolder() {
        return getVariable(DBM_IGRATION_WORKAREA_BACKUP_FOLDER, "");
    }

    public String getDBMigrationFolder() {
        return getVariable(DB_MIGRATION_FOLDER, "");
    }

    public String getScriptPath() {
        return getDBMigrationFolder() + File.separator + configNameDBLive;
    }

    public String getaftermigration_serviceVersion() {
        return getVariable(AFTERMIGRATION_SERVICEVERSION, "");
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private static String dbMigrationFolderRoot = null;
    private static String dbMigrationWorkArea = null;
    private static String work_folder = "migration";
    private static String script_folder = "database";
    private static String preMigrationDBBackupFolder = null;
    private static String moduleName = "DBMigration";

    public static String getModuleName() {
        return moduleName;
    }

    public static void setModuleName(String moduleName) {
        ScriptVariablesGlobal.moduleName = moduleName;
    }

    public static String getDBMigrationWorkAreaRoot() {
        if (dbMigrationWorkArea == null) {
            dbMigrationWorkArea = IpvsServGlobals.getProperty(moduleName + ".dbMigrationWorkArea", "");
        }
        return dbMigrationWorkArea;
    }

    public static String getDBMigrationFolderRoot() {
        if (dbMigrationFolderRoot == null) {
            dbMigrationFolderRoot = IpvsServGlobals.getProperty(moduleName + ".dbMigrationfolder", "");
        }
        return dbMigrationFolderRoot;
    }

    public static String getPreMigrationDBFolder() {
        if (preMigrationDBBackupFolder == null) {
            preMigrationDBBackupFolder = IpvsServGlobals.getProperty(moduleName + ".preMigrationDBBackupFolder", "");
        }
        return preMigrationDBBackupFolder;
    }

    ///todo://///////////////////////////////////
    private String templateDefinationXmlPath;

    public String getTemplateDefinationXmlPath() {
        if (templateDefinationXmlPath == null) {
            templateDefinationXmlPath = IpvsServGlobals.getProperty(moduleName + ".templateDefinationXmlPath", "");
            //todo:
            templateDefinationXmlPath = IpvsServGlobals.verifyAndGetFilePath(templateDefinationXmlPath);
        }
        return templateDefinationXmlPath;
    }

    public void setTemplateDefinationXmlPath(String templateDefinationXmlPath) {
        this.templateDefinationXmlPath = templateDefinationXmlPath;
    }

    public String getDBBackupPath() {
        Date date = new Date();
        SimpleDateFormat format = new SimpleDateFormat("EEEMMMddHHmmsszzzyyyy");
        String dateStr = format.format(date);
        String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbMigrationObjectXmlJar", "");
        //String fileName="From"+dbServiceVersion +"To" +softwareServiceVersion +"Date"+ dateStr+".tgz";
        String fileName = "From" + getServiceVersion_db() + "To" + getaftermigration_serviceVersion() + "Date" + dateStr + ".tgz";
        String wokarea = dbMigrationBackupArea;
        File dir = new File(wokarea);
        if (!dir.exists()) {
            dir.mkdirs();
        }
        File dbFile = new File(dir, fileName);
        dbFile.mkdirs();
        String path = dbFile.getAbsolutePath().replace('\\', '/');
        return path;
    }
    
    public String getXslApplyPath() {
        /*String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbMigrationWorkArea_xslapply", "/data/dbmigration/xslapply");
        File fromDir = new File(backFolderPath);
        File toDir = new File(dbMigrationBackupArea);
        toDir = new File(toDir, fromDir.getName());*/
        return "";
    }
    
    public int getMigrationType(){
    	return migrationType;
    }
    
}
