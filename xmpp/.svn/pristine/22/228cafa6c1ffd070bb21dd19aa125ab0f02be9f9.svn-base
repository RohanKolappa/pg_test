package com.ipvsserv.nxd.api.xmldb;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.CompiledExpression;
import org.xmldb.api.base.Resource;
import org.xmldb.api.base.ResourceIterator;
import org.xmldb.api.base.ResourceSet;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.CollectionManagementService;
import org.xmldb.api.modules.XMLResource;
import org.xmldb.api.modules.XQueryService;
import org.xmldb.api.modules.XUpdateQueryService;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.SchemaConst;
import com.ipvsserv.nxd.adminapi.SaveFileQueue;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.mdc.iq.DomainMDC;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class XmlDBDriverImpl implements IpvsDBDriver {
    protected String DB_COL_PREFIX = "";
    protected XmlDBModuleI xmlDBModule = null;

    public XmlDBDriverImpl() {
        DB_COL_PREFIX = IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX);
    }

    protected boolean isDeleteResourceSkip(String documentName) {
        return false;
    }

    protected boolean isAddResourceSkip(String documentName) {
        return false;
    }

    protected boolean isDeleteCollectionSkip(String name) {
        return false;
    }

    protected boolean isAddCollectionSkip(String name) {
        return false;
    }

    protected boolean isSystemCollection(String name) {
        if (name.equalsIgnoreCase("/db") || name.equalsIgnoreCase("/db/system") || name.startsWith("/db/system") || name.equalsIgnoreCase("/db/system/conf"))
            return true;
        return false;
    }

    protected Collection addConfigCollection(String ID, CollectionManagementService cms, Collection rootCol, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return null;
    }

    protected Collection deleteConfigCollection(String ID, CollectionManagementService cms, Collection rootCol, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return null;
    }
    
    protected String getIpvsCollectionName(String collectionName) {
        return collectionName;
    }  
    
    protected Collection getIpvsCollection( String name , Collection rootCol, String collectionName) throws Exception{
        Collection domainCol = rootCol;
        if (collectionName != null && !collectionName.trim().equals("")) {
            domainCol = rootCol.getChildCollection(collectionName);//rootCol.getChildCollection(getIpvsCollectionName(collectionName));
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
        }
        return domainCol;
    }
    ///////////////////////////////////Collection
    public boolean isCollectionExists(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isCollectionExists-ID:" + ID + " (" + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {

            if (rootCol.getChildCollection(getIpvsCollectionName(collectionName)) != null)
                return true;
            return false;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "  " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    public boolean addCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addCollection-ID:" + ID + " (" + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            cms.createCollection(collectionName);
            addConfigCollection(ID, cms, rootCol, collectionName);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean addChildCollection(String ID, String domainName, int collectionType, String collectionName, String parentcollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addCollection-ID:" + ID + " (" + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            cms.createCollection(parentcollectionName + "/" + collectionName);
            deleteConfigCollection(ID, cms, rootCol, parentcollectionName);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "  " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean deleteCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteCollection-ID:" + ID + " (" + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            cms.removeCollection(collectionName);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    ////////////////////////////////////////////////// document ////////////////////////////
    public boolean isDocumentExists(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isDocumentExists-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            if (domainCol.getResource(docId) == null)
                return false;
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    public String getDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XMLResource document = (XMLResource) domainCol.getResource(docId);
            if (document != null) {
                if (MRLog.isDbqueryresultsLogEnable()) {
                    MRLog.info("queryresult  " + name + " " + (String) document.getContent());
                }
                return (String) document.getContent();
            }
            return null;
        } catch (XMLDBException ex) {
            MRLog.error(name + "  " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "  " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    public boolean addDocument(String ID, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "addDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query " + name + " " + xml);
            }
            XMLResource document = (XMLResource) domainCol.createResource(docId, "XMLResource");
            document.setContent(xml);
            domainCol.storeResource(document);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean addDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "addDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query " + name + " " + xml);
            }
            XMLResource document = (XMLResource) domainCol.createResource(docId, "XMLResource");
            document.setContent(xml);
            domainCol.storeResource(document);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean updateDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml, String indexName, String NID) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "updateDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            try {
                XMLResource document = (XMLResource) domainCol.createResource(docId, "XMLResource");
                document.setContent(xml);
                domainCol.storeResource(document);
                return true;
            } catch (Exception ex) {
                MRLog.error(name, ex);
            }
            return false;
        } catch (Exception ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    public boolean deleteDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            Resource resource = domainCol.createResource(docId, "XMLResource");
            if (resource != null)
                domainCol.removeResource(resource);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + "   " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + "" + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    ////////////////////////////////////////////////// node ////////////////////////////
    //////////////////////// x update //////////////////////////////
    public boolean addNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName,  String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            //MRLog.info("updateNode " + xml  );
            query = XmlDBXpath.addNodeXpath(ID, domainName, nodeType, indexName, NID, xml, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            //MRLog.info("query  "+name+ " " +query );
            long result = service.update(query);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            //MRLog.info("  "+name+ " " + result);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean updateNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //todo: udpate all documents
        String name = "updateNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            //MRLog.info("updateNode " + xml  );
            query = XmlDBXpath.updateNodeXpath(ID, domainName, nodeType, indexName, NID, xml, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            long result = service.update(query);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            //MRLog.info("  "+name + result);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean updateNodeCol(String ID, String domainName, int nodeType, String indexNameDoc, String indexNameCol, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //todo: udpate all documents
        String name = "updateNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //todo:
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            //MRLog.info("updateNode " + xml  );
            query = XmlDBXpath.updateNodeXpath(ID, domainName, nodeType, indexNameCol, NID, xml, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            long result = service.update(query);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            //MRLog.info("  "+name + result);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean deleteNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //todo:
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            query = XmlDBXpath.deleteNodeXpath(ID, domainName, nodeType, indexName, NID, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            long result = service.update(query);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean deleteDocuments(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            query = XmlDBXpath.deleteNodeXpath(ID, domainName, nodeType, indexName, NID, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            long result = service.update(query);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + result);
            }
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    ////////////////////////////////////////////////// get Disco Items////////////////////////////
    public boolean isNodeExists(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isNodeExists-ID:" + ID + " (" + NID + ")";
        ArrayList<String> list = getDiscoItems(name, ID, domainName, nodeType, indexName, NID, collectionName, action, arrXpath, childContext, xQueryPrefix);
        if (list == null || list.size() == 0) {
            return false;
        }
        return true;
    }

    public ArrayList<String> getChildren(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getChildren-ID:" + ID + " (" + NID + ")";
        return getDiscoItems(name, ID, domainName, nodeType, indexName, NID, collectionName, action, arrXpath, childContext, xQueryPrefix);
    }

    private ArrayList<String> getDiscoItems(String name, String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            query = XmlDBXpath.getDiscoItemsXpath(name, domainName, nodeType, indexName, NID, collectionName, action, arrXpath, childContext);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            ArrayList<String> listNodes = new ArrayList<String>();
            CompiledExpression compiled = service.compile(query);
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                listNodes.add(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + listNodes.toString());
            }
            return listNodes;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    public String getNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //public String getNode(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, List<String> arrSort, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            //query = XmlDBXpath.getNodeXpath(ID, domainName, nodeType, indexName, NID);//(indexName, NID);
            boolean addSummary = false; //todo: test true
            //query = XmlDBXpath.selectSingleNodeXpath(indexName, NID, collectionName, arrXpath, childContext, arrSort, asc_desc, selectFilter, addSummary);
            query = XmlDBXpath.selectSingleNodeXpath(ID, domainName, nodeType, indexName, NID, collectionName, arrXpath, childContext, arrSort, asc_desc, selectFilter, addSummary);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
                break;
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            bufNodes.append(rootElementPrefix);
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            bufNodes.append(rootElementSuffix);
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    // //////////////////// getNodes
    public String getNodes(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNodes-ID:" + ID + " (" + NID + ")";
        /*String query = "";
        query = XmlDBXpathMDC.findNodesXpath(indexName, NID, collectionName, startFrom, countToFetch, arrXpath);
        StringBuffer bufNodes = new StringBuffer();
        bufNodes.append(rootElementPrefix);
        SednaApi.execute(query, bufNodes, startFrom, countToFetch, name);
        bufNodes.append(rootElementSuffix);
        return bufNodes.toString();*/
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            //query = XmlDBXpath.findNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath);//(indexName, NID);
            boolean addSummary = false; //test : true value
            query = XmlDBXpath.selectNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, arrSort, asc_desc, selectFilter, addSummary);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            bufNodes.append(rootElementPrefix);
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            bufNodes.append(rootElementSuffix);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public String getNodesCol(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNodesCol-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            //query = XmlDBXpath.findNodesXpathCol(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath);//(indexName, NID);
            boolean addSummary = true;
            query = XmlDBXpath.selectNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, arrSort, asc_desc, selectFilter, addSummary);//(indexName, NID);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            bufNodes.append(rootElementPrefix);
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            bufNodes.append(rootElementSuffix);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }

    }

    public String getDocumentList(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDocumentList-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            //query = XmlDBXpath.findNodesXpathCol(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath);//(indexName, NID);
            boolean addSummary = true;
            query = XmlDBXpath.selectNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, arrSort, asc_desc, selectFilter, addSummary);//(indexName, NID);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            bufNodes.append(rootElementPrefix);
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            bufNodes.append(rootElementSuffix);
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }

    }

    public String readQuery(String ID, String domainName, String xquery, String indexNameCol, String indexName, String collectionName, Map<String, String> xpaths, Map<String, String> nids) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {

        String name = "readQuery-ID:" + ID;
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //String collectionName = IpvsServGlobals.getDomainDBSchema().getCollectionName("data", domainName);
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            //query = XmlDBXpath.getNodeXpath(ID, domainName, nodeType, indexName, NID);//(indexName, NID);
            query = XmlDBXpath.readQuery(domainName, xquery, indexNameCol, indexName, xpaths, nids);

            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
                break;
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    //////////////////////////addIndex/////////////////////////////////////////////////////////////
    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        /*String name = "addIndex-ID:" + ID + " (" + indexName + ", " + docId + ", " + collectionName + ")";
        String query = "";
        
        if (docId != null) {
            query = XmlDBXpathMDC.createIndexQuery(indexName, docId, collectionName, path);
        } else {
            query = XmlDBXpathMDC.createIndexQuery(indexName, collectionName, path);
        }
        return SednaApi.execute(query, name);
        */
        return true;
    }

    public boolean deleteIndex(String ID, String indexName, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        /*String name = "addIndex-ID:" + ID + " (" + indexName + ", " + docId + ", " + collectionName + ")";
        String query = "";
        
        if (docId != null) {
            query = XmlDBXpathMDC.createIndexQuery(indexName, docId, collectionName, path);
        } else {
            query = XmlDBXpathMDC.createIndexQuery(indexName, collectionName, path);
        }
        return SednaApi.execute(query, name);
        */
        return true;
    }

    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path, String Xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        /*String name = "addIndex-ID:" + ID + " (" + indexName + ", " + docId + ", " + collectionName + ")";
        String query = "";
        
        if (docId != null) {
            query = XmlDBXpathMDC.createIndexQuery(indexName, docId, collectionName, path);
        } else {
            query = XmlDBXpathMDC.createIndexQuery(indexName, collectionName, path);
        }
        return SednaApi.execute(query, name);
        */
        return true;
    }

    // ///////////////////////////////////existsDocument
    public List<String> getDomainNames() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDomainNames-ID";
        List<String> domains = new ArrayList<String>();
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            String[] cols = rootCol.listChildCollections();
            for (int i = 0; i < cols.length; i++) {
                String docName = cols[i].trim();
                if (docName.startsWith(DB_COL_PREFIX)) {
                    String serviceDomain = docName.substring(docName.indexOf(DB_COL_PREFIX) + DB_COL_PREFIX.length());
                    //MRLog.info("serviceDomain " + serviceDomain +" " + docName);
                    domains.add(serviceDomain);
                }
            }
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
        if (MRLog.isDbqueryresultsLogEnable()) {
            MRLog.info("queryresult  " + name + " " + domains.toString());
        }
        return domains;

    }

    public List<String> getChildCollectionList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getChildCollectionList-ID";
        List<String> domains = new ArrayList<String>();
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            String[] cols = rootCol.listChildCollections();
            for (int i = 0; i < cols.length; i++) {
                String colName = cols[i].trim();
                if (collectionName != null && !collectionName.trim().equals("")) {
                    if (!collectionName.equals(colName))
                        continue;
                }
                if (filter == null || filter.trim().equals("")) {
                    domains.add(colName);
                } else if (filter.equalsIgnoreCase("startswith") && colName.startsWith(filterValue)) {
                    domains.add(colName);
                } else if (filter.equalsIgnoreCase("endswith") && colName.endsWith(filterValue)) {
                    domains.add(colName);
                } else if (filter.equalsIgnoreCase("indexOf") && colName.indexOf(filterValue) != -1) {
                    domains.add(colName);
                }
            }
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
        if (MRLog.isDbqueryresultsLogEnable()) {
            MRLog.info("queryresult  " + name + " " + domains.toString());
        }
        return domains;
    }

    public Map<String, List<String>> getResourceList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getResourceList-ID";
        Map<String, List<String>> domains = new LinkedHashMap<String, List<String>>();
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            String[] cols = rootCol.listChildCollections();
            for (int i = 0; i < cols.length; i++) {
                String colName = cols[i].trim();
                if (collectionName != null && !collectionName.trim().equals("")) {
                    if (!collectionName.equals(colName))
                        continue;
                }
                boolean found = false;
                if (filter == null || filter.trim().equals("")) {
                    found = true;
                } else if (filter.equalsIgnoreCase("startswith") && colName.startsWith(filterValue)) {
                    found = true;
                } else if (filter.equalsIgnoreCase("endswith") && colName.endsWith(filterValue)) {
                    found = true;
                } else if (filter.equalsIgnoreCase("indexOf") && colName.indexOf(filterValue) != -1) {
                    found = true;
                }
                if (!found)
                    continue;

            }
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
        if (MRLog.isDbqueryresultsLogEnable()) {
            MRLog.info("queryresult  " + name + " " + domains.toString());
        }
        return domains;
    }

    public boolean isCollectionExists(String ID, String collectionName, String childCollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isCollectionExists-" + ID;
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            if (collectionName == null || collectionName.trim().equals("")) {
                Collection childCol = getIpvsCollection(name, rootCol, childCollectionName);
                if (childCol != null) {
                    return true;
                }

            }
            Collection col = getIpvsCollection(name, rootCol, collectionName);
            if (col != null) {
                Collection childCol = col.getChildCollection(childCollectionName);
                if (childCol != null) {
                    return true;
                }
            }
            return false;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }

    }

    public Map<String, Integer> getChildCollectionResourceCount(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getChildCollectioResourceCount-ID";
        Map<String, Integer> domains = new LinkedHashMap<String, Integer>();
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            String[] cols = rootCol.listChildCollections();
            for (int i = 0; i < cols.length; i++) {
                String colName = cols[i].trim();
                if (collectionName != null && !collectionName.trim().equals("")) {
                    if (!collectionName.equals(colName)) {
                        MRLog.info("queryresult  " + colName + " skip ");

                        continue;
                    }
                }
                boolean found = false;
                if (filter == null || filter.trim().equals("")) {
                    found = true;
                } else if (filter.equalsIgnoreCase("startswith") && colName.startsWith(filterValue)) {
                    found = true;
                } else if (filter.equalsIgnoreCase("endswith") && colName.endsWith(filterValue)) {
                    found = true;
                } else if (filter.equalsIgnoreCase("indexOf") && colName.indexOf(filterValue) != -1) {
                    found = true;
                }
                if (!found)
                    continue;
                Collection domainCol = rootCol.getChildCollection(colName);
                if (domainCol != null) {
                    MRLog.info("queryresult  " + colName + " " + domainCol.getResourceCount());
                    domains.put(colName, domainCol.getResourceCount());
                }
                //todo:    
            }
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
        if (MRLog.isDbqueryresultsLogEnable()) {
            MRLog.info("queryresult  " + name + " ");
            MRLog.info("queryresult  " + name + " " + domains.toString());
        }
        return domains;
    }

    @Override
    public boolean execute(String ID, String domainName, String collectionName, String query, StringBuffer bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "execute " + ID;
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            Collection childCollection = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(childCollection);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public boolean execute(String ID, String domainName, String collectionName, String query, List<String> bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "execute " + ID;
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            Collection childCollection = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(childCollection);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.add(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////  shared ///////////////////////////////////////

    ///////////////////////////////////////// todo new methods //////////////////////////////
    @Override
    public void deleteDB() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "XmlDBDriverImpl deleteAll";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //delete old collection
            MRLog.debug("deleting  contents from rootcollection ");
            deleteCollection(rootCol);
        } catch (XMLDBException ex) {
            ex.printStackTrace();
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public void backupDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "XmlDBDriverImpl backup '" + folder + "'";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            File directory = new File(folder);
            //may be: make sure to delete old files.
            if (directory.exists()) {
                MRLog.debug("deleteFolder '" + directory.getAbsolutePath() + "'");
                CompUtils.deleteFolder(directory);
            }
            MRLog.debug("backing up db");
            backupToDirectory(directory, rootCol);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public void restoreDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "XmlDBDriverImpl restore '" + folder + "'";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            File directory = new File(folder);
            if (directory.exists()) {
                MRLog.debug("directory.exists restore '" + directory.getAbsolutePath() + "'");
            }
            MRLog.debug("deleting  contents from rootcollection ");
            deleteCollection(rootCol);
            MRLog.debug("restoring contents into rootcollection ");
            restoreFromDirectory(directory, rootCol);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public void importIntoDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "XmlDBDriverImpl importIntoDB '" + folder + "'";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            File directory = new File(folder);
            if (directory.exists()) {
                MRLog.debug("directory.exists restore '" + directory.getAbsolutePath() + "'");
            }
            MRLog.debug("importIntoDB contents into rootcollection ");
            restoreFromDirectory(directory, rootCol);
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    ///////////////////////////////////////// todo new methods //////////////////////////////
    ////////////////////////////////////// non xmldb functions //////////////////

    /////////////////////////////////////////////////////////////////////////////  shared back up/ restore / delete///////////////////////////////////////
    protected void backupToDirectory(File directory, Collection collection) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists() && !directory.mkdir())
                throw new DBExecutionException("Could not create directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canWrite())
                throw new DBExecutionException("Insufficient permissions to write to the directory '" + directory + "'.");
            // Get all resources, save them to the given local directory.

            String[] resourceNames = collection.listResources();
            for (int i = 0; i < resourceNames.length; i++) {
                Resource resource = collection.getResource(resourceNames[i]);
                try {
                    PrintWriter out = new PrintWriter(new FileOutputStream(directory.getPath() + "/" + resourceNames[i] + ".xml", false));
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

                backupToDirectory(childDirectory, childCollection);
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

    protected void deleteCollection(Collection collection) throws XMLDBException {
        //if system collection ... collection and child collections/resources should not be deleted 
        if (isSystemCollection(collection.getName())) {
            return;
        }
        String collectionName = collection.getName();
        // Recursively remove child resources .
        String[] resourceNames = collection.listResources();
        for (int i = 0; i < resourceNames.length; i++) {
            Resource resource = collection.getResource(resourceNames[i]);
            long t0 = System.currentTimeMillis();
            try {
                if (!isDeleteResourceSkip(resourceNames[i])) {
                    collection.removeResource(resource);
                }
            } catch (Exception e) {
                MRLog.error("Exception whilst trying to delete document '" + resourceNames[i] + "'.");
            } finally {
                MRLog.debug("xmldb deletedb delete doc-(" + resourceNames[i] + " , " + collectionName + ")" + "(" + (System.currentTimeMillis() - t0) + " milisec)");

            }
        }
        // Recursively remove child collections.
        String[] childCollectionNames = collection.listChildCollections();
        Collection originalCollection = collection;
        for (int i = 0; i < childCollectionNames.length; i++) {
            Collection childCollection = originalCollection.getChildCollection(childCollectionNames[i]);
            deleteCollection(childCollection);
        }
        if (collection.getName() == null || collection.getName().trim().equals("")) {
            //root collection.
        } else {
            //if system collection ... should not be deleted 
            Collection parentCollection = collection.getParentCollection();
            if (parentCollection != null) {
                MRLog.debug("deleting collectionName " + collectionName);

                CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(parentCollection);
                cms.removeCollection(collectionName);
            }
        }
    }

    protected void restoreFromDirectory(File directory, Collection collection) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");

            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    String docname = file.getName();
                    if (file.getName().lastIndexOf(".") != -1) {
                        docname = file.getName().substring(0, file.getName().lastIndexOf("."));
                        //MRLog.debug("restoreFromDirectory docname '" + docname + "', '" + directory.getName() + "'");
                    }
                    XMLResource resource = (XMLResource) collection.createResource(docname, XMLResource.RESOURCE_TYPE);
                    resource.setContent(CompUtils.getContents(file));
                    collection.storeResource(resource);

                }
            }

            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    //if (!isAddCollectionSkip(dir.getName())) {
                    if (collection.getChildCollection(dir.getName()) != null) {//!isSystemCollection(dir.getName())){
                        CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(collection);
                        try {
                            Collection collectionChild = cms.createCollection(dir.getName());
                            restoreFromDirectory(dir, collectionChild);
                        } catch (Exception ex) {
                            MRLog.error(ex);
                        }
                    }
                    //}
                }
            }
        } catch (XMLDBException ex) {
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            throw new DBNotConnectedException("XMLDB deregisterDatabase failed.");
        } catch (Exception ex) {
            MRLog.error("ClassNotFoundException: " + ex.getMessage());
            throw new DBExecutionException("XMLDB deregisterDatabase failed.");
        }
    }

    ///////////////////////////////////////// todo new methods //////////////////////////////

    public String getDBServiceVersion() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDBServiceVersion";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(rootCol);
            query = XmlDBXpath.getServiceVersion();
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            CompiledExpression compiled = service.compile(query);
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            //MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            //MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public String getDBMigrationProgressStatus() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDBMigrationProgressStatus";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(rootCol);
            query = XmlDBXpath.getDBMigrationProgressStatus();
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            CompiledExpression compiled = service.compile(query);
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            if (MRLog.isDbqueryresultsLogEnable()) {
                MRLog.info("queryresult  " + name + " " + bufNodes.toString());
            }
            return bufNodes.toString();
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            //MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            //MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    @Override
    public boolean xupdateQuery(String ID, String collectionName, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "xupdateQuery:" + ID + "-" + collectionName;
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);

            //service.updateResource("ul_914f8274-4451-4f17-ac66-9fb77629b6fd", query);
            long result = service.update(query);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    public boolean executeXQueryToUpdate(String ID, String collectionName, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "executeXQueryToUpdate:" + ID + "-" + collectionName;
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(domainCol);
            CompiledExpression compiled = service.compile(query);
            service.execute(compiled);
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }

        }
    }

    //////////////////////// admin migration /////////////////////
    public boolean execute(String name, String collectionName, String query, SaveFileQueue queue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {

        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            Collection childCollection = getIpvsCollection(name, rootCol, collectionName);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(childCollection);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            CompiledExpression compiled = service.compile(query);
            // execute query and get results in ResourceSet
            /*ResourceSet result = service.execute(compiled);
            for (int i = 0; i < (int) result.getSize(); i++) {
                Resource resource = result.getResource(i);
                //MRLog.info(resource.toString()+"  " +  i+"  "+name+ " " +query+ " " );
                //MRLog.info(resource.getContent()+"  " + resource.getContent().getClass().getName() +  i+"  "+name+ " " +query+ " " );
                bufNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
            }*/
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                //queue.addDocumentToQueue(resource.getId(), CompUtils.removeXmlProlog((String) resource.getContent()));
                queue.addDocumentToQueue(CompUtils.removeXmlProlog((String) resource.getContent()));
                if (MRLog.isDbqueryresultsLogEnable()) {
                    MRLog.info("queryresult  " + name + " " + (String) resource.getContent());
                }
            }

            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public boolean execute(String name, String query, List<String> listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(rootCol);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            CompiledExpression compiled = service.compile(query);

            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                //queue.addDocumentToQueue(resource.getId(), CompUtils.removeXmlProlog((String) resource.getContent()));
                listNodes.add(CompUtils.removeXmlProlog((String) resource.getContent()));
                if (MRLog.isDbqueryresultsLogEnable()) {
                    MRLog.info("queryresult  " + name + " " + (String) resource.getContent());
                }
            }

            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public boolean execute(String name, String query, StringBuffer listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            //CollectionManagementService cms = (CollectionManagementService) xmlDBModule.getServiceManager().getCollectionManagementService(rootCol);
            XQueryService service = (XQueryService) xmlDBModule.getServiceManager().getXQueryService(rootCol);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            CompiledExpression compiled = service.compile(query);

            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                //queue.addDocumentToQueue(resource.getId(), CompUtils.removeXmlProlog((String) resource.getContent()));
                listNodes.append(CompUtils.removeXmlProlog((String) resource.getContent()));
                if (MRLog.isDbqueryresultsLogEnable()) {
                    MRLog.info("queryresult  " + name + " " + (String) resource.getContent());
                }
            }

            return true;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            xmlDBModule.checkIn(rootCol, false);
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    @Override
    public void deleteRootCollectionDocs() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        // TODO Auto-generated method stub
        
    }

    ///////////////////////////////////// /////////////////////////////////////
