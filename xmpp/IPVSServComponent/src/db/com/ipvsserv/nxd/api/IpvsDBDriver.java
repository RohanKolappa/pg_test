package com.ipvsserv.nxd.api;

import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.nxd.adminapi.SaveFileQueue;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface IpvsDBDriver {
    

    // /////////////////////////Collection ////////////////////////////////////////////////
    public abstract String getDBServiceVersion() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String getDBMigrationProgressStatus() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean isCollectionExists(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean deleteCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean addCollection(String ID, String domainName, int collectionType, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean addChildCollection(String ID, String domainName, int collectionType, String collectionName, String parentcollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // //////////////////////////////////////////////// Document ////////////////////////////////////////////////

    public abstract boolean isDocumentExists(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String getDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean addDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException;

    public abstract boolean addDocument(String ID, String docId, String collectionName, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException;

    public abstract boolean updateDocument(String ID, String domainName, int docType, String docId, String collectionName, String xml, String indexName, String NID) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, UnsupportedEncodingException, InterruptedException;

    public abstract boolean deleteDocument(String ID, String domainName, int docType, String docId, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // ////////////////////////update
    public abstract boolean addNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean updateNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean updateNodeCol(String ID, String domainName, int nodeType, String indexNameDoc, String indexNameCol, String NID, String collectionName, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean deleteNode(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean deleteDocuments(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // ////////////////////////read 
    public abstract String getNode(String ID, String domainName, int nodeType, String indexName, String NID,  String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String getNodes(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String getNodesCol(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String getDocumentList(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, String rootElementPrefix, String rootElementSuffix, boolean isColNode, List<String> arrSort, boolean asc_desc, String selectFilter) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // //////////////////////// disco

    public abstract boolean isNodeExists(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract List<String> getChildren(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // ////////////////////////addIndex
    public abstract boolean addIndex(String ID, String indexName, String docId, String collectionName, String path) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean addIndex(String ID, String indexName, String docId, String collectionName, String path, String xml) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean deleteIndex(String ID, String indexName, String collectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    // ////////////////////////// domain ////////////// ////////////////////
    public abstract List<String> getDomainNames() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    ////////////////////   admin ////////////////////////
    public abstract void deleteRootCollectionDocs() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void deleteDB() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void backupDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void restoreDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract void importIntoDB(String folder) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    ////////////////////   collection ////////////////////////
    public abstract List<String> getChildCollectionList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract Map<String, List<String>> getResourceList(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract Map<String, Integer> getChildCollectionResourceCount(String ID, String collectionName, String filter, String filterValue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean isCollectionExists(String ID, String collectionName, String childCollectionName) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    ////////////////////   execute  ////////////////////////
    public abstract boolean execute(String ID, String domainName, String collectionName, String query, StringBuffer bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean execute(String ID, String domainName, String collectionName, String query, List<String> bufNodes, int startFrom, int countToFetch) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean xupdateQuery(String ID, String collection, String xQuery) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean executeXQueryToUpdate(String ID, String collection, String xQuery) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract String readQuery(String ID, String domainName, String xquery, String indexNameCol, String indexName, String collectionName, Map<String, String> xpaths, Map<String, String> nids) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean execute(String name, String collection, String query, SaveFileQueue queue) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean execute(String ID, String query, List<String> listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    public abstract boolean execute(String ID, String query, StringBuffer listNodes) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException;

    /////////////////////// db admin migration ///////////////
    //public abstract void dbToDirectory(File directory, String collection, boolean saveToColEnable, boolean saveForDiff) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException ;

    /////////////////////// db admin migration ///////////////
    //public abstract Document addNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, UnsupportedEncodingException ;
    //public abstract Document replaceNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, UnsupportedEncodingException ;
    //public abstract Document deleteNodeList(List<String> nidList, String packetID, String nodeID, RequestMDCParams params, DomainMDC domain, NIDSchemeMDC node, String jid, long enterTime) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, UnsupportedEncodingException ;

}
