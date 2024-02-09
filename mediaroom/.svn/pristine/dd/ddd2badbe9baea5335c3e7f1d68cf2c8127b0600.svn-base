package com.ipvs.common.db;

import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class MRDBHelper {
    private static int RESULT_SUCCESS = 1;
    private MRRequest request = null;
    private MRLog log = null;
    private MRDB db = null;
    private Document resDoc = null;
    private String serviceName = "";
    public static final String PREFIX_OBJ = "#OBJ_";
    public static final String SUFFIX_OBJ = "_OBJ#";
    private long timeout = MRXMLDBUtils.TIME_OUT; 
    public MRDBHelper() {
    }

    public int doInit(String serviceName, String objectType, Document resDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        this.serviceName = serviceName;
        this.request = request;
        this.log = log;
        this.db = db;
        this.resDoc = resDoc;
        return RESULT_SUCCESS;
    }

    public int doUnInit() throws Exception {
        request = null;
        log = null;
        db = null;
        resDoc = null;
        return RESULT_SUCCESS;
    }

    public int doAdd(Element actionEL, String responseElement, String objectType, String pemXPath) throws Exception {
        MRConfig amConfig = null;
        try {
            amConfig = MRConfig.getInstance(serviceName, objectType, log);
            String listName = amConfig.getParentNIDKey(log);

            String NID = MRXMLUtils.getValue(actionEL, "ParentNID", "").trim();
            String childXpath = MRXMLUtils.getValueXpath(actionEL, "ParentPathSpec/In/ChildXPath", "").trim();
            String xQueryPrefix = Utils.decode( MRXMLUtils.getValueXpath(actionEL, "ParentPathSpec/Where/XQueryPrefix", "").trim());

            Element dataElement = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");
            
            //where
            List<String> whereXPathList = MRXMLUtils.getValuesXpath(actionEL, "ParentPathSpec/Where/WhereXPath");

            //todo: no need to send this pem xpath to server for add/replace. 
            // its ok to send to db for get and delete.
            //however pemxpath may be applied on db for partials later.
            //if(pemXPath !=null && pemXPath.trim().length()>0){
            //	whereXPathList.add(pemXPath);
            //}
            String[] arrWhereXpath = null;//EMUtils.getWhereXPathArray(WhereXPath, pemXPath);
            if (whereXPathList != null && whereXPathList.size() > 0) {
                arrWhereXpath = new String[whereXPathList.size()];
                whereXPathList.toArray(arrWhereXpath);
            }

            /* // AddResponse
             String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
             Element ResponseXML = getActionResEL(responseElement, RequestID);

             if(dataElement ==null ){
                 throw new MRException(DBExceptionCodes.DB_ADD_FAILED, "OperationData is empty");
             }
            
             String xml = MRXMLDBUtils.add(request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, dataElement);
             MRXMLUtils.addElement(ResponseXML, (Element) dataElement.clone());*/
            if (dataElement == null) {
                throw new MRException(DBExceptionCodes.DB_UPDATE_FAILED, "OperationData is empty");
            }
            timeout = Utils.getLongValue( MRXMLUtils.getValueXpath(actionEL, "//ParentPathSpec//DBCallTimeOut", "").trim(), timeout); 
            // Add to Response
            Document doc = MRXMLDBUtils.addWithXqueryPrefix(timeout, request, log, db, listName, objectType, NID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            if (doc != null && doc.getRootElement() !=null) {
                getActionResEL(responseElement, RequestID, resDoc, doc.getRootElement());
            } else {
                Element ResponseXML = getActionResEL(responseElement, RequestID);
                MRXMLUtils.addElement(ResponseXML, (Element) dataElement.clone());
            }
            return RESULT_SUCCESS;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_ADD_FAILED, "<error><request>" + objectType + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        } finally {
            amConfig = null;
        }
    }

    public int doUpdate(Element actionEL, String responseElement, String objectType, String pemXPath) throws MRException, Exception {
        MRConfig amConfig = null;
        try {
            amConfig = MRConfig.getInstance(serviceName, objectType, log);
            String listName = amConfig.getParentNIDKey(log);

            String docNID = MRXMLUtils.getValue(actionEL, "NID", "").trim();
            String childXpath = MRXMLUtils.getValueXpath(actionEL, "PathSpec/In/ChildXPath", "").trim();
            String xQueryPrefix = Utils.decode( MRXMLUtils.getValueXpath(actionEL, "PathSpec/Where/XQueryPrefix", "").trim());
            Element dataElement = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");

            //where
            List<String> whereXPathList = MRXMLUtils.getValuesXpath(actionEL, "PathSpec/Where/WhereXPath");

            //todo: no need to send this pem xpath to server for add/replace. 
            //its ok to send to db for get and delete.
            //however pemxpath may be applied on db for partials later.
            //if(pemXPath !=null && pemXPath.trim().length()>0){
            //	whereXPathList.add(pemXPath);
            //}
            String[] arrWhereXpath = null;//EMUtils.getWhereXPathArray(WhereXPath, pemXPath);
            if (whereXPathList != null && whereXPathList.size() > 0) {
                arrWhereXpath = new String[whereXPathList.size()];
                whereXPathList.toArray(arrWhereXpath);
            }

            if (dataElement == null) {
                throw new MRException(DBExceptionCodes.DB_UPDATE_FAILED, "OperationData is empty");
            }
            timeout = Utils.getLongValue(MRXMLUtils.getValueXpath(actionEL, "//PathSpec//DBCallTimeOut", "").trim(), timeout); 
            // Add to Response
            Document doc = MRXMLDBUtils.replaceWithXqueryPrefix(timeout, request, log, db, listName, objectType, docNID, childXpath, xQueryPrefix, arrWhereXpath, dataElement);
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            if (doc != null && doc.getRootElement()!=null) {
                getActionResEL(responseElement, RequestID, resDoc, doc.getRootElement());
            } else {
                Element ResponseXML = getActionResEL(responseElement, RequestID);
                MRXMLUtils.addElement(ResponseXML, (Element) dataElement.clone());
            }
            return RESULT_SUCCESS;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_UPDATE_FAILED, "<error><request>" + objectType + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        } finally {
            amConfig = null;
        }
    }

    public int doDelete(Element actionEL, String responseElement, String objectType, String pemXPath) throws MRException, Exception {
        MRConfig amConfig = null;
        try {
            amConfig = MRConfig.getInstance(serviceName, objectType, log);
            String listName = amConfig.getParentNIDKey(log);

            String docNID = MRXMLUtils.getValue(actionEL, "NID", "").trim();
            String childXpath = MRXMLUtils.getValueXpath(actionEL, "PathSpec/In/ChildXPath", "").trim();
            String xQueryPrefix = Utils.decode( MRXMLUtils.getValueXpath(actionEL, "PathSpec/Where/XQueryPrefix", "").trim());
            //where
            List<String> whereXPathList = MRXMLUtils.getValuesXpath(actionEL, "PathSpec/Where/WhereXPath");
            if (pemXPath != null && pemXPath.trim().length() > 0) {
                whereXPathList.add(pemXPath);
            }
            String[] arrWhereXpath = null;//EMUtils.getWhereXPathArray(WhereXPath, pemXPath);
            if (whereXPathList != null && whereXPathList.size() > 0) {
                arrWhereXpath = new String[whereXPathList.size()];
                whereXPathList.toArray(arrWhereXpath);
            }
            timeout = Utils.getLongValue( MRXMLUtils.getValueXpath(actionEL, "//PathSpec//DBCallTimeOut", "").trim(), timeout); 
            // Add to Response
            Document doc = MRXMLDBUtils.deleteWithXqueryPrefix(timeout, request, log, db, listName, objectType, docNID, childXpath, xQueryPrefix, arrWhereXpath);
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            if (doc != null && doc.getRootElement()!=null) {
                getActionResEL(responseElement, RequestID, resDoc, doc.getRootElement());
            } else {
                Element ResponseXML = getActionResEL(responseElement, RequestID);
                if (objectType == null || objectType.trim().equals("")) {
                    objectType = "deletedObject";
                }
                if (docNID == null || docNID.trim().equals("")) {
                    docNID = "";
                }
                Element deleteEL = MRXMLUtils.addElement(ResponseXML, objectType);
                MRXMLUtils.addAttribute(deleteEL, "NID", docNID);
            }

            return RESULT_SUCCESS;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_DELETE_FAILED, "<error><request>" + objectType + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        } finally {
            amConfig = null;
        }
    }

    public int doGet(Element actionEL, String responseElement, String objectType, String pemXPath) throws MRException, Exception {
        MRConfig amConfig = null;
        try {
            amConfig = MRConfig.getInstance(serviceName, objectType, log);
            String listName = amConfig.getParentNIDKey(log);

            String type = amConfig.getType();
            if ((type != null) && type.equals("LIST")) {
                String ParentNID = MRXMLUtils.getValue(actionEL, "NID", "").trim();
                if (ParentNID.trim().equals(""))
                    return doGetList(actionEL, responseElement, objectType, pemXPath);
            }

            //doc id
            String docNID = MRXMLUtils.getValue(actionEL, "NID", "").trim();
            String childXPath = MRXMLUtils.getValueXpath(actionEL, "PathSpec/In/ChildXPath", "").trim();
            String xQueryPrefix = Utils.decode( MRXMLUtils.getValueXpath(actionEL, "PathSpec/Where/XQueryPrefix", "").trim() );

            //where
            List<String> whereXPathList = MRXMLUtils.getValuesXpath(actionEL, "PathSpec/Where/WhereXPath");
            if (pemXPath != null && pemXPath.trim().length() > 0) {
                whereXPathList.add(pemXPath);
            }
            String[] arrWhereXpath = null;//EMUtils.getWhereXPathArray(WhereXPath, pemXPath);
            if (whereXPathList != null && whereXPathList.size() > 0) {
                arrWhereXpath = new String[whereXPathList.size()];
                whereXPathList.toArray(arrWhereXpath);
            }

            String selectXMLStr = parseRequestXML(actionEL);

            //cursor:
            int startFrom = Utils.intValue(MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/StartFrom", "0").trim(), 0);
            int countToFetch = Utils.intValue(MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/CountToFetch", "").trim(), -2);

            //sort
            String[] sortByXpath = MRXMLUtils.getValuesXpathAsArr(actionEL, "PathSpec/Select/ListSpec/SortByXPathList/SortByXpath");
            String sortOrder = MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/SortOrder", MRXMLDBUtils.SORT_ASCENDING).trim();

            //set default for cursor
            if (countToFetch == -2) {
                countToFetch = -1; // default value
            }
            if (startFrom < 0) {
                startFrom = 0; // default value
            }
            // AddResponse
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            Element ResponseXML = getActionResEL(responseElement, RequestID);
            timeout = Utils.getLongValue( MRXMLUtils.getValueXpath(actionEL, "//PathSpec//DBCallTimeOut", "").trim(), timeout); 
            MRXMLDBUtils.readListWithXqueryPrefix(timeout, request, log, db, listName, objectType, docNID, childXPath, xQueryPrefix, arrWhereXpath, sortByXpath, sortOrder, selectXMLStr, ResponseXML, startFrom, countToFetch);
            return RESULT_SUCCESS;
        } catch (MRException ex) {
        	ex.printStackTrace();
            throw ex;
        } catch (Exception ex) {
            System.out.println("doget error objectType '" + objectType +"' ex ==null?"+ (ex==null));            
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_GET_FAILED, "<error><request>" + objectType + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        } finally {
            amConfig = null;
        }
    }

    // TBD : Policy check for getList - padmaja
    public int doGetList(Element actionEL, String responseElement, String objectType, String pemXPath) throws Exception {
        MRConfig amConfig = null;
        try {
            amConfig = MRConfig.getInstance(serviceName, objectType, log);
            String listName = amConfig.getParentNIDKey(log);

            String type = amConfig.getType();
            if ((type == null) || !type.equals("LIST")) {
                return doGet(actionEL, responseElement, objectType, null);
            }
            //doc id
            String docNID = MRXMLUtils.getValue(actionEL, "NID", "").trim();
            String childXPath = MRXMLUtils.getValueXpath(actionEL, "PathSpec/In/ChildXPath", "").trim();
            String xQueryPrefix = Utils.decode( MRXMLUtils.getValueXpath(actionEL, "PathSpec/Where/XQueryPrefix", "").trim() );
            
            //where
            List<String> WhereXPath = MRXMLUtils.getValuesXpath(actionEL, "PathSpec/Where/WhereXPath");
            if (pemXPath != null && pemXPath.trim().length() > 0) {
                WhereXPath.add(pemXPath);
            }
            String[] arrWhereConditionXpath = null;//EMUtils.getWhereXPathArray(WhereXPath, pemXPath);
            if (WhereXPath != null && WhereXPath.size() > 0) {
                arrWhereConditionXpath = new String[WhereXPath.size()];
                WhereXPath.toArray(arrWhereConditionXpath);
            }

            //select: todo: may be encription is needed or cdata

            String selectXMLStr = parseRequestXML(actionEL);

            //cursor:
            int startFrom = Utils.intValue(MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/StartFrom", "0").trim(), 0);
            int countToFetch = Utils.intValue(MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/CountToFetch", "").trim(), -2);

            //sort
            String[] sortByXpath = MRXMLUtils.getValuesXpathAsArr(actionEL, "PathSpec/Select/ListSpec/SortByXPathList/SortByXpath");
            String sortOrder = MRXMLUtils.getValueXpath(actionEL, "PathSpec/Select/ListSpec/SortOrder", MRXMLDBUtils.SORT_ASCENDING).trim();

            //set default for cursor
            if (countToFetch == -2) {
                countToFetch = -1; // default value
            }
            if (startFrom < 0) {
                startFrom = 0; // default value
            }
            // AddResponse
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            Element ResponseXML = getActionResEL(responseElement, RequestID);
            timeout = Utils.getLongValue( MRXMLUtils.getValueXpath(actionEL, "//PathSpec//DBCallTimeOut", "").trim(), timeout); 
            MRXMLDBUtils.readListWithXqueryPrefix(timeout, request, log, db, listName, objectType, docNID, childXPath, xQueryPrefix, arrWhereConditionXpath, sortByXpath, sortOrder, selectXMLStr, ResponseXML, startFrom, countToFetch);
            return RESULT_SUCCESS;

        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_GET_FAILED, "<error><request>" + objectType + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        } finally {
            amConfig = null;
        }
    }

    public int doReadQueryRequest(Element actionEL, String responseElement, Map<String, String> pemXPathList) throws Exception {
        String query = "";
        try {
            // AddResponse
            String RequestID = MRXMLUtils.getValue(actionEL, "RequestID", "").trim();
            Element ResponseXML = getActionResEL(responseElement, RequestID);
            //may be encription is needed or cdata
            //String query = MRXMLUtils.getValue2(actionEL, "").trim();
            query = Utils.decode(MRXMLUtils.getValue(actionEL, "Query", "").trim());
            if (query.equals("")) {
                throw new MRException(DBExceptionCodes.DB_GET_FAILED, "query is empty");
            }
            timeout = Utils.getLongValue( MRXMLUtils.getValueXpath(actionEL, "//PathSpec//DBCallTimeOut", "").trim(), timeout); 
            String response = MRXMLDBUtils.readQuery(timeout, request, log, db, query, pemXPathList, null);
            Element readQueryResponse = null;
            try {
                Document doc2 = MRXMLUtils.stringToDocument(response);
                readQueryResponse = (Element) doc2.getRootElement().detach();

            } catch (Exception ex) {
                //ex.printStackTrace();

            }
            if (readQueryResponse != null) {
                //Element el = MRXMLUtils.addElement(ResponseXML, "ReadQueryResult");
                //MRXMLUtils.addElement(el, readQueryResponse);
                MRXMLUtils.addElement(ResponseXML, readQueryResponse);
            } else {
                //MRXMLUtils.addElement(ResponseXML, "ReadQueryResult", response);
                MRXMLUtils.setValue(ResponseXML, response);
            }

            return RESULT_SUCCESS;
        } catch (MRException ex) {
            throw ex;
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_READ_QUERY_FAILED, "<error><request>" + query + "</request><errorMsg>" + ex.getMessage() + "</errorMsg></error>");
        }
    }

    ///////////// private methods ///////////////

    private Element getActionResEL(String elementName, String RequestID) throws Exception {
        return getActionResEL(elementName, RequestID, resDoc);
    }

    private String parseRequestXML(Element actionEL) {
        String selectXMLStr = null;
        try {
            Element selectXML = MRXMLUtils.getElementXpath(actionEL, "PathSpec/Select/ItemSpec/SelectXML");
            if (selectXML == null) {
                selectXML = MRXMLUtils.getElementXpath(actionEL, "PathSpec/Select/ItemSpec/SelectXQuery");
            }
            if (selectXML != null) {
                selectXML.setName("SelectXML");
                try {
                    selectXMLStr = MRXMLUtils.elementToString(selectXML);
                    int iPos1 = selectXMLStr.indexOf("<SelectXML>");
                    int iPos2 = selectXMLStr.lastIndexOf("</SelectXML>");
                    if (iPos2 > iPos1 && iPos1 != -1) {
                        selectXMLStr = Utils.decode(selectXMLStr.substring(iPos1 + "<SelectXML>".length(), iPos2));                        
                    } else {
                        selectXMLStr = "";
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            } else {
                selectXMLStr = MRXMLUtils.getValue(selectXML);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return selectXMLStr;
    }

    public static Element getActionResEL(String elementName, String RequestID, Document resDoc) throws Exception {
        Element addResEL = MRXMLUtils.addElement(resDoc.getRootElement(), elementName);
        MRXMLUtils.addElement(addResEL, "ResponseID", RequestID);
        return MRXMLUtils.addElement(addResEL, "ResponseXML");
    }

    public static Element getActionResEL(String elementName, String RequestID, Document resDoc, Element EL) throws Exception {
        Element addResEL = MRXMLUtils.addElement(resDoc.getRootElement(), elementName);
        MRXMLUtils.addElement(addResEL, "ResponseID", RequestID);
        if (EL != null) {
            EL.setName("ResponseXML");
            return MRXMLUtils.addElement(addResEL, (Element) EL.detach());
        }
        return MRXMLUtils.addElement(addResEL, "ResponseXML");
    }

    public String getDbObjectName(Element actionEL, MRRequest request, MRDB schema, MRLog log) throws Exception {
        throw new Exception("ObjectType is required.");
    }

    public Map<String, String> parseObjectTypesInQuery(Element actionEL) throws Exception {
        Map<String, String> list = new Hashtable<String, String>();
        try {

            String query = Utils.decode(MRXMLUtils.getValueXpath(actionEL, "Query", "").trim());
            //modify the query with pem 
            /*
              for each $b1 in #OBJ_objectname1_OBJ# 
              for each $b2 in #OBJ_objectname2_OBJ# 
              where $b1/abc=$b2/efg
              return 
              <result>
              {$b1}
              {$b2}
              </result>
              */
            int startPos = 0;
            startPos = getParsedObjectType(query, startPos, list);
            if (startPos != -1) {
                startPos = 0;
                while (startPos != -1) {
                    startPos = getParsedObjectType(query, startPos, list);
                }
            }
            return list;
        } catch (MRException ex) {
            ex.printStackTrace();
            throw new MRException(ex.getCode(), ex.getData());
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(DBExceptionCodes.DB_GET_FAILED, ex.getMessage());
        } finally {
        }
    }

    public static int getParsedObjectType(String query, int startPos, Map<String, String> list) throws Exception {
        //may be encription is needed or cdata
        int pos1 = query.indexOf(PREFIX_OBJ, startPos);
        if (pos1 == -1) {
            return -1;
        }
        int pos2 = query.indexOf(SUFFIX_OBJ, pos1);
        if (pos2 == -1) {
            throw new Exception("Invalid QueryRequest");
        }
        String objectType = query.substring(pos1 + PREFIX_OBJ.length(), pos2);
        list.put(objectType, "");
        return pos2 + 1;
    }

}
