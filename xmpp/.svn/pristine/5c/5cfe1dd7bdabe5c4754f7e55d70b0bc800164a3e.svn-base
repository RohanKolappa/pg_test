package com.ipvsserv.nxd.xslt.extn;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import org.apache.xml.dtm.ref.DTMNodeIterator;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.IpvsDBUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.api.sednanative.SednaNativeXpath;
import com.ipvsserv.nxd.api.xmldb.exist.ExistXmlDBXpath;

public class IpvsXsltExtension {
    public static String getUUID() {
        UUID objUUID = UUID.randomUUID();

        return objUUID.toString();
    }

    public static String getAutoNID(String parentNID, String listPrefix) throws Exception {
       try{
           
       } catch (Exception ex) {
           MRLog.error("IpvsXsltExtension getAutoNID", ex);
       }  
       NIDSchemeMDC nidScheme = new NIDSchemeMDC();
        nidScheme.parse(parentNID);
        if (nidScheme.getType() != NIDSchemeMDC.COLLECTION) {
            MRLog.debug("getAutoNID Problem ????????1111????? parentNID  " + parentNID);
            throw new Exception("Invalid list parentNID ='" + parentNID + "'");
        }
        String NID = listPrefix + "_" + getUUID();
        return nidScheme.getCollectionNodeID() + "." + NID.toLowerCase();
    }

    public void init(org.apache.xalan.extensions.XSLProcessorContext context, org.w3c.dom.Element elem) {

    }

    public void incr(org.apache.xalan.extensions.XSLProcessorContext context, org.w3c.dom.Element elem) {

    }

    public int read(String name) {
        return 1;
    }

   
    ////////
    public static boolean isDMSUserRegistrationNeeded(String user, String password, String assetResource, String deviceType) {
        try {

        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public static String newNID(String elementName, String serviceDomain) {
        MRLog.debug("newNID elementName " + elementName + " serviceDomain " + serviceDomain);
        return IpvsDBUtils.getNewNID(serviceDomain, elementName);
    }

    public static String newParentNID(String elementName, String serviceDomain) {
        String parentNID = IpvsDBUtils.getNewParentNID(serviceDomain, elementName);
        MRLog.debug("newParentNID elementName " + elementName + " serviceDomain " + serviceDomain + " " + parentNID);
        return parentNID;
    }

    ///////////
    public static boolean printdocument(org.w3c.dom.NodeList nList) {
        //DTMNodeIterator mDTMNodeIterator = (DTMNodeIterator) elem;
        MRLog.debug(" printdocument " + DOMXmlUtils.elementToString(nList.item(0)));
        return true;
    }

    /* public static Document changeXpathValue(org.w3c.dom.NodeList nList, String xpath, String value) {
         Document doc = null;
         try {
             Node el = nList.item(0);
             String xml = DOMXmlUtils.elementToString(el);
             xml = Dom4jXsltUtils.setValue(xml, xpath, value);
             doc = DOMXmlUtils.stringToDocument(xml);
         } catch (Exception e) {
             e.printStackTrace();
         }
         return doc;
     }*/

    ////////// xpath utilities ///////////////////
    public static Document unparsedToDocument(org.w3c.dom.NodeList  nList) {
        Document doc = null;
        try {
             Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-getAutoNID", ex);
        }
        return doc;
    }
    public static Document unparsedToDocument2(Object obj) {
        Document doc = null;
        try {
            DTMNodeIterator nodeIter= (DTMNodeIterator)obj;
            Node el = nodeIter.getRoot();
            String xml = DOMXmlUtils.elementToString(el);
            System.out.print(xml);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-getAutoNID", ex);
        }
        return doc;
    }
    public static Document setValue(org.w3c.dom.NodeList nList, String xpath, String value) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.setValue(xml, xpath, value);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-getAutoNID", ex);
        }
        return doc;
    }
    
