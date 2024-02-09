package com.ipvsserv.common.mdc;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.QName;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Dom4jUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class EngineMDCUtils {
    public static final String NID_SCHEME = "NIDScheme";
    public static final String NODEID_2 = "NID";
    public static final String PARENT_NID = "PARENTNID.";
    public static final String ELEMENT_NAME = "ELEMENTNAME";
    public static final String PARSE_2 = "Parse";
    public static final String NID_SCHEME_AUTO = "AUTO";
    public static final String KEY_VAL = "KEYVAL";
    public static final String KEY_ELEMENT_RELATIVE_XPATH = "KeyElementRelativeXPath";
    public static final String DB_ACTION = "dbAction";
    public static final String ADD_ACTION = "new";
    public static final String UPDATE_ACTION = "update";
    public static final String UPDATE_SKIP_ACTION = "update_skip";
    public static final String NODEID = "nodeid";
    public static final String PARENT = "PARENT.";
    public static final String ELEMENT = "ELEMENT";
    public static final String RUNNINGNUMBER = "#";
    public static final String RUNNINGNUMBEREND = "_%";
    public static final String DATA = "data";
    public static final String PARSE = "parse";
    
    public static final String VERSION = "Version";
    public static final String PERSISTENCE = "Persistence";
    
    // /////////////////// unitlity //////////////////////
    // /////////////////// unitlity //////////////////////
    // /////////////////// unitlity //////////////////////
    public static boolean copyAttributes_checkNew(Element toEL, Element fromEL) throws Exception {
        boolean bHasNew = false;
        List<?> attrList = Dom4jUtils.getAttributeList(fromEL);
        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
            Attribute attrEL = (Attribute) i.next();
            String name = attrEL.getName().trim();
            String val = attrEL.getValue();
            // //MRLog.println(MRLog.ERROR,"remove Attrb " + name +" " + val);
            Dom4jUtils.addAttribute(toEL, name, val);
            if (!bHasNew) {
                bHasNew = !EngineMDCUtils.isSchemaAttrib(name);
            }
        }
        return bHasNew;
    }
    
    public static boolean isSchemaAttrib(String attribName) throws Exception {
        // NODEID_2 DB_ACTION KEY_VAL KEY_ELEMENT_RELATIVE_XPATH NID_SCHEME
        if (attribName.equals(EngineMDCUtils.NODEID_2) || attribName.equals(EngineMDCUtils.NID_SCHEME) || attribName.equals(EngineMDCUtils.PARSE_2) || attribName.equals(EngineMDCUtils.DB_ACTION) || attribName.equals(EngineMDCUtils.KEY_ELEMENT_RELATIVE_XPATH) || attribName.equals(EngineMDCUtils.KEY_VAL) || attribName.equals(EngineMDCUtils.NODEID) || attribName.equals(EngineMDCUtils.PARSE) || attribName.equals(EngineMDCUtils.VERSION) || attribName.equals(EngineMDCUtils.PERSISTENCE)) {
            return true;
        }
        
        return false;
    }
    
    public static boolean copyFreeChilds(Element toEL, Element fromEL) throws Exception {
        boolean bUpdate = false;
        List<?> freeChildList = Dom4jUtils.listXpath(fromEL, "child::*[ not ( (@Parse='true') or (@Parse='1') ) ]");
        for (Iterator<?> k = freeChildList.iterator(); k.hasNext();) {
            Element childEL = (Element) k.next();
            // MRLog.println(MRLog.ERROR,"add EL " + childEL.getName());
            Dom4jUtils.addElement(toEL, (Element) childEL.clone());
            bUpdate = true;
        }
        return bUpdate;
        
    }
    
    public static void removeOldChildsAttributes(Element fromELRead) throws Exception {
        
        List<?> freeChildList = Dom4jUtils.listXpath(fromELRead, "child::*[ not ( (@Parse='true') or (@Parse='1') ) ]");
        for (Iterator<?> k = freeChildList.iterator(); k.hasNext();) {
            Element childEL = (Element) k.next();
            // MRLog.println(MRLog.ERROR,"remove EL " + childEL.getName());
            childEL.detach();
        }
        List<String> namesAtrb = new ArrayList<String>();
        List<?> attrList = Dom4jUtils.getAttributeList(fromELRead);
        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
            Attribute attrEL = (Attribute) i.next();
            namesAtrb.add(attrEL.getName());
        }
        for (Iterator<String> i = namesAtrb.iterator(); i.hasNext();) {
            String attr = i.next();
            // MRLog.println(MRLog.ERROR,"remove Attrb " + attr);
            Dom4jUtils.addAttribute(fromELRead, attr, "");
        }
    }
    
    public static boolean hasNewUpdates(Element fromELReq) throws Exception {
        boolean bUpdate = false;
        try {
            List<?> childReqList = fromELReq.selectNodes("child::*[ not ( (@Parse='true') or (@Parse='1') ) ]");
            if (childReqList.size() > 0) {
                bUpdate = true;
            }
            if (!bUpdate) {
                List<?> attrList = Dom4jUtils.getAttributeList(fromELReq);
                for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
                    Attribute attrEL = (Attribute) i.next();
                    String name = attrEL.getName().trim();
                    // String val = attrEL.getValue();
                    if (!EngineMDCUtils.isSchemaAttrib(name)) {
                        bUpdate = true;
                        break;
                    }
                }
            }
            return bUpdate;
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return false;
    }
    
    public static void copyChilds(Element fromELRead, Element fromELReq) throws Exception {
        try {
            List<?> childReqList = fromELReq.selectNodes("child::*[(@Parse='true') or (@Parse='1')]");
            for (Iterator<?> i = childReqList.iterator(); i.hasNext();) {
                Element elementReqChild = (Element) i.next();
                String elementName = elementReqChild.getName();
                String NIDReq = Dom4jUtils.getAttributeValue(elementReqChild, EngineMDCUtils.NODEID_2, "");
                String xpath = elementName + "[@NID='" + NIDReq + "']";
                // MRLog.println(MRLog.ERROR,"\n/////////////////////////"+
                // elementName + " ///////////////////////////");
                // MRLog.println(MRLog.ERROR,elementName + " copy EL " + xpath);
                // boolean bUpdate = false;
                // Element elementReadChild = MRXMLUtils.element(toEL,
                // elementName);
                // TODO: check performance
                Element elementReadChild = Dom4jUtils.elementXpath(fromELRead, xpath);
                if (elementReadChild == null) {
                    Dom4jUtils.addElement(fromELRead, (Element) elementReqChild.detach());
                } else {
                    // first check in thing to update in current request
                    // if yes remove all free childs and attributes and add from
                    // request
                    
                    // replaceIPVSTempNode(elementReadChild, elementReqChild) ;
                    if (EngineMDCUtils.hasNewUpdates(elementReqChild)) {
                        EngineMDCUtils.removeOldChildsAttributes(elementReadChild);
                        if (EngineMDCUtils.copyFreeChilds(elementReadChild, elementReqChild)) {
                            // bUpdate = true;
                        }
                        if (EngineMDCUtils.copyAttributes_checkNew(elementReadChild, elementReqChild)) {
                            // bUpdate = true;
                        }
                    }
                    EngineMDCUtils.copyChilds(elementReadChild, elementReqChild);
                }
                // MRLog.println(MRLog.ERROR,"/////////////////////////"+
                // elementName + " ///////////////////////////");
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    public static void copyChilds(Element fromELRead, Element fromELReq, BooleanState updateState) throws Exception {
        try {
            List<?> childReqList = fromELReq.selectNodes("child::*[(@Parse='true') or (@Parse='1')]");
            for (Iterator<?> i = childReqList.iterator(); i.hasNext();) {
                Element elementReqChild = (Element) i.next();
                String elementName = elementReqChild.getName();
                String NIDReq = Dom4jUtils.getAttributeValue(elementReqChild, EngineMDCUtils.NODEID_2, "");
                String xpath = elementName + "[@NID='" + NIDReq + "']";
                // MRLog.println(MRLog.ERROR,"\n/////////////////////////"+
                // elementName + " ///////////////////////////");
                // MRLog.println(MRLog.ERROR,elementName + " copy EL " + xpath);
                // boolean bUpdate = false;
                // Element elementReadChild = MRXMLUtils.element(toEL,
                // elementName);
                // TODO: check performance
                Element elementReadChild = Dom4jUtils.elementXpath(fromELRead, xpath);
                if (elementReadChild == null) {
                    Dom4jUtils.addElement(fromELRead, (Element) elementReqChild.detach());
                    updateState.setDifferent(true);
                } else {
                    // first check in thing to update in current request
                    // if yes remove all free childs and attributes and add from
                    // request
                    
                    // replaceIPVSTempNode(elementReadChild, elementReqChild) ;
                    if (EngineMDCUtils.hasNewUpdates(elementReqChild)) {
                        updateState.setDifferent(true);
                        EngineMDCUtils.removeOldChildsAttributes(elementReadChild);
                        if (EngineMDCUtils.copyFreeChilds(elementReadChild, elementReqChild)) {
                            // bUpdate = true;
                        }
                        if (EngineMDCUtils.copyAttributes_checkNew(elementReadChild, elementReqChild)) {
                            // bUpdate = true;
                        }
                    }
                    EngineMDCUtils.copyChilds(elementReadChild, elementReqChild, updateState);
                }
                // MRLog.println(MRLog.ERROR,"/////////////////////////"+
                // elementName + " ///////////////////////////");
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    public static void assertMerge(Element toEL, Element fromEL, BooleanAssert updateState) throws Exception {
        List<?> childListFrom = Dom4jUtils.getList(fromEL);
        List<?> childListTo = Dom4jUtils.getList(toEL);
        if (childListFrom.size() != childListTo.size()) {
            updateState.setDifferent(true);
            // MRLog.println("number of children are not same toEL "+
            // toEL.asXML() + " fromEL "+ fromEL.asXML() );
            return;
        }
        String valueTo = Dom4jUtils.value(toEL, "").trim();
        String valueFrom = Dom4jUtils.value(fromEL, "").trim();
        if (!valueTo.equals(valueFrom)) {
            updateState.setDifferent(true);
            // MRLog.println("value not same toEL "+ toEL.asXML() + " fromEL "+
            // fromEL.asXML() );
            return;
        }
        for (Iterator<?> i = childListTo.iterator(); i.hasNext();) {
            Element toChildEL = (Element) i.next();
            String name = toChildEL.getName();
            Element fromChildEL = Dom4jUtils.element(fromEL, name);
            if (fromChildEL != null) {
                // compare attributes
                List<?> fromChildELAttrList = Dom4jUtils.getAttributeList(fromChildEL);
                List<?> toChildELAttrList = Dom4jUtils.getAttributeList(toChildEL);
                if (fromChildELAttrList.size() != toChildELAttrList.size()) {
                    updateState.setDifferent(true);
                    // MRLog.println(
                    // "number of attributes are not same toChildEL "+
                    // toChildEL.asXML() + " fromChildEL "+ fromChildEL );
                    return;
                }
                for (Iterator<?> k = toChildELAttrList.iterator(); k.hasNext();) {
                    Attribute attrEL = (Attribute) k.next();
                    String nameAttrTo = attrEL.getName().trim();
                    String valAttrTo = attrEL.getValue();
                    String valAttrFrom = Dom4jUtils.getAttributeValue(fromChildEL, nameAttrTo, "").trim();
                    if (!valAttrFrom.equals(valAttrTo.trim())) {
                        updateState.setDifferent(true);
                        // MRLog.println("attribute '"+nameAttrTo+
                        // "' has different value toChildEL "+ valAttrTo +
                        // " fromChildEL "+ valAttrFrom );
                        return;
                    }
                }
                EngineMDCUtils.assertMerge(toChildEL, fromChildEL, updateState);
            } else {
                updateState.setDifferent(true);
                // MRLog.println("child not found '"+toChildEL.getName()+
                // "' in fromChildEL "+ fromEL );
                return;
            }
            
        }
    }
    
    public static final String NID_ATTRB = "NID";
    
    public static String removeAttribute(String record) {
        if (record.indexOf(EngineMDCUtils.NID_ATTRB + "=") != -1) {
            record = record.replace('\"', ' ').replace('\'', ' ').trim();
            
            String[] arr = record.split(EngineMDCUtils.NID_ATTRB + "=");
            return arr[1].trim();
        }
        return record;
    }
    
    public static Element getDiscoItem(String name, String node, String jid, String action) {
        Element element = DocumentHelper.createElement("item");
        element.addAttribute("jid", jid);
        if (action != null) {
            element.addAttribute("action", action);
        }
        if (name != null) {
            element.addAttribute("name", name);
        }
        if (node != null) {
            element.addAttribute("node", node);
        }
        return element;
    }
    
    public static Element discoReplyChild(String node) {
        Element replyChild = DocumentHelper.createElement(QName.get("query", "http://jabber.org/protocol/disco#items"));
        Dom4jUtils.addAttribute(replyChild, "node", node);
        return replyChild;
    }
    
    public static ArrayList<String> getXpathArray(Element queryElement) {
        ArrayList<String> array = new ArrayList<String>();
        try {
            Element el = Dom4jUtils.element(queryElement, RequestIQConst.XPATH_LIST);
            if (el == null) {
                return array;
            }
            List<?> list = Dom4jUtils.getList(el, RequestIQConst.XPATH);
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Element childEL = (Element) i.next();
                String value = childEL.getText();
                if (value != null && !value.trim().equals("")) {
                    array.add(value.trim());
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return array;
    }
    
    public static Element stringToElement(String xml) {
        try {
            if (xml == null || xml.equals("")) {
                return null;
            }
            Document doc = Dom4jUtils.stringToDocument(xml);
            return doc.getRootElement();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public static class BooleanAssert {
        private boolean isDifferent = false;
        
        public boolean isDifferent() {
            return isDifferent;
        }
        
        public void setDifferent(boolean isDifferent) {
            this.isDifferent = isDifferent;
        }
    }
    
    public static class BooleanState {
        private boolean isDifferent = false;
        
        public boolean isDifferent() {
            return isDifferent;
        }
        
        public void setDifferent(boolean isDifferent) {
            this.isDifferent = isDifferent;
        }
    }
    
}
