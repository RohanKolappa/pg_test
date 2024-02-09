package com.ipvsserv.nxd.api.sednanative;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmldb.api.base.XMLDBException;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.EngineMDCUtils;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.SchemaConst;
import com.ipvsserv.nxd.adminapi.DBAdmin;
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
public class SednaNativeDriverImpl implements IpvsDBDriver {
    private String DB_COL_PREFIX = "";

    public SednaNativeDriverImpl() {
        DB_COL_PREFIX = IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX);
    }

    ///////////////////////////////////Collection
    public boolean isCollectionExists(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isCollectionExists-ID:" + ID + " (" + collectionName + ")";
        String query = "";
        try {
            query = SednaNativeXpath.existCollectionXpath(collectionName);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
            if (bufNodes == null || bufNodes.toString().trim().equals("")) {
                return false;
            } else {
                return true;
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            return false;
        }
    }

    public boolean addCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addCollection-ID:" + ID + " (" + collectionName + ")";
        String query = SednaNativeXpath.addCollectionXpath(collectionName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    public boolean addChildCollection(String ID, String domainName, int collectionType, String collectionName, String parentcollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addCollection-ID:" + ID + " (" + collectionName + ")";
        String query = SednaNativeXpath.addCollectionXpath(collectionName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);

    }

    public boolean deleteCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteCollection-ID:" + ID + " (" + collectionName + ")";
        String query = SednaNativeXpath.deleteCollectionXpath(collectionName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);

    }

    ////////////////////////////////////////////////// document ////////////////////////////
    public boolean isDocumentExists(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "isDocumentExists-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        String query = "";
        try {
            query = SednaNativeXpath.existDocumentXpath(docId, collectionName);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            StringBuffer bufNodes = new StringBuffer();
            SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
            if (bufNodes == null || bufNodes.toString().trim().equals("")) {
                return false;
            } else {
                return true;
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            return false;
        }
    }

    public String getDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        String query = SednaNativeXpath.getDocumentXpath(docId, collectionName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        return bufNodes.toString();

    }

    public boolean addDocument(String ID, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "addDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        if (xml.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
            InputStream in = new ByteArrayInputStream(xml.getBytes("UTF-8"));
            return SednaNativeApiUtil.loadDocument(docId, collectionName, in, name);
        } else {
            return SednaNativeApiUtil.loadDocument(docId, collectionName, xml, name);
        }
    }

    public boolean addDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "addDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        if (xml.length() > SednaNativeApiUtil.getQueryStringFormatMaxSize()) {
            InputStream in = new ByteArrayInputStream(xml.getBytes("UTF-8"));
            return SednaNativeApiUtil.loadDocument(docId, collectionName, in, name);
        } else {
            return SednaNativeApiUtil.loadDocument(docId, collectionName, xml, name);
        }
    }

    public boolean updateDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml, String indexName, String NID) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException {
        String name = "updateDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        //InputStream in = new ByteArrayInputStream(xml.getBytes("UTF-8"));
        //return SednaApi.loadDocument(docId, collectionName, in, name);
        if (NID == null || NID.equals("")) {
            deleteDocument(ID, domainName, docType, docId, collectionName);
            return addDocument(ID, domainName, docType, docId, collectionName, xml);
        } else {
            String query = SednaNativeXpath.updateNodeXpath(indexName, NID, xml, null, null, "");
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
            return SednaNativeApiUtil.execute(query, name);
        }
    }

    public boolean deleteDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteDocument-ID:" + ID + " (" + docId + ", " + collectionName + ")";
        String query = SednaNativeXpath.deleteDocumentXpath(docId, collectionName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);

    }

    ////////////////////////////////////////////////// node Disco Items ////////////////////////////
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
        String query = SednaNativeXpath.getDiscoItemsXpath(indexName, NID, collectionName, action, nodeType, arrXpath, childContext, xQueryPrefix);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query " + name + " " + query);
        }
        ArrayList<String> listNodes = new ArrayList<String>();
        SednaNativeApiUtil.execute(query, listNodes, name);
        return listNodes;
    }

    ////////////////////////////////////////////////// node update ////////////////////////////

    public boolean addNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addNode-ID:" + ID + " (" + NID + ")";
        String query = SednaNativeXpath.addNodeXpath(indexName, NID, xml, arrXpath, childContext, xQueryPrefix);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    public boolean updateNodeCol(String ID, String domainName, int nodeType, String indexNameDoc, String indexNameCol, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "updateNode-ID:" + ID + " (" + NID + ")";
        String query = "";
        boolean result = true;
        if ((childContext == null || childContext.trim().equals(""))
        // && ( arrXpath == null || arrXpath.size()==0 )  && SednaNativeXpath.getWhereXpath(arrXpath).equals("")){
        ) {
            //if more than one document is affected.
            //todo: clarify with suarabh..about his extra code. if needed move into to transaction. 
            List<String> updateList = getChildren(ID, domainName, nodeType, indexNameCol, NID, collectionName, RequestIQConst.GETCHILDREN, arrXpath, "@NID", xQueryPrefix);
            if (updateList.size() > 0) {
                MRLog.debug("WARNING : attempt to overrite replace more than one document from list " + NID);
                Document doc = Dom4jUtils.stringToDocument(xml);
                Element root = doc.getRootElement();
                for (Iterator<String> iterator = updateList.iterator(); iterator.hasNext();) {
                    String nid = iterator.next();
                    NIDSchemeMDC schema = new NIDSchemeMDC();
                    schema.parse(nid);
                    Dom4jUtils.addAttribute(root, "NID", schema.getDocNodeID());
                    query = SednaNativeXpath.updateNodeXpath(indexNameDoc, schema.getDocNodeID(), Dom4jUtils.documentToString(doc), arrXpath, childContext, xQueryPrefix);
                    boolean result2 = SednaNativeApiUtil.execute(query, name);
                    if (!result) {
                        result = result2;
                    }
                    if (MRLog.isDbqueryLogEnable()) {
                        MRLog.info("query  " + name + " " + query);
                    }
                }
            }
        } else {
            query = SednaNativeXpath.updateNodeXpath(indexNameCol, NID, xml, arrXpath, childContext, xQueryPrefix);
            result = SednaNativeApiUtil.execute(query, name);
            if (MRLog.isDbqueryLogEnable()) {
                MRLog.info("query  " + name + " " + query);
            }
        }

        return result;
    }

    public boolean updateNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "updateNode-ID:" + ID + " (" + NID + ")";
        String query = SednaNativeXpath.updateNodeXpath(indexName, NID, xml, arrXpath, childContext, xQueryPrefix);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);

    }

    public boolean deleteNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteNode-ID:" + ID + " (" + NID + ")";
        String query = SednaNativeXpath.deleteNodeXpath(indexName, NID, arrXpath, childContext, xQueryPrefix);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    public boolean deleteDocuments(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        //todo: do it in single connection as transaction 
        String name = "deleteDocuments-ID:" + ID + " (" + NID + ")";
        String query = "";
        List<String> deleteList = getChildren(ID, domainName, nodeType, indexName, NID, collectionName, RequestIQConst.GETCHILDREN, arrXpath, "@NID", xQueryPrefix);
        for (Iterator<String> iterator = deleteList.iterator(); iterator.hasNext();) {
            String nid = iterator.next();
            NIDSchemeMDC schema = new NIDSchemeMDC();
            schema.parse(nid);
            deleteDocument(ID, domainName, SchemaConst.DOC_TYPE_DATA, schema.getDocUri(), collectionName);
        }
        return true;
        /*query= SednaNativeXpath.deleteNodeXpath(indexName, NID, arrXpath, childContext);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);*/
    }

    // //////////////////// get Nodes  // ///////////////////
    public String getNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNode-ID:" + ID + " (" + NID + ")";
        //String query = SednaNativeXpath.getNodeXpath(indexName, NID);
        boolean addSummary = false; //todo: test true
        String query = SednaNativeXpath.selectSingleNodeXpath(indexName, NID, collectionName, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, addSummary);
        //System.out.println("query "+ query);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        bufNodes.append(rootElementPrefix);
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        bufNodes.append(rootElementSuffix);
        return bufNodes.toString();
    }

    public String getNodes(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNodes-ID:" + ID + " (" + NID + ")";
        //String query = SednaNativeXpath.findNodesXpath(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, arrSort, selectFilter);
        boolean addSummary = false; //todo: test true
        String query = SednaNativeXpath.selectNodesXpath(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, addSummary);
        //System.out.println("query "+ query);        
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        bufNodes.append(rootElementPrefix);
        SednaNativeApiUtil.execute(query, bufNodes, startFrom, countToFetch, name);
        bufNodes.append(rootElementSuffix);
        return bufNodes.toString();
    }

    public String getNodesCol(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getNodesCol-ID:" + ID + " (" + NID + ")";
        boolean addSummary = true; //todo: test true
        //String query = SednaNativeXpath.findNodesXpathCol(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, arrSort, selectFilter);
        String query = SednaNativeXpath.selectNodesXpath(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, addSummary);
        //System.out.println("query "+ query);        
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        bufNodes.append(rootElementPrefix);
        //do not filter records again...taken care in xquery call with startFrom=1 and countToFetch==-1
        //SDDBApi.execute(query, bufNodes, startFrom, countToFetch, name);
        SednaNativeApiUtil.execute(query, bufNodes, 1, -1, name);
        bufNodes.append(rootElementSuffix);
        return bufNodes.toString();
    }

    public String getDocumentList(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDocumentList-ID:" + ID + " (" + NID + ")";
        boolean addSummary = true; //todo: test true
        //String query = SednaNativeXpath.findNodesXpathCol(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, arrSort, selectFilter);
        String query = SednaNativeXpath.getDocumentList(indexName, NID, collectionName, startFrom, countToFetch, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, addSummary);
        //System.out.println("query "+ query);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        bufNodes.append(rootElementPrefix);
        //do not filter records again...taken care in xquery call with startFrom=1 and countToFetch==-1
        //SDDBApi.execute(query, bufNodes, startFrom, countToFetch, name);
        SednaNativeApiUtil.execute(query, bufNodes, 1, -1, name);
        bufNodes.append(rootElementSuffix);
        return bufNodes.toString();
    }

    public String readQuery(String ID, String domainName, String xquery, String indexNameCol, String indexName, String collectionName, Map<String, String> xpaths, Map<String, String> nids) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "readQuery-ID:" + ID;

        String query = SednaNativeXpath.readQuery(xquery, indexNameCol, indexName, xpaths, nids);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("readQuery  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, -1, name);
        return bufNodes.toString();
    }

    ////////////////////////// Index/////////////////////////////////////////////////////////////
    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "addIndex-ID:" + ID + " (" + indexName + ", " + docId + ", " + collectionName + ")";
        String query = "";
        query = SednaNativeXpath.createIndexQuery(indexName, collectionName, path);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    public boolean deleteIndex(String ID, String indexName, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "deleteIndex-ID:" + ID + " (" + indexName + " )";
        String query = "";
        query = SednaNativeXpath.deleteIndexQuery(indexName);
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    public boolean addIndex(String ID, String indexName, String docId, String collectionName, String path, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return addIndex(ID, indexName, docId, collectionName, path);
    }

    ////////////////////////////////////////////////
    private String readAllDocuments() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "readAllDocuments $documents";
        String query = SednaNativeXpath.readAllDocuments();
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        return bufNodes.toString();
    }

    private String readAllCollections() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "readAllCollections $collections";
        String query = SednaNativeXpath.readCollections();
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        return bufNodes.toString();
    }

    // ///////////////////////////////////existsDocument
    public List<String> getDomainNames() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            List<String> domains = new ArrayList<String>();
            String xml = readAllCollections();
            // collection name starts with DB_COL_PREFIXl
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            List<?> list = Dom4jUtils.getList(docsEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
                if (docName.startsWith(DB_COL_PREFIX)) {
                    String serviceDomain = docName.substring(docName.indexOf(DB_COL_PREFIX) + DB_COL_PREFIX.length());
                    domains.add(serviceDomain);
                }
            }
            return domains;
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("getDomainNames " + ex.getMessage());
        }
    }

    public List<String> getChildCollectionList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            List<String> domains = new ArrayList<String>();
            String xml = readAllCollections();
            // collection name starts with DB_COL_PREFIXl
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            List<?> list = Dom4jUtils.getList(docsEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String colName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
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
            return domains;
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("getDomainNames " + ex.getMessage());
        }
    }

    public Map<String, List<String>> getResourceList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            Map<String, List<String>> domains = new LinkedHashMap<String, List<String>>();
            String xml = readAllDocuments();
            // collection name starts with DB_COL_PREFIXl
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            List<?> list = Dom4jUtils.getList(docsEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String colName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
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
                List<String> params = new ArrayList<String>();
                List<?> listDocs = Dom4jUtils.getList(el, "document");
                for (Iterator<?> k = list.iterator(); k.hasNext();) {
                    Element docel = (Element) k.next();
                    String docName = Dom4jUtils.getAttributeValue(docel, "name", "").trim();
                    params.add(docName);
                }
                domains.put(colName, params);
            }
            return domains;
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("getDomainNames " + ex.getMessage());
        }
    }

    public Map<String, Integer> getChildCollectionResourceCount(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            Map<String, Integer> domains = new Hashtable<String, Integer>();
            String xml = readAllDocuments();
            // collection name starts with DB_COL_PREFIXl
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            List<?> list = Dom4jUtils.getList(docsEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String colName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
                if (collectionName != null && !collectionName.trim().equals("")) {
                    if (!collectionName.equals(colName))
                        continue;
                }
                List<?> listDocs = Dom4jUtils.getList(el, "document");
                domains.put(colName, new Integer(listDocs.size()));
            }
            return domains;
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("getChildCollectionResourceCount " + ex.getMessage());
        }
    }

    public boolean isCollectionExists(String ID, String collectionName, String childCollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String xml = readAllCollections();
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            if (collectionName == null || collectionName.trim().equals("")) {
                Element child = Dom4jUtils.elementXpath(docsEL, "collection[@name='" + childCollectionName + "'");
                if (child != null) {
                    return true;
                } else {
                    return false;
                }
            }
            List<?> list = Dom4jUtils.getList(docsEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                String colName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
                if (collectionName.equals(colName)) {
                    Element child = Dom4jUtils.element(el, childCollectionName);
                    if (child != null) {
                        return true;
                    } else {
                        return false;
                    }

                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("isCollectionExists " + ex.getMessage());
        }
        return false;
    }

    ///////////////////////////////////////// todo new methods //////////////////////////////
    @Override
    public boolean execute(String ID, String query, StringBuffer listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, listNodes, ID);
    }

    @Override
    public boolean execute(String ID, String query, List<String> listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, listNodes, ID);
    }

    @Override
    public boolean execute(String ID, String domainName, String collectionName, String query, StringBuffer bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, bufNodes, startFrom, countToFetch, ID);
    }

    @Override
    public boolean execute(String ID, String domainName, String collectionName, String query, List<String> bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query, bufNodes, startFrom, countToFetch, ID);
    }

   
    protected boolean isDeleteResourceSkip(String documentName) {
        return DBAdmin.getDBAdminApi().isDeleteResourceSkip(documentName);
    }

    protected boolean isAddResourceSkip(String documentName) {
        return DBAdmin.getDBAdminApi().isAddResourceSkip(documentName);
    }

    protected boolean isDeleteCollectionSkip(String name) {
        return DBAdmin.getDBAdminApi().isDeleteCollectionSkip(name);
    }

    protected boolean isAddCollectionSkip(String name) {
        return DBAdmin.getDBAdminApi().isAddCollectionSkip(name);
    }
    /////////////////////////////////////////////////////////////////////////////  shared back up/ restore / delete///////////////////////////////////////
    protected String getDomainFromCollection(String collectionName) {
        //todo: make it work all the time. move to IpvsServGlobal
        String serviceDomain = "";
        if (collectionName.lastIndexOf("_") != -1) {
            serviceDomain = collectionName.substring(collectionName.lastIndexOf("_") + 1);
        }
        return serviceDomain;
    }

    protected void backupToDirectory(File directory, Element colEL) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists() && !directory.mkdir())
                throw new DBExecutionException("Could not create directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canWrite())
                throw new DBExecutionException("Insufficient permissions to write to the directory '" + directory + "'.");
            // Get all resources, save them to the given local directory.

            String collectionName = Dom4jUtils.getAttributeValue(colEL, "name", "").trim();
            String serviceDomain = getDomainFromCollection(collectionName);

            List<?> listDocs = Dom4jUtils.getList(colEL, "document");
            for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                Element childColEL = (Element) i.next();
                String documentName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                try {
                    String xml = getDocument("backupToDirectory-" + collectionName, serviceDomain, 1, documentName, collectionName);
                    PrintWriter out = new PrintWriter(new FileOutputStream(directory.getPath() + "/" + documentName + ".xml", false));
                    out.println(xml);
                    out.flush();
                    out.close();
                } catch (DBNotConnectedException e) {
                    e.printStackTrace();
                } catch (DBConnectTimedoutException e) {
                    e.printStackTrace();
                } catch (DBExecutionException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            List<?> list = Dom4jUtils.getList(colEL, "collection");
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element childColEL = (Element) i.next();
                String childCollectionName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                try {
                    File childDirectory = new File(directory.getPath() + "/" + childCollectionName);
                    backupToDirectory(childDirectory, childColEL);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
            MRLog.error("Exception: " + ex.getMessage());
            throw new DBExecutionException("XMLDB deregisterDatabase failed. " + ex.getMessage());
        }
    }

    protected void deleteCollection(Element colEL) throws XMLDBException {
        String collectionName = Dom4jUtils.getAttributeValue(colEL, "name", "").trim();
        //if system collection ... collection and child collections/resources should not be deleted 
        if (isDeleteCollectionSkip(collectionName)) {
            return;
        }
        String serviceDomain = getDomainFromCollection(collectionName);
        if (collectionName.equals("")) {
            List<?> listDocs = Dom4jUtils.getList(colEL, "document");
            for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                Element childColEL = (Element) i.next();
                String documentName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                if (!isDeleteResourceSkip(documentName)) {
                    try {
                        MRLog.debug("sedna deleteCollection doc-(" + documentName + " , " + collectionName + ")" + "time(" + System.currentTimeMillis() + ")");
                        deleteDocument("deleteCollection doc-(" + documentName + " , " + collectionName + ")", serviceDomain, 1, documentName, collectionName);
                    } catch (DBNotConnectedException e) {
                        e.printStackTrace();
                    } catch (DBConnectTimedoutException e) {
                        e.printStackTrace();
                    } catch (DBExecutionException e) {
                        e.printStackTrace();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        /*List<?> listDocs = Dom4jUtils.getList(colEL, "document");
        for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
            Element childColEL = (Element) i.next();
            String documentName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
            if (!isDeleteResourceSkip(documentName)) {
                try {
                    MRLog.debug("sedna deleteCollection doc-(" + documentName + " , " + collectionName + ")" + "time(" + System.currentTimeMillis() + ")");
                    deleteDocument("deleteCollection doc-(" + documentName + " , " + collectionName + ")", serviceDomain, 1, documentName, collectionName);
                } catch (DBNotConnectedException e) {
                    e.printStackTrace();
                } catch (DBConnectTimedoutException e) {
                    e.printStackTrace();
                } catch (DBExecutionException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }*/
        List<?> list = Dom4jUtils.getList(colEL, "collection");
        for (Iterator<?> i = list.iterator(); i.hasNext();) {
            Element childColEL = (Element) i.next();
            deleteCollection(childColEL);
        }
        if (collectionName == null || collectionName.trim().equals("")) {
            //root collection.
        } else {
            if (!isDeleteCollectionSkip(collectionName)) {
                try {
                    deleteCollection("deleteCollection-col-" + collectionName, serviceDomain, 1, collectionName);
                } catch (DBNotConnectedException e) {
                    e.printStackTrace();
                } catch (DBConnectTimedoutException e) {
                    e.printStackTrace();
                } catch (DBExecutionException e) {
                    e.printStackTrace();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    protected void restoreFromDirectory(File directory, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException {
        try {
            if (!directory.exists())
                throw new DBExecutionException("Could not find directory '" + directory + "'.");
            if (!directory.isDirectory())
                throw new DBExecutionException("'" + directory + "' is not a directory.");
            if (!directory.canRead())
                throw new DBExecutionException("Insufficient permissions to read from the directory '" + directory + "'.");
            String serviceDomain = getDomainFromCollection(collectionName);
            try {
                if (collectionName == null || collectionName.trim().equals("")) {
                } else {
                    addCollection("restoreFromDirectory-col-" + collectionName, serviceDomain, 1, collectionName);
                }
            } catch (Exception ex) {
                MRLog.error("restoreFromDirectory: " + ex.getMessage());
            }

            //restore files from current directory
            File[] dirs = directory.listFiles();
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    String documentName = file.getName();
                    if (file.getName().lastIndexOf(".") != -1) {
                        documentName = file.getName().substring(0, file.getName().lastIndexOf("."));
                    }
                    if (!isAddResourceSkip(documentName)) {
                        addDocument("restoreFromDirectory-doc(" + documentName + "," + collectionName + ")", serviceDomain, 1, documentName, collectionName, CompUtils.getContents(file));
                    }
                }
            }
            //do restore recursively for child directories
            for (int i = 0; i < dirs.length; i++) {
                File dir = dirs[i];
                if (dir.isDirectory()) {
                    if (!isAddCollectionSkip(dir.getName())) {
                        restoreFromDirectory(dir, dir.getName());
                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    //////////////////////////////////////////////////////////
    @Override
    public void deleteDB() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            boolean dbOnRemoteBox=IpvsServGlobals.getBooleanProperty("admindbapi.dbserver_running_onremotebox",false);
            if(!dbOnRemoteBox){
                //FASTER: remove database -- by deleting binary files 
                DBAdmin.getDBAdminApi().createEmptyDatabase();
                return;
            }           
            String xml = readAllDocuments();
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            deleteCollection(docsEL);
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("deleteAll " + ex.getMessage());
        }
    }
    public void deleteRootCollectionDocs() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException{
        try {
            String xml = readAllDocuments();
            Element docsEL = EngineMDCUtils.stringToElement(xml);
            String collectionName = Dom4jUtils.getAttributeValue(docsEL, "name", "").trim();
            //if system collection ... collection and child collections/resources should not be deleted 
            if (isDeleteCollectionSkip(collectionName)) {
                return;
            }
            String serviceDomain = getDomainFromCollection(collectionName);
            if (collectionName.equals("")) {
                List<?> listDocs = Dom4jUtils.getList(docsEL, "document");
                for (Iterator<?> i = listDocs.iterator(); i.hasNext();) {
                    Element childColEL = (Element) i.next();
                    String documentName = Dom4jUtils.getAttributeValue(childColEL, "name", "").trim();
                    if (!isDeleteResourceSkip(documentName)) {
                        try {
                            MRLog.debug("sedna deleteCollection doc-(" + documentName + " , " + collectionName + ")" + "time(" + System.currentTimeMillis() + ")");
                            deleteDocument("deleteCollection doc-(" + documentName + " , " + collectionName + ")", serviceDomain, 1, documentName, collectionName);
                        } catch (DBNotConnectedException e) {
                            e.printStackTrace();
                        } catch (DBConnectTimedoutException e) {
                            e.printStackTrace();
                        } catch (DBExecutionException e) {
                            e.printStackTrace();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("deleteAll " + ex.getMessage());
        }
    }

    @Override
    public void backupDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            String xml = readAllDocuments();
            Element colEL = EngineMDCUtils.stringToElement(xml);
            backupToDirectory(new File(folder), colEL);
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("backup " + ex.getMessage());
        }

    }

    @Override
    public void restoreDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            deleteDB();
            restoreFromDirectory(new File(folder), "");
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("restore " + ex.getMessage());
        }

    }

    @Override
    public void importIntoDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        try {
            restoreFromDirectory(new File(folder), "");
        } catch (DBNotConnectedException ex) {
            throw ex;
        } catch (DBConnectTimedoutException ex) {
            throw ex;
        } catch (DBExecutionException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new DBExecutionException("importIntoDB " + ex.getMessage());
        }

    }

    //////////////////
    public String getDBServiceVersion() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDBServiceVersion";
        String query = SednaNativeXpath.getServiceVersion();
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        return bufNodes.toString();
    }

    public String getDBMigrationProgressStatus() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "getDBMigrationProgressStatus";
        String query = SednaNativeXpath.getDBMigrationProgressStatus();
        StringBuffer bufNodes = new StringBuffer();
        SednaNativeApiUtil.execute(query, bufNodes, 1, 1, name);
        return bufNodes.toString();
    }

    @Override
    public boolean executeXQueryToUpdate(String ID, String collection, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        String name = "executeXQueryToUpdate:" + ID;
        if (MRLog.isDbqueryLogEnable()) {
            MRLog.info("query  " + name + " " + query);
        }
        return SednaNativeApiUtil.execute(query, name);
    }

    @Override
    public boolean xupdateQuery(String ID, String collection, String query) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        throw new DBExecutionException("Not supported in native driver");
    }

    public boolean execute(String name, String collection, String query, SaveFileQueue queue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        return SednaNativeApiUtil.execute(query.toString(), queue, name);
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
