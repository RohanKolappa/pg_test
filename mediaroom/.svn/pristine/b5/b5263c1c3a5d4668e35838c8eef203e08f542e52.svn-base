package com.ipvs.common.db;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.net.URL;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.ProcessArguments;
import com.ipvs.utils.Utils;

public class CreateRequestXmlUtil {
    public static final String xmlPath = "/com/ipvs/common/db/CreateRequestXmlUtil.xml";
    //private Document docConfig = null;
    private MRLog log = null;
    private Hashtable<String, ServiceSetting> settings = new Hashtable<String, ServiceSetting>();
    private Hashtable<String, ServiceSetting> settingsByServiceNames = new Hashtable<String, ServiceSetting>();
    private Hashtable<String, String> fileNameSchemes = new Hashtable<String, String>();
    private Hashtable<String, CreateTestXmlFiles> createTestXmlFiles = new Hashtable<String, CreateTestXmlFiles>();
    private Hashtable<String, RequestXML> requestXMLs = new Hashtable<String, RequestXML>();
    private static CreateRequestXmlUtil instance = null;
    //private long t0 = System.currentTimeMillis();
    private Document docDummy = null;
    private String folder ="";
    public static CreateRequestXmlUtil getInstance() throws Exception {
        if (instance == null) {
            instance = new CreateRequestXmlUtil();
            instance.initialize(xmlPath);
        }
        return instance;
    }
    
    private CreateRequestXmlUtil() {}
    
