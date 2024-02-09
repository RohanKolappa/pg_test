package com.ipvs.common.db;

import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.service.MRDB;

public interface IpvsDBI {
    public Element getMRSchema(long DBCallTimeOut, MRDB db, MRLog log, String schemaPath) throws MRException;

    public void initDB(long DBCallTimeOut, MRDB db, MRLog log, String ipvsRootNID, String serviceDomain, String schemaPath, boolean reset) throws MRException;

    public boolean objectExists(long DBCallDBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String NID, String childXpath, String xQueryPrefix) throws MRException;

    public Document add(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException;

    public Document replace(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException;
    
    public Document replaceAttribute(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String data) throws MRException ;

    public Document delete(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath) throws MRException;

    public Element readSingle(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement) throws MRException;

    public Element readList(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement, int startFrom, int countToFetch) throws MRException;

    public String readQuery(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String query, Map<String, String> pemXPathList, Element appendResultToParentElement) throws MRException;

    public boolean subscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String subscriptionParams) throws MRException;
    
    public boolean updateSubscriptions(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String subscriptionParams) throws MRException;

    public boolean unsubscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String unsubscribeParams) throws MRException;

    public Document importDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String importDBName, String importDBParams) throws MRException;

    public Document exportDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String exportDBName, String exportDBParams) throws MRException;
    
    public Document deleteDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String deleteDBParams) throws MRException;
    
    public Document getSubscriptions(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String JID, String selectFormat) throws MRException;
    
    public Document getSubscribers(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String getSubscribersParams) throws MRException;

//	boolean updateSubscriptions(String subscriptionId, String topic, long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, ArrayList<String> NID)	throws MRException;
    
}
