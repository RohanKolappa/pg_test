package com.ipvs.common.db;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;

public class XmlRepresentation {
    private Document doc = null;
    
    public XmlRepresentation() {}
    
    public Document getDoc() {
        return doc;
    }
   /* public static void main (String[] args) {
         
        try {
            XmlRepresentation obj = new XmlRepresentation();
            obj.setDoc(MRXMLUtils.loadXMLResource(MRConst.MRSCHEMAPATH, MRLog.getInstance(MRLog.CONSOLE)));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }*/
    
    public void setDoc(Document doc) {
        this.doc = doc;
        try {
            //System.out.println("removeAdditionalNameSpace 1 " + MRXMLUtils.documentToString(doc));
            removeAdditionalNameSpace();
            //System.out.println("removeAdditionalNameSpace 2 " + MRXMLUtils.documentToString(doc));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    private void removeAdditionalNameSpace() {
        try {
            Element root = doc.getRootElement();
            List<?> list = root.additionalNamespaces();
            for (Iterator<?> i = list.iterator(); i.hasNext();) {
                Namespace additionalNamespace = (Namespace) i.next();
                //todo: thisn one is removed from objectxml already
                //root.removeAttribute("noNamespaceSchemaLocation", additionalNamespace);
                //root.removeNamespaceDeclaration(additionalNamespace);
                try{
                    System.out.println("removeAdditionalNameSpace  getName " + doc.getRootElement().getName() + " " + additionalNamespace.asXML());
                }catch(Exception ex){
                    ex.printStackTrace();
                }
                try{
                    root.addAttribute("noNamespaceSchemaLocation", null);
                }catch(Exception ex){
                    ex.printStackTrace();
                }
                try{
                    root.remove(additionalNamespace);
                }catch(Exception ex){
                    ex.printStackTrace();
                }
                try{
                    //root.removeNamespaceDeclaration(additionalNamespace);
                    additionalNamespace.detach();
                }catch(Exception ex){
                    ex.printStackTrace();
                }
            }
            
        } catch (Exception ex) {

        }
    }
    
    //    public Document getElementFromXpath(String xpath, MRLog log) throws Exception {
    //        Document retDoc = null;
    //        String[] xpathArr = { xpath };
    //        int copyChildsToDepth = -1;
    //        return getElementFromXpath(copyChildsToDepth, retDoc, xpathArr, log);
    //    }
    //    
    //    public Document getElementFromXpath(Document retDoc, String xpath, MRLog log) throws Exception {
    //        String[] xpathArr = { xpath };
    //        int copyChildsToDepth = -1;
    //        return getElementFromXpath(copyChildsToDepth, retDoc, xpathArr, log);
    //    }
    //    
    //    public Document getElementFromXpath(Document retDoc, String[] xpathArr, MRLog log) throws Exception {
    //        int copyChildsToDepth = -1;
    //        return getElementFromXpath(copyChildsToDepth, retDoc, xpathArr, log);
    //    }
    //    
    //    public Document getElementFromXpath(String[] xpathArr, MRLog log) throws Exception {
    //        Document retDoc = null;
    //        int copyChildsToDepth = -1;
    //        return getElementFromXpath(copyChildsToDepth, retDoc, xpathArr, log);
    //    }
    
    public Document getElementFromXpath(int copyChildsToDepth, Document retDoc, String xpath, MRLog log) throws Exception {
        String[] xpathArr = { xpath };
        return getElementFromXpath(copyChildsToDepth, retDoc, xpathArr, log);
    }
    
    public Document getElementFromXpath(int copyChildsToDepth, Document retDoc, String[] xpathArr, MRLog log) throws Exception {
        for (int i = 0; i < xpathArr.length; i++) {
            List<?> childList = MRXMLUtils.getListXpath(doc, xpathArr[i]);
            if (childList == null || childList.isEmpty()) {
                System.out.println("ERROR NOT found any thing for xpathArr[i] " + xpathArr[i] + "///////////////////////////////////////");
            } else {
                for (Iterator<?> k = childList.iterator(); k.hasNext();) {
                    Element xpathEL = (Element) k.next();
                    //  System.out.println(xpathArr[i] + " xpathEL " + MRXMLUtils.elementToString(xpathEL));
                    retDoc = resolveParents(copyChildsToDepth, retDoc, xpathEL, xpathArr[i], log);
                }
            }
        }
        return retDoc;
    }
    
    private Document resolveParents(int copyChildsToDepth, Document retDoc, Element xpathEL, String xpath, MRLog log) throws Exception {
        Element retEL = null;
        if (retDoc == null) retDoc = MRXMLUtils.getDocument();
        if (retDoc.getRootElement() ==null) {
            retEL = resolveParents_copyElement(retEL, doc.getRootElement(), copyChildsToDepth, retDoc, xpathEL, xpath, log);
        } else {
            retEL = retDoc.getRootElement();
        }
        
        ArrayList<Element> arr = new ArrayList<Element>();
        boolean bHasParent = true;
        arr.add(xpathEL);
        Element parent = MRXMLUtils.getParentElement(xpathEL);
        if (parent != null) {
            do {
                try {
                    if (parent == null || parent.isRootElement()) {
                        bHasParent = false;
                    } else {
                        arr.add(parent);
                        parent = MRXMLUtils.getParentElement(parent);
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            } while (bHasParent);
            
            // Element parent = (Element)parentEL.clone();
            for (int i = arr.size() - 1; i > -1; i--) {
                Element EL = arr.get(i);
                retEL = resolveParents_copyElement(retEL, EL, copyChildsToDepth, retDoc, xpathEL, xpath, log);
            }
        }
        
        return retDoc;
    }
    
    private Element resolveParents_copyElement(Element toEL, Element fromEL, int copyChildsToDepth, Document retDoc, Element xpathEL, String xpath, MRLog log) throws Exception {
        //Element EL =  doc.getRootElement();
        Element toChildEL = null;
        String name = fromEL.getName();
        if (toEL != null) {
            toChildEL = MRXMLUtils.getElement(toEL, name);
        }
        if (fromEL == xpathEL) {
            if (copyChildsToDepth == -1) {
                toChildEL = (Element) xpathEL.clone();
                if (toEL == null)
                    MRXMLUtils.addElement(retDoc, toChildEL);
                else
                    MRXMLUtils.addElement(toEL, toChildEL);
            } else if (copyChildsToDepth >= 1) {
                toChildEL = MRXMLUtils.newElement(name);
                if (toEL == null)
                    MRXMLUtils.addElement(retDoc, toChildEL);
                else
                    MRXMLUtils.addElement(toEL, toChildEL);
                copyAttributes(toChildEL, fromEL, log);
                if (copyChildsToDepth > 1) {
                    copyChildsUptoDepth(toChildEL, fromEL, copyChildsToDepth, (copyChildsToDepth - 1), log);
                }
            }
            //return retDoc;
        } else if (toChildEL == null) {
            toChildEL = MRXMLUtils.newElement(name);
            if (toEL == null)
                MRXMLUtils.addElement(retDoc, toChildEL);
            else
                MRXMLUtils.addElement(toEL, toChildEL);
            copyAttributes(toChildEL, fromEL, log);
            
        }
        
        return toChildEL;
    }
    
    private void copyAttributes(Element toEL, Element fromEL, MRLog log) throws Exception {
        List<?> attrList = MRXMLUtils.getAttributeList(fromEL);
        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
            Attribute attrEL = (Attribute) i.next();
            String name = attrEL.getName();
            String val = attrEL.getValue();
            String val2 = MRXMLUtils.getAttributeValue(toEL, name, "").trim();
            if (name.equals("Parse")) val2 = "";
            
            if (val2.equals("")) {
                if (name.equals("NID")) val = "";
                MRXMLUtils.addAttribute(toEL, name, val);
            }
        }
    }
    
    private void copyChildsUptoDepth(Element toEL, Element fromEL, int ParentDepth, int CurrentDepth, MRLog log) throws Exception {
        try {
            if (CurrentDepth <= 0) {
                return;
            }
            CurrentDepth = CurrentDepth - 1;
            List<?> childList = MRXMLUtils.getList(fromEL);
            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                Element childFromEL = (Element) i.next();
                String name = childFromEL.getName();
                Element childToEL = MRXMLUtils.getElement(toEL, name);
                if (childToEL == null) {
                    childToEL = MRXMLUtils.newElement(name);
                }
                MRXMLUtils.addElement(toEL, childToEL);
                copyAttributes(childToEL, childFromEL, log);
                
                if (CurrentDepth <= 0) {
                    //skip; break this loop here 
                } else {
                    copyChildsUptoDepth(childToEL, childFromEL, ParentDepth, CurrentDepth, log);
                }
                
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    public void patchNID(Element toEL, MRLog log) throws Exception {
        if (doc == null) {
            throw new MRException(DBExceptionCodes.INVALID_PATCH_DEFAULT_VALUE_XML_URI, "<error>" + "patchNID : XmlRepresentation is null or not valid. " + "</error>");
        }
        Element fromEL = doc.getRootElement();
        if (fromEL == null || toEL == null) {
            throw new MRException(DBExceptionCodes.INVALID_PATCH_VALUES, "<error>" + "patchNID Elements are null or not valid. " + "</error>");
        }
        String fromELName = fromEL.getName();
        String toELName = toEL.getName();
        if (!fromELName.equals(toELName)) {
            throw new MRException(DBExceptionCodes.INVALID_PATCH_VALUES, "<error>" + "patchNID Elements are not valid. " + "</error>");
        }
        copyAttributes(toEL, fromEL, log);
    }
    
    public boolean isRootNIDKeyVal() {
        return false;
    }
    
    public String getDBObjectName() {
        return doc.getRootElement().getName();
    }
    
    //
    //
    //
    //
    //
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //    //////////////////////////////////////  to be removed ////////////////////////////////////////////////
    //
    //
    //
    //    public Element getOperationData_default(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Element fragmentEL = MRXMLUtils.newElement(getDoc().getRootElement().getName());
    //        patchNID(fragmentEL, getDoc().getRootElement(), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        return fragmentEL;
    //
    //    }
    //     
    //    /*private void copyAttributes(Element toEL, Element fromEL , MRLog log ) throws Exception {
    //        List<?> attrList = MRXMLUtils.getAttributeList(fromEL);
    //        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
    //            Attribute attrEL = (Attribute) i.next();
    //            String name = attrEL.getName();
    //            String val = attrEL.getValue();
    //            String val2 = MRXMLUtils.getAttributeValue(toEL, name, "").trim();
    //            if(name.equals("Parse"))  val2= ""; 
    //            
    //            if (val2.equals("")) {
    //                if(name.equals("NID"))  val= ""; 
    //                MRXMLUtils.addAttribute(toEL, name, val);
    //            }
    //        }
    //    }*/
    //    private void copyFreeChilds(Element toEL, Element fromEL, MRLog log) throws Exception {
    //        List<?> attrList = MRXMLUtils.getList(fromEL);
    //        boolean bFirstChild=true;
    //        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
    //            Element fromChildEL = (Element) i.next();
    //            String name = fromChildEL.getName();
    //            boolean parse = IpvsDBUtils.getParseAttrb(null, log, fromChildEL);
    //            if (!parse) {
    //                if(bFirstChild){
    //                    Element toChildEL = MRXMLUtils.getElement(toEL, name);
    //                    if(toChildEL !=null)
    //                        return;
    //                    bFirstChild=false;
    //                }
    //                MRXMLUtils.addElement(toEL, (Element)fromChildEL.clone()) ;
    //            }
    //            
    //        }
    //    }
    //    // /////////////////////////// patchNID /////////////////////////
    //    public void patchNID(Element toEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        if (doc == null) {
    //            throw new MRException(DBExceptionCodes.INVALID_PATCH_DEFAULT_VALUE_XML_URI,"<error>"+ "patchNID : XmlRepresentation is null or not valid. "+"</error>");
    //        }
    //        Element fromEL = doc.getRootElement();
    //        if (fromEL == null || toEL == null) {
    //            throw new MRException(DBExceptionCodes.INVALID_PATCH_VALUES, "<error>"+"patchNID Elements are null or not valid. "+"</error>");
    //        }
    //        String fromELName = fromEL.getName();
    //        String toELName = toEL.getName();
    //        if (!fromELName.equals(toELName)) {
    //            throw new MRException(DBExceptionCodes.INVALID_PATCH_VALUES,"<error>"+ "patchNID Elements are not valid. "+"</error>");
    //        }
    //        
    //        boolean parse = IpvsDBUtils.getParseAttrb(request, log, fromEL);
    //        if (parse)
    //            patchNID(toEL, fromEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //    }
    //
    //    public void patchNID(Element toEL, Element fromEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        copyAttributes(toEL, fromEL,log);
    //        List<?> childList = MRXMLUtils.getList(toEL);
    //        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //            Element toChildEL = (Element) i.next();
    //            String name = toChildEL.getName();
    //            Element fromChildEL = MRXMLUtils.getElement(fromEL, name);
    //            if (fromChildEL != null) {
    //                boolean parse = IpvsDBUtils.getParseAttrb(request, log, fromChildEL);
    //                if (parse) {
    //                        patchNID(toChildEL, fromChildEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                }
    //            }
    //        }
    //    }
    //
    //    // /////////////////////////// patchNID /////////////////////////
    //
    //    // /////////////////////////// fragmentEL /////////////////////////
    //    public String getFragXpath(Element selectParentEL, Element fragmentEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Element fromEL = getDoc().getRootElement();
    //        StringBuffer buf = new StringBuffer();
    //        boolean bFirst = true;
    //        Element fromChildEL = null;
    //        Element toEL = (Element) selectParentEL.clone();
    //        String toELName = "";
    //        while (true) {
    //            toELName = toEL.getName();
    //            //System.out.println(" toELName " + toELName);
    //            if (bFirst) {
    //                buf.append("//" + toELName);
    //                bFirst = false;
    //            } else {
    //                buf.append("/" + toELName);
    //            }
    //            boolean parse = IpvsDBUtils.getParseAttrb(request, log, fromEL);
    //            if (!parse)
    //                break;
    //            List<?> childList = MRXMLUtils.getList(toEL);
    //            if (childList.isEmpty())
    //                break;
    //            boolean parseChild = false;
    //            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //                Element toChildEL = (Element) i.next();
    //                toELName = toChildEL.getName();
    //                fromChildEL = MRXMLUtils.getElement(fromEL, toELName);
    //                if (fromChildEL == null) {
    //                    throw new MRException(DBExceptionCodes.XML_IS_NOT_VALID, "<error>"+"SelectXML is not valid"+"</error>");
    //                }
    //                parseChild = IpvsDBUtils.getParseAttrb(request, log, fromChildEL);
    //                if (parseChild) {
    //                    toEL = toChildEL;
    //                    fromEL = fromChildEL;
    //                    break;
    //                }
    //            }
    //            if (!parseChild)
    //                break;
    //        }
    //        if(fragmentEL !=null){
    //            toELName = fragmentEL.getName();
    //            if (bFirst) {
    //                buf.append("//" + toELName);
    //                bFirst = false;
    //            } else {
    //                buf.append("/" + toELName);
    //            }
    //        }
    //        //System.out.println("/////////////////// getFragXpath :" + buf.toString());
    //        return buf.toString();
    //    }
    //
    //    public String patchFragmentNID(Element selectXML, Element fragmentEL, String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        String xPath = "";
    //        Element fromEL = null;
    //        if (selectXML == null ){
    //            if ( fragmentEL == null) {
    //                fragmentEL = MRXMLUtils.newElement(getDoc().getRootElement().getName());
    //                patchNID(fragmentEL, getDoc().getRootElement(), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                return "self"; //TBD
    //            }
    //            if (fragmentEL != null) {
    //                String fragXpath=  "//"+fragmentEL.getName();
    //                List<?> list = MRXMLUtils.getListXpath(doc,fragXpath);
    //                if(list.isEmpty() ){
    //                    throw new MRException(DBExceptionCodes.FRAG_XPATH,MRXMLUtils.generateErrorXML("fragXpath", fragXpath));
    //                }
    //                if(list.size() ==1){
    //                    patchNID(fragmentEL, (Element)list.get(0), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                    return fragXpath;
    //                }
    //                else{
    //                    throw new MRException(DBExceptionCodes.FRAG_XPATH, "<error>"+fragXpath + "  found multiple childs."+"</error>");
    //
    // 
    //                }
    //            }
    //        }
    //        if (fragmentEL == null) {
    //            //throw new MRException(1, "patchFragmentNID : selectParentEL/ fragmentEL are null or not valid. ");
    //            fragmentEL = selectXML;
    //            patchNID(fragmentEL, getDoc().getRootElement(), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //            return "self"; //TBD
    //        }
    //
    //        xPath = getFragXpath(selectXML, fragmentEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        fromEL = MRXMLUtils.getElementXpath(getDoc(), xPath);
    //        if (fromEL == null) {
    //            throw new MRException(DBExceptionCodes.PATCH_FRAGMENT_NID, "<error>"+"patchFragmentNID : fragmentEL is null or not valid. "+"</error>");
    //        }
    //        patchNID(fragmentEL, fromEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        return xPath;
    //    }
    //
    //    public String prefixParents(Element selectXML, Element fragmentEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        if (selectXML == null ){
    //            if ( fragmentEL == null) {
    //                fragmentEL = MRXMLUtils.newElement(getDoc().getRootElement().getName());
    //                patchNID(fragmentEL, getDoc().getRootElement(), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                return "self"; //TBD
    //            }
    //            if (fragmentEL != null) {
    //                String fragXpath=  "//"+fragmentEL.getName();
    //                List<?> list = MRXMLUtils.getListXpath(doc,fragXpath);
    //                if(list.isEmpty() ){
    //                    throw new MRException(DBExceptionCodes.FRAG_XPATH,MRXMLUtils.generateErrorXML("fragXpath", fragXpath));
    //                }
    //                if(list.size() ==1){
    //                    patchNID(fragmentEL, (Element)list.get(0), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                    return fragXpath;
    //                }
    //            }
    //        }
    //        String xpath = null;
    //        if(selectXML!=null) {
    //            xpath = getFragXpath(selectXML, fragmentEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        }   
    //        patchNID(selectXML, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        attachChild(selectXML, fragmentEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        patchNID(selectXML, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //        return xpath;
    //    }
    //
    //    private void attachChild(Element selectParentEL, Element fragmentEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //
    //        //String name = selectParentEL.getName();
    //        //System.out.println("attachChild  name " + name);
    //        List<?> childList = MRXMLUtils.getList(selectParentEL);
    //        if (childList.isEmpty()) {
    //            selectParentEL.add((Element) fragmentEL.clone());
    //            return;
    //        }
    //        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //            Element selectChildEL = (Element) i.next();
    //            boolean parse = IpvsDBUtils.getParseAttrb(request, log, selectChildEL);
    //            if (parse) {
    //                attachChild(selectChildEL, fragmentEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //            }
    //        }
    //    }
    //
    //    // /////////////////////////// fragmentEL /////////////////////////
    //    // /////////////////////////// getElementFromXpath /////////////////////////
    //    public Document getFragElementFromXpath(String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Document retDoc = null;
    //        Element xpathEL = MRXMLUtils.getElementXpath(doc, xpath);
    //        if(xpathEL !=null){
    //            retDoc = MRXMLUtils.getDocument();
    //            MRXMLUtils.addElement(retDoc, (Element)xpathEL.clone());
    //        }
    //        return retDoc;
    //    }
    //    public Document getFragParentElementFromXpath(String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Document retDoc = null;
    //        Element xpathEL = MRXMLUtils.getElementXpath(doc, xpath);
    //        if(xpathEL !=null){
    //            retDoc = MRXMLUtils.getDocument();
    //            xpathEL = xpathEL.getParentElement();
    //            MRXMLUtils.addElement(retDoc, (Element)xpathEL.clone());
    //        }
    //        return retDoc;
    //    }   
    //    ///////////////////////////////////////getElementFromXpath/////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    public Document getElementFromXpath(String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Document retDoc = null;
    //        String[] xpathArr = {xpath};
    //        boolean bCopyFreeChilds=true;
    //        int copyChildsToDepth =1;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //
    //    public Document getElementFromXpath(Document retDoc, String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        String[] xpathArr = {xpath};
    //        boolean bCopyFreeChilds=true;
    //        int copyChildsToDepth =1;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //    public Document getElementFromXpath(Document retDoc,String[] xpathArr, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //                boolean bCopyFreeChilds=true;
    //        int copyChildsToDepth =1;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //    public Document getElementFromXpath(String[] xpathArr, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Document retDoc = null;
    //        boolean bCopyFreeChilds=true;
    //        int copyChildsToDepth =1;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //    ////////////////////new boolean bCopyFreeChilds , int copyChildsToDepth 
    //    public Document getElementFromXpath(boolean bCopyFreeChilds , int copyChildsToDepth , String  xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        String[] xpathArr = {xpath};
    //        Document retDoc = null;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //
    //
    //    public Document getElementFromXpath(boolean bCopyFreeChilds , int copyChildsToDepth , String[] xpathArr, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Document retDoc = null;
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //    
    //    public Document getElementFromXpath(boolean bCopyFreeChilds , int copyChildsToDepth , Document retDoc, String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        String[] xpathArr = {xpath};
    //        return getElementFromXpath( bCopyFreeChilds , copyChildsToDepth , retDoc, xpathArr, request, xmpp, log, schema, resourceFactory,  serviceAgentFactory);
    //    }
    //
    //
    //    public Document getElementFromXpath(boolean bCopyFreeChilds , int copyChildsToDepth , Document retDoc, String[] xpathArr, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        for (int i = 0; i < xpathArr.length; i++) {
    //            List<?> childList = MRXMLUtils.getListXpath(doc, xpathArr[i]);
    //            if(childList==null || childList.isEmpty() ){
    //                System.out.println("ERROR NOT found any thing for xpathArr[i] " + xpathArr[i] +"///////////////////////////////////////");
    //            }
    //            else {
    //                for (Iterator<?> k = childList.iterator(); k.hasNext();) {
    //                    Element xpathEL = (Element) k.next();
    //                //  System.out.println(xpathArr[i] + " xpathEL " + MRXMLUtils.elementToString(xpathEL));
    //                    retDoc = resolveParents(bCopyFreeChilds, copyChildsToDepth, retDoc, xpathEL, xpathArr[i], request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    //                }
    //            }   
    //        }
    //        return retDoc;
    //    }
    //    private Element resolveParents_copyElement (Element retEL, Element EL , boolean bCopyFreeChilds , int copyChildsToDepth ,
    //            Document retDoc, Element xpathEL, String xpath, MRLog log ) throws Exception {
    //        //Element EL =  doc.getRootElement();
    //        Element curEL =null;
    //        String name = EL.getName();
    //        if(retEL!=null)
    //            curEL = MRXMLUtils.getElement(retEL, name);
    //        if (EL == xpathEL) {
    //            if(copyChildsToDepth == -1 ) {
    //                curEL = (Element)xpathEL.clone();
    //                if(retEL==null)
    //                    MRXMLUtils.addElement(retDoc, curEL);
    //                else
    //                    MRXMLUtils.addElement(retEL, curEL);
    //            } else if(copyChildsToDepth >= 1 ) {  
    //                curEL = MRXMLUtils.newElement(name);
    //                if(retEL==null)
    //                    MRXMLUtils.addElement(retDoc, curEL);
    //                else
    //                    MRXMLUtils.addElement(retEL, curEL);
    //                copyAttributes(curEL, EL,log);
    //                if(bCopyFreeChilds){
    //                    copyFreeChilds(curEL, EL,log);
    //                }
    //                if(copyChildsToDepth > 1 ) 
    //                    copyChildsUptoDepth(curEL, EL,bCopyFreeChilds,  copyChildsToDepth, (copyChildsToDepth-1),log);
    //            }
    //            //return retDoc;
    //        } else if (curEL == null) {
    //            curEL = MRXMLUtils.newElement(name);
    //            if(retEL==null)
    //                MRXMLUtils.addElement(retDoc, curEL);
    //            else
    //                MRXMLUtils.addElement(retEL, curEL);
    //            copyAttributes(curEL, EL,log);
    //            if(bCopyFreeChilds){
    //                copyFreeChilds(curEL, EL,log);
    //            }
    //        }
    //         
    //       return curEL;
    //    }
    //    private Document resolveParents(boolean bCopyFreeChilds , int copyChildsToDepth ,Document retDoc, Element xpathEL, String xpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //        Element retEL = null;
    //        if ( retDoc == null) 
    //            retDoc = MRXMLUtils.getDocument();
    //        if ( !retDoc.hasRootElement() ) {
    //            retEL = resolveParents_copyElement (retEL, doc.getRootElement() , bCopyFreeChilds , copyChildsToDepth ,
    //                    retDoc, xpathEL, xpath,  log );
    //        } else {
    //            retEL = retDoc.getRootElement();
    //        }
    //
    //        ArrayList<Element> arr = new ArrayList<Element>();
    //        boolean bHasParent = true;
    //        arr.add(xpathEL);
    //        Element parent =xpathEL.getParentElement();
    //        if(parent!=null){
    //            do {
    //                try{
    //                    if (parent==null ||  parent.isRootElement() ) {
    //                        bHasParent = false;
    //                    } else {
    //                        arr.add(parent);
    //                        parent = parent.getParentElement();
    //                    }
    //                }catch(Exception ex){
    //                    ex.printStackTrace();
    //                }
    //            } while (bHasParent);
    //    
    //            // Element parent = (Element)parentEL.clone();
    //            for (int i = arr.size() - 1; i > -1; i--) {
    //                Element EL = (Element) arr.get(i);
    //                retEL = resolveParents_copyElement (retEL, EL , bCopyFreeChilds , copyChildsToDepth ,
    //                        retDoc, xpathEL, xpath,  log );
    //            }
    //        }
    // 
    //        return retDoc;
    //    }
    // 
    //    private void copyChildsUptoDepth( Element toEL, Element fromEL, boolean bCopyFreeChilds, int ParentDepth, int CurrentDepth ,  MRLog log ) throws Exception {
    //        try{
    //            if(CurrentDepth <=0 ){
    //                return;
    //            }
    //            CurrentDepth = CurrentDepth-1;
    //            List<?> childList = MRXMLUtils.getList(fromEL);
    //            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //                Element childFromEL = (Element) i.next();
    //                String name = childFromEL.getName();
    //                Element childToEL = MRXMLUtils.getElement(toEL, name);
    //                if (childToEL == null) {
    //                    childToEL = MRXMLUtils.newElement(name);
    //                }
    //                MRXMLUtils.addElement(toEL, childToEL);
    //                copyAttributes(childToEL, childFromEL,log);
    //                //if(bCopyFreeChilds)
    //                //  copyFreeChilds( childToEL, childFromEL,log);
    //                boolean parse = IpvsDBUtils.getParseAttrb(null, log, childFromEL);
    //                if (parse) {
    //                    if(CurrentDepth <=0 ){
    //                        //skip;
    //                    }else{
    //                        copyChildsUptoDepth(childToEL, childFromEL, bCopyFreeChilds,  ParentDepth, CurrentDepth,log);
    //                    }
    //                }
    //                 
    //            }
    //        }catch(Exception ex ){
    //            ex.printStackTrace();
    //        }
    //    }   
    //    // /////////////////////////// getElementFromXpath /////////////////////////
    //
    //    public void removeSchemAttrbs(Element parentEL) throws Exception {
    //        
    //        if(parentEL==null) {
    //            return;
    //        }
    //        try{
    //            parentEL.removeAttribute("NID");
    //        }catch(Exception ex ){
    //            
    //        }
    //        try{
    //            parentEL.removeAttribute("Parse");
    //        }catch(Exception ex ){
    //            
    //        }
    //        try{
    //            parentEL.removeAttribute("KeyElementRelativeXPath");
    //        }catch(Exception ex ){
    //            
    //        }           
    //        try{
    //            parentEL.removeAttribute("NIDScheme");
    //        }catch(Exception ex ){
    //            
    //        } 
    //        try{
    //            parentEL.removeAttribute("noNamespaceSchemaLocation");
    //        }catch(Exception ex ){
    //            
    //        }           
    //        //System.out.println(bRemoved +" " + parentEL.getName());
    //        try{
    //            List<?> childList = MRXMLUtils.getList(parentEL);
    //            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //                Element chilValidatedEL = (Element) i.next();
    //                removeSchemAttrbs(chilValidatedEL)  ;        
    //            }
    //        }catch(Exception ex ){
    //            ex.printStackTrace();
    //        }
    //    }
    //    public int getMaxChildrenDepth(Element parentEL, MRLog log) throws Exception {
    //        return getMaxChildrenDepth( parentEL, 1, log);
    //    }
    //    public int getMaxChildrenDepth(Element parentEL, int ParentDepth, MRLog log) throws Exception {
    //        int nodeDepth=ParentDepth;
    //        try{
    //            List<?> childList = MRXMLUtils.getList(parentEL);
    //            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //                Element xpathEL = (Element) i.next();
    //                boolean parse = IpvsDBUtils.getParseAttrb(null, log, xpathEL);
    //                if (parse) {
    //                    int curNodeDepth = getMaxChildrenDepth(xpathEL,ParentDepth+1,log);
    //                    if(curNodeDepth>nodeDepth)
    //                        nodeDepth=curNodeDepth;
    //                }
    //            }
    //        }catch(Exception ex ){
    //            ex.printStackTrace();
    //        }
    //        return nodeDepth;
    //    }   
    //    public Element removeChildsUptoLevel(Element fromEL, int NodeDepth,MRLog log) throws Exception {
    //        Element toEL= new Element (fromEL.getName());
    //        copyAttributes(toEL, fromEL,log);
    //        int maxDepth = getMaxChildrenDepth( fromEL,1, log);
    //        int removeDepth=maxDepth-NodeDepth;
    //        //int removeDepth=NodeDepth-;
    //        if(removeDepth > 1 )
    //            removeChildsUptoLevel( fromEL, toEL, NodeDepth, removeDepth , log) ;
    //        else if(removeDepth <= 1)
    //            removeChildsUptoLevel( fromEL, toEL, NodeDepth, 1, log) ;
    //        return toEL;
    //    }
    //    public void removeChildsUptoLevel(Element fromEL, Element toEL,  int ParentDepth, int CurrentDepth, MRLog log) throws Exception {
    //        try{
    //            CurrentDepth = CurrentDepth-1;
    //            List<?> childList = MRXMLUtils.getList(fromEL);
    //            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
    //                Element childFromEL = (Element) i.next();
    //                Element childToEL = MRXMLUtils.addElement(toEL , childFromEL.getName());
    //                copyAttributes(childFromEL, childToEL,log);
    //                boolean parse = IpvsDBUtils.getParseAttrb(null, log, childFromEL);
    //                if (parse) {
    //                    if(CurrentDepth <=0 ){
    //                        //skip;
    //                    }else{
    //                        removeChildsUptoLevel( childFromEL, childToEL,  ParentDepth,  CurrentDepth,  log);
    //                    }
    //                }
    //            }
    //        }catch(Exception ex ){
    //            ex.printStackTrace();
    //        }
    //    }
    //    public void removeFreeChilds(Element fromEL, MRLog log) throws Exception {
    //        try {
    //
    //            List<?> freeChilds = getChildEL_PARSE(false , fromEL, log);
    //            for (Iterator<?> i = freeChilds.iterator(); i.hasNext();) {
    //                Element childFromEL = (Element) i.next();
    //                childFromEL.detach();
    //            }
    //            List<?> parseChilds = getChildEL_PARSE(true , fromEL, log);
    //            for (Iterator<?> i = parseChilds.iterator(); i.hasNext();) {
    //                Element childFromEL = (Element) i.next();
    //                removeAttributes( childFromEL ,  log);
    //            }
    //            for (Iterator<?> i = parseChilds.iterator(); i.hasNext();) {
    //                Element childFromEL = (Element) i.next();
    //                removeFreeChilds( childFromEL ,  log);
    //            }
    //            removeAttributes( fromEL ,  log);
    //
    //        }catch(Exception ex ){
    //            ex.printStackTrace();
    //        }
    //    }
    //    private void removeAttributes(Element fromEL, MRLog log) throws Exception {
    //        List<?> attrList = MRXMLUtils.getAttributeList(fromEL);
    //        Vector<String> vec = new Vector<String>();
    //        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
    //            Attribute attrEL = (Attribute) i.next();
    //            vec.add(attrEL.getName());
    //        }
    //        if(vec.size()>0){
    //            for (Iterator<?> i = vec.iterator(); i.hasNext();) {
    //                String attrName = (String ) i.next();
    //                boolean bRemoved = fromEL.removeAttribute( attrName);
    //                if(bRemoved ){
    //                    //System.out.println("aa");
    //                }
    //            }
    //        }
    //    }
    // 
    //      
    //    private static XPath xpath_parse_true  = null; 
    //    private static XPath xpath_parse_false = null; 
    //    
    //    private static XPath xpath_NID_CHILDS_TRUE = null; 
    //    private static XPath xpath_NID_CHILDS_FALSE = null; 
    // 
    //    public static List<?> getChildEL_PARSE(boolean parse , Element element, MRLog log) throws  Exception {
    //        // TODO: remove old method
    //        if(parse){
    //            if(xpath_parse_true ==null ) {
    //                xpath_parse_true = XPath.newInstance("child::*[@Parse='true']"); 
    //            }
    //            return xpath_parse_true.selectNodes(element);
    //        } else {
    //            if(xpath_parse_false ==null ) {
    //                xpath_parse_false = XPath.newInstance("child::*[ not ( @Parse='true') ]"); 
    //            }
    //            return xpath_parse_false.selectNodes(element);
    //        }
    //    }
    //    public static  List<?> getChildEL_AUTO(boolean bAuto, String parentNID, Element element,  MRLog log, MRDB db) throws Exception {
    //        if(bAuto){
    //            if( xpath_NID_CHILDS_TRUE == null ) {
    //                //String xpath_str = "child::*[ ( not  ( @NID='') and not (  @NID = 'string') ) or ( @NIDScheme='AUTO') ]";
    //                String xpath_str = "self::*[ ((@NID='') or (@NID='string') )  and ( @NIDScheme='AUTO' or  @NIDScheme='') ]";
    //                xpath_NID_CHILDS_TRUE = XPath.newInstance(xpath_str);//"self::*[ ( not ( @NID='') ) and  ( not ( @NID='string') ) ]"); 
    //            }       
    //            return xpath_NID_CHILDS_TRUE.selectNodes(element);
    //        } else {
    //            if( xpath_NID_CHILDS_FALSE == null ) {
    //                //String xpath_str = "child::*[ ( ( @NID='') or (  @NID = 'string') )  and ( @NIDScheme='AUTO')  )]";
    //                String xpath_str = "self::*[ not(((@NID='') or (@NID='string'))  and ( @NIDScheme='AUTO' or  @NIDScheme='')) ]";
    //                xpath_NID_CHILDS_FALSE = XPath.newInstance(xpath_str);//"self::*[ ( not ( @NID='') ) and  ( not ( @NID='string') ) ]"); 
    //            }       
    //            return xpath_NID_CHILDS_FALSE.selectNodes(element);
    //        }
    //    }   
    //    public static  Hashtable<String,Element> getChild_AUTO_ELNames(  String parentNID, Element element, MRLog log, MRDB db ) throws Exception {
    //         Hashtable<String,Element> list = new  Hashtable<String,Element>();
    //        List<?> attrList = getChildEL_AUTO(true, parentNID,  element,  log, db);
    //        for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
    //            Element attrEL = (Element) i.next();
    //            list.put(attrEL.getName(), attrEL);
    //        }
    //        return list;
    //        
    //    }   
    // 
    //    public static boolean isSchemaAttrib(String attribName) throws Exception {
    //        // NODEID_2 DB_ACTION KEY_VAL KEY_ELEMENT_RELATIVE_XPATH NID_SCHEME
    //        if (attribName.equals(IpvsDBUtils.NODEID_2) || attribName.equals(IpvsDBUtils.NID_SCHEME) || attribName.equals(IpvsDBUtils.PARSE_2) 
    //                || attribName.equals(IpvsDBUtils.DB_ACTION) || attribName.equals(IpvsDBUtils.KEY_ELEMENT_RELATIVE_XPATH) 
    //                || attribName.equals(IpvsDBUtils.KEY_VAL) || attribName.equals(IpvsDBUtils.NODEID) || attribName.equals(IpvsDBUtils.PARSE)) {
    //            return true;
    //        }
    //
    //        return false;
    //    }   
    //    public static boolean hasFree_Childs_Attrb(Element fromEL , MRLog log ) throws Exception {
    //        boolean bHasFreeChildsAttrb=false;  
    //        List<?> freeChilds = getChildEL_PARSE(false , fromEL, log);
    //        for (Iterator<?> i = freeChilds.iterator(); i.hasNext();) {
    //            //Element childFromEL = (Element) i.next();
    //            bHasFreeChildsAttrb=true;
    //        }
    //        if(!bHasFreeChildsAttrb){
    //            List<?> attrList = MRXMLUtils.getAttributeList(fromEL);
    //            //Vector<String> vec = new Vector<String>();
    //            for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
    //                Attribute attrEL = (Attribute) i.next();
    //                if(!isSchemaAttrib(attrEL.getName())){
    //                    bHasFreeChildsAttrb=true;
    //                    break;
    //                }
    //            }
    //        }
    //        return bHasFreeChildsAttrb;
    //    }
    
}