    public static void main(String[] args) {
        try {
            CreateRequestXmlUtil test = CreateRequestXmlUtil.getInstance();
            if (args.length > 0) {
                ProcessArguments argments = new ProcessArguments();
                argments.process(args);
                String folder = argments.getArgumentParam("folder");
                String sessionXmlPath = argments.getArgumentParam("xmlPath");
                if(sessionXmlPath !=null && !sessionXmlPath.equals("")){
                	MRLog log = MRLog.getInstance(MRLog.CONSOLE);
                	Document requestDoc = MRXMLUtils.loadXML(sessionXmlPath, log); 
                	String rootName = requestDoc.getRootElement().getName();
                	//The following if condition is for do not read the Template_file and response xmls's in session service
                	if (ServiceClient.REQUEST.equals(rootName)|| !rootName.endsWith(ServiceClient.REQUEST)){
                		return;
                	}
                	String requestName = rootName;
                	String dataElementName = requestName.substring(0, requestName.length() - ServiceClient.REQUEST.length());
                	Element dataEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName + "Data/" + dataElementName);
                	Document requestWithHeaderDoc = SessionServiceUtils.getRequestXML(requestName, log, dataEL);
                	MRXMLUtils.saveDocument(sessionXmlPath, requestWithHeaderDoc);
                	return;
                }
                if(folder !=null && !folder.trim().equals("")){
                    test.setFolder(folder);
                    Document docConfig = test.initialize(xmlPath);
                    test.createXMLs(docConfig);
                    return ;
                }
                String xmlName = argments.getArgumentParam("xmlName");
                if(xmlName !=null && !xmlName.equals("")){
                    Document doc = test.loadXMLResource(xmlName) ;   
                    if (args.length > 1) {
                        String saveXmlNamePath =args[1];
                        MRXMLUtils.saveDocument(saveXmlNamePath, doc);
                    }else{
                        System.out.print(MRXMLUtils.documentToString(doc)) ;
                    }
                    return ;
                }
            } 
            Document docConfig = test.initialize(xmlPath);
            test.createXMLs(docConfig);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    private void setFolder(String folder) {
       this.folder=folder;
        
    }

    public Document initialize(String xmlFilePath) throws Exception {
        log = MRLog.getInstance(MRLog.CONSOLE);
        Document docConfig = MRXMLUtils.loadXML(xmlFilePath, log);
        docDummy = MRXMLUtils.getDocument();
        if (docConfig != null) {
            List<?> list = MRXMLUtils.getList(docConfig, "Setting");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                ServiceSetting setting = new ServiceSetting(el);
                settings.put(setting.name, setting);
                if (settingsByServiceNames.containsKey(setting.serviceName) && setting.isDefaultSettingForServiceName)
                    settingsByServiceNames.put(setting.serviceName, setting);
                else
                    settingsByServiceNames.put(setting.serviceName, setting);
                if (!settingsByServiceNames.containsKey("default") && setting.isDefaultSetting) settingsByServiceNames.put("default", setting);
            }
            list = MRXMLUtils.getListXpath(docConfig, "//File_Name_Scheme");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                String name = MRXMLUtils.getAttributeValue(el, "name", "").trim();
                String scheme = MRXMLUtils.getAttributeValue(el, "scheme", "").trim();
                fileNameSchemes.put(name, scheme);
            }
            list = MRXMLUtils.getListXpath(docConfig, "//createTestXmlFiles");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                CreateTestXmlFiles createTestXmlFile = new CreateTestXmlFiles(el, log);
                createTestXmlFiles.put(createTestXmlFile.name, createTestXmlFile);
            }
            list = MRXMLUtils.getListXpath(docConfig, "//requestXML");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                RequestXML requestXML = new RequestXML(el, log, null);
                //System.out.println(" " + requestXML.dbObjectName + requestXML.suffix);
                requestXMLs.put(requestXML.dbObjectName + requestXML.suffix, requestXML);
            }
            
        }
        return docConfig;
    }
    
    public void createXMLs(Document docConfig) throws Exception {
        String dbObjectName = null;
        String requestXMLSuffix = null;
        String actionSuffix = null;
        createXMLs(docConfig, dbObjectName, requestXMLSuffix, actionSuffix);
    }
    
    public void createXMLs(Document docConfig, String dbObjectName, String requestXMLSuffix, String actionSuffix) throws Exception {
        Hashtable<String, Document> docList = new Hashtable<String, Document>();
        boolean bGetDocList = false;
        String xpath = getXpathFilter(dbObjectName, requestXMLSuffix);
        List<?> list = MRXMLUtils.getListXpath(docConfig, xpath);
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element el = (Element) iterator.next();
            RequestXML requestXML = new RequestXML(el, log, actionSuffix);
            createXML(docList, requestXML, actionSuffix, bGetDocList);
        }
    }
    
    ////////////// getRequest
    public Document getRequest(String objectType, String docNID,String childXpath, int startFrom, int countToFetch,
    		String[] arrwhereXPath, String[] sortByXpath, String sortOrder, String selectXml) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest(objectType);
        //action
        request.setAction(new ServiceRequestAction(ServiceRequestAction.ACTION_GET_REQUEST));
        request.getAction().setNID(docNID);
        request.getAction().setWhereXPathList(arrwhereXPath);
        request.getAction().setSortByXPathList(sortByXpath);
        request.getAction().setSortOrder(sortOrder);
        request.getAction().setChildXpath(childXpath);
        request.getAction().setSelectXml(selectXml);
        request.getAction().setStartFrom(startFrom + "");
        request.getAction().setCountToFetch(countToFetch + "");
        return request.getDoc();
    }
    
    ////////////// addRequest 
    public Document addRequest(String objectType, String docNID, Element element, String childXpath, String[] arrwhereXPath) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest(objectType);
        //ACTION_ADD_REQUEST
        request.setAction(new ServiceRequestAction(ServiceRequestAction.ACTION_ADD_REQUEST));
        request.getAction().setParentNID(docNID);
        request.getAction().setOperationDataEL(element);
        request.getAction().setWhereXPathList(arrwhereXPath);
        request.getAction().setChildXpath(childXpath);
        //request.getAction().setElementContextXpathAdd(parentElementContextXpath);
        return request.getDoc();
    }
    
    //////////////replaceRequest 
    public Document replaceRequest(String objectType, String docNID, Element element, String childXpath, String[] arrwhereXPath) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest(objectType);
        //ACTION_REPLACE_REQUEST
        request.setAction(new ServiceRequestAction(ServiceRequestAction.ACTION_REPLACE_REQUEST));
        request.getAction().setNID(docNID);
        request.getAction().setOperationDataEL(element);
        request.getAction().setWhereXPathList(arrwhereXPath);
        request.getAction().setChildXpath(childXpath);
        //request.getAction().setElementContextXpathReplace(replaceElementContextXpath);
        return request.getDoc();
    }
    
    //////////////deleteRequest 
    public Document deleteRequest(String objectType, String docNID, String childXpath, String[] arrwhereXPath) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest(objectType);
        //action
        request.setAction(new ServiceRequestAction(ServiceRequestAction.ACTION_DELETE_REQUEST));
        request.getAction().setNID(docNID);
        request.getAction().setChildXpath(childXpath);
        //request.getAction().setElementContextXpathDelete(deleteElementContextXpath);
        return request.getDoc();
    }
    
    //////////////queryRequest 
    public Document readQueryRequest(String xquery) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest("");
        //ACTION_QUERY_REQUEST
        request.setAction(new ServiceRequestAction(ServiceRequestAction.ACTION_QUERY_REQUEST));
        request.getAction().setQuery(xquery);
        return request.getDoc();
    }
    
    public Document readQueryRequest(File file) throws Exception {
        return readQueryRequest(FileUtils.readFile(file));
    }
    
    public Document readQueryRequest(URL file) throws Exception {
        return readQueryRequest(FileUtils.readFileURL(file));
    }
    
    public URL getReourceURL(String xqueryFilePath) throws Exception {
        URL url = null;
        try {
            url = MRDB.class.getResource(xqueryFilePath);
        } catch (Exception ex) {
            log.addLog(MRLog.INFO, MRLog.OK, "loadXMLResource url failed:" + ex.toString());
            throw new MRException(MRExceptionCodes.MRSCHEMA_XML_NOT_FOUND, MRXMLUtils.generateErrorXML("xsdFilePath", xqueryFilePath));
        }
        return url;
    }
    
    ////////////////////////////////////////////////////// ///////////////////////////////////////
    public static String resolveOldRequestXMLName(String xmlfilePath) throws Exception {
        if (xmlfilePath.indexOf("_BASE_") == -1) {
            return xmlfilePath;
        }
        File file = new File(xmlfilePath);
        String fileName = file.getName();
        String dbObjectName = "";
        String requestXMLSuffix = "_";
        String actionSuffix = "";
        String[] arr = fileName.split("_");
        if (arr.length > 1) {
            dbObjectName = arr[0];
            if (fileName.indexOf("ADD") != -1) {
                actionSuffix = "add";
            } else if (fileName.indexOf("UPDATE") != -1) {
                actionSuffix = "replace";
            } else if (fileName.indexOf("DELETE") != -1) {
                actionSuffix = "delete";
            } else if (fileName.indexOf("GET") != -1) {
                actionSuffix = "get";
            } else if (fileName.indexOf("GET_LIST") != -1) {
                actionSuffix = "getlist";
            } else if (fileName.indexOf("READ_QUERY") != -1) {
                actionSuffix = "readquery";                
            } else if (fileName.indexOf("REPLACE") != -1) {
                actionSuffix = "replace";             
            } else {
                return null;
            }
            String parentPath = "";
            xmlfilePath = xmlfilePath.replace('\\', '/');
            if (xmlfilePath.lastIndexOf("/") != -1) parentPath = xmlfilePath.substring(0, xmlfilePath.lastIndexOf("/"));
            return parentPath + File.separator + dbObjectName + requestXMLSuffix + actionSuffix + ".xml";
        }
        return xmlfilePath;
    }
    
    public Document loadXMLResource_oldName(String oldxmlName) throws Exception {
        File file = new File(oldxmlName);
        oldxmlName = file.getName();
        String dbObjectName = "";
        String requestXMLSuffix = "_";
        String actionSuffix = "";
        String[] arr = oldxmlName.split("_");
        if (arr.length > 1) {
            dbObjectName = arr[0];
            if (oldxmlName.indexOf("ADD") != -1) {
                actionSuffix = "add";
            } else if (oldxmlName.indexOf("UPDATE") != -1) {
                actionSuffix = "replace";
            } else if (oldxmlName.indexOf("DELETE") != -1) {
                actionSuffix = "delete";
            } else if (oldxmlName.indexOf("GET") != -1) {
                actionSuffix = "get";
            } else if (oldxmlName.indexOf("GET_LIST") != -1) {
                actionSuffix = "getlist";
            } else {
                return null;
            }
            return loadXMLResource(dbObjectName + requestXMLSuffix + actionSuffix);
        }
        return null;
    }
    
    public Document loadXMLResource(String xmlName) throws Exception {
        if (xmlName.indexOf("_BASE_") != -1) {
            xmlName = resolveOldRequestXMLName(xmlName);
            File file = new File(xmlName);
            xmlName = file.getName();
        }
        String[] arr = xmlName.split("_");
        String dbObjectName = "";
        String requestXMLSuffix = "";
        String actionSuffix = "";
        if (arr.length < 2) {
            throw new Exception("invalid name " + xmlName);
        }
        if (arr.length == 2) {
            dbObjectName = arr[0];
            requestXMLSuffix = "_";
            actionSuffix = arr[1];
            
        }
        if (arr.length == 3) {
            dbObjectName = arr[0];
            requestXMLSuffix = arr[1];
            actionSuffix = arr[2];
        }
        if (actionSuffix.indexOf('.') != -1) {
            actionSuffix = actionSuffix.substring(0, actionSuffix.lastIndexOf('.'));
        }
        boolean bGetDocList = true;
        Hashtable<String, Document> docList = new Hashtable<String, Document>();
        RequestXML requestXML = requestXMLs.get(dbObjectName + requestXMLSuffix);
        if (requestXML == null) {
            return null;
        }
        createXML(docList, requestXML, actionSuffix, bGetDocList);
        return docList.get(xmlName);
    }
    
    /////////////////////////////////
    private void createXML(Hashtable<String, Document> docList, RequestXML requestXML, String actionSuffix, boolean bGetDocList) throws Exception {
        //ServiceRequest  and header
        ServiceRequest request = createServiceRequest(requestXML.dbObjectName);
        //action
        request.setAction(new ServiceRequestAction());
        
        //ACTION_GET_REQUEST
        request.getAction().setNID("");
        request.getAction().setWhereXPathList(requestXML.whereXPathList);
        request.getAction().setSortByXPathList(requestXML.sortByXpathList);
        request.getAction().setSortOrder(requestXML.sortOrder);
        
        request.getAction().setSelectXml(requestXML.selectXml);
        request.getAction().setStartFrom(requestXML.startFrom);
        request.getAction().setCountToFetch(requestXML.countToFetch);
        
        //ACTION_READ_QUERY_REQUEST
        request.getAction().setQuery(requestXML.getReadQuery());
        
        //ACTION_REPLACE_REQUEST or ACTION_ADD_REQUEST or ACTION_DELETE_REQUEST
        request.getAction().setChildXpath(requestXML.childXpath);
        //request.getAction().setElementContextXpathReplace(requestXML.elementContextXpathReplace);
        //request.getAction().setElementContextXpathAdd(requestXML.elementContextXpathAdd);
        request.getAction().setNID("");
        
        //ACTION_DELETE_REQUEST
        //request.getAction().setElementContextXpathDelete(requestXML.elementContextXpathDelete);
        CreateTestXmlFiles createTestXmlFile = createTestXmlFiles.get(requestXML.createTestXmlFiles);
        if (actionSuffix != null && !actionSuffix.trim().equals("")) {
            TestXmlFile testXmlFile = createTestXmlFile.testXmlFiles.get(actionSuffix);
            request.getAction().getFilter().reset();
            request.getAction().getFilter().setFilter(testXmlFile.enableFilter);
            request.getAction().setAction(testXmlFile.requestAction);
            if (request.getAction().getFilter().isOperationDataEnabled()) {
                Document operationData = requestXML.getOperationData();
                if (operationData != null && operationData.getRootElement() != null) {
                    request.getAction().setOperationDataEL(operationData.getRootElement());
                }
            }
            saveTestXmlFile(docList, requestXML, actionSuffix, bGetDocList, request, testXmlFile);
        } else {
            
            Document operationData = requestXML.getOperationData();
            if (operationData != null && operationData.getRootElement() != null) {
                request.getAction().setOperationDataEL(operationData.getRootElement());
            }
            
            for (Enumeration<TestXmlFile> e = createTestXmlFile.testXmlFiles.elements(); e.hasMoreElements();) {
                TestXmlFile testXmlFile = e.nextElement();
                saveTestXmlFile(docList, requestXML, actionSuffix, bGetDocList, request, testXmlFile);
            }
        }
    }
    private void ensureFolder(String folder){
        File file = new File(folder);
        if(!file.exists())
            file.mkdirs();
    }
    private void saveTestXmlFile(Hashtable<String, Document> docList, RequestXML requestXML, String actionSuffix, boolean bGetDocList, ServiceRequest request, TestXmlFile testXmlFile) throws Exception {
        if (testXmlFile != null) {
            String xmlFile = getNameingScheme(testXmlFile.File_Name_Scheme, requestXML.suffix, testXmlFile.suffix, requestXML.dbObjectName);
            request.getAction().getFilter().reset();
            request.getAction().getFilter().setFilter(testXmlFile.enableFilter);
            request.getAction().setAction(testXmlFile.requestAction);
            
            ServiceSetting serviceSetting = settings.get(requestXML.settings);
            if (serviceSetting == null) {
                serviceSetting = getServiceSettingBySeriviceName(requestXML.dbObjectName);
            }
            if (!bGetDocList) {
                //create and save only if asked
                String testXmlFolder = serviceSetting.testXmlFolder;
                if(folder !=null && !folder.trim().equals("")){
                    testXmlFolder = folder;
                }
                ensureFolder(testXmlFolder);
                
                docList.put(xmlFile, docDummy);
                xmlFile = getFullPath(testXmlFolder + "/" + xmlFile + ".xml");
                System.out.println("saveDoc " + testXmlFile.requestAction + " " + testXmlFile.enableFilter + " " + xmlFile);
                request.saveDoc(xmlFile);
            } else {
                docList.put(xmlFile + ".xml", request.getDoc());
            }
        }
    }
    
    //////////////request 
    private ServiceRequest createServiceRequest(String objectType) throws Exception {
        String serviceName = MRConfig.getObjectServiceName(objectType);
        ServiceSetting serviceSetting = getServiceSettingBySeriviceName(objectType);
        ServiceRequest request = new ServiceRequest();
        request.setDbObjectName(objectType);
        request.setDataNodeName(serviceSetting.dataNodeName);
        request.setHandlerName(ServiceRequest.HANDLER_ROOT);
        //header 
        request.setHeader(new ServiceRequestHeader());
        request.getHeader().setClientData("clientData");
        request.getHeader().setLogLevel(0);
        request.getHeader().setRequestName(serviceSetting.requestName);
        request.getHeader().setServiceName(serviceName);
        return request;
    }
    
    private ServiceSetting getServiceSettingBySeriviceName(String objectType) throws Exception {
        String serviceName = MRConfig.getObjectServiceName(objectType);
        ServiceSetting serviceSetting = settingsByServiceNames.get(serviceName);
        if (serviceSetting == null) {
            serviceSetting = settingsByServiceNames.get("default");
        }
        return serviceSetting;
    }
    
    private String getXpathFilter(String dbObjectName, String requestXMLSuffix) {
        StringBuilder buf = new StringBuilder();
        buf.append("//object");
        if ((dbObjectName != null) && !dbObjectName.trim().equals("")) {
            buf.append("[@name='" + dbObjectName + "' and (not(@enable='false'))]");  
        } else {
            buf.append("[not(@enable='false')]");
        }
        buf.append("/requestXML");
        if (requestXMLSuffix != null && !requestXMLSuffix.trim().equals("")) {
            buf.append("[ @suffix='" + requestXMLSuffix + "' and (not(@enable='false'))]");
        } else {
            buf.append("[not(@enable='false')]");
        }
        return buf.toString();
    }
    
    private String getNameingScheme(String file_Name_Scheme, String data_typeSuffix, String testXmlFile, String dbObjectName) {
        StringBuilder xmlFile = new StringBuilder();
        String scheme = fileNameSchemes.get(file_Name_Scheme);
        if (scheme == null) scheme = "123";
        for (int index = 0; index < scheme.length(); index++) {
            char c = scheme.charAt(index);
            if (c == '1') {
                xmlFile.append("" + dbObjectName);
            }
            if (c == '2') {
                xmlFile.append("" + data_typeSuffix);
            }
            if (c == '3') {
                xmlFile.append("" + testXmlFile);
            }
        }
        return xmlFile.toString();
    }
    
    private String getFullPath(String xsdExtracUtilXmlPath) throws Exception {
        xsdExtracUtilXmlPath = xsdExtracUtilXmlPath.replace('\\', '/').trim();
        if (xsdExtracUtilXmlPath.startsWith("/")) {
            return getCurrentPath() + xsdExtracUtilXmlPath;
        }
        return xsdExtracUtilXmlPath;
    }
    
    private String getCurrentPath() throws Exception {
        File xmlFile = new File("tempFile.txt");
        if (!xmlFile.exists()) {
            BufferedWriter out = new BufferedWriter(new FileWriter(xmlFile, false));
            out.write("test");
            out.close();
        }
        String parentPath = xmlFile.getAbsolutePath().replace('\\', '/').trim();
        int iPos = parentPath.lastIndexOf('/');
        if (iPos != -1) {
            parentPath = parentPath.substring(0, iPos);
        }
        return parentPath;
    }
    
    /////////////////////////////// RequestXML  ////////////////////////////////////////////////////////////////////////////////////////
    static class CreateTestXmlFiles {
        /*
        <createTestXmlFiles name="createXmls">
            <testXmlFile requestAction="AddRequest" File_Name_Scheme="File_Name_Scheme1" suffix="add" enableFilter="AB" />
            <testXmlFile requestAction="ReplaceRequest" File_Name_Scheme="File_Name_Scheme1" suffix="replace" enableFilter="AB" />
            <testXmlFile requestAction="DeleteRequest" File_Name_Scheme="File_Name_Scheme1" suffix="delete" enableFilter="A" />
            <testXmlFile requestAction="GetRequest" File_Name_Scheme="File_Name_Scheme1" suffix="get" enableFilter="ABE" />
        </createTestXmlFiles>        
         */
        String name = "";
        Hashtable<String, TestXmlFile> testXmlFiles = new Hashtable<String, TestXmlFile>();
        
        CreateTestXmlFiles(Element EL, MRLog log) throws Exception {
            try {
                name = MRXMLUtils.getAttributeValue(EL, "name", "").trim();
                List<?> list = MRXMLUtils.getList(EL, "testXmlFile");
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    Element child = (Element) iterator.next();
                    TestXmlFile testXmlFile = new TestXmlFile(child, log);
                    testXmlFiles.put(testXmlFile.suffix, testXmlFile);
                }
            } catch (Exception ex) {
                throw ex;
            }
        }
    }
    
    static class TestXmlFile {
        String requestAction = "";
        String File_Name_Scheme = "";
        String suffix = "";
        String enableFilter = "";
        
        TestXmlFile(Element EL, MRLog log) throws Exception {
            try {
                suffix = MRXMLUtils.getAttributeValue(EL, "suffix", "").trim();
                File_Name_Scheme = MRXMLUtils.getAttributeValue(EL, "File_Name_Scheme", "").trim();
                requestAction = MRXMLUtils.getAttributeValue(EL, "requestAction", "").trim();
                enableFilter = MRXMLUtils.getAttributeValue(EL, "enableFilter", "").trim();
                
            } catch (Exception ex) {
                throw ex;
            }
        }
    }
    
    static class RequestXML {
        /*
        <object settings="AMSettings" name="AssetResource">
            <requestXML suffix="_" createTestXmlFiles="createBaseAllXmls">
                <get>
                    <whereXPathList>
                        <xpath>//AssetResource/Info</xpath>
                    </whereXPathList>
                    <In>
                        <xpath>./Info/Title</xpath>
                    </In>
                    <SortByXpathList>
                        <xpath>./Info/Title</xpath>
                    </SortByXpathList>
                    <SortOrder>true</SortOrder>
                    <selectXml>
                        <AssetResource>
                            ELEMENT_START_./info_ELEMENT_END
                        </AssetResource>
                    </selectXml>
                    <cursor>
                        <startFrom>0</startFrom>
                        <countToFetch>200</countToFetch>
                    </cursor>
                </get>
                <addReplace>
                    <whereXPathList>
                        <xpath>//AssetResource/Info</xpath>
                    </whereXPathList>
                    <elementContextXpathReplace></elementContextXpathReplace>
                    <elementContextXpathAdd></elementContextXpathAdd>
                    <operationDataXpathList>
                        <xpath bCopyFreeChilds="true" iCopyChildsToDepth="-1">//AssetResource/Info</xpath>
                    </operationDataXpathList>
                </addReplace>
                <delete>
                    <whereXPathList>
                        <xpath>//AssetResource/Info</xpath>
                    </whereXPathList>
                    <elementContextXpath></elementContextXpath>
                </delete>
            </requestXML>
        </object>
         */
        MRLog log = null;
        String settings = "";
        String dbObjectName = "";
        String suffix = "";
        String createTestXmlFiles = "";
        List<String> whereXPathList = new ArrayList<String>();
        List<String> sortByXpathList = new ArrayList<String>();
        String sortOrder=MRXMLDBUtils.SORT_ASCENDING;
        String selectXml = "";
        String startFrom = "";
        String countToFetch = "";
        String childXpath="";
        String readQueryType="";
        String readQuery="";
        
        //String elementContextXpathReplace = "";
        //String elementContextXpathAdd = "";
        //String elementContextXpathDelete = "";
        
        List<OperationDataXPath> operationDataXpathList = new ArrayList<OperationDataXPath>();
        Document addReplaceOperationData = null;
        //String xquery = "";
        
        RequestXML(Element EL, MRLog log, String action) throws Exception {
            this.log = log;
            try {
                if (action == null) action = "";
                action = action.toLowerCase();
                settings = MRXMLUtils.getAttributeValue(MRXMLUtils.getParentElement(EL), "settings", "").trim();
                dbObjectName = MRXMLUtils.getAttributeValue(MRXMLUtils.getParentElement(EL), "name", "").trim();
                
                suffix = MRXMLUtils.getAttributeValue(EL, "suffix", "").trim();
                createTestXmlFiles = MRXMLUtils.getAttributeValue(EL, "createTestXmlFiles", "").trim();
                
                //whereXPathList
                List<?> list = MRXMLUtils.getListXpath(EL, "./whereXPathList/xpath");
                for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                    Element child = (Element) iterator.next();
                    whereXPathList.add(MRXMLUtils.getValue(child));
                }
                childXpath = MRXMLUtils.getValueXpath(EL, "childXpath", "").trim();
                if (action.equals("") || action.contains("get")) {
                    
                    //cursor
                    startFrom = MRXMLUtils.getValueXpath(EL, "./get/cursor/startFrom", "").trim();
                    countToFetch = MRXMLUtils.getValueXpath(EL, "./get/cursor/countToFetch", "").trim();
                    //selectXml
                    Element selectXmlEL = MRXMLUtils.getElementXpath(EL, "./get/selectXml");
                    if (selectXmlEL != null) {
                        selectXmlEL = MRXMLUtils.getFirstChild(selectXmlEL);
                        if (selectXmlEL != null) {
                            selectXml = MRXMLUtils.elementToString(selectXmlEL);
                        } else {
                        	selectXml =  MRXMLUtils.getValue2(selectXmlEL,"");
                        }
                    }
                    
                    //SortByXpathList
                    list = MRXMLUtils.getListXpath(EL, "./get/sortByXpathList/xpath");
                    for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                        Element child = (Element) iterator.next();
                        sortByXpathList.add(MRXMLUtils.getValue(child));
                    }
                    sortOrder = MRXMLUtils.getValueXpath(EL, "./get/sortOrder", "").trim(); 
                }
                if (action.equals("") || action.contains("add") || action.contains("replace")) {
                    //elementContextXpathReplace = MRXMLUtils.getValueXpath(EL, "./addReplace/elementContextXpathUpdate", "").trim();
                    //elementContextXpathAdd = MRXMLUtils.getValueXpath(EL, "./addReplace/elementContextXpathAdd", "").trim();
                    if(action.contains("add")){
                    	childXpath = MRXMLUtils.getValueXpath(EL, "./addReplace/parentContextXpath", "").trim();
                    }
                    //whereXPathList
                    list = MRXMLUtils.getListXpath(EL, "./addReplace/whereXPathList/xpath");
                    for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                        Element child = (Element) iterator.next();
                        whereXPathList.add(MRXMLUtils.getValue(child));
                    }
                    
                    list = MRXMLUtils.getListXpath(EL, "./addReplace/operationDataXpathList/xpath");
                    for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                        Element child = (Element) iterator.next();
                        operationDataXpathList.add(new OperationDataXPath(child, log));
                    }
                    
                    /*list = MRXMLUtils.getListXpath(EL, "./addReplace/operationDataXpathList/xpath");
                    if (list.size() > 0) {
                        operationData = getOperationData(list);
                    } else {
                        operationData = MRConfig.getInstance(dbObjectName, log).getXmlRepresentation(log).getDoc();
                    }*/
                }
                if (action.equals("") || action.contains("delete")) {
                    //whereXPathList
                    list = MRXMLUtils.getListXpath(EL, "./delete/whereXPathList/xpath");
                    for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                        Element child = (Element) iterator.next();
                        whereXPathList.add(MRXMLUtils.getValue(child));
                    }
                    //elementContextXpathDelete = MRXMLUtils.getValueXpath(EL, "./delete/elementContextXpathDelete", "").trim();
                }
                if (action.equals("") || action.contains("readquery")) {
                	//file/resource/text
                	readQueryType = MRXMLUtils.getValueXpath(EL, "./readquery/queryType", "text").trim();
                	readQuery = MRXMLUtils.getValueXpath(EL, "./readquery/query", "").trim();
                }
            } catch (Exception ex) {
                //ex.printStackTrace();
                throw ex;
            }
        }
        private String getReadQuery() throws Exception {
        	//file/resource/text
        	if(readQueryType.equals("text")){
        		return readQuery;
        	}
        	if(readQueryType.equals("resource")){
        		return FileUtils.readFileResource(readQuery, this.getClass());
        	}
        	if(readQueryType.equals("text")){
        		return FileUtils.readFileResource(readQuery, this.getClass());
        	} 
        	return "";
        }
        private Document getOperationData() throws Exception {
            try {
                if (operationDataXpathList.isEmpty()) {
                    return MRConfig.getInstance(dbObjectName, log).getXmlRepresentation(log).getDoc();
                } else {
                    //if(addReplaceOperationData!=null)
                    //    return addReplaceOperationData;
                    Document docRet = null;
                    StringBuffer buf = new StringBuffer();
                    XmlRepresentation rep = MRConfig.getInstance(dbObjectName, log).getXmlRepresentation(log);
                   
                    Element objEL = null;
                    for (Iterator<OperationDataXPath> iterator = operationDataXpathList.iterator(); iterator.hasNext();) {
                        OperationDataXPath operationDataXPath = iterator.next();
                        try {
                            buf.append("\n" + operationDataXPath.xpathValue);
                            if (operationDataXPath.isRemoveChild) {
                                if (docRet != null) {
                                    objEL = MRXMLUtils.getElementXpath(docRet, operationDataXPath.xpathValue);
                                    if (objEL != null) {
                                        objEL.detach();
                                        continue;
                                    }
                                }
                            }
                            docRet = rep.getElementFromXpath(operationDataXPath.iCopyChildsToDepth, docRet, operationDataXPath.xpathValue, log);
                            //docRet = MRConfig.getInstance(dbObjectName, log).getXmlRepresentation(log).getElementFromXpath(operationDataXPath.bCopyFreeChilds, operationDataXPath.CopyChildsToDepth, docRet, operationDataXPath.xpathValue, null, null, log, null, null, null);
                        } catch (Exception ex) {
                            System.out.println("ERROR getXml : " + dbObjectName + " " + operationDataXPath.xpathValue);
                            System.out.println("ERROR getXml : " + buf.toString());
                            ex.printStackTrace();
                        }
                    }
                    if ((docRet == null) || (docRet.getRootElement() == null)) {
                        System.out.println("ERROR getXml : buf.toString() " + buf.toString());
                    }
                    //set 
                    //addReplaceOperationData=docRet;
                    //return addReplaceOperationData;
                    return docRet;
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            return null;
        }
        
    }
    
    /////////////////////////////// Setting  ////////////////////////////////////////////////////////////////////////////////////////
    static class OperationDataXPath {
        int iCopyChildsToDepth;
        boolean isRemoveChild;
        String xpathValue = "";
        OperationDataXPath(Element EL, MRLog log) throws Exception {
            xpathValue = MRXMLUtils.getValue2(EL, "");
            iCopyChildsToDepth = Utils.intValue(MRXMLUtils.getAttributeValue(EL, "iCopyChildsToDepth", "1"), 1);
            isRemoveChild = Utils.booleanValue(MRXMLUtils.getAttributeValue(EL, "removeChild", "false"), false);
        }
        
    }
    
    /////////////////////////////// Setting  ////////////////////////////////////////////////////////////////////////////////////////
    static class ServiceSetting {
        /*
            <Setting name="AMSettings">
                <objectXmlFolder>src/com/ipvs/xsd/objectXml</objectXmlFolder>
                <testXmlFolder>src/com/ipvs/assetadminservice/xml</testXmlFolder>
                <testScriptXmlFolder>src/com/ipvs/assetadminservice/testscript</testScriptXmlFolder>
                <templateXmlFolder>src/com/ipvs/assetadminservice/testscript</templateXmlFolder>
                <serviceName>assetadminservice</serviceName>
                <requestName>AssetManagementRequest</requestName>
                <dataNodeName>AssetManagementRequestData</dataNodeName>
                <startFrom>0</startFrom>
                <countToFetch>-1</countToFetch>
                <isDefaultSettingForServiceName>true</isDefaultSettingForServiceName>
                <isDefaultSetting>true</isDefaultSetting>
            </Setting>
         */
        String name = "";
        String objectXmlFolder = "";
        String testXmlFolder = "";
        String testScriptXmlFolder = "";
        String templateXmlFolder = "";
        String serviceName = "";
        String requestName = "";
        String dataNodeName = "";
        String startFrom = "";
        String countToFetch = "";
        boolean isDefaultSettingForServiceName = true;
        boolean isDefaultSetting = true;
        
        ServiceSetting(Element EL) {
            try {
                name = MRXMLUtils.getAttributeValue(EL, "name", "").trim();
                
                objectXmlFolder = MRXMLUtils.getValue(EL, "objectXmlFolder", "").trim();
                
                testXmlFolder = MRXMLUtils.getValue(EL, "testXmlFolder", "").trim();
                testScriptXmlFolder = MRXMLUtils.getValue(EL, "testScriptXmlFolder", "").trim();
                templateXmlFolder = MRXMLUtils.getValue(EL, "templateXmlFolder", "").trim();
                
                requestName = MRXMLUtils.getValue(EL, "requestName", "").trim();
                dataNodeName = MRXMLUtils.getValue(EL, "dataNodeName", "").trim();
                serviceName = MRXMLUtils.getValue(EL, "serviceName", "").trim();
                
                startFrom = MRXMLUtils.getValue(EL, "startFrom", "").trim();
                countToFetch = MRXMLUtils.getValue(EL, "countToFetch", "").trim();
                isDefaultSettingForServiceName = Utils.booleanValue(MRXMLUtils.getValue(EL, "isDefaultSettingForServiceName", "false").trim(), false);
                isDefaultSetting = Utils.booleanValue(MRXMLUtils.getValue(EL, "isDefaultSetting", "false").trim(), false);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        
    }
    
    private static void showUsage() {
        // "AssetResource" "_BASE_ALL" "_DELETE_NID"
        //-service "assetadminservice" -dir "./test/xml2" "AssetResource" "_BASE_ALL" "_DELETE_NID"
        System.err.println("\nUsage:");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil [-xml script_setting_xml] [-dir ./temp/xml ] [ -service \"assetadminservice\" ] [ -file  \"./temp/test1.xml\" ] [ dbObjectName OperationDataSuffix TestXmlFileSuffix] ");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil -xml \"/com/ipvs/test/xml/AMTestScript.xml\" -dir \"./test/xml\" -service \"assetadminservice\"  \"AssetResource\" \"_\" \"delete\" ");
        
        System.err.println("\nUsage Example 1 :");
        System.err.println("to create all xmls; using default script_setting_xml .");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil ");
        
        System.err.println("\nUsage Example 2 :");
        System.err.println("to create all xmls; using specific script_setting_xml.  ");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil -xml \"/com/ipvs/test/xml/AMTestScript.xml\"  ");
        
        System.err.println("\nUsage Example 3 :");
        System.err.println("to overrite the xml out put folder from AMTestScript.xml.");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil -dir \"./test/xml\" ");
        
        System.err.println("\nUsage Example 4 :");
        System.err.println("to restict xml creation to specific service e.g assetadminservice.");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil -service \"assetadminservice\" ");
        
        System.err.println("\nUsage Example 5 :");
        System.err.println(" to create one Xml e.g 'AssetResource_add.xml' ");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil \"AssetResource\" \"_\" \"add\"");
        
        System.err.println("\nUsage Example 6 :");
        System.err.println(" to create one Xml e.g 'AssetResource_delete.xml' ");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil \"AssetResource\" \"_\" \"delete\"");
        
        System.err.println("\nUsage Example 7 :");
        System.err.println(" to create one Xml and save it './temp/test1.xml' ");
        System.err.println("com.ipvs.common.db.CreateRequestXmlUtil -file  \"./temp/test1.xml\" \"AssetResource\" \"_\" \"add\"");
    }
    
}

