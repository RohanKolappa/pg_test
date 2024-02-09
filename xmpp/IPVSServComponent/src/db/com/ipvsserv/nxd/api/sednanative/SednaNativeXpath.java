package com.ipvsserv.nxd.api.sednanative;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class SednaNativeXpath {
    public static final String INSERT_INTO = "into";
    public static final String INSERT_PRECEDING = "preceding";
    public static final String INSERT_FOLLOWING = "following";

    ///////////////////private ///////////
    // ////////////////////// get nodeID /////////////////
    public static String getNodeXpathFromNIDSchemeMDC(NIDSchemeMDC node, String whereXpath){
        if(node.getType()==NIDSchemeMDC.DOCUMENT){
            return "index-scan('" + node.getIndexName() + "', '" + node.getDocNodeID()+ "', 'EQ')/..[("+whereXpath+")]";
        }else{
            return "index-scan('" + node.getIndexNameCol() + "', '" + node.getCollectionNodeID() + "', 'EQ')/..[("+whereXpath+")]";
        }
    }
    
    private static String getNodeXpath(String indexName, String nodeID) {
        return "index-scan('" + indexName + "', '" + nodeID + "', 'EQ')/..";
    }

    private static String getNodeXpath(String indexName, String nodeID, List<String> arrXpath, String childContext) {
        StringBuffer buf = new StringBuffer();
        buf.append(" " + getNodeXpath(indexName, nodeID));
        buf.append(getWhereXpath(arrXpath));
        buf.append(prefixContext(childContext) + " ");
        return buf.toString();
    }

    //    public static String getColNodeXpath(String indexName, String nodeID) {
    //        return "index-scan('" + indexName + "', '" + nodeID + "', 'EQ')/..";
    //    }
    private static String prefixContext(String childContext) {
        if (childContext == null)
            return "";
        childContext = childContext.trim();
        if (childContext.length() > 0) {
            if (childContext.equals(".")) {
                return "";
            } else if (!childContext.trim().startsWith("/")) {
                return "/" + childContext.trim();
            }
        }
        return childContext;
    }

    private static String getSelectClause(String varPrefix, String selectXml, String defaultSelectClause) throws DBExecutionException {
        if (selectXml == null || selectXml.length() == 0) {
            return defaultSelectClause;
        }
        selectXml = selectXml.trim();
        int startPos = 0;
        StringBuffer buf = new StringBuffer(100);
        int pos1 = selectXml.indexOf(RequestIQConst.PREFIX_SELECTXML);
        int pos2 = 0;
        if (pos1 == -1) {
            return " return " + selectXml.replace(RequestIQConst.SELECT_XML_VAR, varPrefix);
        }
        if (pos1 > 0) {
            buf.append("\n" + selectXml.substring(startPos, pos1));
        }
        while (pos1 != -1) {
            pos2 = selectXml.indexOf(RequestIQConst.SUFFIX_SELECTXML, pos1);
            if (pos2 == -1) {
                throw new DBExecutionException("Invalid QueryRequest PREFIX_SELECTXML not found. " + selectXml);
            }
            String xpath = selectXml.substring(pos1 + RequestIQConst.PREFIX_SELECTXML_LEN, pos2);
            startPos = pos2 + RequestIQConst.SUFFIX_SELECTXML_LEN;
            if (startPos == selectXml.length() && pos1 == 0) {
                buf.append(" " + varPrefix + prefixContext(xpath) + " ");
            } else {
                buf.append(" { " + varPrefix + prefixContext(xpath) + "} ");
            }
            pos1 = selectXml.indexOf(RequestIQConst.PREFIX_SELECTXML, startPos);
        }
        if (selectXml.length() > startPos) {
            buf.append(selectXml.substring(startPos));
        }
        return " return " + buf.toString();

    }

    private static String getSortClause(String varPrefix, List<String> arrSort, boolean asc_desc, String defaultSortClause) {
        if (arrSort == null || arrSort.size() == 0) {
            return defaultSortClause;
        } else {
            StringBuffer buf = new StringBuffer();
            buf.append("  order by ");
            boolean exists = false;
            for (Iterator<String> iterator = arrSort.iterator(); iterator.hasNext();) {
                String xpath = iterator.next();
                xpath = xpath.trim();
                /*if (!xpath.equals("")) {
                    if (!exists) {
                        buf.append(" " + varPrefix + prefixContext(xpath.trim()));
                    } else {
                        buf.append(" , " + varPrefix + prefixContext(xpath.trim()));
                    }
                    exists = true;
                }*/
                if (!xpath.equals("")) {
                    int pos1 = xpath.indexOf(RequestIQConst.SELECT_XML_VAR);
                    if (pos1 != -1) {
                        xpath = xpath.replace(RequestIQConst.SELECT_XML_VAR, varPrefix);
                    } else {
                        xpath = varPrefix + prefixContext(xpath.trim());
                    }
                    if (exists) {
                        buf.append(" , ");
                    } else {
                        exists = true;
                    }
                    buf.append(" " + xpath);
                }
            }

            if (!exists) {
                return defaultSortClause;
            }

            if (asc_desc) {
                buf.append(" ascending");
            } else {
                buf.append(" descending");
            }
            return buf.toString();
        }
    }

    public static String getWhereXpath(List<String> arrXpath) {
        if (arrXpath == null || arrXpath.isEmpty()) {
            return "";
        }
        StringBuffer buf = new StringBuffer();
        boolean bAddAND = false;
        boolean bAdded = false;
        for (Iterator<?> i = arrXpath.iterator(); i.hasNext();) {
            String xpath = (String) i.next();
            if (xpath != null && xpath.trim().length() > 0) {
                if (bAddAND) {
                    buf.append(" and ");
                } else {
                    bAdded = true;
                    buf.append("[");
                    bAddAND = true;
                }
                //buf.append("(" + prefixContext(xpath.trim()) + ")");
                buf.append("(" + xpath.trim() + ")");
            }
        }
        if (bAdded) {
            buf.append("]");
        }

        return buf.toString();
    }

    ///////////////////private ///////////

    // ///////////////// Disco Items ////////////////
    public static String existNodeXpath(String indexName, String nodeID, String collectionName, String action, int nodeType, List<String> arrXpath, String childContext, String xQueryPrefix) {
        //return getNodeXpath(indexName, nodeID) + "/name()";
        /*
         	for $b in index-scan('IPVSis_colnids_default', 'default.grouptagdeflist', 'EQ')/..[(//GroupTagDef[TagName='superuser-defaultABC1'])]/@NID 
        return  string($b) 
         */
        StringBuffer buf = new StringBuffer();
        buf.append("for $b in ");
        buf.append(getNodeXpath(indexName, nodeID, arrXpath, childContext));
        buf.append("return $b/name() ");
        /*String xpath = buf.toString();//getNodeXpath(indexName, nodeID);
          if (action.equals(RequestIQConst.GETCHILDREN)) {
              return xpath + "/ * /@NID";
          }
          return xpath + "/@NID";
          */
        return buf.toString().replace('\n', ' ');
    }

    public static String getDiscoItemsXpath(String indexName, String nodeID, String collectionName, String action, int nodeType, List<String> arrXpath, String childContext, String xQueryPrefix) {
        /*
            for $b in index-scan('IPVSis_colnids_default', 'default.grouptagdeflist', 'EQ')/..[(//GroupTagDef[TagName='superuser-defaultABC1'])]/@NID 
        return  string($b) 
         */
        StringBuffer buf = new StringBuffer();
        buf.append("for $b in ");
        buf.append(getNodeXpath(indexName, nodeID, arrXpath, childContext));
        buf.append("return string($b) ");
        /*String xpath = buf.toString();//getNodeXpath(indexName, nodeID);
          if (action.equals(RequestIQConst.GETCHILDREN)) {
              return xpath + "/ * /@NID";
          }
          return xpath + "/@NID";
          */
        return buf.toString().replace('\n', ' ');
    }

    // ///////////////// update ////////////////
    public static String addNodeXpath(String indexName, String nodeID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) {
        // UPDATE insert Expr1 (into|preceding|following) Expr2
        // UPDATE insert <warning>High Blood Pressure!</warning>
        // preceeding doc("hospital")//blood_pressure[systolic>180]
        // String parentXpath = "//*[" + nodeID + "]";
        StringBuffer buf = new StringBuffer();
        buf.append(xQueryPrefix);
        buf.append(" UPDATE insert ");
        buf.append(" " + xml + " ");
        buf.append(" " + INSERT_INTO + " ");
        buf.append(getNodeXpath(indexName, nodeID, arrXpath, childContext));
        return buf.toString().replace('\n', ' ');//.replace('\n', ' ');
    }

    public static String updateNodeXpath(String indexName, String nodeID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) {
        // UPDATE replace $var [as type] in Expr1 with Expr2($var)
        // String childXpath = "//*[" + nodeID + "]";
        StringBuffer buf = new StringBuffer();
        buf.append(xQueryPrefix);
        buf.append(" UPDATE replace ");
        buf.append(" $d in  ");
        buf.append(getNodeXpath(indexName, nodeID, arrXpath, childContext));
        buf.append(" with ");
        buf.append(xml);
        return buf.toString();//.replace('\n', ' ');
    }

    public static String deleteNodeXpath(String indexName, String nodeID, List<String> arrXpath, String childContext, String xQueryPrefix) {
        // UPDATE delete document("regionWithCol4" , "regionCollection"
        // )//africa2
        StringBuffer buf = new StringBuffer();
        buf.append(xQueryPrefix);
        buf.append(" UPDATE delete ");
        buf.append(getNodeXpath(indexName, nodeID, arrXpath, childContext));
        return buf.toString().replace('\n', ' ');
    }

    //////////////////////// /////////////////////////////////
    public static String selectSingleNodeXpath(String indexName, String nodeID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        return selectNodesXpath(indexName, nodeID, collectionName, 0, 1, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, false);
    }

    public static String getServiceVersion() throws DBExecutionException {
        return "for $b in document('"+IpvsServGlobals.SERVICE_VERSION+"')/Service return data($b/@Ver)";
    }

    public static String getDBMigrationProgressStatus() throws DBExecutionException {
        return "for $b in document('"+IpvsServGlobals.MIGRATION_PROGRESS_STATUS+"')/* return $b";
    }

    public static String selectNodesXpath(String indexName, String nodeID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        StringBuffer buf = new StringBuffer();
        if (countToFetch == -1) {
            countToFetch = 1;
        }
        if (startFrom <= 0) {
            startFrom = 1;
        }
        String applychildContext = prefixContext(childContext);
        String sortDefault = "";
        if (applychildContext.equals("")) { //if doc root use
            sortDefault = "  order by $b/@NID ";
        } else {
            sortDefault = "  order by root($b)/@NID ";
        }
        String findXpath = getWhereXpath(arrXpath);
        //if the summary not needed 
        if (!addSummary && startFrom == 0 && countToFetch == 1) {
            buf.append(" for $b in   " + getNodeXpath(indexName, nodeID, null, childContext));
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
        } else {
            buf.append(" let $list := for $b in   " + getNodeXpath(indexName, nodeID, null, childContext));
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
            if (addSummary) {
                buf.append(" return insert-before(subsequence($list, " + startFrom + ", " + countToFetch + "), 1, ");
                //buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectStartFrom>"+startFrom+"</ObjectStartFrom><ObjectLastWhereEndCount>"+end+"</ObjectLastWhereEndCount></GetListObjectResponseData>) ");
                buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectLastWhereEndCount>" + (startFrom - 1) + "</ObjectLastWhereEndCount><CountToFetch>" + countToFetch + "</CountToFetch></GetListObjectResponseData>) ");
            } else {
                buf.append(" return subsequence($list, " + startFrom + ", " + countToFetch + ")");

            }
        }
        return buf.toString().replace('\n', ' ');
    }

    public static String getDocumentList(String indexName, String nodeID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        StringBuffer buf = new StringBuffer();
        if (countToFetch == -1) {
            countToFetch = 1;
        }
        if (startFrom <= 0) {
            startFrom = 1;
        }
        String findXpath = "";
        if (childContext != null && childContext.trim().length() > 0) {
            List<String> arrXpath2 = new ArrayList<String>();
            arrXpath2.addAll(arrXpath);
            String childContext2 = prefixContext(childContext);
            if (childContext2.equals("")) {
            } else if (childContext2.startsWith("//")) {
                arrXpath2.add(prefixContext(childContext2));
            } else {
                arrXpath2.add(prefixContext("//*" + childContext2));
            }
            findXpath = getWhereXpath(arrXpath2);
        } else {
            findXpath = getWhereXpath(arrXpath);
        }
        String sortDefault = "  order by $b/@NID ";
        //MRLog.debug("findXpath "+ findXpath);
        buf.append(xQueryPrefix);
        //if the summary not needed 
        if (!addSummary && startFrom == 0 && countToFetch == 1) {
            buf.append(" for $b in   " + getNodeXpath(indexName, nodeID, null, ""));
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
        } else {
            buf.append(" let $list := for $b in   " + getNodeXpath(indexName, nodeID, null, ""));
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
            if (addSummary) {
                buf.append(" return insert-before(subsequence($list, " + startFrom + ", " + countToFetch + "), 1, ");
                //buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectStartFrom>"+startFrom+"</ObjectStartFrom><ObjectLastWhereEndCount>"+end+"</ObjectLastWhereEndCount></GetListObjectResponseData>) ");
                buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectLastWhereEndCount>" + (startFrom - 1) + "</ObjectLastWhereEndCount><CountToFetch>" + countToFetch + "</CountToFetch></GetListObjectResponseData>) ");
            } else {
                buf.append(" return subsequence($list, " + startFrom + ", " + countToFetch + ")");

            }
        }
        return buf.toString().replace('\n', ' ');
    }

    public static String readQuery(String query, String indexNameCol, String indexName, Map<String, String> xpaths, Map<String, String> nids) throws DBExecutionException {
        int startPos = 0;
        //for $b in #OBJ_AssetResource_OBJ#/@NID return string($b)

        StringBuffer buf = new StringBuffer(100);
        int pos1 = query.indexOf(RequestIQConst.PREFIX_OBJ, startPos);
        int pos2 = 0;
        if (pos1 == -1) {
            return query;
        }
        while (pos1 != -1) {
            if (pos1 > startPos) {
                buf.append("\n" + query.substring(startPos, pos1));
            }
            pos2 = query.indexOf(RequestIQConst.SUFFIX_OBJ, pos1);
            if (pos2 == -1) {
                throw new DBExecutionException("Invalid QueryRequest SUFFIX_OBJ not found. " + query);
            }
            String objectType = query.substring(pos1 + RequestIQConst.PREFIX_OBJ_LEN, pos2);
            if (!nids.containsKey(objectType)) {
                throw new DBExecutionException("Invalid QueryRequest nid not found for " + objectType + " " + query);
            }
            String where = "/..";
            if (xpaths.containsKey(objectType)) {
                where = "/..[" + xpaths.get(objectType) + "]";
            }
            NIDSchemeMDC schema = new NIDSchemeMDC();
            schema.parse(nids.get(objectType));
            if (schema.getType() == NIDSchemeMDC.COLLECTION) {
                objectType = "index-scan('" + indexNameCol + "', '" + schema.getCollectionNodeID() + "', 'EQ')" + where;
            } else if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
                objectType = "index-scan('" + indexName + "', '" + schema.getDocNodeID() + "', 'EQ')" + where;
            }
            buf.append(objectType);
            startPos = pos2 + RequestIQConst.SUFFIX_OBJ_LEN;
            pos1 = query.indexOf(RequestIQConst.PREFIX_OBJ, startPos);
        }
        if (query.length() > startPos) {
            buf.append(query.substring(startPos));
        }
        return buf.toString().replace('\n', ' ');
    }

    // ////////////////////// index //////////////////////
    public static String createIndexQuery(String indexName, String collectionName, String path) {
        // return "CREATE INDEX '" + indexName + "' ON collection('" +
        // collectionName + "')//@NID BY
        return "CREATE INDEX '" + indexName.trim() + "' ON collection('" + collectionName.trim() + "')" + path.trim() + " BY . AS xs:string ";
    }

    public static String createIndexQuery(String indexName, String docId, String collectionName, String path) {
        // return "CREATE INDEX '" + indexName + "' ON document( '" + docId +
        // "' ,  '" + collectionName + "')//@NID BY . AS xs:string ";
        return "CREATE INDEX '" + indexName.trim() + "' ON document( '" + docId.trim() + "' ,  '" + collectionName.trim() + "')" + path.trim() + " BY . AS xs:string ";
    }

    public static String deleteIndexQuery(String indexName) {
        return "DROP INDEX '" + indexName.trim() + "' ";
    }

    public static String existsIndexQuery(String indexName) {
        return "for $b in document('$indexes')//index[@name='" + indexName.trim() + "'] return $b";
    }

    // ///////////////// Document ////////////////
    public static String getDocumentXpath(String docId, String collectionName) {
        if (collectionName == null || collectionName.trim().equals(""))
            return "document('" + docId.trim() + "')";
        return "document('" + docId.trim() + "' , '" + collectionName.trim() + "')";
    }

    /*
     * public static String addDocumentXpath(String docId, String collectionName) { return "CREATE DOCUMENT '" + docId +
     * "' IN COLLECTION '" + collectionName + "'"; }
     */
    public static String deleteDocumentXpath(String docId, String collectionName) {
        if (collectionName == null || collectionName.trim().equals(""))
            return "DROP DOCUMENT '" + docId.trim() + "' ";
        return "DROP DOCUMENT '" + docId.trim() + "' IN COLLECTION '" + collectionName.trim() + "'";
    }

    public static String existDocumentXpath(String docId, String collectionName) {
        // for $b in document(
        // '$documents')//collection[@name='test']/document[@name='1.xml']
        // return $b
        //return "for $b in document('$documents')//collection[@name='" + collectionName.trim() + "']/document[@name='" + docId.trim() + "'] return $b";
        return "for $b in document('" + docId.trim() + "','" + collectionName.trim() + "')/*/name() return $b ";
    }

    public static String deleteEmptyDocumentXpath(String docId, String collectionName) {
        StringBuffer buf = new StringBuffer();
        buf.append(" DROP DOCUMENT ");
        //buf.append(" string( for $docName in document('$documents')//document[@name='" + docId + "']/@name  return $docName ) ");
        buf.append("'" + docId + "'");
        buf.append(" IN COLLECTION '" + collectionName.trim() + "'");
        return buf.toString();
    }

    // ///////////////// Collection //////////////////
    public static String addCollectionXpath(String collectionName) {
        return "CREATE COLLECTION '" + collectionName.trim() + "'";
    }

    public static String deleteCollectionXpath(String collectionName) {
        return "DROP COLLECTION '" + collectionName.trim() + "'";
    }

    public static String existCollectionXpath(String collectionName) {
        return "document('$collections')/collections/collection[@name='" + collectionName.trim() + "']/@name";
    }

    public static String readAllDocuments() {
        // return
        // "for $b in document('$documents')//documents/document return $b";
        return "document('$documents')";
    }

    public static String readCollections() {
        // return
        // "for $b in document('$documents')//documents/document return $b";
        return "document('$collections')";
    }
}
 