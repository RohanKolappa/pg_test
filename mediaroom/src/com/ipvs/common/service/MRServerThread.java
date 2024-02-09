package com.ipvs.common.service;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;

public class MRServerThread implements Runnable {
	
	MRRequest request;
	MRRequestHandlerI handler;
	XMPPI xmpp;
	MRLog log;
	MRDB schema;
	MRResourceFactoryI resourceFactory;
	ServiceAgentFactoryI profileFactory;
	MRResponseHandlerI responseHandler;
		
	public void init(MRRequest request, 
			MRRequestHandlerI handler,
			XMPPI xmpp, MRLog log, MRDB schema,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory,
			MRResponseHandlerI responseHandler) {
		this.request = request;
		this.handler = handler;
		this.xmpp = xmpp;
		this.log = log;
		this.schema = schema;
		this.resourceFactory = resourceFactory;
		this.profileFactory = profileFactory;
		this.responseHandler = responseHandler;
	}
	
	@SuppressWarnings("fallthrough")
	public void handleRequest() throws Exception {
		
		this.log.addTraceBegin(request);
		try {
			switch(request.getState()) {
			//Findbugs intend to fall through
			case MRRequest.STATE_ADDED:
				request.setState(MRRequest.STATE_CHECKINGPOLICY);
				MRRequest.updateRequest(request);
				MRAuthorization auth = handler.authorizeRequest(request,
						xmpp, log, schema, resourceFactory, profileFactory);
				request.setAuth(auth);
				request.setState(MRRequest.STATE_POLICYCHECKED);
				MRRequest.updateRequest(request); 
			case MRRequest.STATE_POLICYCHECKED:
			case MRRequest.STATE_AUTHORIZED:
				request.setState(MRRequest.STATE_PROCESSING);
				MRRequest.updateRequest(request);
				Document response = handler.getResponse(request,
						xmpp, log, schema, resourceFactory, profileFactory);
				if(response != null) {
					request.setResult(response);
					request.setState(MRRequest.STATE_DONE);
				}
			default:
			}
		}
    catch(MRException mre) {
      mre.printStackTrace();
      request.setException(mre);
      request.setState(MRRequest.STATE_EXCEPTION);
    }
    catch(Exception e) {
      String st = MRException.getStackTrace(e);
      e.printStackTrace();
      MRException me = new MRException(MRExceptionCodes.SYSTEMERROR, st);
      request.setException(me);
      request.setState(MRRequest.STATE_EXCEPTION);
    }

		// Send the response if it is done
		if( (request.getState() == MRRequest.STATE_DONE) ||
			(request.getState() == MRRequest.STATE_EXCEPTION)) {
		  if(request.getState() == MRRequest.STATE_EXCEPTION) {
		    MRException mre = request.getException();
		    this.log.addLog(request, MRLog.ERROR, mre.getCode(), mre.getData());
		  }
	    try {	    			
	    	AuditEvent auditEvent = null;
	    	
	    	//log audit event for for given request	    	
	    	if(AuditEventUtils.logAduitEventForGivenRequest(request.getRequestName(), log)) {
	    		Document resultDoc  = request.getResult();
	    		if(request.getLoggingDocument()!=null) {
	    			resultDoc = request.getLoggingDocument();
	    		}
	    		auditEvent = AuditEventUtils.getAuditEventObjectForGivenRequest(log, request.getUserJID() , request.getMessage().getDocument(), resultDoc, request.getOperatorGroups(), 
	    				AuditEventUtils.getAuditEventStatus(request), AuditEventUtils.getAuditEventLevel(request), AuditEventUtils.getAuditEventErrorDescription(request), request.getRequestName());
	    		
	    	} else if(handler instanceof EventLoggerI) {	
	    		try {
					EventLoggerI eventLogger = (EventLoggerI) handler;
					auditEvent = eventLogger.getAuditEvent(request, xmpp, log, schema, AuditEventUtils.getAuditEventStatus(request), 
							AuditEventUtils.getAuditEventLevel(request),AuditEventUtils.getAuditEventErrorDescription(request));
				} catch (Exception e) {
					log.addInfo("Exception occur during generating audit event."+MRException.getStackTrace(e),MRLog.INFO,MRLog.NOTIFY);
					e.printStackTrace();
				}				
	    	}
	        String logStr = this.log.flush(request.getNID());
	        
	        if(auditEvent!=null) {
	        	request.addAuditEvent(auditEvent);
	        }
	        
	        this.responseHandler.handleResponse(request, logStr);
		  }
		  catch(Exception e) {
		    this.log.addLog(request, MRLog.ERROR, MRExceptionCodes.RESPONSEHANDLERFAILED, request.getNID());
		  }
		}
		this.log.addTraceEnd(request);
		
	}

	
	public void run() {
		try {
			this.handleRequest();
		}
		catch(Exception e) {
			e.printStackTrace(); // TBD
		}
	}
	
}
