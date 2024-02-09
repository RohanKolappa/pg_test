package com.ipvs.common.service;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;


/**
 * Common Implementation of the service
 * Requests are received by the service and dispatched to the registered handlers
 * @author miyer
 *
 */
public class  MRServer {
	
	protected XMPPI xmpp;
	protected MRLog log;
	protected MRDB db;
	protected String adminPrefix;
	protected MRResponseHandlerI defaultResponseHandler;
	protected Hashtable<String,MRService> serviceMap;
	protected MRResourceFactoryI resourceFactory;
	protected ServiceAgentFactoryI profileFactory;
	
	
  public static final String ADMIN = "admin";
	
	/**
	 * The MRService receives all MediaRoom requests
	 * The MRM and PEM modules can be instantiated (current implementation)
	 * The MRM and PEM can also be setup as independent entities 
	 * All the messaging between the MRService, MRM and PEM can be
	 * handled thru node changes to the RequestRoot
	 * @param xmpp the xmpp server handle
	 * @param log the log file to use
	 * @param ipvsRootNID the root node for the service
	 */
	public MRServer(XMPPI xmpp, MRLog log, MRDB db, 
	   MRResponseHandlerI resultHandler,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) {
		this.xmpp = xmpp;
		this.resourceFactory = resourceFactory;
		this.profileFactory = profileFactory;
		this.log = log;
		this.db = db;
		this.adminPrefix = ADMIN + "@";
		this.defaultResponseHandler = resultHandler;
		this.serviceMap = new Hashtable<String,MRService>();

		// TBD Subscribe to the this.requestRootNID		
	}
		
	/*
	 * Adds a request to the request tree
	 * @param userJID the JID of the user making the request
	 * @param logLevel the logging level for this request
	 * @param requestType the type of this request
	 * @param message the message which specifies the parameters for the request
	 * @return requestNID the NID of the request object created in the tree
	 * @throws Exception
	 */
	public String handleRequest(String userJID, String userAgentName, String appServerJID, String resourceGroup,
	    Document requestDoc, MRResponseHandlerI responseHandler) throws Exception {
		// TBD add to this.requestRootNID
		// System.out.println("MRServer:handleRequest message " + messageXML);
		
	    MRMessage message = new MRMessage(requestDoc);
		
		Element header = MRXMLUtils.getElement(message.getDocument(), "Header");
		int logLevel = MRLog.ERROR;
		try {
		  logLevel = Integer.parseInt(MRXMLUtils.getAttributeValue(header, MRConst.LOG_LEVEL, "0"));
		  

		}
		catch(Exception e) {
		  // Nothing to be done
		}
		String serviceName = MRXMLUtils.getAttributeValue(header, MRConst.SERVICE_NAME, null);
		if(serviceName == null)
			throw new MRException(MRExceptionCodes.SERVICENOTFOUND,MRXMLUtils.generateErrorXML("serviceName", "null"));

		String requestName = MRXMLUtils.getAttributeValue(header, MRConst.REQUEST_NAME, null);
		if(requestName == null)
			throw new MRException(MRExceptionCodes.REQUESTNOTFOUND,MRXMLUtils.generateErrorXML("requestName", "null"));

		this.log.addInfo(userJID + "@MRServer:handleRequest " + requestName, MRLog.OK, MRLog.NOTIFY);

		MRService service = serviceMap.get(serviceName);
		if(service == null)
			throw new MRException(MRExceptionCodes.SERVICENOTFOUND,MRXMLUtils.generateErrorXML("serviceName", serviceName));

		MRRequestHandlerI handler = service.getHandler(requestName);
		if(handler == null)
			throw new MRException(MRExceptionCodes.REQUESTNOTFOUND,MRXMLUtils.generateErrorXML("requestName", requestName));

		MRServerThread serverThread = new MRServerThread();
		Thread t = new Thread(serverThread);
		String clientData = MRXMLUtils.getValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA, "");
		// Generate the requestNID
		String requestNID = userJID + "/" + String.valueOf(serverThread.hashCode());
		if(handler instanceof EventLoggerI) {
			logLevel = MRLog.INFO;
		}
		MRRequest request = new MRRequest(userJID, userAgentName, appServerJID, logLevel, requestNID, 
				serviceName, requestName, message, resourceGroup,clientData);
		
		if(userJID.startsWith(this.adminPrefix))
		  request.setSuperUser(true);
		
		if(responseHandler == null)
			responseHandler = defaultResponseHandler;
		
		serverThread.init(request, 
				handler, 
				xmpp, log, db,
				resourceFactory, profileFactory,
				responseHandler);
		
		// Add the node to the db
		MRRequest.addRequest(requestNID, request);
		
		// Start the serverThread
		t.start();
		
		return requestNID;
	}
	
	/*
	public Document getResponse(String requestNID, boolean deleteIfDone) throws Exception  {
		MRRequest request = MRRequest.getRequest(requestNID);
		Document result = request.getResult();
		
		if(request.getState() == MRRequest.STATE_EXCEPTION) {
			MRException e = request.getException();
			if(deleteIfDone)
				MRRequest.deleteRequest(requestNID);
			throw e;
		}
		if(request.getState() == MRRequest.STATE_DONE) {
			if(deleteIfDone)
				MRRequest.deleteRequest(requestNID);
		}
		
		return result;
	}
	*/
	

		
	public void addService(MRService service) {
		String serviceName = service.getName();
		this.serviceMap.put(serviceName, service);
	}
		
	public String flushLog(String requestNID) {
		return this.log.flush(requestNID);
	}

}
