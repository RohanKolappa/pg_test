package com.ipvsserv.nxd.migration;

import java.util.ArrayList;
import java.util.List;

public class DBDocuments {
    private List<String> domainNames =new ArrayList<String>();
    public void setDomainNames(List<String> domainNames){
        this.domainNames = domainNames;
    }
    public  List<String> getDomainNames() {
        return this.domainNames;
    }
//
//    private String DB_COL_PREFIX = "";
//
//    private Element docsEL = null;
//    public Element getDocsEL() {
//        return docsEL;
//    }
//
//    public void setDocsEL(Element docsEL) {
//        this.docsEL = docsEL;
//    }
//
//    public DBDocuments() {
//        DB_COL_PREFIX = IpvsServGlobals.getDomainDBSchema().getDomainsPropterty(IpvsServGlobals.DB_COL_PREFIX);
//    }
//
//    public void init(String xml) {
//        Document doc = Dom4jUtils.stringToDocument(xml);
//        docsEL = doc.getRootElement();//EngineMDCUtils.stringToElement(xml);
//    }
//
//    public static String getQuery() {
//        StringBuffer bufDocs = new StringBuffer();
//        bufDocs.append(" let $ret := for $col in document('$collections')//collection/@name");
//        bufDocs.append(" return ");
//        bufDocs.append(" <collection>");
//        bufDocs.append(" { attribute name  {   data($col)  }  }");
//        bufDocs.append(" {");
//        bufDocs.append(" for $b in collection(data($col) )/*");
//        bufDocs.append(" let $docuri := document-uri( root( $b ) )");
//        bufDocs.append(" return ");
//        bufDocs.append(" element document{");
//        bufDocs.append(" attribute name {   document-uri( root( $b ) )  } ,");
//        bufDocs.append(" attribute root {    $b/name()      },");
//        bufDocs.append(" $b/@NID,");
//        bufDocs.append(" $b/@parentNID");
//        bufDocs.append(" }");
//        bufDocs.append(" }");
//        bufDocs.append(" </collection>");
//        bufDocs.append(" return    <documents> { $ret  } </documents> ");
//        return bufDocs.toString();
//    }
//
//    public List<String> getDomainNames() throws DBExecutionException {
//        try {
//            List<String> domains = new ArrayList<String>();
//            List<?> list = Dom4jUtils.getList(docsEL, "collection");
//            for (Iterator<?> i = list.iterator(); i.hasNext();) {
//                Element el = (Element) i.next();
//                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
//                if (docName.startsWith(DB_COL_PREFIX)) {
//                    String serviceDomain = docName.substring(docName.indexOf(DB_COL_PREFIX) + DB_COL_PREFIX.length());
//                    domains.add(serviceDomain);
//                }
//            }
//            return domains;
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public boolean isDocumentExists(String ID, String domainName, int docType, String docId, String collectionName) throws DBExecutionException {
//        try {
//            Element el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']/document[@name='" + docId + "']");
//            if (el != null) {
//                return true;
//            }
//            return false;
//        } catch (Exception ex) {
//            throw new DBExecutionException("isDocumentExists " + ex.getMessage());
//        }
//    }
//
//    public boolean isCollectionExists(String ID, String collectionName, String childCollectionName) throws DBExecutionException {
//        try {
//            Element el = null;
//            if (collectionName == null || collectionName.trim().equals("")) {
//                el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + childCollectionName + "']");
//            } else {
//                el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']/collection[@name='" + childCollectionName + "']");
//            }
//            if (el != null) {
//                return true;
//            }
//            return false;
//        } catch (Exception ex) {
//            throw new DBExecutionException("isDocumentExists " + ex.getMessage());
//        }
//    }
//
//    public List<String> getCollectionNames() throws DBExecutionException {
//        try {
//            List<String> domains = new ArrayList<String>();
//            List<?> list = Dom4jUtils.getList(docsEL, "collection");
//            for (Iterator<?> i = list.iterator(); i.hasNext();) {
//                Element el = (Element) i.next();
//                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
//                domains.add(docName);
//            }
//            return domains;
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public List<String> getDocumentNames(String collectionName) throws DBExecutionException {
//        try {
//            List<String> domains = new ArrayList<String>();
//            List<?> list = Dom4jUtils.getList(docsEL, "collection");
//            for (Iterator<?> i = list.iterator(); i.hasNext();) {
//                Element el = (Element) i.next();
//                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
//                domains.add(docName);
//            }
//            return domains;
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public List<String> getDocumentNames(String collectionName, String xpath) throws DBExecutionException {
//        try {
//            List<String> domains = new ArrayList<String>();
//            //System.out.println(docsEL.asXML());
//            List<?> list = Dom4jUtils.listXpath(docsEL, xpath);
//            for (Iterator<?> i = list.iterator(); i.hasNext();) {
//                Element el = (Element) i.next();
//                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
//                domains.add(docName);
//            }
//            return domains;
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public Hashtable<String, Element> getDocumentELList(String collectionName, String xpath) throws DBExecutionException {
//        try {
//            Hashtable<String, Element> domains = new Hashtable<String, Element>();
//            //System.out.println(docsEL.asXML());
//            List<?> list = Dom4jUtils.listXpath(docsEL, xpath);
//            for (Iterator<?> i = list.iterator(); i.hasNext();) {
//                Element el = (Element) i.next();
//                String docName = Dom4jUtils.getAttributeValue(el, "name", "").trim();
//                domains.put(docName, el);
//            }
//            return domains;
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public boolean addDocument(String docId, String collectionName, String root, String NID, String parentNID) throws DBExecutionException {
//        try {
//            Element el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']/document[@name='" + docId + "']");
//            if (el != null) {
//                //document already exits
//                return false;
//            } else {
//                el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']");
//                if (el != null) {
//                    //document does not exits
//                    Element documentEL = Dom4jUtils.addElement(el, "document");
//                    Dom4jUtils.addAttribute(documentEL, "name", docId);
//                    Dom4jUtils.addAttribute(documentEL, "NID", NID);
//                    Dom4jUtils.addAttribute(documentEL, "parentNID", parentNID);
//                    Dom4jUtils.addAttribute(documentEL, "root", root);
//                    return true;
//                }
//                return false;
//            }
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//
//    public boolean deleteDocument(String docId, String collectionName) throws DBExecutionException {
//        try {
//            Element el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']/document[@name='" + docId + "']");
//            if (el != null) {
//                //document already exits
//                return false;
//            } else {
//                el = Dom4jUtils.getElementXpath(docsEL, "//collection[@name='" + collectionName + "']");
//                if (el != null) {
//                    //document does not exits
//                    el.detach();
//                    return true;
//                }
//                return false;
//            }
//        } catch (Exception ex) {
//            throw new DBExecutionException("getDomainNames " + ex.getMessage());
//        }
//    }
//    /*
//     
//    <documents>
//    <collection name="$modules"/>
//    <collection name="IPVSc_default">
//    <document name="mspl_1b060a9a-0719-465b-9da1-b994b282e911" root="V2DMediaStreamProfile" NID="default.mediastreamprofilelist.mspl_1b060a9a-0719-465b-9da1-b994b282e911" parentNID="default.mediastreamprofilelist"/>
//    <document name="msprl_038b8551-a8dc-433b-8104-6878a572b334" root="MediaStorePortResource" NID="default.mediastoreportresourcelist.msprl_038b8551-a8dc-433b-8104-6878a572b334" parentNID="default.mediastoreportresourcelist"/>
//    <documen      
//     <documents>
//     <document name="$db_security_data"/>
//     <document name="service.version"/>
//     <collection name="$modules"/>
//     <collection name="IPVSc_default">
//      <document name="1729d"/>
//      <document name="1740d"/>
//      <document name="1775d"/> 
//      
//     */

}
