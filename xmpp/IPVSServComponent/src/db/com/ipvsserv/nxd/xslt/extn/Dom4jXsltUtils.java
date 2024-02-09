package com.ipvsserv.nxd.xslt.extn;

import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsDBUtils;
import com.ipvsserv.nxd.adminapi.DBAdmin;

public class Dom4jXsltUtils {

    //////////// xml modifications utils /////////////////
    public static String deleteChildren(String xml, String xpath) {
        try {
            //MRLog.debug("start deleteElement  "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), xpath);
            if (el != null) {
                List<?> list = Dom4jUtils.getList(el);
                for (Object object : list) {
                    try{
                        Element elChild = (Element)object;
                        elChild.detach();
                    } catch (Exception ex) {
                        MRLog.error("IpvsXsltExtension deleteElement", ex);
                    }
                }
            }
            //MRLog.debug("end deleteElement  "+doc.asXML());
            return Dom4jUtils.documentToString(doc);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension deleteElement", ex);
        }
        return xml;
    }
    public static String deleteElement(String xml, String xpath) {
        try {
            //MRLog.debug("start deleteElement  "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), xpath);
            if (el != null) {
                el.detach();
            }
            //MRLog.debug("end deleteElement  "+doc.asXML());
            return Dom4jUtils.documentToString(doc);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension deleteElement", ex);
        }
        return xml;
    }
    public static String setValue(String xml, String xpath, String value) {
        try {
            //MRLog.debug("start setValue  "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), xpath);
            if (el != null) {
                el.setText(value);
            }
            //MRLog.debug("end setValue  "+doc.asXML());
            return Dom4jUtils.documentToString(doc);

        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension setValue", ex);
        }
        return xml;
    }
    
    public static String setAttributeValue(String xml, String elementXpath, String attributeName,String value) {
        try {
            //MRLog.debug("start setValue  "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), elementXpath);
            if (el != null) {
                Attribute attribute  =  el.attribute(attributeName);
                if(attribute!=null) {
                	attribute.setText(value);
                }
            }
            //MRLog.debug("end setValue  "+doc.asXML());
            return Dom4jUtils.documentToString(doc);

        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension setValue", ex);
        }
        return xml;
    }

    public static String addChildElements(String xml, String parentXpath, String elementName, String value) {
        try {
            //MRLog.debug("start setValue  "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), parentXpath);
            String[] values = value.split(",");
            if (el != null) {
                for (int i = 0; i < values.length; i++) {
                    Dom4jUtils.addElement(el, elementName, values[i]);
                }
            }
            //MRLog.debug("end setValue  "+doc.asXML());
            return Dom4jUtils.documentToString(doc);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension addChildElements", ex);
        }
        return xml;
    }

    public static String addChildElement(String xml, String xpath, String childXML) {
        try {
            //MRLog.debug("start addChildElement "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), xpath);
            if (el != null) {
                Element child = Dom4jUtils.stringToElement(childXML);
                Dom4jUtils.addElement(el, (Element) child.detach());
            }
            //MRLog.debug("end addChildElement "+doc.asXML());
            return Dom4jUtils.documentToString(doc);
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension addChildElement", ex);
        }
        return xml;
    }

    public static String getChildElement(String xml, String xpath) {
        try {
            //MRLog.debug("start getChildElement "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element el = Dom4jUtils.elementXpath(doc.getRootElement(), xpath);
            if (el != null) {
                //MRLog.debug("end getChildElement "+el .asXML());
                return el.asXML();
            }
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension getChildElement", ex);
        }
        return "<nochildfound/>";
    }

    /////////////////// DB operations ///////////////////
    public static String addDocument_DB(String ID, String xml, String serviceDomain) throws Exception {
        String nodeID = "";
        try {
            //MRLog.debug("addDocument_DB  using "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element root = doc.getRootElement();
            String parentNID = IpvsDBUtils.getNewParentNID(serviceDomain, root.getName());
            try {
                IpvsDBUtils.applyNID(root, parentNID);
            } catch (Exception ex) {
                MRLog.error("IpvsXsltExtension addDocument_DB", ex);
            }
            nodeID = Dom4jUtils.getAttributeValue(root, "NID", "");
            NIDSchemeMDC nidScheme = new NIDSchemeMDC();
            nidScheme.parse(nodeID);
            DBAdmin.getDBAdminApi().getIpvsDBDriver().addDocument(ID, nidScheme.getDocUri(), nidScheme.getCollectionName(), root.asXML());
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension addDocument_DB", ex);
        }
        return nodeID;
    }

    public static String updateDocument_DB(String ID, String xml) throws Exception {
        String nodeID = "";
        try {
            //MRLog.debug("updateDocument_DB  using "+xml);
            Document doc = Dom4jUtils.stringToDocument(xml);
            Element root = doc.getRootElement();
            nodeID = Dom4jUtils.getAttributeValue(root, "NID", "");
            if (nodeID.equals("")) {
                throw new Exception("Update Document Invalid NID.");
            }
            NIDSchemeMDC nidScheme = new NIDSchemeMDC();
            nidScheme.parse(nodeID);
            try {
                DBAdmin.getDBAdminApi().getIpvsDBDriver().deleteDocument(nodeID, nidScheme.getDomain(), 1, nidScheme.getDocUri(), nidScheme.getCollectionName());
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            DBAdmin.getDBAdminApi().getIpvsDBDriver().addDocument("DMSUserRegistration", nidScheme.getDocUri(), nidScheme.getCollectionName(), root.asXML());
            return nodeID;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension addDocument_DB", ex);
        }
        return nodeID;
       
    }

    public static String deleteDocument_DB(String ID, String nodeID) throws Exception {
        try {
            //MRLog.debug("deleteDocument_DB  using "+nodeID);
            if (nodeID.equals("")) {
                throw new Exception("Delete Document Invalid NID.");
            }
            NIDSchemeMDC nidScheme = new NIDSchemeMDC();
            nidScheme.parse(nodeID);
            DBAdmin.getDBAdminApi().getIpvsDBDriver().deleteDocument(nodeID, nidScheme.getDomain(), 1, nidScheme.getDocUri(), nidScheme.getCollectionName());
            return nodeID;
        } catch (Exception ex) {
            MRLog.error("IpvsXsltExtension deleteDocument_DB", ex);
        }     
        return nodeID;
      
    }
}
