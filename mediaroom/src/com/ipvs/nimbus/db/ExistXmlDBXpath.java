package com.ipvs.nimbus.db;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class ExistXmlDBXpath {
/*
    public final static String XUPDATE_NS = "http://www.xmldb.org/xupdate";
    public static final String INSERT_INTO = "into";
    public static final String INSERT_PRECEDING = "preceding";
    public static final String INSERT_FOLLOWING = "following";

    /////////////// private /////////////
    public static String getNodeXpathFromNIDSchemeMDC(NIDSchemeMDC node, String whereXpath){
        return getCollectionConext(node.getDomain(), node.getNID()) +"[("+whereXpath+")]";
    }    
    private static String getNodeXpathContext(String ID, String domainName, int nodeType, String indexName, String NID) {
        if (nodeType == NIDSchemeMDC.COLLECTION)
            return getCollectionConext(domainName, NID) + "[@parentNID='" + NID.trim() + "']";
        else if (nodeType == NIDSchemeMDC.DOCUMENT)
            return getCollectionConext(domainName, NID) + "[@NID='" + NID.trim() + "']";
        return "";
    }

    public static String getNodeXpathContext(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext) {
        NIDSchemeMDC node = new NIDSchemeMDC(); 
        node.parse(NID);
        if (node.getType() == NIDSchemeMDC.COLLECTION) {
            arrXpath.add("@parentNID='" + NID.trim() + "'");
        } else {//if (nodeType == NIDSchemeMDC.DOCUMENT)
            arrXpath.add("@NID='" + NID.trim() + "'");
        }
        StringBuffer buf = new StringBuffer();
        buf.append(getCollectionConext(domainName, NID));
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
                buf.append("(" + replaceContext(xpath.trim()) + ")");
                if(xpath.startsWith("//*[@parentNID") || xpath.startsWith("//*[@NID")){
                    buf.append("(" + xpath.trim() + ")");
                }else{
                    buf.append("(" + xpath.trim().replace("//", ".//") + ")");
                }
            }
        }
        if (bAdded) {
            buf.append("]");
        }
        return buf.toString();
    }

    //private static Pattern pattern = Pattern.compile("[\\W.]//");
    private static String replaceContext(String query) {
        Matcher m = Pattern.compile("([^a-zA-Z0-9.]//|^//)").matcher(query);
        StringBuffer sb = new StringBuffer();
        while(m.find()) {
            String ss = m.group(0);
            ss = ss.replaceAll("//", ".//");
            m.appendReplacement(sb, ss);
        }
        m.appendTail(sb);
        //System.out.println("'REGED='" + sb.toString() + "' " + "=========ORIG='"+query );
        return sb.toString();
        
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

    //    private static String getSelectClause(String varPrefix, String selectXml, String defaultSelectClause) throws DBExecutionException {
    //        if (selectXml == null || selectXml.equals("")) {
    //            return defaultSelectClause;
    //        } else {
    //            try {
    //                Document doc = Dom4jUtils.stringToDocument(selectXml);
    //                if (doc.getRootElement().getName().equals("xpath")) {
    //                    String xpath = Dom4jUtils.value(doc.getRootElement(), "").trim();
    //                    return " {" + varPrefix + prefixContext(xpath) + "} ";
    //                }
    //                List<?> list = Dom4jUtils.listXpath(doc, "//xpath");
    //                Vector<Element> deleteList = new Vector<Element>();
    //                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
    //                    try {
    //                        Element el = (Element) iterator.next();
    //                        String xpath = Dom4jUtils.value(el, "").trim();
    //                        Element parent = el.getParent();
    //                        parent.addText("  {" + varPrefix + prefixContext(xpath) + "}       ");
    //                        deleteList.add(el);
    //                    } catch (Exception ex) {
    //                        ex.printStackTrace();
    //                    }
    //                }
    //                for (int i = 0; i < deleteList.size(); i++) {
    //                    Element el = deleteList.get(i);
    //                    el.detach();
    //                }
    //                deleteList.clear();
    //                deleteList = null;
    //                StringBuffer buf = new StringBuffer();
    //                buf.append(doc.getRootElement().asXML());
    //                return buf.toString();
    //            } catch (Exception ex) {
    //                throw new DBExecutionException("Parse SelectClause failed " + ex.getMessage());
    //            }
    //        }
    //    }

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
                if (!xpath.equals("")) {
                    if (!exists) {
                        buf.append(" " + varPrefix + prefixContext(xpath.trim()));
                    } else {
                        buf.append(" , " + varPrefix + prefixContext(xpath.trim()));
                    }
                    exists = true;
                }
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
    public static String getCollectionConextFromNIDVar(String domainName, String NIDVariable) {
        if(IpvsServGlobals.isUseListCol()){
            if(domainName==null || domainName.equals("")){
                MRLog.debug("getCollectionConextFromNIDVar domain Name ????'" + domainName+"'");
                domainName=IpvsServGlobals.getDomainDefault();
            }
            //String parentCol = "concat('/db/ipvs.default.',  substring-before( substring-after( "+NIDVariable+", 'default.') , '.'  )  )";
            String docuri = "concat('/db/"+IpvsServGlobals.getListColPrefix()+""+domainName+".',  substring-before( substring-after( "+NIDVariable+", '"+domainName+".') , '.'  ) , '/', substring-after( substring-after( "+NIDVariable+", '"+domainName+".'),'.'))";
             //return " collection(" + parentCol+ ")/*";
            return " doc(" + docuri+ ")/*";
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }   
    public static String getCollectionConextFromParentNIDVar(String domainName, String NIDVariable) {
        if(IpvsServGlobals.isUseListCol()){
            return " collection(concat('"+IpvsServGlobals.getListColPrefix()+"'," + NIDVariable+ "))/*";
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }     
    public static String getCollectionConext(String domainName, String NID) {
        if(IpvsServGlobals.isUseListCol()){
             //todo:pass NIDSchemeMDC as argument
             NIDSchemeMDC node = new NIDSchemeMDC();
             node.parse(NID);
             if(node.getType()==NIDSchemeMDC.COLLECTION) {
                 return " collection('/db/" + node.getCollectionName()+ "')/*";
             } else if(node.getType()==NIDSchemeMDC.DOCUMENT) {
                 return " doc('/db/" + node.getCollectionName()+ "/" + node.getDocUri() +"')/*";
             }  
             return null;
        }
        return " collection('" + IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX) + domainName + "')/*";
    }

    ///////////////////////////////////// private end ////////
    ////////////////////////////////
    public static String addNodeXpath2(String ID, String domainName, int nodeType, String indexName, String NID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) {
        StringBuffer buf = new StringBuffer();
        buf.append(" <xu:modifications version=\"1.0\" xmlns:xu=\"" + XUPDATE_NS + "\">");
        buf.append(" <xu:append select=\"" + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "\">");
        buf.append(xml);
        buf.append(" </xu:append>");
        buf.append(" </xu:modifications>");
        return buf.toString().replace('\n', ' ');
    }
    public static String addNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBExecutionException {
        // update insert  <email type="office">andrew@gmail.com</email> into  //address[fname="Andrew"]
        StringBuffer buf = new StringBuffer();
        buf.append(sednaToExistQuery(domainName, xQueryPrefix));
        buf.append(" update insert ");
        buf.append(" " + xml + " ");
        buf.append(" " + INSERT_INTO + " ");
        buf.append(getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) );
        return buf.toString(); 
    }
    public static String updateNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String xml, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBExecutionException {
        StringBuffer buf = new StringBuffer();
        buf.append(sednaToExistQuery(domainName, xQueryPrefix));
        if (childContext == null || childContext.equals("")) {
            buf.append(" for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + "/*  ");
        } else {
            buf.append(" for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + "  ");
        }
        buf.append(" return  ");
        buf.append(" update replace $b with   ");
        buf.append(xml);
        return buf.toString();
    }

    public static String deleteNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext, String xQueryPrefix) {
        StringBuffer buf = new StringBuffer();
        buf.append(" <xu:modifications version=\"1.0\" xmlns:xu=\"" + XUPDATE_NS + "\">");
        buf.append(" <xu:remove  select=\"" + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + "\">");
        buf.append(" </xu:remove >");
        buf.append(" </xu:modifications>");
        return buf.toString();
    }
    public static String deleteNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, List<String> arrXpath, String childContext, String xQueryPrefix) throws DBExecutionException {
        // for $city  in //address/city return  update delete  $city
        StringBuffer buf = new StringBuffer();
        buf.append(sednaToExistQuery(domainName, xQueryPrefix));
        buf.append(" for $b in  ");
        buf.append(getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext));
        buf.append(" return  update delete  $b  ");
        return buf.toString();
    }
    public static String getDiscoItemsXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, String action, List<String> arrXpath, String childContext, String xQueryPrefix) {
        if (action.equals(RequestIQConst.IS_OBJECT_EXISTS)) {
            return "for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + " return  $b/name() ";
        }
        return "for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpath, childContext) + " return  data($b)";
    }

    /////////////////////////
    public static String getNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID) {
        return " for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID) + " return  $b";
    }

    public static String selectSingleNodeXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {

        return selectNodesXpath(ID, domainName, nodeType, indexName, NID, collectionName, 0, 1, arrXpath, childContext, xQueryPrefix, arrSort, asc_desc, selectFilter, false);
    }

    public static String selectNodesXpath(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        StringBuffer buf = new StringBuffer();
        
         let $list :=  for $b in  index-scan('IPVSis_colnids_default', 'default.assetresourcelist', 'EQ')   
         where $b[(//AssetResource/Info[Type='MS-Device']) ]    
         order by $b      
         return $b  
         return insert-before(subsequence($list, 1, 20), 1,  <GetListObjectResponseData><ObjectCount> { count($list) } </ObjectCount><ObjectLastWhereEndCount>  20 </ObjectLastWhereEndCount></GetListObjectResponseData>) 
         
        if (countToFetch == -1) {
            countToFetch = 1;
        }
        if (startFrom <= 0) {
            startFrom = 1;
        }
        List<String> arrXpathList = new ArrayList<String>();
        String applychildContext = prefixContext(childContext);
        String sortDefault = "";
        if (applychildContext.equals("")) { //if doc root use
            sortDefault = "  order by $b/@NID ";
        } else {
            sortDefault = "  order by root($b)/@NID ";
        }        
        String findXpath = getWhereXpath(arrXpath);
        if (!addSummary && startFrom == 0 && countToFetch == 1) {
            buf.append(" for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, childContext) + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));//"  order by $b ");
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
        } else {
            buf.append(" let $list := for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, childContext) + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault));//"  order by $b ");
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
    public static String getDocumentList(String ID, String domainName, int nodeType, String indexName, String NID, String collectionName, int startFrom, int countToFetch, List<String> arrXpath, String childContext, String xQueryPrefix, List<String> arrSort, boolean asc_desc, String selectFilter, boolean addSummary) throws DBExecutionException {
        List<String> arrXpathList = new ArrayList<String>();
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
            arrXpath2.add(childContext);
            findXpath = getWhereXpath(arrXpath2);
        } else {
            findXpath = getWhereXpath(arrXpath);
        }
        String sortDefault = "  order by $b/@NID ";
        buf.append(sednaToExistQuery(domainName, xQueryPrefix));
        if (!addSummary && startFrom == 0 && countToFetch == 1) {
            buf.append(" for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, "") + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault)); 
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
        } else {
            buf.append(" let $list := for $b in " + getNodeXpathContext(ID, domainName, nodeType, indexName, NID, arrXpathList, "") + "  ");
            if (!findXpath.trim().equals("")) {
                buf.append(" where $b" + findXpath);
            }
            buf.append(getSortClause("$b", arrSort, asc_desc, sortDefault )); 
            buf.append(getSelectClause("$b", selectFilter, "  return $b "));
            if (addSummary) {
                buf.append(" return insert-before(subsequence($list, " + startFrom + ", " + countToFetch + "), 1, ");
                buf.append("  <GetListObjectResponseData><ObjectCount> { count($list)} </ObjectCount><ObjectLastWhereEndCount>" + (startFrom - 1) + "</ObjectLastWhereEndCount><CountToFetch>" + countToFetch + "</CountToFetch></GetListObjectResponseData>) ");
            } else {
                buf.append(" return subsequence($list, " + startFrom + ", " + countToFetch + ")");
            }
        }
        //MRLog.debug("getDocumentList \n:"+buf.toString());
        return buf.toString();
    }

    public static String readQuery(String id, String query, String indexNameCol, String indexName, String domainName, Map<String, String> xpaths, Map<String, String> nids) throws DBExecutionException {
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

    public static String sednaToExistQuery(String domainName, String query) throws DBExecutionException {
        //System.out.println("-----------------before fix='" + query + "'-----------------");
        StringBuffer buf = new StringBuffer(100);
        query = sednaToExistQuery_indexscan(domainName, query);
        try {
            int len = query.length();
            for (int i = 0; i < len; i++) {
                char c = query.charAt(i);
                if (c == '(') {
                    if (i + 1 < len) {
                        char c2 = query.charAt(i + 1);
                        if (c2 == ':') {
                            int ipos = query.indexOf(":)", i);
                            if (ipos == -1) {
                                throw new DBExecutionException("Invalid comment");
                            }
                            i = ipos + ":)".length() - 1;
                            continue;
                        } else {
                            buf.append(c);
                            continue;
                        }
                    } else {
                        buf.append(c);
                        continue;
                    }

                } else if (c == '[') {
                    int closeBracketCounter=1;
                    StringBuffer localBuf = new StringBuffer();
                    localBuf.append(c);
                    while (true) {
                        if (i + 1 > len) {
                            buf.append(localBuf.toString());
                            break;
                        }
                        i++;
                        c = query.charAt(i);
                        localBuf.append(c);
                        if (c == '[') {
                            closeBracketCounter++; 
                        }
                        if (c == ']') {
                            closeBracketCounter--; 
                        }
                        if(closeBracketCounter==0){
                             break;
                        }
                    }
                    
                    //buf.append(localBuf.toString().replace("//", ".//"));
                    buf.append(replaceContext(localBuf.toString()) );
                    
                    
//                    ///////////
//                    int ipos = query.indexOf("]", i);
//                    if (ipos == -1) {
//                        throw new DBExecutionException("Invalid []");
//                    }
//                    String whereXpath = query.substring(i, ipos + "]".length());
//                    i = ipos;
//                    buf.append(whereXpath.replace("//", ".//"));
//                    
                    
                } else {
                    buf.append(c);
                }
            }
        } finally {
            //System.out.println("-----------------after fix='" + buf.toString() + "'-----------------");
        }
        return buf.toString();
    }
    
    public static String sednaToExistQuery_indexscan(String domainName, String query) throws DBExecutionException {
        StringBuffer buf = new StringBuffer(100);
        String indexSecan = "index-scan";
        try {
            int len = query.length();
            for (int i = 0; i < len; i++) {
                char c = query.charAt(i);
                if (c == 'i') {
                    StringBuffer localBuf = new StringBuffer();
                    localBuf.append(c);
                    while (true) {
                        if (i + 1 > len) {
                            buf.append(localBuf.toString());
                            break;
                        }
                        i++;
                        c = query.charAt(i);
                        localBuf.append(c);
                        if (indexSecan.equals(localBuf.toString())) {
                            int ipos = query.indexOf(")", i);
                            if (ipos == -1) {
                                throw new DBExecutionException("Invalid index-scan");
                            }
                            ipos = ipos + ")".length();
                            localBuf.append(query.substring(i + 1, ipos));
                            i = ipos;

                            /////////////// get next chars which will look like /..
                            int count = 0;
                            StringBuffer localBuf2 = new StringBuffer();

                            while (true) {
                                if (ipos + 1 > len) {
                                    break;
                                }
                                c = query.charAt(ipos);
                                if (Character.isWhitespace(c)) {
                                } else {
                                    localBuf2.append(c);
                                    count++;
                                }
                                ipos++;
                                if (count == 3) {
                                    break;
                                }
                            }
                            if (localBuf2.toString().equals("/..")) {
                                localBuf.append(localBuf2.toString());
                                i = ipos - 1;
                            }
                            buf.append(sednaToExistQueryContext(domainName, localBuf.toString()));
                            break;
                        } else if (indexSecan.startsWith(localBuf.toString())) {
                            //keep reading
                        } else {
                            buf.append(localBuf.toString());
                            break;
                        }
                    }
                } else {
                    buf.append(c);
                }
            }
        } finally {
            //System.out.println("-----------------after fix='" + buf.toString() + "'-----------------");
        }
        return buf.toString();
    }    
    
//    public static String sednaToExistQuery2(String domainName, String query) throws DBExecutionException {
//        //System.out.println("-----------------before fix='" + query + "'-----------------");
//        StringBuffer buf = new StringBuffer(100);
//        String indexSecan = "index-scan";
//        try {
//            int len = query.length();
//            for (int i = 0; i < len; i++) {
//                char c = query.charAt(i);
//                if (c == 'i') {
//                    StringBuffer localBuf = new StringBuffer();
//                    localBuf.append(c);
//                    while (true) {
//                        if (i + 1 > len) {
//                            buf.append(localBuf.toString());
//                            break;
//                        }
//                        i++;
//                        c = query.charAt(i);
//                        localBuf.append(c);
//                        if (indexSecan.equals(localBuf.toString())) {
//                            int ipos = query.indexOf(")", i);
//                            if (ipos == -1) {
//                                throw new DBExecutionException("Invalid index-scan");
//                            }
//                            ipos = ipos + ")".length();
//                            localBuf.append(query.substring(i + 1, ipos));
//                            i = ipos;
//
//                            /////////////// get next chars which will look like /..
//                            int count = 0;
//                            StringBuffer localBuf2 = new StringBuffer();
//
//                            while (true) {
//                                if (ipos + 1 > len) {
//                                    break;
//                                }
//                                c = query.charAt(ipos);
//                                if (Character.isWhitespace(c)) {
//                                } else {
//                                    localBuf2.append(c);
//                                    count++;
//                                }
//                                ipos++;
//                                if (count == 3) {
//                                    break;
//                                }
//                            }
//                            if (localBuf2.toString().equals("/..")) {
//                                localBuf.append(localBuf2.toString());
//                                i = ipos - 1;
//                            }
//                            buf.append(sednaToExistQueryContext(domainName, localBuf.toString()));
//                            break;
//                        } else if (indexSecan.startsWith(localBuf.toString())) {
//                            //keep reading
//                        } else {
//                            buf.append(localBuf.toString());
//                            break;
//                        }
//                    }
//                } else if (c == '(') {
//                    if (i + 1 < len) {
//                        char c2 = query.charAt(i + 1);
//                        if (c2 == ':') {
//                            int ipos = query.indexOf(":)", i);
//                            if (ipos == -1) {
//                                throw new DBExecutionException("Invalid comment");
//                            }
//                            i = ipos + ":)".length() - 1;
//                            continue;
//                        } else {
//                            buf.append(c);
//                            continue;
//                        }
//                    } else {
//                        buf.append(c);
//                        continue;
//                    }
//
//                } else if (c == '[') {
//                    int ipos = query.indexOf("]", i);
//                    if (ipos == -1) {
//                        throw new DBExecutionException("Invalid []");
//                    }
//                    String whereXpath = query.substring(i, ipos + "]".length());
//                    i = ipos;
//                    buf.append(whereXpath.replace("//", ".//"));
//                } else {
//                    buf.append(c);
//                }
//            }
//        } finally {
//            //System.out.println("-----------------after fix='" + buf.toString() + "'-----------------");
//        }
//        return buf.toString();
//    }
    public static String sednaToExistQueryContext(String domainName, String context) throws DBExecutionException {
        String docIndex = "IPVSi_nids_";
        String colIndex = "IPVSis_colnids_";
        //String colIndex= IpvsServGlobals.getDomainDBSchema().getIndexNamePrefix("nodeColIndex");
        //String docIndex= IpvsServGlobals.getDomainDBSchema().getIndexNamePrefix("nodeIndex");
        int ipos = context.indexOf("(");
        if (ipos == -1) {
            throw new DBExecutionException("Invalid context '" + context + "'");
        }
        int ipos2 = context.lastIndexOf(")");
        if (ipos2 == -1) {
            throw new DBExecutionException("Invalid context '" + context + "'");
        }

        String params = context.substring(ipos + 1, ipos2);
        //System.out.println("sednaToExistQueryContext params " + params);
        String[] arr = params.split(",");
        String indexName = arr[0].replace('\'', ' ').trim();
        String nid = arr[1].replace('\'', ' ').trim();
        if (indexName.startsWith(colIndex)) {
            //domainName = indexName.substring(colIndex.length());
            if (nid.startsWith("$")) {
                return ExistXmlDBXpath.getCollectionConextFromParentNIDVar(domainName, nid) + "[@parentNID=" + nid.trim() + "]";
            } else {
                return ExistXmlDBXpath.getCollectionConext(domainName, nid) + "[@parentNID='" + nid.trim() + "']";
            }
        } else if (indexName.startsWith(docIndex)) {
            //domainName = indexName.substring(docIndex.length());
            if (nid.startsWith("$")) {
                return ExistXmlDBXpath.getCollectionConextFromNIDVar(domainName, nid) + "[@NID=" + nid.trim() + "]";
            } else {
                return ExistXmlDBXpath.getCollectionConext(domainName, nid) + "[@NID='" + nid.trim() + "']";
            }
        } else {
            throw new DBExecutionException("Invalid context '" + context + "'");
        }
    }

    //    public static String sednaToExistQuery(String query) throws DBExecutionException {
    //        String PREFIX_OBJ = "index-scan";
    //        String SUFFIX_OBJ = ")";
    //        int SUFFIX_OBJ_LEN = SUFFIX_OBJ.length();
    //        String SUFFIX_OBJ2 = ")/..";
    //        int SUFFIX_OBJ2_LEN = SUFFIX_OBJ2.length();        
    //        StringBuffer buf = new StringBuffer(100);
    //        //System.out.println("-----------------before sednaToExistQuery='" + query  +"'-----------------");
    //        try {
    //            int startPos = 0;
    //            //for $b in index_scan return string($b)
    //            int pos1 = query.indexOf(PREFIX_OBJ, startPos);
    //            int pos2 = 0;
    //            if (pos1 == -1) {
    //                return fixContext(query);
    //            }
    //            while (pos1 != -1) {
    //                if (pos1 > startPos) {
    //                    buf.append("\n" + query.substring(startPos, pos1));
    //                }
    //                pos2 = query.indexOf(SUFFIX_OBJ2, pos1);
    //                int suffix_len=SUFFIX_OBJ2_LEN;
    //                if (pos2 == -1) {
    //                    pos2 = query.indexOf(SUFFIX_OBJ, pos1);
    //                    suffix_len=SUFFIX_OBJ_LEN;
    //                }  
    //                if (pos2 == -1) {
    //                    throw new DBExecutionException("Invalid sednaToExistQuery SUFFIX_OBJ not found. " + query);
    //                }
    //                pos2+=suffix_len;                
    //                String objectType = query.substring(pos1, pos2).replace('\'', ' ');
    //                //MRLog.debug(" sednaToExistQuery SUFFIX_OBJ '" + objectType + "' " );
    //                String[] arguments = objectType.split(",");
    //                String nid = "";
    //                for (String arg : arguments) {
    //                    if (arg.indexOf('.') != -1) {
    //                        nid = arg.trim();
    //                        break;
    //                    }
    //                }
    //                NIDSchemeMDC schema = new NIDSchemeMDC();
    //                schema.parse(nid);
    //                if (schema.getDomain().equals("")) {
    //                    throw new DBExecutionException("Invalid sednaToExistQuery SUFFIX_OBJ not found. '"+nid+"' '" + objectType + "' " + query);
    //                }
    //                objectType = getNodeXpathContext("sednaToExistQuery", schema.getDomain(), schema.getType(), "", nid);
    //    
    //                buf.append(objectType);
    //                startPos = pos2 ;
    //                //startPos = pos2 ;
    //                pos1 = query.indexOf(PREFIX_OBJ, startPos);
    //            }
    //            if (query.length() > startPos) {
    //                buf.append(query.substring(startPos));
    //            }
    //            query = fixContext(buf.toString());
    //        }finally{
    //            //System.out.println("-----------------after sednaToExistQuery='" + query  +"'-----------------");
    //        }
    //        return  fixContext(buf.toString());
    //    }
    // 
    //   
    //    public static String fixContext(String query)  {
    //        System.out.println("-----------------before fixContext='" + query  +"'-----------------");
    //        StringBuffer buf = new StringBuffer(100);
    //        try{
    //            int startPos = 0; 
    //            //for $b in index_scan return string($b)
    //            int pos1 = query.indexOf("[", startPos);
    //            int pos2=0;
    //            while (pos1 != -1) {
    //                if (pos1 > startPos) {
    //                    buf.append("\n" + query.substring(startPos, pos1));
    //                }
    //                pos2=query.indexOf("]", pos1);
    //                String objectType = query.substring(pos1, pos2);
    //                //System.out.println(" fixContext SUFFIX_CONTEXT '" + objectType + "' " );
    //                objectType = objectType.replace("//", ".//");
    //                buf.append(objectType);
    //                startPos = pos2 ;
    //                //startPos = pos2 ;
    //                pos1 = query.indexOf("[", startPos);
    //            }
    //            if (query.length() > startPos) {
    //                buf.append(query.substring(startPos));
    //            }
    //        }finally{
    //            System.out.println("-----------------after fixContext='" + query  +"'-----------------");
    //        }
    //        return buf.toString();
    //    }

    public static void main2(String[] args){
        String query= "for $b in collection('IPVSc_default')/AssetResource[//Title='srcrelay1'] return $b";
        String str ="";
        try {
            str = fixContext(query);
        } catch (DBExecutionException e) {
            e.printStackTrace();
        }
            System.out.println("str " + str);
    }*/
}
