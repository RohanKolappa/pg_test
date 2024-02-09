package com.ipvsserv.nxd.api.xmldb.sedna;

import java.util.List;

import com.ipvsserv.nxd.api.sednanative.SednaNativeXpath;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class SednaXmlDBXpath {

    public static String addNodeXpath(String indexName, String nodeID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) {
        return SednaNativeXpath.addNodeXpath(indexName, nodeID, xml, arrXpath, childContext, xQueryPrefix);
    }

    public static String updateNodeXpath(String indexName, String nodeID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) {
        return SednaNativeXpath.updateNodeXpath(indexName, nodeID, xml, arrXpath, childContext, xQueryPrefix);
    }

    public static String deleteNodeXpath(String indexName, String nodeID, List<String> arrXpath, String childContext, String xQueryPrefix) {
        return SednaNativeXpath.deleteNodeXpath(indexName, nodeID, arrXpath, childContext, xQueryPrefix);
    }

    public static String createIndexQuery(String indexName, String docId, String collectionName, String path) {
        return SednaNativeXpath.createIndexQuery(indexName, docId, collectionName, path);
    }

    public static String createIndexQuery(String indexName, String collectionName, String path) {
        return SednaNativeXpath.createIndexQuery(indexName, collectionName, path);
    }

    public static String deleteIndexQuery(String indexName) {
        return SednaNativeXpath.deleteIndexQuery(indexName);
    }
}
