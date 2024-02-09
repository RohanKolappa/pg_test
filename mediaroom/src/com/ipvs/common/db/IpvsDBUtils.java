package com.ipvs.common.db;

import java.net.URL;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class IpvsDBUtils {
    public static final String NODEID_2 = "NID";
    public static final String PARENT_NODEID = "parentNID";
    public static final int DEFAULT_NODE_DEPTH = 20;
    public static final Random wheel = new Random();
    
    public static MRException invalidContext( String action, String listNameContext, String objectTypeContext, String NIDContext){
        StringBuffer buf = new StringBuffer();
        buf.append(action+ " NID Context arguments= listName='"+listNameContext+"', ");
        buf.append("objectType='"+objectTypeContext+"', ");
        buf.append("NID='"+NIDContext+"', ");
        return  new MRException(DBExceptionCodes.INVALID_NID_CONTEXT, "<error><arguments>" + buf.toString() + "</arguments></error>");

    }
    public static String getUserJID(MRRequestI request, MRDB db) {
        try {
            MRRequest request2 = (MRRequest) request;
            return request2.getUserJID();
        } catch (Exception ex) {}
        
        return "";
    }
    public static NIDSchemeMDC resolveNID(MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID,
    		Element element, String childConextXpath, boolean isAdd, boolean isDocumentRootNode) throws Exception {
        if(NID==null){
            NID="";
        }else{
            NID = NID.trim();
        }
    	//if document NID, than  return
    	NIDSchemeMDC schema = new NIDSchemeMDC();
    	if(!NID.equals("")){
        	schema.parse(NID) ;
        	if(schema.getType()==NIDSchemeMDC.DOCUMENT){
        		return schema;
        	}
    	}
    	//if dataElement got attribute for document NID, than  return
    	if(element !=null){
	    	String docNID2 = MRXMLUtils.getAttributeValue(element, IpvsDBUtils.NODEID_2, "").trim();
	    	if(!docNID2.equals("")){
		        NIDSchemeMDC schema2 = new NIDSchemeMDC();
		        schema2.parse(docNID2 );
		    	if(schema2.getType()==NIDSchemeMDC.DOCUMENT){
					return schema2;
				}
	    	}
    	}
    	//if the argument is valid collectionNID
    	if(schema.getType()==NIDSchemeMDC.COLLECTION && !isAdd){
    		return schema;
    	}
    	//else if add - need to create a new docNID
    	if(isAdd && element !=null){
    		//create a new nid
    		NID = db.getServiceDomainRootNID(listName, objectType);
    		if(NID.trim().equals("")){
    			throw invalidContext("add", listName, objectType, NID);
            }
            if(isDocumentRootNode){
            	NID = IpvsDBUtils.parseNID_BULK(element, NID);
            }
        }else{//get list nid only
        	//if 
        	NID = db.getServiceDomainRootNID(listName, objectType);
    	}

    	schema.parse(NID);
    	if(schema.getType() ==NIDSchemeMDC.UNKNOWN){
    		if(isAdd){
    			throw invalidContext("add", listName, objectType, NID);
    		}else{
    			throw invalidContext("replace", listName, objectType, NID);
    		}
        }  
    	return schema;
    }      
    
    public static String resolveNID(MRRequestI request, MRLog log, MRDB db, String listName, String objectType, 
    		String NID, String caller) throws Exception {
        if(NID==null){
            NID="";
        }else{
            NID = NID.trim();
        }
    	//if document NID, than  return
    	NIDSchemeMDC schema = new NIDSchemeMDC();
    	if(!NID.equals("")){
        	schema.parse(NID) ;
        	if(schema.getType()==NIDSchemeMDC.DOCUMENT){
        		return schema.getNID();
        	}
        	if(schema.getType()==NIDSchemeMDC.COLLECTION){
        		return schema.getNID();
        	}
    	}
    	NID =  db.getServiceDomainRootNID(listName, objectType);
    	if (NID == null || NID.trim().equals("")) {
            throw IpvsDBUtils.invalidContext(caller, listName, objectType, NID);
        }
    	return NID;
    }  
    
    public static String concatWhereXpath(ArrayList<String> arrXpath) {
        if (arrXpath == null || arrXpath.isEmpty()) {
            return "";
        }
        StringBuffer buf = new StringBuffer();
        boolean bAddAND = false;
        boolean bAdded = false;
        for (Iterator<?> i = arrXpath.iterator(); i.hasNext();) {
            String xpath = (String) i.next();
            //System.out.print(" xpath " + xpath);
            if (xpath != null && xpath.trim().length() > 0) {
                if (bAddAND) {
                    buf.append(" and ");
                } else {
                    bAdded = true;
                    buf.append("(");
                    bAddAND = true;
                }
                buf.append("(" + xpath.trim() + ")");
            }
        }
        if (bAdded) buf.append(")");
        
        return buf.toString();
    }
    
    public static void moveEL_GetListObjectResponseData(Element elementRes, Element elementResChild) {
        try {
            /*
             <GetListObjectResponseData><ObjectCount>0</ObjectCount><ObjectLastWhereEndCount>0</ObjectLastWhereEndCount></GetListObjectResponseData> 
            */
            Element elChild = null;
            if (elementResChild != null) {
                elChild = MRXMLUtils.getElement(elementResChild, "GetListObjectResponseData");
                if (elChild != null) elChild.detach();
            }
            if (elChild == null) {
                elChild = MRXMLUtils.stringToElement("<GetListObjectResponseData><ObjectCount>0</ObjectCount><ObjectLastWhereEndCount>0</ObjectLastWhereEndCount></GetListObjectResponseData>");
            }
            if (elementRes == null || MRXMLUtils.getParentElement(elementRes) == null) return;
            MRXMLUtils.addElement(MRXMLUtils.getParentElement(elementRes), elChild);
        } catch (Exception ex) {}
    }
    
    ///////////////////////
    private static Hashtable<String, String> rootPrefixTable = null;
    
    private static void initDBListPrefix() throws Exception {
        String schemaPath = MRConst.MRSCHEMAPATH;
        Document schemadoc = MRXMLUtils.loadXMLResource(schemaPath, MRLog.getInstance(MRLog.CONSOLE));
        rootPrefixTable = new Hashtable<String, String>();
        initDBListPrefix(rootPrefixTable, schemadoc.getRootElement());
    }
    
    private static void initDBListPrefix(Hashtable<String, String> rootsPrefix, Element el) throws Exception {
        String elementName = el.getName();
        String elementNamePrefix = createListPrefix(elementName);
        rootsPrefix.put(elementName.toLowerCase(), elementNamePrefix);
        List<?> childList = MRXMLUtils.getList(el);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childReqEL = (Element) i.next();
            initDBListPrefix(rootsPrefix, childReqEL);
        }
    }
    
    private static String createListPrefix(String elementName) {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < elementName.length(); i++) {
            //65 - 90
            char c = elementName.charAt(i);
            if ((c < 'Z' && c > 'A') || c == 'A' || c == 'Z') {
                buf.append(c);
            }
        }
        return buf.toString();
    }
    
    private static String getListPrefix(String key) {
        if (rootPrefixTable == null || !rootPrefixTable.containsKey(key)) return "zzzz_";
        return rootPrefixTable.get(key);
    }
    
    public static String getAutoNID(String parentNID) throws Exception {
        if (rootPrefixTable == null) {
            try {
                initDBListPrefix();
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        NIDSchemeMDC nidScheme = new NIDSchemeMDC();
        nidScheme.parse(parentNID);
        if (nidScheme.getType() != NIDSchemeMDC.COLLECTION) {
            System.out.print("getAutoNID Problem ????????1111????? parentNID  " + parentNID);
            throw new Exception("Invalid list parentNID ='" +parentNID+"'");
        }
        String listPrefix = getListPrefix(nidScheme.getCollectionId());
        String NID = listPrefix + "_" + Utils.getUUID();
        return nidScheme.getCollectionNodeID() + "." + NID.toLowerCase();
    }
    
    public static String parseNID_BULK_SCHEMA(Element element, String serviceDomain) throws Exception {
        // Find if its new method
        String elementName = element.getName();
        String NID = serviceDomain + "." + elementName;
        MRXMLUtils.addAttribute(element, NODEID_2, NID.toLowerCase());
        List<?> childList = MRXMLUtils.getList(element);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element toELChild = (Element) i.next();
            parseNID_BULK_SCHEMA(toELChild, serviceDomain);
        }
        return NID;
    }
    
    public static String parseNID_BULK(Element element, String parentNID) throws Exception {
        // Find if its new method
        String NID = MRXMLUtils.getAttributeValue(element, NODEID_2, "").trim();
        if (!NID.trim().equals("")) {
            return NID;
        }
        return getAutoNID(parentNID);
    }
    
    public static String getWhereXpath(String[] arrXpath) {
        if (arrXpath == null || arrXpath.length == 0) {
            return "";
        }
        StringBuffer buf = new StringBuffer();
        boolean bAddAND = false;
        boolean bAdded = false;
        for (int i = 0; i < arrXpath.length; i++) {
            String xpath = arrXpath[i];
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
        if (bAdded) buf.append("]");
        
        return buf.toString();
    }
    
    public static void validateElement(MRRequestI request, MRLog log, MRDB db, String NID, String servicename) throws Exception {
        Document doc = MRXMLDBUtils.readElement(request, log, db, NID);
        String xml = MRXMLUtils.documentToString(doc);
        URL url = ClassLoader.getSystemClassLoader().getResource("com/ipvs/xsd/mediaroom." + servicename + ".datamodel.xsd");
        String xsdURL = url.getFile();
        
        try {
            MRXMLUtils.getDocument(xml, xsdURL);
        } catch (Exception e) {
            //System.exit(1);
        }
    }
    
    public static boolean validateXpath(Element whereSelectEL, String[] arrXpath, MRRequestI request, MRDB db, MRLog log) throws Exception {
        if ((arrXpath == null) || (arrXpath.length < 1) || (whereSelectEL == null)) {
            return false;
        }
        Document doc = MRXMLUtils.getDocument();
        try {
            doc.add((Element) whereSelectEL.clone());
        } catch (Exception ex) {}
        
        int countcheck = 0;
        
        for (int i = 0; i < arrXpath.length; i++) {
            String xPath = arrXpath[i];
            
            // System.out.println("XPATH$$$$ " + xPath);
            if (!xPath.equals("")) {
                Object b = null;
                try {
                    b = MRXMLUtils.selectSingleNodeXpath(doc, xPath);
                } catch (Exception ex) {
                    ex.printStackTrace();
                    b = null;
                }
                if (b == null) {
                    return false;
                } else {
                    countcheck++;
                }
            }
        }
        if (countcheck == arrXpath.length) {
            return true;
        }
        return false;
    }
}