//    private static List<String> renameEL(Document doc, String action) {
//        List<String> returnlist = new ArrayList<String>();
//        try {
//            if (doc != null) {
//                List<?> list = Dom4jUtils.getList(doc.getRootElement(), "aa");
//                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
//                    Element el = (Element) iterator.next();
//                    String value = el.getTextTrim();
//                    el.setName(value);
//                    el.setText(action);
//                    String NID = Dom4jUtils.getAttributeValue(el, "NID", "");
//                    returnlist.add(NID);
//                }
//                Element sumEL = Dom4jUtils.element(doc.getRootElement(), "GetListObjectResponseData");
//                if (sumEL != null) {
//                    sumEL.detach();
//                }
//            }
//        } catch (Exception ex) {
//            MRLog.error(ex);
//        }
//        return returnlist;
//    }
//
//    private Document getDocumentList(String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
//        Document doc = null;
//        int countToFetch = domain.getCountToFetch_ByListName(node.getCollectionId());
//        StringBuffer selectXml = new StringBuffer();
//        selectXml.append("<aa><xpath>@*</xpath><xpath>name()</xpath></aa>");
//        String xml2 = "";
//        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//            xml2 = this.getDocumentList(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, params.getCollectionName(), 1, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), selectXml.toString());
//        } else {
//            xml2 = this.getDocumentList(packetID, params.getDomainName(), node.getType(), params.getIndexNameCol(), nodeID, params.getCollectionName(), 1, countToFetch, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix(), "<DBResp>", "</DBResp>", false, params.getOrderby(), params.getAsc_desc(), selectXml.toString());
//        }
//        doc = Dom4jUtils.stringToDocument(xml2);
//        return doc;
//    }
//
//    public Document addNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, UnsupportedEncodingException {
//        String xml = params.getItemDataStr();
//        boolean childAdd = false;
//        Document docAdd = null;
//        try {
//            childAdd = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
//            if (childAdd) {
//                docAdd = this.getDocumentList(packetID, nodeID, params, domain, node, jid, enterTime);
//                nidList = renameEL(docAdd, "child added");
//                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//                    this.addNode(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                } else {
//                    this.addNode(packetID, params.getDomainName(), node.getType(), params.getIndexNameCol(), nodeID, xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                }
//            } else {
//                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//                    docAdd = Dom4jUtils.getDocument();
//                    nidList = new ArrayList<String>();
//                    Dom4jUtils.addElement(docAdd, "DBResp");
//                    if (params.getItemData() != null) {
//                        Dom4jUtils.addElement(docAdd.getRootElement(), (Element) params.getItemData().detach());
//                    }
//                    nidList.add(nodeID);
//                    this.addDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), params.getCollectionName(), xml);
//                } else {
//                    throw new DBExecutionException("document NID not provided");
//                }
//            }
//            return docAdd;
//        } finally {
//
//        }
//    }
//
//    public Document replaceNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
//        String xml = params.getItemDataStr();
//        boolean childReplace = false;
//        Document docReplace = null;
//        try {
//            childReplace = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
//            //old replace
//            if (!childReplace && node.getType() == NIDSchemeMDC.DOCUMENT && (params.getWhereXpath() == null || params.getWhereXpath().size() == 0)) {
//                docReplace = Dom4jUtils.getDocument();
//                Dom4jUtils.addElement(docReplace, "DBResp");
//                if (params.getItemData() != null) {
//                    Dom4jUtils.addElement(docReplace.getRootElement(), (Element) params.getItemData().detach());
//                }
//                nidList.add(nodeID);
//                this.updateNode(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, params.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//            } else {
//                //new replace
//                docReplace = this.getDocumentList(packetID, nodeID, params, domain, node, jid, enterTime);
//                if (docReplace != null && docReplace.getRootElement() != null) {
//                    if (childReplace) {
//                        nidList = renameEL(docReplace, "child replaced");
//                        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//                            this.updateNode(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, params.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                        } else {
//                            this.updateNode(packetID, params.getDomainName(), node.getType(), params.getIndexNameCol(), nodeID, params.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                        }
//                    } else {
//                        nidList = renameEL(docReplace, "document replaced");
//                        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//                            //one read has increased here. todo: may be get rid of this if,  its a performance hit.
//                            this.updateNode(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, params.getCollectionName(), xml, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                        } else {
//                            Document doc = Dom4jUtils.stringToDocument(xml);
//                            Element root = doc.getRootElement();
//                            for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
//                                String nid = iterator.next();
//                                NIDSchemeMDC schema = new NIDSchemeMDC();
//                                schema.parse(nid);
//                                if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
//                                    Dom4jUtils.addAttribute(root, "NID", nid.trim());
//                                    this.updateNode(packetID, params.getDomainName(), node.getType(), params.getIndexNameCol(), schema.getNID(), params.getCollectionName(), root.asXML(), params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            return docReplace;
//        } finally {
//        }
//
//    }
//
//    public Document deleteNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
//        boolean childDelete = false;
//        Document docDelete = null;
//        try {
//            childDelete = (params.getChildXpath() != null && !params.getChildXpath().trim().equals(""));
//            if (!childDelete && node.getType() == NIDSchemeMDC.DOCUMENT && params.getObjectType() != null && !params.getObjectType().trim().equals("")) {
//                String objectType = params.getObjectType().trim();
//                docDelete = Dom4jUtils.stringToDocument("<DBResp><" + objectType.trim() + " NID='" + nodeID + "' parentNID='" + node.getCollectionNodeID() + "'/>document deleted</DBResp>");
//                nidList.add(nodeID);
//                this.deleteDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, node.getDocUri(), params.getCollectionName());
//            } else {
//                docDelete = this.getDocumentList(packetID, nodeID, params, domain, node, jid, enterTime);
//                if (docDelete != null && docDelete.getRootElement() != null) {
//                    if (childDelete) {
//                        nidList = renameEL(docDelete, "child deleted");
//                        if (node.getType() == NIDSchemeMDC.DOCUMENT) {
//                            this.deleteNode(packetID, params.getDomainName(), node.getType(), params.getIndexName(), nodeID, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                        } else {
//                            this.deleteNode(packetID, params.getDomainName(), node.getType(), params.getIndexNameCol(), nodeID, params.getWhereXpath(), params.getChildXpath(), params.getXQueryPrefix());
//                        }
//                    } else {
//                        nidList = renameEL(docDelete, "document deleted");
//                        for (Iterator<String> iterator = nidList.iterator(); iterator.hasNext();) {
//                            String nid = iterator.next();
//                            NIDSchemeMDC schema = new NIDSchemeMDC();
//                            schema.parse(nid);
//                            if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
//                                this.deleteDocument(packetID, params.getDomainName(), SchemaConst.DOC_TYPE_DATA, schema.getDocUri(), params.getCollectionName());
//                            }
//                        }
//                    }
//                }
//            }
//            return docDelete;
//        } finally {
//        }
//
//    }  
}
 

