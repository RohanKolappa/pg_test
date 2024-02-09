package com.ipvsserv.nxd.api.xmldb;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class XmlDBXpath {
    public final static String XUPDATE_NS = "http://www.xmldb.org/xupdate";

    /////////////// private /////////////
    private static String getNodeXpathContext(String ID, String domainName, int nodeType, String indexName, String NID) {
        if (nodeType == NIDSchemeMDC.COLLECTION)
            return getCollectionConext(domainName) + "//*[@parentNID='" + NID + "']";
        else if (nodeType == NIDSchemeMDC.DOCUMENT)
            return getCollectionConext(domainName) + "//*[@NID='" + NID + "']";
        return "";
    }

    public static String getNodeXpathContext(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext) {
        if (nodeType == NIDSchemeMDC.COLLECTION) {
            arrXpath.add("//*[@parentNID='" + NID + "']");
        } else {//if (nodeType == NIDSchemeMDC.DOCUMENT)
            arrXpath.add("//*[@NID='" + NID + "']");
        }
        StringBuffer buf = new StringBuffer();
        buf.append(getCollectionConext(domainName));
        buf.append(getWhereXpath(arrXpath));
        buf.append(prefixContext(childContext));
        return buf.toString();
    }

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

    private static String getWhereXpath(List<String> arrXpath) {
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
                buf.append("(" + xpath.trim() + ")");
            }
        }
        if (bAdded) {
            buf.append("]");
        }
        return buf.toString().replace("//", ".//");
    }

    private static String getSelectClause(String varPrefix, String selectXml, String defaultSelectClause) throws DBExecutionException {
        if (selectXml == null || selectXml.equals("")) {
            return defaultSelectClause;
        } else {
            try {
                Document doc = Dom4jUtils.stringToDocument(selectXml);
                if (doc.getRootElement().getName().equals("xpath")) {
                    String xpath = Dom4jUtils.value(doc.getRootElement(), "").trim();
                    return " " + varPrefix + prefixContext(xpath) + " ";
                }
                List<?> list = Dom4jUtils.listXpath(doc, "//xpath");
                Vector<Element> deleteList = new Vector<Element>();
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    try {
                        Element el = (Element) iterator.next();
                        String xpath = Dom4jUtils.value(el, "").trim();
                        Element parent = el.getParent();
                        parent.addText("  " + varPrefix + prefixContext(xpath) + "       ");
                        deleteList.add(el);
                    } catch (Exception ex) {
                        ex.printStackTrace();
                    }
                }
                for (int i = 0; i < deleteList.size(); i++) {
                    Element el = deleteList.get(i);
                    el.detach();
                }
                deleteList.clear();
                deleteList = null;
                StringBuffer buf = new StringBuffer();
                buf.append(doc.getRootElement().asXML());
                return buf.toString();
            } catch (Exception ex) {
                throw new DBExecutionException("Parse SelectClause failed " + ex.getMessage());
            }
        }
    }

    private static String getSortClause(String varPrefix, List<String> arrSort, boolean asc_desc, String defaultSortClause) {
        if (arrSort == null || arrSort.size() == 0) {
            return defaultSortClause;
        } else {
            StringBuffer buf = new StringBuffer();
            buf.append("  order by ");
            for (Iterator<String> iterator = arrSort.iterator(); iterator.hasNext();) {
                String xpath = iterator.next();
                if (asc_desc) {
                    buf.append(varPrefix + prefixContext(xpath.trim()) + " ascending");
                } else {
                    buf.append(varPrefix + prefixContext(xpath.trim()) + " descending");
                }
            }
            return buf.toString();
        }
    }

    private static String getCollectionConext(String domainName) {
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')";
    }

    ///////////////////////////////////// private end ////////
    ////////////////////////////////
    public static String addNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String xml, List<String> arrXpath, String childContext) {
        StringBuffer buf = new StringBuffer();
        buf.append(" <xu:modifications version=\"1.0\" xmlns:xu=\"" + XUPDATE_NS + "\">");
        buf.append(" <xu:append select=\"" + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "\">");
        buf.append(xml);
        buf.append(" </xu:append>");
        buf.append(" </xu:modifications>");
        return buf.toString().replace('\n', ' ');
    }

    public static String updateNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String xml, List<String> arrXpath, String childContext) {
        StringBuffer buf = new StringBuffer();
        buf.append(" <xu:modifications version=\"1.0\" xmlns:xu=\"" + XUPDATE_NS + "\">");
        buf.append(" <xu:update select=\"" + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "" + "\">");
        buf.append(xml);
        buf.append(" </xu:update>");
        buf.append(" </xu:modifications>");
        return buf.toString().replace('\n', ' ');
    }

    public static String deleteNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext) {
        StringBuffer buf = new StringBuffer();
        buf.append(" <xu:modifications version=\"1.0\" xmlns:xu=\"" + XUPDATE_NS + "\">");
        buf.append(" <xu:remove  select=\"" + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "\">");
        buf.append(" </xu:remove >");
        buf.append(" </xu:modifications>");
        return buf.toString().replace('\n', ' ');
    }

    //    public static String existNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID) {
    //        return XmlDBXpath.getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "/name()";
    //    }

    public static String getDiscoItemsXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext) {
        if (action.equals(RequestIQConst.IS_OBJECT_EXISTS)) {
            return "for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + " return  $b/name() ";
        }
        return "for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + " return  data($b)";
    }

    /////////////////////////
    public static String getNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID) {
        return " for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + " return  $b";
    }

    public static String selectSingleNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {

        return selectNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, 0, 1, arrXpath, childContext, arrSort, asc_desc, selectFilter, false);
    }

    public static String selectNodesXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        StringBuffer buf = new StringBuffer();
        /*
         let $list :=  for $b in  index-scan('IPVSis_colnids_default', 'default.assetresourcelist', 'EQ')   
         where $b[(//AssetResource/Info[Type='MS-Device']) ]    
         order by $b      
         return $b  
         return insert-before(subsequence($list, 1, 20), 1,  <GetListObjectResponseData><ObjectCount> { count($list) } </ObjectCount><ObjectLastWhereEndCount>  20 </ObjectLastWhereEndCount></GetListObjectResponseData>) 
         */
        if (countToFetch == -1) {
            countToFetch = 1;
        }
        if (startFrom <= 0) {
            startFrom = 1;
        }
        List<String> arrXpathList = new ArrayList<String>();
        String findXpath = getWhereXpath(arrXpath);
        if (!addSummary && startFrom == 0 && countToFetch == 1) {
            buf.append(" for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, childContext) + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, " order by $b/@NID  "));//"  order by $b ");
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
        } else {
            buf.append(" let $list := for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, childContext) + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, "  order by $b/@NID  "));//"  order by $b ");
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
            if (addSummary) {
                buf.append(" return insert-before(subsequence($list, " + startFrom + ", " + countToFetch + "), 1, ");
                buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectLastWhereEndCount>" + (startFrom - 1) + "</ObjectLastWhereEndCount><CountToFetch>" + countToFetch + "</CountToFetch></GetListObjectResponseData>) ");
            } else {
                buf.append(" return subsequence($list, " + startFrom + ", " + countToFetch + ")");
            }
        }
        return buf.toString();
    }

    public static String readQuery(String query, String indexNameCol, String indexName, String domainName, Map<String, String> xpaths, Map<String, String> nids) throws DBExecutionException {
        //may be encription is needed or cdata
        int startPos = 0;
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

            String xpath = null;
            List<String> arrXpath = new ArrayList<String>();
            if (xpaths.containsKey(objectType)) {
                xpath = xpaths.get(objectType);
                if (!xpath.trim().equals("")) {
                    arrXpath.add(xpath);
                }
            }
            String NID = nids.get(objectType);
            buf.append(getNodeXpathContext("readQuery-" + objectType, domainName, 1, indexName, NID, arrXpath, ""));

            startPos = pos2 + RequestIQConst.SUFFIX_OBJ_LEN;
            pos1 = query.indexOf(RequestIQConst.PREFIX_OBJ, startPos);
        }
        if (query.length() > startPos) {
            buf.append(query.substring(startPos));
        }
        //System.out.print(buf.toString());
        return buf.toString();
    }

    public static String getServiceVersion() throws DBExecutionException {
        return "for $b in document('"+IpvsServGlobals.SERVICE_VERSION+"')/Service return data($b/@Ver)";
    }

    public static String getDBMigrationProgressStatus() throws DBExecutionException {
        return "for $b in document('"+IpvsServGlobals.MIGRATION_PROGRESS_STATUS+"')/* return $b";
    }
}