    public static Document setAttributeValue(org.w3c.dom.NodeList nList, String xpath, String attribute,String value) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.setAttributeValue(xml, xpath, attribute, value);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-getAutoNID", ex);
        }
        return doc;
    }
    
    public static Document addChildElements(org.w3c.dom.NodeList nList, String parentXpath, String elementName, String value) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.addChildElements(xml, parentXpath, elementName, value);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-addChildElements", ex);
        }
        return doc;
    }
    public static Document deleteElement(org.w3c.dom.NodeList nList, String xpath) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.deleteElement(xml, xpath);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-deleteElement", ex);
        }
        return doc;
    }
    public static Document deleteChildren(org.w3c.dom.NodeList nList, String xpath) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.deleteChildren(xml, xpath);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-deleteElement", ex);
        }
        return doc;
    }
    public static Document addChildElement(org.w3c.dom.NodeList nList, String xpath, Object childXML) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            DTMNodeIterator elm = ( DTMNodeIterator)childXML;
            String childxml = DOMXmlUtils.elementToString(elm.getRoot());
            xml = Dom4jXsltUtils.addChildElement(xml, xpath, childxml);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-addChildElement", ex);
        }
        return doc;
    }
    public static Document appendChildElement(org.w3c.dom.NodeList nList, String xpath, org.w3c.dom.NodeList nList2) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            if(nList !=null){
                Node el2 = nList2.item(0);
                String childxml = DOMXmlUtils.elementToString(el2);
                MRLog.debug("appendChildElement "+ xpath+" childxml " + childxml);
                xml = Dom4jXsltUtils.addChildElement(xml, xpath, childxml);
            }
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-addChildElement", ex);
        }
        return doc;
    }    
    public static Document getChildElement(org.w3c.dom.NodeList nList, String xpath) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.getChildElement(xml, xpath);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-getChildElement", ex);
        }
        return doc;
    }
    public static Document addAttribute(org.w3c.dom.NodeList nList, String xpath, String value) {
        Document doc = null;
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            xml = Dom4jXsltUtils.getChildElement(xml, xpath);
            doc = DOMXmlUtils.stringToDocument(xml);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-addAttribute", ex);
        }
        return doc;
    }
    //////////////////// DB operations /////////////////////

    public static String addDocument_DB(org.w3c.dom.NodeList nList, String serviceDomain) throws Exception {
        String nid = "";
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            nid = Dom4jXsltUtils.addDocument_DB("DMSUserRegistration", xml, serviceDomain);
            MRLog.debug("addDocument " + nid);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-addDocument_DB", ex);
        }
        return nid;
    }

    public static boolean updateDocument_DB(org.w3c.dom.NodeList nList) throws Exception {
        String nid = "";
        try {
            Node el = nList.item(0);
            String xml = DOMXmlUtils.elementToString(el);
            nid = Dom4jXsltUtils.updateDocument_DB("DMSUserRegistration", xml);
            MRLog.debug("updateDocument " + nid);
            return true;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-updateDocument_DB", ex);
        }        
        return false;
    }

    public static boolean deleteDocument_DB(org.w3c.dom.NodeList nList) throws Exception {
        String nid = "";
        try {
            Element el = (Element) nList.item(0);
            nid = el.getAttribute("NID");
            nid = Dom4jXsltUtils.deleteDocument_DB("DMSUserRegistration", nid);
            MRLog.debug("updateDocument " + nid);
            return true;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-deleteDocument_DB", ex);
        }  
        return false;

    }

    public static boolean deleteDocumentByNodeID_DB(org.w3c.dom.NodeList nList, String nid) throws Exception {
        try {
            nid = Dom4jXsltUtils.deleteDocument_DB("DMSUserRegistration", nid);
            MRLog.debug("deleteDocumentByNodeID " + nid);
            return true;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-deleteDocumentByNodeID_DB", ex);
        }  
        return false;
        
    }

    
   /* public static boolean deleteDocumentXpath_DB(org.w3c.dom.NodeList nList, String xpath, String value) {
        try {

        } catch (Exception e) {
            e.printStackTrace();
        }
        return true;
    }*/
    
    private static String getContext(NIDSchemeMDC node, String whereXpath){
        if(IpvsServGlobals.isUseListCol()){
            return ExistXmlDBXpath.getNodeXpathFromNIDSchemeMDC(node, whereXpath); 
        }else{
            return SednaNativeXpath.getNodeXpathFromNIDSchemeMDC(node, whereXpath);
        }
    }    
    
    public static boolean deleteDocumentWhereXpath_DB(org.w3c.dom.NodeList nList, String domainName, String whereXpath, String nodeID) {
        try {
            try {
                if(whereXpath==null || whereXpath.equals("")){
                    return false;
                }
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.parse(nodeID);
                List<String> listDocuments = new ArrayList<String>();
                String query = "for $b in "+getContext(node,whereXpath )+" return data($b/@NID)";
                DBAdmin.getDBAdminApi().getIpvsDBDriver().execute("dmsUserRegistration", domainName, node.getCollectionName(), query, listDocuments, 1, -1);
                for (String nid : listDocuments) {
                    nid = Dom4jXsltUtils.deleteDocument_DB("DMSUserRegistration", nid);
                    MRLog.debug("updateDocument " + nid);
                }
            } catch (Exception ex) {
                MRLog.error("IpvsXsltExtension-readDocument_DB", ex);
            }
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    } 

    public static Document readDocument_DB(org.w3c.dom.NodeList nList, String domainName, String whereXpath, String nodeID) {
        Document doc = null;
        try {
            if(whereXpath==null || whereXpath.equals("")){
                doc = DOMXmlUtils.stringToDocument("<norecords/>");
                return doc;
            }
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(nodeID);
            List<String> listDocuments = new ArrayList<String>();
            String query = "for $b in "+getContext(node,whereXpath )+" return $b";
            DBAdmin.getDBAdminApi().getIpvsDBDriver().execute("dmsUserRegistration", domainName, node.getCollectionName(), query, listDocuments, 1, -1);
            if (listDocuments.size() > 1) {
                MRLog.debug("" + whereXpath + " ?? more than one record " + listDocuments.size());
            }
            if (listDocuments.size() == 0) {
                MRLog.debug("" + whereXpath + " ?? no record found" + listDocuments.size());
            }
            String xml = "";
            if (listDocuments.size() == 1) {
                xml = listDocuments.get(0);
                doc = DOMXmlUtils.stringToDocument(xml);
            } else if (listDocuments.size() > 1) {
                xml = listDocuments.get(0);
                StringBuffer buf = new StringBuffer();
                buf.append("<list>");
                for (String string : listDocuments) {
                    buf.append(string);
                }
                buf.append("</list>");
                doc = DOMXmlUtils.stringToDocument(buf.toString());
            } else {
                doc = DOMXmlUtils.stringToDocument("<norecords/>");
            }

        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-readDocument_DB", ex);
        }
        return doc;
    }
     
    public static Document readDocument_DB_xquery(org.w3c.dom.NodeList nList, String domainName, String xquery, String nodeID) {
        Document doc = null;
        try {
            if(xquery==null || xquery.equals("")){
                doc = DOMXmlUtils.stringToDocument("<norecords/>");
                return doc;
            }
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(nodeID);
            List<String> listDocuments = new ArrayList<String>();
            String query = xquery;
            DBAdmin.getDBAdminApi().getIpvsDBDriver().execute("dmsUserRegistration", domainName, node.getCollectionName(), query, listDocuments, 1, -1);
            if (listDocuments.size() > 1) {
                MRLog.debug("" + xquery + " ?? more than one record " + listDocuments.size());
            }
            if (listDocuments.size() == 0) {
                MRLog.debug("" + xquery + " ?? no record found" + listDocuments.size());
            }
            String xml = "";
            if (listDocuments.size() == 1) {
                xml = listDocuments.get(0);
                doc = DOMXmlUtils.stringToDocument(xml);
            } else if (listDocuments.size() > 1) {
                xml = listDocuments.get(0);
                StringBuffer buf = new StringBuffer();
                buf.append("<list>");
                for (String string : listDocuments) {
                    buf.append(string);
                }
                buf.append("</list>");
                doc = DOMXmlUtils.stringToDocument(buf.toString());
            } else {
                doc = DOMXmlUtils.stringToDocument("<norecords/>");
            }

        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-readDocument_DB", ex);
        }
        return doc;
    } 
    public static boolean update_DB_xquery(org.w3c.dom.NodeList nList, String domainName, String query, String nodeID) {
        try {
            if(query==null || query.equals("")){
                return false;
            }
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(nodeID);
            DBAdmin.getDBAdminApi().getIpvsDBDriver().executeXQueryToUpdate("dmsUserRegistration", node.getCollectionName(), query);
            return true;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension-readDocument_DB", ex);
            return false;
        }
    }
}
