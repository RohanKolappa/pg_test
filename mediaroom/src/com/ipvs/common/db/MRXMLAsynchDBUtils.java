package com.ipvs.common.db;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AsyncThreadBase;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.params.DBParams;

class AsynDBRequest {
	MRRequestI request= null;
	MRLog log=null;
	MRDB db = null;
	ServiceAgentFactoryI serviceAgentFactory=null;
	String listName= null;
	String objectType=null;
	String NID =null;
	String childXpath = null;
	String[] arrWhereXpath = null;
	Element dataElement = null;
	String action = null;	  
	long DBCallTimeOut = 0;
	Object callBackDataObject = null;
	AsynchDBRequestResponseListenerI callBackObject = null;
	public AsynDBRequest(long DBCallTimeOut,MRRequestI request, MRLog log, MRDB db, String listName, String objectType, 
			String NID, String childXpath, String[] arrWhereXpath, Element dataElement,String action,
			Object callBackDataObject,AsynchDBRequestResponseListenerI callBackObject, ServiceAgentFactoryI serviceAgentFactory)
	{
		this.DBCallTimeOut = DBCallTimeOut;
		this.request = request;
		this.log = log;
		this.db= db;
		this.listName=listName;
		this.objectType = objectType;
		this.NID =NID;
		this.childXpath=childXpath;
		this.arrWhereXpath=arrWhereXpath;
		this.dataElement = dataElement;	
		this.action = action;
		this.callBackDataObject = callBackDataObject;
		this.callBackObject = callBackObject;		
		this.serviceAgentFactory = serviceAgentFactory;
	}

}

public class MRXMLAsynchDBUtils extends AsyncThreadBase {	
	private static MRXMLAsynchDBUtils asynchDbUtils = null;
	private MRXMLAsynchDBUtils() {
		super();
	}
	private void addRequestToQueue(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, 
			String[] arrWhereXpath, Element dataElement,String action,Object callBackDataObject,AsynchDBRequestResponseListenerI callBackObject, ServiceAgentFactoryI serviceAgentFactory) {
		AsynDBRequest dbRequest = new AsynDBRequest(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, 
				arrWhereXpath, dataElement, action,callBackDataObject,callBackObject, serviceAgentFactory);
		
		addRequest(dbRequest);
		log.addInfo("Add Asynch Request to Queue. Action="+action + " NID:="+NID +" ObjectType := "+objectType);
	}
	
	public  void addAsynch(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, 
			String childXpath, String[] arrWhereXpath, Element dataElement,Object callBackDataObject,
			AsynchDBRequestResponseListenerI callBackObject, ServiceAgentFactoryI serviceAgentFactory) throws MRException {
		
		addRequestToQueue(DBCallTimeOut, request, log, db, listName, objectType, NID, 
				childXpath, arrWhereXpath, dataElement, DBParams.ADD,callBackDataObject,callBackObject, serviceAgentFactory);
	}

	public  void replaceAsynch(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, String objectType, String NID, 
			String childXpath, String[] arrWhereXpath, Element dataElement,Object callBackDataObject
			,AsynchDBRequestResponseListenerI callBackObject, ServiceAgentFactoryI serviceAgentFactory) throws MRException {
		
		addRequestToQueue(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, 
				dataElement, DBParams.REPLACE,callBackDataObject,callBackObject, serviceAgentFactory);
	}

	public  void deleteAsynch(long DBCallTimeOut, MRRequestI request, MRLog log, MRDB db, String listName, 
			String objectType, String NID, String childXpath, String[] arrWhereXpath,
			Object callBackDataObject, AsynchDBRequestResponseListenerI callBackObject, ServiceAgentFactoryI serviceAgentFactory) throws MRException {
		addRequestToQueue(DBCallTimeOut, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, 
				null, DBParams.DELETE, callBackDataObject, callBackObject, serviceAgentFactory);
	}
	
	protected void handleRequest(Object object) throws MRException,Exception {
		if(!(object instanceof AsynDBRequest)) {
			return;
		}
		AsynDBRequest request = (AsynDBRequest)object;
		request.log.addInfo("Handle Asynch Request. Action :="+request.action + " NID:="+request.NID +" ObjectType := "+request.objectType);
		String action   = request.action;
		int status = MRRequest.STATE_DONE;
		String logData = null;
		Document  response = null;
		
		try {
			if(action.equals(DBParams.ADD)){
				response = MRXMLDBUtils.add(request.DBCallTimeOut, request.request, request.log, request.db, request.listName, request.objectType, request.NID, request.childXpath, request.arrWhereXpath, request.dataElement);
			} else if(action.equals(DBParams.REPLACE)){
				response = MRXMLDBUtils.replace(request.DBCallTimeOut, request.request, request.log, request.db, request.listName, request.objectType, request.NID, request.childXpath, request.arrWhereXpath, request.dataElement);
			} else if(action.equals(DBParams.DELETE)){
				response = MRXMLDBUtils.delete(request.DBCallTimeOut, request.request, request.log, request.db, request.listName, request.objectType, request.NID, request.childXpath, request.arrWhereXpath);
			}
		}catch(MRException exp) {
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
			exp.printStackTrace();
			status = MRRequest.STATE_EXCEPTION;
		}catch(Exception exp){
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(DBExceptionCodes.ASYN_DB_ACTION_FAILED), "Description", "Action:="+request.action + "Message := " + exp.getMessage());
			exp.printStackTrace();
			status = MRRequest.STATE_EXCEPTION;
		}
		if(request.callBackObject!= null) {
			Element responseEl = null;
			if(response != null && response.getRootElement() != null){
				responseEl = (Element) response.getRootElement().detach();
			}
			request.callBackObject.handleAsynchDBRequestResponse(request.action, responseEl, request.callBackDataObject,status , logData, request.serviceAgentFactory);
		}
	}

	public static MRXMLAsynchDBUtils getInstance() {
		if(asynchDbUtils == null) {
			asynchDbUtils = new MRXMLAsynchDBUtils();
		}
		return asynchDbUtils;
	}
}
