package com.ipvsserv.nxd.adminapi;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.Resource;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;
import org.xmldb.api.modules.XMLResource;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;
import com.ipvsserv.server.IpvsServModule;
import com.ipvsserv.server.IpvsServer;

public class SednaXmlDBAdminApi implements DBAdminApi {
    private SednaXmlDBModule dbModule = null;
    private IpvsDBDriver db = null;
    
    public String getDBConnectionName(){
        return DBAdminApi.DB_CONNECTION_SEDNA_XMLDB;
    }     

    public SednaXmlDBAdminApi() {
        dbModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
        try {
            db = dbModule.getDefaultIpvsDBDriver();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    public boolean isDBModuleInited(){
        if(dbModule==null){
            return false;
        }
        return true;
    }
    
    public boolean isDBModuleStarted(){
        return dbModule.isStarted();
    }

    public void waitForDBModuleStart() throws InterruptedException {
        while (!dbModule.isStarted()) {
            Thread.sleep(1 * 1000);
        }
    }

    @Override
    public IpvsDBDriver getIpvsDBDriver() throws Exception {
        return db;
    }

    @Override
    public IpvsServModule getDBModule() throws Exception {
        return dbModule;
    }

    @Override
    public void upgradeIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                upgradeIndexes("upgradeIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception e) {

            e.printStackTrace();
        }
    }

    
    private boolean isFactoryResetNeeded(File directory, DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        if (directory.exists()) {
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (file.isDirectory()) {
                    MRLog.debug("isFactoryResetNeeded " + file.getAbsolutePath());
                    boolean bExists = db.isCollectionExists("isFactoryResetNeeded", null, file.getName());
                    MRLog.debug(bExists + " isFactoryResetNeeded NULL? childCollection " + file.getName());
                    return !bExists;
                }
            }
        }
        return true;
    }

