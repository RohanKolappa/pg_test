package com.ipvs.common.service;


import java.util.LinkedList;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.MRRequestI;
import com.ipvs.utils.Utils;


/**
 * The request implementation for all services
 * Each service will have service specific messages which implement the MRMessageI
 * @author miyer
 *
 */
public class MRRequest implements MRRequestI {
    public static final int STATE_EXCEPTION = 0;
    public static final int STATE_NONE = 1;
    public static final int STATE_ADDED = 2;
    public static final int STATE_CHECKINGPOLICY = 3;
    public static final int STATE_POLICYCHECKED = 4;
    public static final int STATE_AUTHORIZING = 5;
    public static final int STATE_AUTHORIZED = 6;
    public static final int STATE_PROCESSING = 7;
    public static final int STATE_DONE = 8;
    private String userJID;
    private String userAgentName;
    private String appServerJID;
    private String resourceGroup;
    private int logLevel;
    private String NID;
    private String serviceName;    private String requestName;
    private MRMessage message;
    private int state;
    private MRAuthorization auth;
    private Document result;
    private MRException exception;
    private MRRequestCache cache;
    private Document loggingDocument = null;
    private boolean logEvent = true;
    private LinkedList<AuditEvent> auditEventList = null;

    // static Hashtable<String, MRRequest> requestDB = null;
    boolean superUser;
    private long startTime = 0;
    String clientData = null;

    public MRRequest(String userJID, String userAgentName, String appServerJID, int logLevel, String nid, String serviceName, String requestName, MRMessage message, String resourceGroup, String clientData) {
        this.userJID = userJID;
        this.userAgentName = userAgentName;
        this.appServerJID = appServerJID;
        this.logLevel = logLevel;
        this.NID = nid;
        this.serviceName = serviceName;
        this.requestName = requestName;
        this.message = message;
        this.resourceGroup = resourceGroup;
        this.clientData = clientData;

        this.state = MRRequest.STATE_ADDED;
        this.auth = null;
        this.result = null;
        this.exception = null;
        this.superUser = false;
        this.startTime = System.currentTimeMillis();
        this.cache = null;
    }
    public MRRequest clone(){
        return new MRRequest(this.userJID, this.userAgentName, this.appServerJID, this.logLevel, this.NID, this.serviceName, this.requestName, this.message, this.resourceGroup, this.clientData);
    }
    public String getClientData() {
        return clientData;
    }

    public void setClientData(String clientData) {
        this.clientData = clientData;
    }

    public long getDuration() {
        return System.currentTimeMillis() - this.startTime;
    }

    public void setSuperUser(boolean superUser) {
        this.superUser = superUser;
    }

    public void clearRequest() {
        this.state = MRRequest.STATE_NONE;
        this.auth = null;
        this.result = null;
        this.exception = null;
        this.message = null;
    }

    public String getUserJID() {
        return this.userJID;
    }

    public String getUserAgentName() {
        return this.userAgentName;
    }

    public String getAppServerJID() {
        return this.appServerJID;
    }

    public String getResourceGroup() {
        return this.resourceGroup;
    }

    public int getLogLevel() {
        return logLevel;
    }

    public void setLogLevel(int logLevel) {
        this.logLevel = logLevel;
    }

    public String getNID() {
        return NID;
    }

    public String getServiceName() {
        return serviceName;
    }

    public String getRequestName() {
        return requestName;
    }

    public void setRequestType(String requestName) {
        this.requestName = requestName;
    }

    public MRMessage getMessage() {
        return message;
    }

    public int getState() {
        return state;
    }

    public void setState(int state) {
        this.state = state;
    }

    public MRAuthorization getAuth() {
        return auth;
    }

    public void setAuth(MRAuthorization auth) {
        this.auth = auth;
    }

    public Document getResult() {
        return result;
    }

    public void setResult(Document result) {
        this.result = result;
    }

    public MRException getException() {
        return exception;
    }

