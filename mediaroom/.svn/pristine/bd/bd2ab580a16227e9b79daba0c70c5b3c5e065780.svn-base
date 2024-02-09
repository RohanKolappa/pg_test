package com.ipvs.common.db;

import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class MRXMLDBUtils {
    private static IpvsDBI ipvsDB = null;
    public static String SORT_ASCENDING = "ascending";
    public static String SORT_DESCENDING = "Descending";
    //public static final long TIME_OUT = 60 * 1000;
    public static final long TIME_OUT = 300 * 1000;
    static {
        String useMRXMLDBUtilsImpl = "com.ipvs.common.db.PubsubDBImpl";
        useMRXMLDBUtilsImpl = IPVSConst.getInstance().getUseMRXMLDBUtilsImpl();
        
        // Added to point the db to the sc db component
        if(DBUtilsBase.theIpvsDB != null)
        	ipvsDB = DBUtilsBase.theIpvsDB;

        try {
        	if(ipvsDB == null)
        		ipvsDB = (IpvsDBI) Class.forName(useMRXMLDBUtilsImpl).newInstance();
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        if (ipvsDB == null) {
            ipvsDB = new IpvsDBImpl();
        }
    }

    public static void deleteElement(MRRequestI request, MRLog log, MRDB db, String NID) throws MRException {
        //ipvsDB.deleteElement(request, log, db, NID);
        String listName = null;
        String objectType = null;
        String childConextXpath = null;
        String[] arrWhereConditionXpath = null;
        delete(TIME_OUT, request, log, db, listName, objectType, NID, childConextXpath, arrWhereConditionXpath);
    }

    public static void setObjectData(MRRequestI request, MRDB db, MRLog log, String NID, String data) throws MRException {
        //ipvsDB.setObjectData(request, db, log, NID, data);
        String listName = null;
        String objectType = null;
        String childConextXpath = null;
        String[] arrWhereConditionXpath = null;
        Element dataElement = null;
        try {
            dataElement = MRXMLUtils.stringToElement(data);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        if (dataElement != null) {
            replace(TIME_OUT, request, log, db, listName, objectType, NID, childConextXpath, arrWhereConditionXpath, dataElement);
        } else {
            //raise error 
        }
    }

    public static Document updateElement(MRRequestI request, MRLog log, MRDB db, String parentNID, Element element) throws MRException {
        //return ipvsDB.updateElement(request, log, db, parentNID, element);
        String listName = null;
        String objectType = null;
        String childConextXpath = null;
        String[] arrWhereConditionXpath = null;
        return replace(TIME_OUT, request, log, db, listName, objectType, parentNID, childConextXpath, arrWhereConditionXpath, element);

    }

    public static String getObjectData(MRRequestI request, MRDB db, MRLog log, String NID) throws MRException {
        //return ipvsDB.getObjectData(request, db, log, NID);
        try {
            Document resDoc = readElement(request, log, db, NID);
            if ((resDoc != null) && (resDoc.getRootElement() != null)) {
                return MRXMLUtils.elementToString(resDoc.getRootElement());
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    public static Element readChildElement(int nodeDepth, String objectNID, String serviceName, String dbObjectName, String readXml, MRLog log, MRRequest request, MRDB db) throws MRException {
        try {
            Document resDoc = readElement(request, log, db, objectNID);

            if ((resDoc != null) && (resDoc.getRootElement() != null)) {
                return resDoc.getRootElement();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    public static Element readElement(int nodeDepthWhereAssist, Element elementWhereAssist, String[] arrXpath, String rootNID, int depth, Element elementRes, Element elementReq, MRRequestI request, MRDB db, MRLog log) throws MRException {
        //return ipvsDB.readElement(nodeDepthWhereAssist, elementWhereAssist, arrXpath, rootNID, depth, elementRes, elementReq, request, db, log);
        String listName = null;
        String objectType = null;
        String[] SortByXpath = null;
        String selectXML = null;
        String childXPath = null;
        return readSingle(TIME_OUT, request, log, db, listName, objectType, rootNID, childXPath, arrXpath, SortByXpath, SORT_ASCENDING, selectXML, elementRes);
    }

    public static Element readElement(int nodeDepthWhereAssist, Element elementWhereAssist, String[] arrXpath, String rootNID, Element elementRes, MRRequestI request, MRLog log, MRDB db) throws MRException {
        //return ipvsDB.readElement(nodeDepthWhereAssist, elementWhereAssist, arrXpath, rootNID, elementRes, request, log, db);
        String listName = null;
        String objectType = null;
        String[] SortByXpath = null;
        String selectXML = null;

        String childXPath = null;
        return readSingle(TIME_OUT, request, log, db, listName, objectType, rootNID, childXPath, arrXpath, SortByXpath, SORT_ASCENDING, selectXML, elementRes);
    }

    public static Document readElement(MRRequestI request, MRLog log, MRDB db, String NID) throws MRException {
        //return ipvsDB.readElement(request, log, db, NID);
    	
    	MRRequestCache cache = request.getCache();
    	if((cache != null) && cache.containsKey(NID)) {
    		Document doc = (Document)cache.get(NID);
    		return (Document)doc.clone();
    	}
    	
        String listName = null;
        String objectType = null;
        String[] arrWhereConditionXpath = null;
        String[] SortByXpath = null;
        String selectXML = null;
        Element appendToParentElement = null;

        String childXPath = null;
        //Element ret = readElement_BULK(NID, -1, null, null, 1, null, null, request, db, log);
        Element ret = readSingle(TIME_OUT, request, log, db, listName, objectType, NID, childXPath, arrWhereConditionXpath, SortByXpath, SORT_ASCENDING, selectXML, appendToParentElement);
        if (ret == null) {
            return null;
        }
        Document doc = null;
        try {
            //shoudl never happen
            doc = MRXMLUtils.getDocument();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        if (doc == null) {
            return null;
        }
        doc.setRootElement(ret);
    	if(cache != null)
    		cache.put(NID, doc.clone());
        return doc;
    }

    public static Element findChildElement_JumpNID(int nodeDepth, String serviceName, String dbObjectName, String readXml, String whereAssistXml, String whereAssistXpath, MRLog log, MRRequest request, MRDB db) throws MRException {
        try {
        	
        	String key = dbObjectName + "." + readXml + "." + whereAssistXpath;
        	MRRequestCache cache = request.getCache();
        	if((cache != null) && cache.containsKey(key)) {
        		Element element = (Element)cache.get(key);
        		return (Element)element.clone();
        	}
        	
            //return ipvsDB.findChildElement_JumpNID(nodeDepth, serviceName, dbObjectName, readXml, whereAssistXml, whereAssistXpath, log, request, db);
            MRConfig mrconfig = MRConfig.getInstance(serviceName, dbObjectName, log);
            String parentNID = db.getServiceDomainRootNID(mrconfig.getParentNIDKey(log));
            Element elementRes = null;
            //Document docReq = mrconfig.getXmlRepresentation(log).getElementFromXpath(readXml, request, null, log, db, null, null);

            String[] arrXpath = { whereAssistXpath };
            int startFrom = 0;
            int countToFetch = 1;
            elementRes = MRXMLUtils.addElement("temp");
            //found bug 
            nodeDepth = -1;

            String listName = null;
            String objectType = null;
            String[] SortByXpath = null;
            String selectXML = null;

            String childXPath = null;
            //Element elementResChild = findChildElements_BULK(startFrom, countToFetch, nodeDepth, parentNID, elementRes, null, 1, null, arrXpath, request, db, log);
            Element elementResChild = readList(TIME_OUT, request, log, db, listName, objectType, parentNID, childXPath, arrXpath, SortByXpath, SORT_ASCENDING, selectXML, elementRes, startFrom, countToFetch);

            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, elementResChild);

            Element ret = MRXMLUtils.getElementXpath(doc, readXml);

            if (ret != null) {
                //create doc with the child in 'readXml' xpath. MRClientUtils.updateAssetResourceInternal uses the document
                doc = MRXMLUtils.getDocument();
                MRXMLUtils.addElement(doc, (Element) ret.detach());
            	if(cache != null)
            		cache.put(key, ret.clone());
                return ret;
            }
            return null;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_READ_LIST, "<error><request>" + dbObjectName + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }

    }

    public static Element findChildElements(int startFrom, int countToFetch, int nodeDepth, String docNID, Element appendToParentElement, Element elementReq, int nodeDepthWhereAssist, Element elementWhereAssist, String[] arrWhereConditionXpath, MRRequestI request, MRDB db, MRLog log) throws MRException {
        //return ipvsDB.findChildElements(startFrom, countToFetch, nodeDepth, parentElementNID, elementRes, elementReq, nodeDepthWhereAssist, elementWhereAssist, arrXpath, request, db, log);
        String listName = null;
        String objectType = null;
        String[] SortByXpath = null;
        String selectXML = null;

        String childXPath = null;
        return readList(TIME_OUT, request, log, db, listName, objectType, docNID, childXPath, arrWhereConditionXpath, SortByXpath, SORT_ASCENDING, selectXML, appendToParentElement, startFrom, countToFetch);
    }

    public static Element findChildElements(String docNID, Element elementRes, Element elementReq, int nodeDepthWhereAssist, Element elementWhereAssist, String[] arrXpath, MRRequestI request, MRDB db, MRLog log) throws MRException {
        //return ipvsDB.findChildElements(parentElementNID, elementRes, elementReq, nodeDepthWhereAssist, elementWhereAssist, arrXpath, request, db, log);
        String listName = null;
        String objectType = null;
        String[] SortByXpath = null;
        String selectXML = null;
        int startFrom = 0;
        int countToFetch = -1;

        String childXPath = null;
        return readList(TIME_OUT, request, log, db, listName, objectType, docNID, childXPath, arrXpath, SortByXpath, SORT_ASCENDING, selectXML, elementRes, startFrom, countToFetch);
    }

    public static void initDB(MRDB db, MRLog log, String ipvsRootNID, String serviceDomain, String schemaPath, boolean reset) throws MRException {
        initDB(TIME_OUT, db, log, ipvsRootNID, serviceDomain, schemaPath, reset);
    }

    private static String createSubscriptionParams(String NID, String subscriptionID, String topic, String selectFormat) {
    	StringBuffer buf = new StringBuffer();
    	buf.append("<" + SessionServiceConst.SUBSCRIPTION_PARAMS_OBJECT);
    	buf.append(" " + SessionServiceConst.WHERE_NID + "='");
    	buf.append(NID + "'");
    	buf.append(" " + SessionServiceConst.SELECTFORMAT + "='");
    	buf.append(selectFormat);
    	buf.append("'>");
    	buf.append("<" + SessionServiceConst.WHERE_SUBSCRIPTION_ID + ">");
    	buf.append(subscriptionID);
    	buf.append("</" + SessionServiceConst.WHERE_SUBSCRIPTION_ID+ ">");
    	buf.append("<" + SessionServiceConst.TOPIC + ">");
    	buf.append(topic);
    	buf.append("</" + SessionServiceConst.TOPIC + ">");
    	buf.append("</" + SessionServiceConst.SUBSCRIPTION_PARAMS_OBJECT + ">");
    	
    	return buf.toString();
    }
    
    public static boolean subscribe(MRRequestI request, MRDB db, MRLog log, String nidList, String subscriptionId, String topic) throws MRException {
    	String subscriptionParams = createSubscriptionParams(nidList, subscriptionId, topic, "");     
    	return subscribe(TIME_OUT, request, db, log, subscriptionParams);
    }
    
    public static boolean updateSubscriptions(MRRequestI request, MRDB db, MRLog log, String nidList, String subscriptionId, String topic) throws MRException {
    	String subscriptionParams = createSubscriptionParams(nidList, subscriptionId, topic, "");
    	return updateSubscriptions(TIME_OUT, request, db, log, subscriptionParams);
    }

    public static boolean unsubscribe(MRRequestI request, MRDB db, MRLog log, String NID, String subscriptionID) throws MRException {
    	String unsubscribeParams = createSubscriptionParams(NID, subscriptionID, "", "");
    	return unsubscribe(TIME_OUT, request, db, log, unsubscribeParams);
    }

    //////////////////// keep prev /////////////////////////////////////////
    public static Element getMRSchema(MRDB db, MRLog log, String schemaPath) throws MRException {
        return ipvsDB.getMRSchema(TIME_OUT, db, log, schemaPath);
    }

    public static void initDB(long DBCallTimeOut, MRDB db, MRLog log, String ipvsRootNID, String serviceDomain, String schemaPath, boolean reset) throws MRException {
        ipvsDB.initDB(DBCallTimeOut, db, log, ipvsRootNID, serviceDomain, schemaPath, reset);
    }

    public static boolean objectExists(MRRequestI request, MRDB db, MRLog log, String NID) throws MRException {
        String listName = null;
        String objectType = null;
        String childXpath = null;
        String xQueryPrefix = null;
        return ipvsDB.objectExists(TIME_OUT, request, db, log, listName, objectType, NID, childXpath, xQueryPrefix);
    }

    
    public static boolean subscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String subscriptionParams) throws MRException {
        String listName = null;
        String objectType = null;
        try {
            return ipvsDB.subscribe(DBCallTimeOut, request, db, log, listName, objectType, subscriptionParams);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return false;
    }
    
    public static boolean updateSubscriptions(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String subscriptionParams) throws MRException {
        String listName = null;
        String objectType = null;
        try {
            return ipvsDB.updateSubscriptions(DBCallTimeOut, request, db, log, listName, objectType, subscriptionParams);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return false;
    }

    public static boolean unsubscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String unsubscribeParams) throws MRException {
        String listName = null;
        String objectType = null;
        try {
            return ipvsDB.unsubscribe(DBCallTimeOut, request, db, log, listName, objectType, unsubscribeParams);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return false;
    }

    ////////////////////////////////////// add new //////////////////////
    public static Document add(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String[] arrWhereXpath, Element dataElement) throws MRException {
        String xQueryPrefix = null;
        return ipvsDB.add(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
    }

    public static Document replace(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String[] arrWhereXpath, Element dataElement) throws MRException {
        String xQueryPrefix = null;
        return ipvsDB.replace(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
    }
    
    public static Document replaceAttribute(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String[] arrWhereXpath, String data) throws MRException {
        String xQueryPrefix = null;
        return ipvsDB.replaceAttribute(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, data);
    }
    
    public static Document delete(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String[] arrWhereXpath) throws MRException {
            String xQueryPrefix = null;
            return ipvsDB.delete(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath);
      
    }

    public static Element readSingle(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXPath, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement) throws MRException {
            String xQueryPrefix = null;
            return ipvsDB.readSingle(DBCallTimeOut, request, log, db, listName, objectType, NID, childXPath, xQueryPrefix, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement);
    }

    public static Element readList(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXPath, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement, int startFrom, int countToFetch) throws MRException {
            String xQueryPrefix = null;
            return ipvsDB.readList(DBCallTimeOut, request, log, db, listName, objectType, NID, childXPath, xQueryPrefix, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, startFrom, countToFetch);
        

    }
    
    ////////////////////////////////////// add xQueryPrefix //////////////////////
    public static Document addWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException {
        return ipvsDB.add(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
    }

    public static Document replaceWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException {
        return ipvsDB.replace(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
    }
    
    public static Document replaceAttributeWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String data) throws MRException {
        return ipvsDB.replaceAttribute(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, data);
    }
    
    public static Document deleteWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath) throws MRException {
        try {
            return ipvsDB.delete(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    public static Element readSingleWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXPath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement) throws MRException {
        try {
            return ipvsDB.readSingle(DBCallTimeOut, request, log, db, listName, objectType, NID, childXPath, xQueryPrefix, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement);
        } catch (Exception ex) {
            //todo: raise error
            ex.printStackTrace();
        }
        return null;
    }

    public static Element readListWithXqueryPrefix(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXPath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement, int startFrom, int countToFetch) throws MRException {
        try {
            return ipvsDB.readList(DBCallTimeOut, request, log, db, listName, objectType, NID, childXPath, xQueryPrefix, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, startFrom, countToFetch);
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }
    ////////
    
    public static String readQuery(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String query, Map<String, String> pemXPathList, Element appendResultToParentElement) throws MRException {
        return ipvsDB.readQuery(DBCallTimeOut, request, log, db, query, pemXPathList, appendResultToParentElement);
    }

    public static Document importDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String importDBParams) throws MRException {
        return ipvsDB.importDB(DBCallTimeOut, request, db, log, type, importDBParams);
    }

    public static Document exportDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String exportDBParams) throws MRException {
        return ipvsDB.exportDB(DBCallTimeOut, request, db, log, type, exportDBParams);
    }
    public static Document deleteDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String deleteDBParams) throws MRException{
        return ipvsDB.deleteDB(DBCallTimeOut, request, db, log, type, deleteDBParams);
        
    }
    
    /////////////////////////////// getSubscriptionsList getSubscribersList /////////////////////////////
    
    public static Document getSubscriptions(MRRequestI request, MRLog log, MRDB db, String JID, String selectFormat) throws MRException {
    	String listName = null;
    	String objectType = null;
    	return ipvsDB.getSubscriptions(TIME_OUT, request, log, db, listName, objectType, JID, selectFormat);
    }
    
    public static Document getSubscribers(MRRequestI request, MRLog log, MRDB db, String NID, String selectFormat) throws MRException {
    	String listName = null;
    	String objectType = null;
    	String getSubscribersParams = createSubscriptionParams("", "", "", selectFormat);
    	return ipvsDB.getSubscribers(TIME_OUT, request, log, db, listName, objectType, NID, getSubscribersParams);
    }
    
    /////////////////////////////// getSubscriptionsList getSubscribersList /////////////////////////////

}
