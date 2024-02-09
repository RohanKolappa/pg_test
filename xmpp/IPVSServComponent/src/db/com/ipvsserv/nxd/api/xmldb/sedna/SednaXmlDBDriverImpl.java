package com.ipvsserv.nxd.api.xmldb.sedna;

import java.io.UnsupportedEncodingException;
import java.util.List;

import net.cfoster.sedna.IndexManagementService;
import net.cfoster.sedna.SednaUpdateService;

import org.xmldb.api.base.Collection;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.XMLResource;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.api.xmldb.XmlDBDriverImpl;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SednaXmlDBDriverImpl extends XmlDBDriverImpl {
    public SednaXmlDBDriverImpl() {
        super();
        //xmlDBModule = IpvsServer.getInstance().getSednaXmlDBModule();
        xmlDBModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
    }

    protected boolean isDeleteResourceSkip(String documentName) {
        if (documentName.equals("$db_security_data"))
            return true;
        if (documentName.startsWith("$"))
            return true;
        return false;
    }

    protected boolean isAddResourceSkip(String documentName) {
        return false;
    }

    protected boolean isDeleteCollectionSkip(String name) {
        if (name.indexOf("$") != -1)
            return true;
        return false;
    }

    protected boolean isAddCollectionSkip(String name) {
        return false;
    }

    protected boolean isSystemCollection(String name) {
        if (name.indexOf("$") != -1)
            return true;
        return false;
    }

    //////////////////// add document /////////////////
    public boolean addDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "addDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol.getChildCollection(collectionName);
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query " + name + " " + xml);
            }
            XMLResource document = (XMLResource) domainCol.createResource(docId, XMLResource.RESOURCE_TYPE);
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

    //////////////////////// x update //////////////////////////////
    public boolean addNode(String ID, String domainName, int nodeType,String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol.getChildCollection(collectionName);
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
            query = SednaXmlDBXpath.addNodeXpath(indexName, NID, xml, arrXpath, childContext, xQueryPrefix);
            //XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            SednaUpdateService service = (SednaUpdateService) xmlDBModule.getServiceManager().getSednaUpdateService(domainCol);
            service.update(query);
            MRLog.info("  " + name + " " + query + " ");
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
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

    public boolean updateNode(String ID, String domainName, int nodeType,  String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "updateNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol.getChildCollection(collectionName);
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
            query = SednaXmlDBXpath.updateNodeXpath(indexName, NID, xml, arrXpath, childContext, xQueryPrefix);
            //XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            SednaUpdateService service = (SednaUpdateService) xmlDBModule.getServiceManager().getSednaUpdateService(domainCol);
            service.update(query);
            MRLog.info("  " + name + " " + query + " ");
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
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

    public boolean deleteNode(String ID, String domainName, int nodeType,String indexName, String NID,  String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol.getChildCollection(collectionName);
            if (domainCol == null) {
                throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
            }
            query = SednaXmlDBXpath.deleteNodeXpath(indexName, NID, arrXpath, childContext, xQueryPrefix);
            //XUpdateQueryService service = (XUpdateQueryService) xmlDBModule.getServiceManager().getXUpdateQueryService(domainCol);
            SednaUpdateService service = (SednaUpdateService) xmlDBModule.getServiceManager().getSednaUpdateService(domainCol);
            service.update(query);
            MRLog.info("  " + name + " " + query + " ");
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
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

    ///////////////////////////////////////// todo new methods //////////////////////////////
    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addIndex-ID:" + ID + " (" + indexName + " " + path + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol;
            if (collectionName != null && !collectionName.equals("")) {
                domainCol = rootCol.getChildCollection(collectionName);
                if (domainCol == null) {
                    //throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
                } else {
                    domainCol = rootCol;
                }
            }
            query = SednaXmlDBXpath.createIndexQuery(indexName, collectionName, path);
            IndexManagementService service = (IndexManagementService) xmlDBModule.getServiceManager().getIndexManagementService(domainCol);
            //SednaUpdateService service = (SednaUpdateService) xmlDBModule.getServiceManager().getSednaUpdateService(domainCol);
            //service.update(query);
            service.createIndex(indexName, "collection('" + collectionName + "')" + path, ".", "xs:string");
            MRLog.info("  " + name + " " + query + " ");
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
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

    public boolean deleteIndex(String ID, String indexName, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteIndex-ID:" + ID + " (" + indexName + ")";
        String query = "";
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        Collection rootCol = xmlDBModule.checkOut();
        tCon = System.currentTimeMillis();
        try {
            Collection domainCol = rootCol;
            if (collectionName != null && !collectionName.equals("")) {
                domainCol = rootCol.getChildCollection(collectionName);
                if (domainCol == null) {
                    //throw new DBExecutionException("collection '" + collectionName + "' not found in db. " + name);
                } else {
                    domainCol = rootCol;
                }
            }
            query = SednaXmlDBXpath.deleteIndexQuery(indexName);
            IndexManagementService service = (IndexManagementService) xmlDBModule.getServiceManager().getIndexManagementService(domainCol);
            //SednaUpdateService service = (SednaUpdateService) xmlDBModule.getServiceManager().getSednaUpdateService(domainCol);
            //service.update(query);
            service.removeIndex(indexName);
            MRLog.info("  " + name + " " + query + " ");
            return true;
        } catch (XMLDBException ex) {
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
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

    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path, String Xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return addIndex(ID, indexName, docId, collectionName, path);
    }

}