    public void setException(MRException exception) {
        this.exception = exception;
    }

    /*
    public static Hashtable<String,MRRequest> getRequestDB() {
            if(MRRequest.requestDB == null)
                    MRRequest.requestDB = new Hashtable<String, MRRequest>();
            return MRRequest.requestDB;
    }
    */
    public static void addRequest(String requestNID, MRRequest request) throws Exception {
        // getRequestDB().put(requestNID, request);
    }

    /*
    public static MRRequest getRequest(String requestNID) throws Exception {
            return getRequestDB().get(requestNID);
    }
    */
    public static void deleteRequest(String requestNID) {
        // getRequestDB().remove(requestNID);
    }

    public static void updateRequest(MRRequest request) throws Exception {
        // Nothing to be done
    }

    public String getRequestDataXPath() {
        return "//" + requestName + "Data";
    }

    public String getResponseDataName() {
        // return requestName.replace("Request", "Response") + "Data";
    	return MRRequest.getResponseName(requestName) + "Data";
    }

    public boolean isSuperUser() {
        return this.superUser;
    }

    public Document createResponseData(String name, String value) throws Exception {
        String rdname = getResponseDataName();

        return MRXMLUtils.stringToDocument("<" + rdname + ">" + "<" + name + ">" + value + "</" + name + ">" + "</" + rdname + ">");
    }
    
    public Document createResponseUsingRequestRootName(Element element) throws Exception {
    	Element requestRootNameEl = MRXMLUtils.newElement(MRRequest.getRequestRootName(this.getRequestName()));
    	if(element!=null) {
    		MRXMLUtils.addElement(requestRootNameEl,element);
    	}
        return createResponseData(requestRootNameEl);
    }

    public Document createResponseData(Element element) throws Exception {
        String rdname = getResponseDataName();
        Document doc = MRXMLUtils.stringToDocument("<" + rdname + ">" + "</" + rdname + ">");
        MRXMLUtils.addElement(doc.getRootElement(),element);

        return doc;
    }
    
    public Document createResponseData() throws Exception {
        String rdname = getResponseDataName();

        return MRXMLUtils.stringToDocument("<" + rdname + ">" + "</" + rdname + ">");
    }

    public static String getResponseName(String requestName) {
        if ((requestName == null) || requestName.isEmpty()) {
            return null;
        }

    	int index = requestName.lastIndexOf(ServiceClient.REQUEST);
    	if(index == -1)
    		return requestName;
    	
    	return requestName.substring(0, index) + ServiceClient.RESPONSE;
    }

    public static MRRequest createRequest(String userJID) throws Exception {
        Document doc = MRXMLUtils.stringToDocument("<request/>");

        return new MRRequest(userJID, null, null, MRLog.INFO, "requestNID", "serviceName", "requestName", new MRMessage(doc), "", "");
    }

    public static MRRequest createRequest(String userJID, int logLevel, String nid, String serviceName, String requestName, MRMessage message, String resourceGroup) {
        return new MRRequest(userJID, null, null, logLevel, nid, serviceName, requestName, message, resourceGroup, "");
    }

    public static MRRequest createRequest(String userJID, int logLevel, String nid, String serviceName, String requestName, MRMessage message, String resourceGroup, String appServerAgentJID) {
        return new MRRequest(userJID, null, appServerAgentJID, logLevel, nid, serviceName, requestName, message, resourceGroup, "");
    }

    public static MRRequest createRequest(String userJID, int logLevel, String nid, String serviceName, String requestName, MRMessage message, String resourceGroup, String appServerAgentJID, String userAgentName, String clientData) {
        return new MRRequest(userJID, userAgentName, appServerAgentJID, logLevel, nid, serviceName, requestName, message, resourceGroup, clientData);
    }

    public static MRRequest createRequest(String userJID, String appServerAgentJID)
        throws Exception {
        Document doc = MRXMLUtils.stringToDocument("<request/>");

        return new MRRequest(userJID, null, appServerAgentJID, MRLog.INFO, "requestNID", "serviceName", "requestName", new MRMessage(doc), "", "");
    }

