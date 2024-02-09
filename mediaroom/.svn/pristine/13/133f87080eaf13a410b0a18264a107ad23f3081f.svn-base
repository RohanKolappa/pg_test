package com.ipvs.common.db;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.params.DBParams;

public class IpvsDBImpl implements IpvsDBI {

    /////////////////////////////////////////////////////domain create/delete/get  start ////////////////////////////////////////////////////
    public Element getMRSchema(long DBCallTimeOut, MRDB db, MRLog log, String schemaPath) throws MRException {
        try {
            Document doc = MRXMLUtils.loadXMLResource(schemaPath, log);
            Element element = doc.getRootElement();
            IpvsDBUtils.parseNID_BULK_SCHEMA(element, db.getServiceDomain());
            return (Element) element.clone();
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_GET_MRSCHEMA, "<error><request>" + schemaPath + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    public void initDB(long DBCallTimeOut, MRDB db, MRLog log, String ipvsRootNID, String serviceDomain, String schemaPath, boolean reset) throws MRException {
        try {
            Document schemadoc = MRXMLUtils.loadXMLResource(schemaPath, log);
            db.init(ipvsRootNID, serviceDomain, schemadoc);
            if (!reset) {
                return;
            }
            String userJID = "*";
            MRRequest request = MRRequest.createRequest(userJID);
            String rootNID = db.getRootNID(); //just place holder 
            try {
                //todo: for now neglect.
                deleteRootNode(DBCallTimeOut, request, log, db, rootNID, serviceDomain);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            //TBD: use ipvsRootNID to create and delete document.
            addRootNode(DBCallTimeOut, request, log, db, rootNID, serviceDomain, schemadoc.getRootElement());
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_INITDB, "<error><request>" + serviceDomain + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }

    }

    private void deleteRootNode(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String NID, String serviceDomain) throws Exception {
        DBParams dbParams = new DBParams();
        dbParams.setAction(DBParams.DELETE_DOMAIN);
        dbParams.setDBCallTimeOut(DBCallTimeOut);
        dbParams.setServiceDomain(serviceDomain);
        dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
        dbParams.setNodeID(NID);
        db.getXMPPI().ipvsdb_deleteNode(dbParams);
    }

    private String addRootNode(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String parentNID, String serviceDomain, Element element) throws Exception {
        String NID = "";
        DBParams dbParams = new DBParams();
        dbParams.setAction(DBParams.CREATE_DOMAIN);
        dbParams.setDBCallTimeOut(DBCallTimeOut);
        dbParams.setServiceDomain(serviceDomain);
        dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
        //update the xml with all NIDs
        IpvsDBUtils.parseNID_BULK_SCHEMA(element, serviceDomain);
        dbParams.setItemData(MRXMLUtils.elementToString(element));
        dbParams.setNodeID(NID);
        //dbParams.setParentNodeID(parentNID);
        db.getXMPPI().ipvsdb_addNodeWithNodeID(dbParams);
        return NID;
    }

    ///////////////////////////////////////////////////// domain create/delete/get  edn ////////////////////////////////////////////////////

    public boolean objectExists(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String NID, String childXpath, String xQueryPrefix) throws MRException {
        try {
            NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, "objectExists");
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.IS_OBJECT_EXISTS);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);
            dbParams.setNodeID(NID);
            return db.getXMPPI().ipvsdb_nodeExists(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_OBJ_EXISTS, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    ///////////////////////////////////////////////////// add/replace/delete start ////////////////////////////////////////////////////
    @Override
    public Document add(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException {
        try {
            boolean isDocumentRootNode = true;
            if (childXpath != null && !childXpath.trim().equals("")) {
                isDocumentRootNode = false;
            }
            NIDSchemeMDC schema = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, dataElement, childXpath, true, isDocumentRootNode);
            if (isDocumentRootNode) {
                //update the xml with all NIDs
                //does not creates new NID, if the element already has NID attribute. 
                MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.NODEID_2, schema.getDocNodeID());
                MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.PARENT_NODEID, schema.getCollectionNodeID());
            }
            NID = schema.getNID();
            //create request params
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.ADD);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setNodeID(schema.getNID());
            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            dbParams.setItemData(MRXMLUtils.elementToString(dataElement));
            return db.getXMPPI().ipvsdb_addNodeWithNodeID(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_ADD, "<error><request> add " + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    @Override
    public Document replace(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, Element dataElement) throws MRException {
        try {
            boolean isDocumentRootNode = true;
            if (childXpath != null && !childXpath.trim().equals("")) {
                isDocumentRootNode = false;
            }
            NIDSchemeMDC schema = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, dataElement, childXpath, true, isDocumentRootNode);
            if (isDocumentRootNode) {
                //update the xml with all NIDs
                //does not creates new NID, if the element already has NID attribute. 
                if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
                    MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.NODEID_2, schema.getDocNodeID());
                }
                MRXMLUtils.addAttribute(dataElement, IpvsDBUtils.PARENT_NODEID, schema.getCollectionNodeID());
            }
            if(dataElement==null){
                throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_REPLACE, "<error><request>" + NID + "</request><errorMsg>dataElement is null</errorMsg></error>");

            }
            NID = schema.getNID();
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.REPLACE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setNodeID(schema.getNID());
            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            dbParams.setItemData(MRXMLUtils.elementToString(dataElement));
            return db.getXMPPI().ipvsdb_addNodeWithNodeID(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_REPLACE, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    public Document replaceAttribute(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String data) throws MRException {
        try {
            if (childXpath == null || childXpath.trim().equals("")) {
                throw new Exception("invalid child xpath");
            }
            NIDSchemeMDC schema = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, null, childXpath, true, false);
            NID = schema.getNID();
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.REPLACE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setNodeID(schema.getNID());
            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            dbParams.setItemData(data);
            return db.getXMPPI().ipvsdb_addNodeWithNodeID(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_REPLACE, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }
    @Override
    public Document delete(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath) throws MRException {
        try {
            NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, "delete");
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.DELETE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setNodeID(NID);
            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            return db.getXMPPI().ipvsdb_deleteNode(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_DELETE, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }

    }

    ///////////////////////////////////////////////////// add/replace/delete end ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////// read single/list/query start ////////////////////////////////////////////////////
    // context = one of the context is needed 
    // context = listNameContext or objectTypeContext or NIDContext -- needed.  
    // NIDContext = could be docNID or listNID

    // optional = arrWhereConditionXpath, SortByXpath, selectXML, appendToParentElement

    // to search objects = (listNameContext or objectTypeContext or listNID) and arrWhereConditionXpath 
    // to search within object = docNID  and arrWhereConditionXpath 

    // to partial read  =  selectXML
    // to sort = SortByXpath
    // context - listNameContext or objectTypeContext or NIDContext -- needed.  

    @Override
    public Element readSingle(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement) throws MRException {
        try {
            NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, "readSingle");

            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.READ_SINGLE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setNodeID(NID);
            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            dbParams.setSelectXML(selectXML);
            dbParams.setSortBy(sortByXpath);
            dbParams.setSortOrder(sortOrder);

            
            Element data = db.getXMPPI().ipvsdb_getData(dbParams);
            //String data = "";
            try {
                /*if (result != null) {
                    //todo: test: may be already decoded
                    data = Utils.decode(result);
                }*/
                if ((data == null)) {
                    return null;
                }
                Element elementResChild = null;
                //new db api may return count for single read also 
                if (data.getName().equals("DBResp")) {
                    List<?> childList = MRXMLUtils.getList(data);
                    for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                        Element childEL = (Element) i.next();
                        //todo: may be use GetListObjectResponseData info for single read too.
                        if (!childEL.getName().equals("GetListObjectResponseData")) {
                            elementResChild = (Element) childEL.detach();
                            break;
                        }
                    }
                } else {
                    elementResChild = (Element) data.detach();
                }
                //not found
                if (elementResChild == null) {
                    return null;
                }
                //found attach to parent . detach from document
                if (appendToParentElement != null) {
                    MRXMLUtils.addElement(appendToParentElement, elementResChild);
                }
                return elementResChild;
            } catch (Exception ex) {
                ex.printStackTrace();
                return null;
            }
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_READ_SINGLE, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }

    }

    @Override
    public Element readList(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, String xQueryPrefix, String[] arrWhereXpath, String[] sortByXpath, String sortOrder, String selectXML, Element appendToParentElement, int startFrom, int countToFetch) throws MRException {
        try {
            NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, "readList");
            //System.out.println("-------------- new function 43----- readList " + NID);

            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.READ_LIST);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setListName(listName);
            dbParams.setObjectType(objectType);

            dbParams.setWhereXpath(arrWhereXpath);
            dbParams.setNodeID(NID);
            dbParams.setChildXpath(childXpath);
            dbParams.setXQueryPrefix(xQueryPrefix);
            dbParams.setStartFrom(startFrom);
            dbParams.setCountToFetch(countToFetch);

            dbParams.setSelectXML(selectXML);
            dbParams.setSortBy(sortByXpath);
            dbParams.setSortOrder(sortOrder);
            
            Element data = db.getXMPPI().ipvsdb_getData(dbParams);
            try {
                /*String data = "";
                if (result != null) {
                    //todo: test: may be already decoded
                    data = Utils.decode(result);
                }*/
                if ((data == null)) {
                    return appendToParentElement;
                }
                //System.out.println("**********************findElement**************************************");
                //System.out.println(data);
                //System.out.println("************************findElement************************************");

                Element elementResChild = data;
                //this is temp change. This info needs to be available to other calling functions inside mediaroom 
                IpvsDBUtils.moveEL_GetListObjectResponseData(appendToParentElement, elementResChild);

                //added this code of line if elementRes==null return <bulkread> element as recvd from db 
                if (appendToParentElement == null)
                    return elementResChild;

                appendToParentElement.setText(MRXMLUtils.getValue2(elementResChild, ""));
                List<?> childList = MRXMLUtils.getList(elementResChild);
                for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                    Element childEL = (Element) i.next();
                    MRXMLUtils.addElement(appendToParentElement, (Element) childEL.clone());
                }
                return appendToParentElement;
            } catch (Exception ex) {
                ex.printStackTrace();
                return null;
            }
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_READ_LIST, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    @Override
    public String readQuery(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String query, Map<String, String> pemXPathList, Element appendResultToParentElement) throws MRException {

        try {
            Map<String, String> nidList = new Hashtable<String, String>();
            for (Iterator<String> iterator = pemXPathList.keySet().iterator(); iterator.hasNext();) {
                String objectType = iterator.next();
                if (objectType.indexOf('.') != -1) {
                    NIDSchemeMDC schema = new NIDSchemeMDC();
                    schema.parse(objectType);
                    if (schema.getType() == NIDSchemeMDC.COLLECTION) {
                        nidList.put(objectType, schema.getCollectionNodeID());
                    } else if (schema.getType() == NIDSchemeMDC.DOCUMENT) {
                        nidList.put(objectType, schema.getDocNodeID());
                    } else {
                        throw new Exception(" invalid NID or Objectype " + objectType);
                    }
                } else {
                    MRConfig config = MRConfig.getInstance(objectType, log);
                    String parentKey = config.getParentNIDKey(log);
                    String nid = db.getServiceDomainRootNID(parentKey);
                    if (nid == null) {
                        throw new Exception(" invalid NID or Objectype  '" + objectType + "' parentKey='" + parentKey + "'");
                    }
                    nidList.put(objectType, nid);
                }
            }

            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.READ_QUERY);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setNodeID("");

            //new 
            dbParams.setQuery(query);
            dbParams.setQueryXPathList(pemXPathList);
            dbParams.setQueryNIDList(nidList);
            String data ="";
            Element dataEL = db.getXMPPI().ipvsdb_getData(dbParams);
            if(dataEL !=null  ) {
                //MRLog.println(" readQuery " + data.getName());
                //result comes in DBRes. remove the parent
                Element elementResChild = dataEL.createCopy();
                if(elementResChild !=null){
                    List<?> childList = MRXMLUtils.getList(elementResChild);
                    if(childList.size()==0){
                        //the returned value can be non xml ... like int or boolean
                        data = MRXMLUtils.getValue2(elementResChild,"").trim();
                        //append to parent    
                        if (appendResultToParentElement != null ) {
                            MRXMLUtils.setValue(appendResultToParentElement, data);
                        }
                    }else{
                        //the returned value can be be multiple children nodes
                        StringBuffer buf = new StringBuffer();
                        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                            Element childEL = (Element) i.next();
                            buf.append(MRXMLUtils.elementToString(childEL));
                        }
                        data = buf.toString();
                        //append to parent    
                        if (appendResultToParentElement != null ) {
                            MRXMLUtils.addElement(appendResultToParentElement, elementResChild);
                        }
                    }
                }
            }            
            /*String data = "";
            if (result != null) {
                //todo: find if already decoded
                data = URLDecoder.decode(result, "UTF-8");
            }*/
            return data;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_READ_QUERY, "<error><request>" + query + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }

    }

    ///////////////////////////////////////////////////// read single/list/query end ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////// subscribe/unsubscribe start ////////////////////////////////////////////////////

    @Override
    public boolean subscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String subscriptionParams) throws MRException {
        try {
        	DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.SUBSCRIBE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setSubscriptionParams(subscriptionParams);
            
      
            return db.getXMPPI().ipvsdb_Subscribe(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_SUBSCRIBE, "<error><request>" + "subscribe with NIDs" + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }
    
    @Override
    public boolean updateSubscriptions(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String subscriptionParams) throws MRException {
        try {
        	DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.UPDATESUBSCRIPTIONS);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setSubscriptionParams(subscriptionParams);
            
      
            return db.getXMPPI().ipvsdb_updateSubscriptions(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_SUBSCRIBE, "<error><request>" + "updateSubscriptions with NIDs" + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    @Override
    public boolean unsubscribe(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String listName, String objectType, String unsubscribeParams) throws MRException {
        try {
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.UNSUBSCRIBE);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
            dbParams.setSubscriptionParams(unsubscribeParams);
            
            return db.getXMPPI().ipvsdb_Unsubscribe(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_UNSUBSCRIBE, "<error><request>" + "Unsubscribe" + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }
    ///////////////////////////////////////////////////// subscribe/unsubscribe end ////////////////////////////////////////////////////

    ///////////////////////////////////////////////////// exportdb/ importdb start ////////////////////////////////////////////////////

    public Document exportDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String importDBParams) throws MRException{
        try {
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.EXPORT_DB);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setType(type);
            dbParams.setExportDBParams(importDBParams);
            return db.getXMPPI().ipvsdb_exportDB(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_EXPORTDB, "<error><request>"+type+" " + importDBParams + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
   }
   

   public Document importDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String exportDBParams) throws MRException{
        try {
            DBParams dbParams = new DBParams();
            dbParams.setAction(DBParams.IMPORT_DB);
            dbParams.setDBCallTimeOut(DBCallTimeOut);
            dbParams.setServiceDomain(db.getServiceDomain());
            dbParams.setType(type);
            dbParams.setImportDBParams(exportDBParams);
            return db.getXMPPI().ipvsdb_importDB(dbParams);
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_IMPORTDB, "<error><request>"+type+" " + exportDBParams + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
   }

   public Document deleteDB(long DBCallTimeOut, MRRequestI request, MRDB db, MRLog log, String type, String importDBParams) throws MRException{
       try {
           DBParams dbParams = new DBParams();
           dbParams.setAction(DBParams.DELETE_DB);
           dbParams.setDBCallTimeOut(DBCallTimeOut);
           dbParams.setServiceDomain(db.getServiceDomain());
           dbParams.setType(type);
           dbParams.setDeleteDBParams(importDBParams);
           return db.getXMPPI().ipvsdb_deleteDB(dbParams);
       } catch (MRException ex) {
           throw ex;
       } catch (Exception ex) {
           throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_DELETEDB, "<error><request>"+type+" " + importDBParams + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
       }
  }

     
    ///////////////////////////////////////////////////// exportdb/ importdb end ////////////////////////////////////////////////////

   
///////////////////////////////////////////////////// get subscription list start ///////////////////////////////////////////////////
   
   public Document getSubscriptions(long DBCallTimeOut, MRRequestI request,
   		MRLog log, MRDB db, String listName, String objectType, String JID, String selectFormat) throws MRException {
	   
	   try {
           //NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, "", "getSubscriptionList");
           DBParams dbParams = new DBParams();
           dbParams.setAction(DBParams.GET_SUBSCRIPTIONS);
           dbParams.setDBCallTimeOut(DBCallTimeOut);
           dbParams.setServiceDomain(db.getServiceDomain());
           dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
           dbParams.setSelectFormat(selectFormat); // BASIC/EXTENDED
           //dbParams.setNodeID(NID);
           return db.getXMPPI().ipvsdb_getSubscriptions(dbParams);
       } catch (MRException ex) {
           throw ex;
       } catch (Exception ex) {
           throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_GET_SUBSCRIPTIONS, "<error><request>" + "" + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
       }
   }
   
   public Document getSubscribers(long DBCallTimeOut, MRRequestI request,
	   		MRLog log, MRDB db, String listName, String objectType, String NID, String getSubscribersParams) throws MRException {
		   
		   try {
			   if(!NID.trim().equals(""))
				   NID = IpvsDBUtils.resolveNID(request, log, db, listName, objectType, NID, "getSubscribers");
	           DBParams dbParams = new DBParams();
	           dbParams.setAction(DBParams.GET_SUBSCRIBERS);
	           dbParams.setDBCallTimeOut(DBCallTimeOut);
	           dbParams.setServiceDomain(db.getServiceDomain());
	           dbParams.setSubscribeJID(IpvsDBUtils.getUserJID(request, db));
	           dbParams.setSubscriptionParams(getSubscribersParams); // BASIC/EXTENDED
	           dbParams.setNodeID(NID);
	           return db.getXMPPI().ipvsdb_getSubscribers(dbParams);
	       } catch (MRException ex) {
	           throw ex;
	       } catch (Exception ex) {
	           throw new MRException(DBExceptionCodes.DB_ACTION_FAILED_GET_SUBSCRIBERS, "<error><request>" + NID + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
	       }
	   }

/////////////////////////////////////////////////// get subscription list end ///////////////////////////////////////////////////
    
}
    


 