/*public Document getRequest(String objectType, String docNID, String selectXml) throws Exception {
String[] arrwhereXPath = null;
String[] arrSortByXpath = null;
return getRequest(objectType, docNID, selectXml, arrSortByXpath, 0, -1, arrwhereXPath);
}

public Document getRequest(String objectType, String docNID, String selectXml, String SortByXpath, int startFrom, int countToFetch, String whereXPath) throws Exception {
String[] arrwhereXPath = { whereXPath };
String[] arrSortByXpath = { SortByXpath };
return getRequest(objectType, docNID, selectXml, arrSortByXpath, startFrom, countToFetch, arrwhereXPath);
}

public Document getRequest(String objectType, String docNID, String selectXml, String[] SortByXpath, int startFrom, int countToFetch, String whereXPath) throws Exception {
String[] arrwhereXPath = { whereXPath };
return getRequest(objectType, docNID, selectXml, SortByXpath, startFrom, countToFetch, arrwhereXPath);
}*/

/*public Document addRequest(String objectType, Element element) throws Exception {
    String parentElementContextXpath = null;
    String docNID = null;
    return addRequest(objectType, docNID, element, parentElementContextXpath);
}

public Document addRequest(String objectType, String docNID, Element element) throws Exception {
    String parentElementContextXpath = null;
    return addRequest(objectType, docNID, element, parentElementContextXpath);
}

public Document addRequest(String objectType, String docNID, Element element, String parentElementContextXpath) throws Exception {
    String[] arrwhereXPath = null;
    return addRequest(objectType, docNID, element, parentElementContextXpath, arrwhereXPath);
}

public Document addRequest(String objectType, String docNID, Element element, String parentElementContextXpath, String whereXpath) throws Exception {
    String[] arrwhereXPath = { whereXpath };
    return addRequest(objectType, docNID, element, parentElementContextXpath, arrwhereXPath);
}
*/
/*public Document replaceRequest(String objectType, String docNID, Element element) throws Exception {
    String replaceElementContextXpath = null;
    return replaceRequest(objectType, docNID, element, replaceElementContextXpath);
}

public Document replaceRequest(String objectType, String docNID, Element element, String replaceElementContextXpath) throws Exception {
    String[] arrwhereXPath = null;
    return replaceRequest(objectType, docNID, element, replaceElementContextXpath, arrwhereXPath);
}

public Document replaceRequest(String objectType, String docNID, Element element, String replaceElementContextXpath, String whereXpath) throws Exception {
    String[] arrwhereXPath = { whereXpath };
    return replaceRequest(objectType, docNID, element, replaceElementContextXpath, arrwhereXPath);
}*/

/* 
 public Document deleteRequest(String objectType, String docNID) throws Exception {
     String deleteElementContextXpath = null;
     String[] arrwhereXPath = null;
     return deleteRequest(objectType, docNID, deleteElementContextXpath, arrwhereXPath);
 }
 public Document deleteRequest(String objectType, String docNID, String whereXpath) throws Exception {
     String deleteElementContextXpath = null;
     String[] arrwhereXPath = { whereXpath };
     return deleteRequest(objectType, docNID, deleteElementContextXpath, arrwhereXPath);
 } 
 public Document deleteRequest(String objectType, String docNID, String[] arrwhereXPath) throws Exception {
     String deleteElementContextXpath = null;
     return deleteRequest(objectType, docNID, deleteElementContextXpath, arrwhereXPath);
 } 
 public Document deleteRequest(String objectType, String docNID, String deleteElementContextXpath, String whereXpath) throws Exception {
     String[] arrwhereXPath = { whereXpath };
     return deleteRequest(objectType, docNID, deleteElementContextXpath, arrwhereXPath);
 } */
