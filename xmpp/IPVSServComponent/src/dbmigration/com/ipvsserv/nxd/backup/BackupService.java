package com.ipvsserv.nxd.backup;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
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
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class BackupService {
    private VendorBackupService vendor;
    private SednaXmlDBModule dbModule = null;
    protected boolean deleteColRecursive = false;
    protected boolean saveToColEnable = false;
    protected boolean saveForDiff = false;

    public void setSaveForDiff(boolean saveForDiff) {
        this.saveForDiff = saveForDiff;
    }

    public boolean isSaveForDiff() {
        return saveForDiff;
    }

    public static VendorBackupService getVendorBackupService(String dbVendor) {
        if (dbVendor.equalsIgnoreCase("sedna")) {
            return new SednaBackupService();
        }
        //todo:dbVendor... for exists
        return new SednaBackupService();
    }

    public BackupService(VendorBackupService vendor, SednaXmlDBModule xmlDBModule) {
        this.vendor = vendor;
        this.dbModule = xmlDBModule;
    }

    public void upgradeIndexes() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        try {
            List<String> list = dbModule.getDefaultIpvsDBDriver().getDomainNames();
            for (String serviceDomain : list) {
                vendor.upgradeIndexes("upgradeIndexes_" + serviceDomain, serviceDomain, dbModule);
            }
        } catch (Exception e) {

            e.printStackTrace();
        }
    }

    public void factoryReset(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        if (isFactoryResetNeeded(directory)) {
            //should work for all vendors
            directoryToDB(directory, "", false);
            //TODO: triggers and indexes
            vendor.factorydb(dbModule, directory);
        }
    }

    public void backupdb(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //should work for all vendors
        dbToDirectory(directory);
        //TODO: triggers and indexes
        vendor.backupdb(dbModule, directory);
        MRLog.debug("backup done");
    }

    public void restoredb(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, "", false);
        //TODO: triggers and indexes
        vendor.restoredb(dbModule, directory);
    }

    public void restoredb(File directory, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, collectionName, false);
        //TODO: triggers and indexes
        vendor.restoredb(dbModule, directory);
    }

    public void importdb(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, "", true);
        //TODO: triggers and indexes
        vendor.importdb(dbModule, directory);
    }

    public void importdb(File directory, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        //should work for all vendors
        directoryToDB(directory, collectionName, true);
        //TODO: triggers and indexes
        vendor.importdb(dbModule, directory);
    }

    ////////////////////////////////// backupToDirectory restoreFromDirectory ///////////////////////////////////////////////////////////////////////////////////////
    public boolean isFactoryResetNeeded(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        if (directory.exists()) {
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (file.isDirectory()) {
                    MRLog.debug("isFactoryResetNeeded " + file.getAbsolutePath());
                    Collection rootCollection = dbModule.checkOut();
                    try {
                        //should work for all vendors
                        Collection childCollection = rootCollection.getChildCollection(file.getName());
                        MRLog.debug("isFactoryResetNeeded NULL? childCollection " + file.getName() + " " + (childCollection == null));
                        if (childCollection != null) {
                            return false;
                        }
                        return true;
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
            }
        }
        return true;
    }

    private void dbToDirectory(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        Collection rootCollection = dbModule.checkOut();
        try {
            //should work for all vendors
            dbToDirectory(directory, rootCollection);
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

    private String getFilePath(boolean isRootCollection, String folder, String rootName, String save_path) {
        if (isRootCollection) {
            return save_path;
        }
        int counter = 1;
        String save_path2 = folder + "/" + rootName + "/" + rootName + ".xml";
        File file = new File(save_path2);
        while (file.exists()) {
            save_path2 = folder + "/" + rootName + "/" + rootName + "_" + counter + ".xml";
            file = new File(save_path2);
            counter++;
        }
        return file.getAbsolutePath();
    }

    private void dbToDirectory(File directory, Collection collection) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
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
            if (saveToColEnable) {
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
                    String rootName = getNodeName(resource.toString());
                    String colPath = directory.getPath() + "/" + rootName;
                    File file = new File(colPath);
                    if (!file.exists()) {
                        file.mkdirs();
                    }
                    save_path = colPath + "/" + resourceNames[i] + ".xml";
                    if (isRootCollection) {
                        save_path = colPath + "/" + resourceNames[i];
                    }
                    if (saveForDiff) {
                        save_path = getFilePath(isRootCollection, directory.getPath(), rootName, save_path);
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
                dbToDirectory(childDirectory, childCollection);
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

    private void directoryToDB(File directory, Collection collection, boolean isImport) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
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
            if (saveToColEnable) {
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
                    addFileToDB(file, collection, isRootCollection);
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
                                addFileToDB(childFile, collection, false);
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
                    directoryToDB(dir, childCollection, isImport);
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

    private void directoryToDB(File directory, String collectionName, boolean isImport) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, XMLDBException {
        Collection rootCollection = dbModule.checkOut();
        try {
            if (collectionName != null && !collectionName.trim().equals("")) {
                //get childCollection
                Collection childCollection = createChildCollection(dbModule, rootCollection, collectionName, isImport);
                directoryToDB(directory, childCollection, isImport);
            } else {
                if (!isImport) {
                    deleteCollection(rootCollection, false);
                }
                //should work for all vendors
                directoryToDB(directory, rootCollection, isImport);

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

    private void addFileToDB(File file, Collection collection, boolean isRootCollection) {
        try {
            if (!file.isDirectory()) {
                String docname = file.getName();
                if (isRootCollection) {
                } else if (file.getName().endsWith(".xml")) {
                    if (saveForDiff) {
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
                resource.setContent(getContents(file));
                collection.storeResource(resource);
            }
        } catch (XMLDBException ex) {
            MRLog.error(file.getName(), ex);
        }
    }

    public void deleteCollection(Collection collection, boolean deleteColRecursive) throws XMLDBException {
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

    public Collection createChildCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName, boolean isImport) throws XMLDBException {
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

    public Collection createChildCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName) throws XMLDBException {
        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
        return cms.createCollection(collectionName);
    }

    public void deleteCollection(SednaXmlDBModule xmlDBModule, Collection collection, String collectionName) throws XMLDBException {
        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
        cms.removeCollection(collectionName);
    }

    private boolean isDeleteSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //if(name.equals("$db_security_data"))
        //    return true;
        if (vendor.isDeleteSkip(documentName))
            return true;
        return false;
    }

    private boolean isAddSkip(String documentName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //if(name.equals("$db_security_data"))
        //    return true;
        if (vendor.isAddSkip(documentName))
            return true;
        return false;
    }

    //    private  boolean isUpdateSkip(String documentName){
    //        //if(name.equals("$db_security_data"))
    //        //    return true;
    //        if(vendor.isAddSkip(documentName))
    //            return true;
    //        return false;
    //    }  
    public String getContents(File aFile) {
        StringBuilder contents = new StringBuilder();
        try {
            BufferedReader input = new BufferedReader(new FileReader(aFile));
            try {
                String line = null;
                while ((line = input.readLine()) != null) {
                    contents.append(line);
                    contents.append(System.getProperty("line.separator"));
                }
            } finally {
                input.close();
            }
        } catch (IOException ex) {
            MRLog.error(ex);
        }

        return contents.toString();
    }

    public void deleteFolder(File directory) {
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

    }

    public boolean isSaveToColEnable() {
        return saveToColEnable;
    }

    public void setSaveToColEnable(boolean saveToColEnable) {
        this.saveToColEnable = saveToColEnable;
    }

    private String getNID(String xml) {
        String NID = "";
        try {
            Document doc = Dom4jUtils.stringToDocument(xml);
            NID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return NID;
    }

    private String getNodeName(String xml) {
        String NID = "";
        try {
            Document doc = Dom4jUtils.stringToDocument(xml);
            NID = doc.getRootElement().getName();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return NID;
    }
}