    @Override
    public boolean factoryReset(File directory, DBAdminParams params, boolean force) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        boolean resetDone=false;
        if (force || isFactoryResetNeeded(directory, params)) {
            //should work for all vendors
            directoryToDB(directory, "", false, params);
            //TODO: triggers and indexes
            ensureIndexes("factoryReset", directory);
            resetDone=true;
        }
        return resetDone;
    }

    @Override
    public void backupdb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //should work for all vendors
        dbToDirectory(directory, params);
        //TODO: triggers and indexes
        ensureIndexes("factoryReset", directory);
        MRLog.debug("backup done");
    }
    
    @Override
    public void deleteDB(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        db.deleteDB();
    }
    
    @Override
    public void restoredb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, "", false, params);
        //TODO: triggers and indexes
        ensureIndexes("factoryReset", directory);
        MRLog.debug("restoredb done " + directory.getAbsolutePath());

    }

    //TODO: not used    
    @Override
    public void restoredb(File directory, String collectionName, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, collectionName, false, params);
        //TODO: triggers and indexes
        ensureIndexes("factoryReset", directory);
        MRLog.debug("restoredb done " + directory.getAbsolutePath() + " " + collectionName);
    }

    @Override
    public void importdb(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, "", true, params);
        //TODO: triggers and indexes
        ensureIndexes("factoryReset", directory);
        MRLog.debug("importdb done " + directory.getAbsolutePath());

    }

    @Override
    public void importdb(File directory, String collectionName, String domain, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, collectionName, true, params);
        //TODO: triggers and inde
        upgradeIndexes(collectionName + "_" + directory.getAbsolutePath(), collectionName);
        MRLog.debug("importdb done " + directory.getAbsolutePath() + " " + collectionName);
    }

    ///////////////
    private void ensureIndexes(String packetID, File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String collectionNamePrefix = IpvsServGlobals.getDomainDBSchema().getCollectionNamePrefix("data");
            String domainName = DBAdminUtils.getDomainName(directory, collectionNamePrefix);
            MRLog.info("create index for domainName '" + domainName + "'");
            upgradeIndexes(packetID + "_" + directory.getAbsolutePath(), domainName);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void createIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                createIndexes("createIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    @Override
    public void deleteIndexes(DBAdminParams adminParams) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = db.getDomainNames();
            for (String serviceDomain : list) {
                deleteIndexes("deleteIndexes_" + serviceDomain, serviceDomain);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void dbToDirectory(File directory, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        Collection rootCollection = dbModule.checkOut();
        try {
            //should work for all vendors
            dbToDirectory(directory, rootCollection, params);
            //TODO: triggers and indexes
        } finally {
            try {
                MRLog.debug("closing rootCollection");
                dbModule.checkIn(rootCollection, false);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void dbToDirectory(File directory, Collection collection, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists() && !directory.mkdir())
                throw new DBExecutionException("Could not create directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canWrite())
                throw new DBExecutionException("Insufficient permissions to write to the directory '" + directory + "'.");

            // Get all resources, save them to the given local directory.
            boolean saveToCol = false;
            String domainName = "";
            if (params.isSaveToColEnable()) {
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collection.getName());
                if (domainName != null) {
                    saveToCol = true;
                }
            }
            boolean isRootCollection = collection.getName().equals("");
            String[] resourceNames = collection.listResources();
            for (int i = 0; i < resourceNames.length; i++) {
                Resource resource = collection.getResource(resourceNames[i]);
                String save_path = directory.getPath() + "/" + resourceNames[i] + ".xml";
                if (isRootCollection) {
                    save_path = directory.getPath() + "/" + resourceNames[i];
                }
                if (saveToCol) {
                    String rootName = DBAdminUtils.getNodeName(resource.toString());
                    String colPath = directory.getPath() + "/" + rootName;
                    File file = new File(colPath);
                    if (!file.exists()) {
                        file.mkdirs();
                    }
                    save_path = colPath + "/" + resourceNames[i] + ".xml";
                    if (isRootCollection) {
                        save_path = colPath + "/" + resourceNames[i];
                    }
                    if (params.isSaveForDiff()) {
                        save_path = DBAdminUtils.getFilePath(isRootCollection, directory.getPath(), rootName, save_path);
                    }
                }
                try {
                    PrintWriter out = new PrintWriter(new FileOutputStream(save_path, false));
                    out.println(resource.toString());
                    out.flush();
                    out.close();
                } catch (IOException ex) {
                    MRLog.error("IOException whilst trying to write file '" + directory.getPath() + "/" + resourceNames[i] + "'.");
                }
            }
            // Recursively backup child collections to according child directories.
            String[] childCollectionNames = collection.listChildCollections();
            Collection originalCollection = collection;
            for (int i = 0; i < childCollectionNames.length; i++) {
                File childDirectory = new File(directory.getPath() + "/" + childCollectionNames[i]);
                Collection childCollection = originalCollection.getChildCollection(childCollectionNames[i]);
                dbToDirectory(childDirectory, childCollection, params);
            }
            if (collection.getName() == null || collection.getName().trim().equals("")) {
                //MRLog.debug(" 1 collection.getName() " + collection.getName());
            } else {
                // MRLog.debug(" 2 collection.getName() " + collection.getName());
            }
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            MRLog.error("XMLDBException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB deregisterDatabase failed. " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(ex);
            MRLog.error("Exception: " + ex.getMessage());
            throw new DBExecutionException("XMLDB deregisterDatabase failed. " + ex.getMessage());
        }
    }

    ////////////////////////////////// directoryToDB ///////////////////////////////////////////////////////////////////////////////////////

    private void directoryToDB(File directory, Collection collection, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

            boolean isRootCollection = collection.getName().equals("");
            boolean saveToCol = false;
            String domainName = "";
            if (params.isSaveToColEnable()) {
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collection.getName());
                if (domainName != null) {
                    saveToCol = true;
                }
            }
            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    addFileToDB(file, collection, isRootCollection, params);
                }
            }
            if (saveToCol) {
                for (int i = 0; i < dirs.length; i++) {
                    File file = dirs[i];
                    if (file.isDirectory()) {
                        File[] childirs = file.listFiles();
                        for (int k = 0; k < childirs.length; k++) {
                            File childFile = childirs[k];
                            if (!childFile.isDirectory()) {
                                addFileToDB(childFile, collection, false, params);
                            }
                        }
                    }
                }
                return;
            }
            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    String collectionName = dir.getName();
                    Collection childCollection = createChildCollection(dbModule, collection, collectionName, isImport);
                    directoryToDB(dir, childCollection, isImport, params);
                }
            }
        } catch (XMLDBException ex) {
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB deregisterDatabase failed.");
        } catch (Exception ex) {
            ex.printStackTrace();
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            throw new DBExecutionException("XMLDB deregisterDatabase failed.");
        }
    }

    private void directoryToDB(File directory, String collectionName, boolean isImport, DBAdminParams params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        Collection rootCollection = dbModule.checkOut();
        try {
            if (collectionName != null && !collectionName.trim().equals("")) {
                //get childCollection
                Collection childCollection = createChildCollection(dbModule, rootCollection, collectionName, isImport);
                directoryToDB(directory, childCollection, isImport, params);
            } else {
                if (!isImport) {
                    deleteCollection(rootCollection, false);
                }
                //should work for all vendors
                directoryToDB(directory, rootCollection, isImport, params);

            }
        } finally {
            try {
                MRLog.debug("closing rootCollection");
                dbModule.checkIn(rootCollection, false);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private void addFileToDB(File file, Collection collection, boolean isRootCollection, DBAdminParams params) {
        try {
            if (!file.isDirectory()) {
                String docname = file.getName();
                if (isRootCollection) {
                } else if (file.getName().endsWith(".xml")) {
                    if (params.isSaveForDiff()) {
                        Document doc = Dom4jUtils.getDocument(file);
                        String nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                        NIDSchemeMDC node = new NIDSchemeMDC();
                        node.parse(nodeID);
                        docname = node.getDocUri();
                    } else {
                        docname = file.getName().substring(0, file.getName().lastIndexOf("."));
                    }
                } else {
                    return;
                }
                XMLResource resource = (XMLResource) collection.createResource(docname, XMLResource.RESOURCE_TYPE);
                resource.setContent(DBAdminUtils.getContents(file));
                collection.storeResource(resource);
            }
        } catch (XMLDBException ex) {
            MRLog.error(file.getName(), ex);
        }
    }

    //////////////////////////////
    private void deleteCollection(Collection collection, boolean deleteColRecursive) throws XMLDBException {
        String collectionName = collection.getName();
        boolean isRoot = (collectionName == null || collectionName.trim().equals(""));
        //may be needed to delete partially in future deleteColRecursive
        if (!deleteColRecursive && !isRoot) {
            //MRLog.debug(" 2 collection.getName() " + collection.getName());
            Collection parentCollection = collection.getParentCollection();
            deleteCollection(dbModule, parentCollection, collection.getName());
            return;
        } else {
            // Get all resources, save them to the given local directory.
            String[] resourceNames = collection.listResources();
            for (int i = 0; i < resourceNames.length; i++) {
                Resource resource = collection.getResource(resourceNames[i]);
                try {
                    if (!isDeleteSkip(resourceNames[i])) {
                        collection.removeResource(resource);
                        MRLog.debug("deleting document '" + resourceNames[i] + "'.");
                    }
                } catch (Exception e) {
                    MRLog.error("Exception whilst trying to delete document '" + resourceNames[i] + "'.");
                }
            }

            // Recursively backup child collections to according child directories.
            String[] childCollectionNames = collection.listChildCollections();
            Collection originalCollection = collection;
            for (int i = 0; i < childCollectionNames.length; i++) {
                Collection childCollection = originalCollection.getChildCollection(childCollectionNames[i]);
                deleteCollection(childCollection, deleteColRecursive);
            }

            if (isRoot) {
                //MRLog.debug(" 1 collection.getName() " + collection.getName());
                //root collection.
            } else {
                //MRLog.debug(" 2 collection.getName() " + collection.getName());
                Collection parentCollection = collection.getParentCollection();
                deleteCollection(dbModule, parentCollection, collection.getName());
            }
        }
    }

    private Collection createChildCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName, boolean isImport) throws XMLDBException {
        //todo: some problem here... test while import
        Collection childCollection = createChildCollection(dbModule, collection, collectionName);
        if (childCollection != null) {
            if (!isImport) {
                MRLog.debug("deleting collection '" + collectionName + "'");
                deleteCollection(childCollection, false);
                MRLog.debug("creating  collection '" + collectionName + "'");
                childCollection = createChildCollection(dbModule, collection, collectionName);
            }
        } else {
            MRLog.debug("creating  collection '" + collectionName + "'");
            childCollection = createChildCollection(dbModule, collection, collectionName);
        }
        return childCollection;
    }

    private Collection createChildCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName) throws XMLDBException {
        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
        return cms.createCollection(collectionName);
    }

    private void deleteCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName) throws XMLDBException {
        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
        cms.removeCollection(collectionName);
    }

    private boolean isDeleteSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //if(name.equals("$db_security_data"))
        //    return true;
        return false;
    }

    @SuppressWarnings("unused")
    private boolean isAddSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //if(name.equals("$db_security_data"))
        //    return true;
        return false;
    }

    //    private  boolean isUpdateSkip(String documentName){
    //        //if(name.equals("$db_security_data"))
    //        //    return true;
    //        if(vendor.isAddSkip(documentName))
    //            return true;
    //        return false;
    //    }  

    /*  private void deleteFolder(File directory) {
          try {
              //restore files from current directory
              File[] dirs = directory.listFiles();
              for (int i = 0; i < dirs.length; i++) {
                  File file = dirs[i];
                  if (!file.isDirectory()) {
                      file.delete();
                  }
              }
              dirs = directory.listFiles();
              for (int i = 0; i < dirs.length; i++) {
                  File file = dirs[i];
                  if (file.isDirectory()) {
                      deleteFolder(file);
                  }
              }
              directory.delete();
          } catch (Exception ex) {
              MRLog.error(ex);
          }

      }*/

    /////////////////
    private void upgradeIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        createIndexes(ID, domainName);
        deleteIndexes(ID, domainName);
    }

    private void createIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.info("Create Indexes for domainName '" + domainName + "'");
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                try {
                    db.addIndex(ID, node.getIndexName(), null, node.getCollectionName(), "/*/@NID");
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }
                try {
                    db.addIndex(ID, node.getIndexNameCol(), null, node.getCollectionName(), "/*/@parentNID");
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);

                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    private void deleteIndexes(String ID, String domainName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            MRLog.info("Delete Indexes for domainName '" + domainName + "'");
            if (domainName != null) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.setDomain(domainName);
                try {
                    db.deleteIndex(ID, node.getIndexName(), null);
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }

                try {
                    db.deleteIndex(ID, node.getIndexNameCol(), null);
                } catch (DBExecutionException ex) {
                    MRLog.error(ID, ex);
                } catch (Exception ex) {
                    MRLog.error(ID, ex);
                }

            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    /* @SuppressWarnings("unused")
      private String getNID(String xml){
          String NID="";
          try{
              Document doc = Dom4jUtils.stringToDocument(xml);
              NID= Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
          }catch(Exception ex){
              ex.printStackTrace();
          }
          return NID;
      }*/
    
    @Override
    public boolean isDBModuleStart() throws Exception {
        return dbModule.isStarted();

    }

    @Override
    public void exportdb(File directory, DBAdminParams adminParams, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        
    }

    @Override
    public void exportdb(File directory, DBAdminParams adminParams, List<String> query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
         
        
    }

    @Override
    public boolean isConnected() {
        
        return false;
    }

    @Override
    public boolean isDatabaseServerRunning() throws DBExecutionException, InterruptedException {
         
        return false;
    }

    

    @Override
    public int restart() throws DBExecutionException, InterruptedException {
        
        return 0;
    }

    @Override
    public int start() throws DBExecutionException, InterruptedException {
        
        return 0;
    }

    @Override
    public int stop() throws DBExecutionException, InterruptedException {
        
        return 0;
    }

    @Override
    public void connected() {
        
        
    }

    @Override
    public void disconnected() {
        
        
    }

    @Override
    public boolean isAddCollectionSkip(String name) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean isAddResourceSkip(String documentName) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean isDeleteCollectionSkip(String name) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean isDeleteResourceSkip(String documentName) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public int createEmptyDatabase() throws DBExecutionException, InterruptedException {
     
        return 0;
    }
 
    @Override
    public int createFactoryDatabase() throws DBExecutionException, InterruptedException {
        
        return 0;
    }

    @Override
    public void clearManualMode() {
          
    }

    @Override
    public String getDbSscript() {
        // TODO Auto-generated method stub
        return null;
    }

    //////////////// new api ///////////////////// 

    public void deletedb_xquery(String dbquery, String domainName) throws Exception {
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(domainName);
        String collectionName = requestMDCParams.getCollectionName().trim();
        List<String> listNodes = new ArrayList<String>();
        //MRLog.debug( "dbadmin_DeleteQuery \n "+ dbquery);
        db.execute("dbadmin_DeleteQuery", dbquery, listNodes);
        boolean delete_useSednaUtils = IpvsServGlobals.getBooleanProperty("admindbapi.delete_useSednaUtils", false);

        if (!delete_useSednaUtils) {
            for (String docURI : listNodes) {
                String dropdoc = "";
                try {
                    docURI = docURI.trim();
                    dropdoc = "DROP DOCUMENT '" + docURI + "' IN COLLECTION '" + collectionName + "'  ";
                    db.executeXQueryToUpdate("dbadmin_Delete_" + docURI, collectionName, dropdoc);
                } catch (Exception ex) {
                    MRLog.error(docURI +" " + dropdoc, ex);
                }
            }
        } else {
            File xquery_file = null;
            BufferedWriter queries = null;
            StringBuffer buf = new StringBuffer();
            try {
                xquery_file = DBAdminUtils.getScript("delet_docs");
                queries = new BufferedWriter(new FileWriter(xquery_file, false));
                for (String docURI : listNodes) {
                    String dropdoc = "";
                    try {
                        docURI =docURI.trim();
                        dropdoc = "declare boundary-space preserve; DROP DOCUMENT \"" + docURI + "\" IN COLLECTION \"" + collectionName + "\" &\n";
                        //db.executeXQueryToUpdate("dbadmin_Delete_" + docURI, collectionName, dropdoc);
                        queries.write(dropdoc);
                        queries.flush();
                    } catch (Exception ex) {
                        MRLog.error(docURI +" " + dropdoc, ex);
                    }
                }
                queries.write(" for $b in document('$indexes') return $b/dummyname");
                queries.flush();
                queries.close();
                //MRLog.debug(CompUtils.getContents(xquery_file));
                DBAdminUtils.execute_xquery_file_native(xquery_file.getAbsolutePath(), buf);
                //MRLog.debug(xquery_file.getAbsolutePath() + " : " + buf.toString());

            } finally {
                if (queries != null) {
                    try {
                        queries.flush();
                        queries.close();
                    } catch (Exception ex) {
                    }
                }
                if (xquery_file != null) {
                    xquery_file.delete();
                }

            }

        }   
    }    
    
    
}