    public static MRRequest createRequest(MRRequest request, Document requestDoc, String requestName, String serviceName)
        throws Exception {
    	MRRequest newRequest = new MRRequest(request.getUserJID(), request.getUserAgentName(), request.getAppServerJID(), request.getLogLevel(), request.getNID(), serviceName, requestName, new MRMessage(requestDoc), request.getResourceGroup(), request.getClientData());
    	newRequest.setSuperUser(request.isSuperUser());
        
    	return newRequest;
    }
    
    public static MRRequest createRequest(MRRequest request, Document requestDoc, String requestName, String serviceName,String clientData)
    throws Exception {
    	MRXMLUtils.setValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA,clientData);         
        return new MRRequest(request.getUserJID(), request.getUserAgentName(), request.getAppServerJID(), request.getLogLevel(), request.getNID(), serviceName, requestName, new MRMessage(requestDoc), request.getResourceGroup(),clientData);
    }
    public static MRRequest createRequest(String userJID, MRRequest request, Document requestDoc, String requestName, String serviceName,String clientData)
    	    throws Exception {
    	    	MRXMLUtils.setValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA,clientData);         
    	        return new MRRequest(userJID, request.getUserAgentName(), request.getAppServerJID(), request.getLogLevel(), request.getNID(), serviceName, requestName, new MRMessage(requestDoc), request.getResourceGroup(),clientData);
    	    }  
    public static MRRequest createRequest(String userJID, Document doc, String appServerJID) throws Exception {
        int logLevel = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.LOG_LEVEL,""),0);
        String serviceName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH, MRConst.SERVICE_NAME,"");
        String requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME,"");
        MRMessage message = new MRMessage(doc);
        String resourceGroup = null;
        String requestNID = "";
        MRRequest request = MRRequest.createRequest(userJID, logLevel, requestNID, serviceName, requestName, message, resourceGroup, appServerJID);

        return request;
    }
    public static MRRequest createRequest(String userJID, Document doc) throws Exception {
    	return MRRequest.createRequest(userJID, doc, null);
    }

	public Document marshalRequestObject(MRLog log) throws MRException,Exception{
		return this.marshalRequestObject(log, "");
	}
	
	public Document marshalRequestObject(MRLog log, String responseHandlerName) throws MRException,Exception{
		String filePath = "/com/ipvs/common/service/MRRequest.xml";
		Document mrRequestDoc = MRXMLUtils.loadXML(filePath, log);
		MRXMLUtils.setValueXpath(mrRequestDoc,"//requestNID", getNID());
		MRXMLUtils.setValueXpath(mrRequestDoc,"//userJID", getUserJID());
		MRXMLUtils.setValueXpath(mrRequestDoc,"//useragentname", getUserAgentName());
		MRXMLUtils.setValueXpath(mrRequestDoc,"//appserveragentJID", getAppServerJID());
		MRXMLUtils.setValueXpath(mrRequestDoc,"//admin", String.valueOf(isSuperUser()));
		MRXMLUtils.setValueXpath(mrRequestDoc,"//responseHandlerName", responseHandlerName);
		if(getMessage().getDocument() !=null  && getMessage().getDocument().getRootElement() !=null) {
			MRXMLUtils.addElement(MRXMLUtils.getElementXpath(mrRequestDoc,"//requestDoc"),(Element)getMessage().getDocument().getRootElement().clone());
		}
		return mrRequestDoc;		
	}
	
	

    public static MRRequest unMarshalRequestObject(Document mrRequestDoc, Document requestDoc, String resourceGroup, MRLog log)
        throws MRException, Exception {
    	String userJID = MRXMLUtils.getValueXpath(mrRequestDoc, "//userJID", "");
        String requestNID = MRXMLUtils.getValueXpath(mrRequestDoc, "//requestNID", "");
        String requestName = MRXMLUtils.getAttributeValueXpath(mrRequestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
        String serviceName = MRXMLUtils.getAttributeValueXpath(mrRequestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
        String clientData = MRXMLUtils.getValueXpath(mrRequestDoc, "//ClientData", "");
        String userAgentName = MRXMLUtils.getValueXpath(mrRequestDoc, "//useragentname", "");
        String appserverAgentJID = MRXMLUtils.getValueXpath(mrRequestDoc, "//appserveragentJID", "");
        boolean admin = Utils.getBoolean(MRXMLUtils.getValueXpath(mrRequestDoc, "//admin", ""), false);
        int logLevel = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(mrRequestDoc,MRConst.HEADER_XPATH,MRConst.LOG_LEVEL, ""), MRLog.INFO);
        MRRequest request = new MRRequest(userJID, userAgentName, appserverAgentJID, logLevel, requestNID, serviceName, requestName, new MRMessage(requestDoc), resourceGroup, clientData);
        request.setSuperUser(admin);
        return request;
    }  
    
    public static MRRequest unMarshalRequestObject(Document mrRequestDoc, String resourceGroup, MRLog log) throws MRException, Exception {
    	Element requestEl = MRXMLUtils.getElementXpath(mrRequestDoc, "//requestDoc");
    	Document requestDoc = MRXMLUtils.elementToDocument((Element)MRXMLUtils.getFirstChild(requestEl).clone());
    	return unMarshalRequestObject(mrRequestDoc, requestDoc,resourceGroup, log);
    }
    

    //Returns the request name from given handler class. 
    //Instead of creating constant for each handler. It will return the request name from class name.
    public static String getRequestName(String requestHandlerClassName) {
        if (requestHandlerClassName == null) {
            return null;
        }
        
        String className = Utils.getClassName(requestHandlerClassName);

        String handler = "Handler";
        int indexOfHandler = className.indexOf(handler);

        if (indexOfHandler == -1) {
            return null;
        }

        return className.substring(0,indexOfHandler);
    }
    
    public void setCache(MRRequestCache cache) {
    	this.cache = cache;
    }
    
    public MRRequestCache getCache() {
    	return this.cache;
    }
    
    public void setOperatorGroups(Element operator) {
    	if(cache == null) {
    		setCache(new MRRequestCache());
    	}
    	String key = "OperatorGroups" + "." + getUserJID();
    	if(operator!=null) {
    		this.cache.put(key,operator.clone());
    	}
    }
    
    public Element getOperatorGroups(){
    	String key = "OperatorGroups" + "." +getUserJID();
        
    	if(cache == null || !cache.containsKey(key)) {
    		return null;
    	}
    	return (Element)cache.get(key);
    }
    public static String getRequestRootName(String requestName) {
    	if(!requestName.endsWith(ServiceClient.REQUEST)){
    		return requestName;
    	}
    	return requestName.substring(0, requestName.length() - ServiceClient.REQUEST.length());
    }
	/**
	 * @return the loggingDocument
	 */
	public Document getLoggingDocument() {
		return loggingDocument;
	}
	/**
	 * @param loggingDocument the loggingDocument to set
	 */
	public void setLoggingDocument(Document loggingDocument) {
		this.loggingDocument = loggingDocument;
	}
	/**
	 * @return the logEvent
	 */
	public boolean logEvent() {
		return logEvent;
	}
	/**
	 * @param logEvent the logEvent to set
	 */
	public void setLogEvent(boolean logEvent) {
		this.logEvent = logEvent;
	}
	
	
	public void addAuditEvent(AuditEvent auditEvent) {
		if(this.auditEventList == null) {
			auditEventList = new LinkedList<AuditEvent>();
		}
		auditEventList.add(auditEvent);
	}
	
	public AuditEvent removeAuditEvent() {
		if(auditEventList==null || auditEventList.isEmpty()){
			return null;
		}
		return auditEventList.removeFirst();
	}
}
