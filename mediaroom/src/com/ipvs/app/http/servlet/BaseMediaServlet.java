package com.ipvs.app.http.servlet;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.http.common.HttpConst;
import com.ipvs.http.servlet.IPVSHttpServlet;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;

public class BaseMediaServlet extends IPVSHttpServlet{

	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public BaseMediaServlet(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, String userJID) {
		super(xf, log, db, resourceFactory, userJID);
	}

	public Document getURLQuery(HttpServletRequest req, HttpServletResponse resp, MRRequest request) throws Exception {
		HttpAuthentication.validateSignature(req, resp, xf.xmpp(userJID), xf.sf(userJID),db, log, true);	

		String mediaID =  req.getParameter(HttpConst.MEDIAID_PARAMETER);
		if(mediaID==null) {
			throw new MRException(HttpServletResponse.SC_BAD_REQUEST, "MediaId Missing");
		}
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceResourceConst.VIEWSNAPSHOTURL + SessionServiceConst.XQUERYXML;	    	   
			 
		String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
		
		String destNID =  req.getParameter(HttpConst.DSTNID_PARAMETER);		
		if(destNID==null) {
			log.addInfo("BaseMediaServlt: Destination information not found return primary ipaddress " ,MRLog.INFO,MRLog.NOTIFY);
			destNID = "";
		}
		query = query.replace(SessionServiceResourceConst._FILENID_,mediaID);
		query = query.replace(SessionServiceResourceConst._DSTNID_,destNID);
		String protocol = req.getHeader("X-Forwarded-Proto");
		if(protocol == null || protocol.isEmpty())
			protocol = "http";
		query = query.replace(SessionServiceConst._PROTOCOL_,protocol);
		
		Document queryResultDoc = GetObjectUtils.executeQuery(query,request, xf.xmpp(userJID), log, db, xf.sf(userJID), resourceFactory);
		
    	return queryResultDoc;
	}

	public String updateURLQueryParams(MRRequest request, String url, Document queryResultDoc, Element ApiKeyTableEl) throws Exception {
		
		String returnURL = url;
		
		//apply NAT policy before redirecting to DMS	    	
    	Element sourceElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Source");
    	Element destinationElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Destination");
    	Document deviceDoc = MRXMLUtils.getDocumentXpath(queryResultDoc, "//Device");	
    
    	String resolveIP = StreamPolicyUtils.getResolvedIPAddressForURL(request, xf.xmpp(userJID), log, db, destinationElement, sourceElement, deviceDoc);
    	returnURL = returnURL.replace("_IPADDRESS_",resolveIP);
    	
    	//update timestamp and signature
    	long now = System.currentTimeMillis();
		String timeStamp = String.valueOf(now + (SessionServiceResourceConst.REDIRECT_EXPIRE_TIME * 1000));
		String signature = HttpAuthentication.generateExpiryBasedSignature(ApiKeyTableEl, timeStamp);

		returnURL = returnURL.replaceAll(SessionServiceResourceConst.TIMESTAMP_CONST, timeStamp);
		returnURL = returnURL.replaceAll(SessionServiceResourceConst.SIGNATURE_CONST, signature);
		
		return returnURL;
	}
	
	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {
		// TODO Auto-generated method stub
		
	}
}
