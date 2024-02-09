package com.ipvsserv.nxd.adminapi.mediadir;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.db.DBActionManager;
import com.ipvsserv.nxd.adminapi.db.DBQueryAction;

public abstract class MediaDirBase implements DBActionManager {
    protected String type = null;
    protected long timeout = 0;
    protected long t0 = System.currentTimeMillis();

    protected String mediaStreamDirResourceNID = null;
    protected String zipFolderPath = null;
    protected String zipFilePath = null;
    protected String scriptFilePath = null;
    protected String workarea_folder = null;
    protected String mediaDirZipType = null;
    protected String domainName = "";

    protected Element requestEL = null;
    protected Map<String, String> params = new HashMap<String, String>();
    protected int maxPriority;
    protected Element queueEL = null;

    public MediaDirBase() {

    }

    public abstract void execute() throws Exception;

    @SuppressWarnings("unchecked")
    public Object invokeActions(DBQueryAction parent, Element EL) throws Exception {
        //List<Element> list = (List<Element>) Dom4jUtils.getList(EL, "query");
        String xpath = "query[ contains(@db_server, '" + DBAdmin.getDBAdminApi().getDBConnectionName() + "') and @enable='true']";
        List<Element> list = (List<Element>) Dom4jUtils.listXpath(EL, xpath);
        for (int i = 1; i < maxPriority; i++) {
            for (Iterator<Element> iterator = list.iterator(); iterator.hasNext();) {
                Element queryEL = iterator.next();
                String type = Dom4jUtils.getAttributeValue(queryEL, "type", "");
                boolean enabled = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(queryEL, "enable", ""), false);
                int prority = CompUtils.intValue(Dom4jUtils.getAttributeValue(queryEL, "priority", ""), 1);
                if (i == prority && !type.equals("") && enabled) {
                    if (MRLog.isDbadminLogEnable()) {
                        MRLog.debug("execute DBQueryAction type=" + type + " enabled=" + enabled + " prority=" + prority);
                    }
                    DBQueryAction dbQueryHandler = DBQueryAction.getInstance(type);
                    if (parent != null) {
                        dbQueryHandler.setParent(parent);
                    }
                    dbQueryHandler.execute(this, queryEL, params);
                }
            }
        }
        return "DONE";
    }

    protected String getExportDBXMLPath(String exportDBXML) {
        File file = new File(exportDBXML);
        if (file.exists()) {
            return file.getAbsolutePath();
        }
        file = new File(IpvsServGlobals.getHomeDirectory() + "/conf/xmls/mediadir/" + exportDBXML);
        if (file.exists()) {
            return file.getAbsolutePath();
        }
        return null;
    }

    protected static String workareaFolderPath(String prefix, String mediaStreamDirResourceNID) {
        NIDSchemeMDC node = new NIDSchemeMDC();
        node.parse(mediaStreamDirResourceNID);
        String dbMigrationBackupArea = IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup");
        String wokarea = dbMigrationBackupArea;
        File dir = new File(wokarea + "/" + prefix + "/" + node.getDocUri());
        if (!dir.exists()) {
            dir.mkdirs();
        }
        String path = dir.getAbsolutePath().replace('\\', '/');
        return path;
    }

    protected Document getScriptDocument(String scriptFilePath2) throws Exception {
        scriptFilePath = getExportDBXMLPath(scriptFilePath2);
        if (scriptFilePath == null) {
            throw new Exception("Invalid exportDBXML file " + scriptFilePath2);
        }
        Document doc = null;
        try {
            doc = Dom4jUtils.loadXMLFile(scriptFilePath);
        } catch (Exception ex) {
            MRLog.error(scriptFilePath, ex);
        }
        if (doc == null) {
            throw new Exception("Invalid exportDBXML Xml " + scriptFilePath2);
        }
        return doc;
    }

    protected void ensureZipFolder(String zipFolderPath) throws Exception {
        String zipFolderPathExport = IpvsServGlobals.getProperty("admindbapi.mediadir_export", "/data/mediadir/export");
        String zipFolderPathImport = IpvsServGlobals.getProperty("admindbapi.mediadir_import", "/data/mediadir/import");
        //get zip folder/file path 
        File file = new File(zipFolderPath);
        if (!file.exists()) {
            file.mkdirs();
            file = new File(zipFolderPathExport);
            file.mkdirs();
            file = new File(zipFolderPathImport);
            file.mkdirs();
        }
        file = new File(zipFolderPath);
        if (getMediaDirZipType().equalsIgnoreCase("tar")) {
            file = new File(file, mediaStreamDirResourceNID + ".tar");
        } else if (getMediaDirZipType().equalsIgnoreCase("tgz")) {
            file = new File(file, mediaStreamDirResourceNID + ".tgz");
        } else {
            file = new File(file, mediaStreamDirResourceNID + ".tgz");
        }
        zipFilePath = file.getAbsolutePath();
    }

    ///////////// export db interface ////////////

    public String getDomainName() {
        return domainName;
    }

    public Element getRequestEL() {
        return requestEL;
    }

    public void setRequestEL(Element requestEL) {
        this.requestEL = requestEL;
    }

    public Element getQueueEL() {
        return queueEL;
    }

    public void setQueueEL(Element queueEL) {
        this.queueEL = queueEL;
    }

    public String getType() {
        return type;
    }

    public long getTimeout() {
        return timeout;
    }

    public String getZipFolderPath() {
        return zipFolderPath;
    }

    public String getScriptFilePath() {
        return scriptFilePath;
    }

    public Map<String, String> getParams() {
        return params;
    }

    public String getZipFilePath() {
        return zipFilePath;
    }

    public int getMaxPriority() {
        return maxPriority;
    }

    public String getWorkarea_folder() {
        return workarea_folder;
    }

    public String getMediaDirZipType() {
        return IpvsServGlobals.getProperty("admindbapi.mediadir_zip_type", "tar");
    }

    public String getDBIndexFilePath() {
        if (zipFilePath == null || zipFilePath.equals("")) {
            return null;
        } else {
            File file = new File(zipFilePath);
            file.getParentFile().mkdirs();
            File indexFile = new File(file.getParentFile(), mediaStreamDirResourceNID + "_index");
            return indexFile.getAbsolutePath();
        }
    }

    public File getDBImportFolder() {
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(getDomainName());
        String collectionName = requestMDCParams.getCollectionNameSCol();

        File directory = new File(getWorkarea_folder());
        File dbDir = new File(directory, collectionName);
        if (dbDir.exists()) {
            return dbDir;
        }
        File[] list = directory.listFiles();
        for (File file : list) {
            if (file.isDirectory()) {
                //MRLog.debug(collectionName+ " findCollectionFolder " + file.getAbsolutePath());
                File fileRet = findCollectionFolder(collectionName, file);
                if (fileRet != null) {
                    return fileRet;
                }
            } else {
                //MRLog.debug(collectionName+ " listFiles file " + file.getAbsolutePath());
                if (file.getName().equals(collectionName)) {
                    return file;
                }
            }
        }
        MRLog.debug("getDBImportFolder " + dbDir.getAbsolutePath());
        return null;
    }

    private File findCollectionFolder(String collectionName, File directory) {
        File[] list = directory.listFiles();
        for (File file : list) {
            //MRLog.debug(collectionName+ " findCollectionFolder listFiles file " + file.getAbsolutePath());
            if (file.getName().equals(collectionName)) {
                return file;
            }
        }
        return null;
    }
}
